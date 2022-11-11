// Fill out your copyright notice in the Description page of Project Settings.


#include "NeatAlgoGen.h"
#include <algorithm>
#include <fstream>
#include <thread>


NeatAlgoGen::NeatAlgoGen(unsigned int _populationSize, unsigned int _input, unsigned int _output, NeatParameters _neatParam)
{
	if (_neatParam.activationFunctions.size() == 0) return;//Avoid Unreal crash, should probably put a debug message

	populationSize = _populationSize;
	input = _input;
	output = _output;

	neatParam = _neatParam;

	networks.resize(populationSize);
	genomes = new Genome[populationSize];


	for (unsigned int i = 0; i < populationSize; i++)
	{
		genomes[i] = (Genome(input, output, neatParam.activationFunctions));
		Genome* genome = &genomes[i];
		genome->mutateLink(allConnections);//Minimum structure
		genome->mutateWeightRandom(neatParam.pbWeightRandom);

		addToSpecies(genome);
	}

	generateNetworks();
}

NeatAlgoGen::NeatAlgoGen()
{
}

NeatAlgoGen::~NeatAlgoGen()
{
	delete[] genomes;

	for (int i = 0; i < neatParam.activationFunctions.size(); i++)
	{
		delete neatParam.activationFunctions[i];
	}
}

void NeatAlgoGen::mutate(Genome& genome)
{
	if (neatParam.activationFunctions.size() == 0)
		return;

	if (neatParam.pbMutateLink > randFloat()) 
	{
		genome.mutateLink(allConnections);
	}
	//Official implementation says that a link can't be added after a node
	//Don't understand why
	else if (neatParam.pbMutateNode > randFloat()) 
	{

		unsigned int index = randInt(0, neatParam.activationFunctions.size() - 1);
		genome.mutateNode(allConnections, neatParam.activationFunctions[index]);
	}
	else {
		if (neatParam.pbWeightShift > randFloat()) 
		{
			genome.mutateWeightShift(neatParam.pbWeightShift);
		}

		if (neatParam.pbWeightRandom > randFloat()) 
		{
			genome.mutateWeightRandom(neatParam.pbWeightRandom);
		}

		if (neatParam.pbToggleLink > randFloat()) 
		{
			genome.mutateLinkToggle();
		}
	}
}

void NeatAlgoGen::generateNetworks()
{

	for (unsigned int cpt = 0; cpt < populationSize; cpt++)
	{
		networks[cpt].clear();

		std::vector<GeneNode>* nodes = genomes[cpt].getNodes();
		std::vector<std::pair<unsigned int, unsigned int>> nodePosition;//Stores postion of the nodes in the network
		nodePosition.reserve(nodes->size());

		//Add the nodes to the layer
		for (std::vector<GeneNode>::iterator node = nodes->begin(); node != nodes->end(); ++node)
		{
			unsigned int layer = node->getLayer();

			switch(node->getType())
			{
			case NODE_TYPE::HIDDEN:
				nodePosition.push_back(std::pair<unsigned int, unsigned int>(layer, networks[cpt].getNHiddenNode(layer)));
				networks[cpt].addHiddenNode(layer, node->getActivation());
				break;

			case NODE_TYPE::INPUT:
				nodePosition.push_back(std::pair<unsigned int, unsigned int>(0, networks[cpt].getNInputNode()));
				networks[cpt].addInputNode();
				break;
				
			case NODE_TYPE::OUTPUT:
				nodePosition.push_back(std::pair<unsigned int, unsigned int>(-1, networks[cpt].getNOutputNode()));
				networks[cpt].addOutputNode(node->getActivation());
				break;
			}
		}

		//Need to find on which layer the output nodes really are
		for (unsigned int i = 0; i < nodePosition.size(); i++)
		{
			if (nodePosition[i].first == -1)
			{
				nodePosition[i].first = networks[cpt].getLayerSize() - 1;
			}
		}

		std::map<unsigned int, GeneConnection>* connections = genomes[cpt].getConnections();

		for (std::map<unsigned int, GeneConnection>::iterator connection = connections->begin(); connection != connections->end(); ++connection)
		{
			if (connection->second.isEnabled() == true)
			{
				//if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 60.f, FColor::Red, FString::Printf(TEXT("%i %i %i %i"), nodePosition[connection->second.getNodeA()].first, nodePosition[connection->second.getNodeA()].second, nodePosition[connection->second.getNodeB()].first, nodePosition[connection->second.getNodeB()].second));
				//GEngine->AddOnScreenDebugMessage(-1, 60.f, FColor::Red, FString::Printf(TEXT("%i %i"), connection->second.getNodeA(), nodes->size()));

				networks[cpt].connectNodes(nodePosition[connection->second.getNodeA()], nodePosition[connection->second.getNodeB()], connection->second.getWeight());
			}
		}

		//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 60.f, FColor::Red, "End network");
	}
}

void NeatAlgoGen::evolve()
{
	generation++;

	if (neatParam.adaptSpeciation == true && generation > 1)
	{
		if (species.size() < neatParam.numSpeciesTarget)
			neatParam.speciationDistance -= neatParam.speciationDistanceMod;
		else if (species.size() < neatParam.numSpeciesTarget)
			neatParam.speciationDistance += neatParam.speciationDistanceMod;

		if (neatParam.speciationDistance < 0.3) neatParam.speciationDistance = 0.3;
	}
	
	std::list<Species*> sortedSpecies;

	for (std::vector<Species>::iterator it = species.begin(); it != species.end(); ++it)
	{
		it->computeScore();
		sortedSpecies.push_back(&*it);
	}

	if (neatParam.bestHigh == true)
	{
		sortedSpecies.sort(speciesSortAsc());
	}
	else {
		sortedSpecies.sort(speciesSortDesc());
	}
	
	//Flag the lowest performing species over age 20 every 30 generations 
	//NOTE: THIS IS FOR COMPETITIVE COEVOLUTION STAGNATION DETECTION
	if ((generation % 30) == 0)
	{
		std::list<Species*>::reverse_iterator currSpecies = sortedSpecies.rbegin();

		while (currSpecies != sortedSpecies.rend() && (*currSpecies)->age < 20)
		{
			++currSpecies;
		}

		if (currSpecies != sortedSpecies.rend())
		{
			(*currSpecies)->obliterate = true;
		}
	}

	adjustFitness();

	//Go through the organisms and add up their fitnesses to compute the
	//overall average
	float avgFitness = 0;

	for (int i = 0; i < populationSize; i++)
	{
		avgFitness += genomes[i].getScore();
	}

	avgFitness /= populationSize;

	if (avgFitness < 1) avgFitness = 1;

	float skim = 0;
	int totalExpected = 0;

	for (std::vector<Species>::iterator it = species.begin(); it != species.end(); ++it)
	{
		it->countOffspring(skim, avgFitness);
		totalExpected += it->getExpectedOffspring();
	}

	Species* bestSpecies = nullptr;

	//Need to make up for lost foating point precision in offspring assignment
	//If we lost precision, give an extra baby to the best Species
	if (totalExpected < populationSize)
	{
		//Find the Species expecting the most
		int maxExpected = 0;
		int finalExpected = 0;
		

		for (std::vector<Species>::iterator it = species.begin(); it != species.end(); ++it)
		{
			if (it->getExpectedOffspring() >= maxExpected) 
			{
				maxExpected = it->getExpectedOffspring();
				bestSpecies = &(*it);
			}

			finalExpected += it->getExpectedOffspring();
		}

		//Give the extra offspring to the best species
		bestSpecies->incrementExpectedOffspring();
		finalExpected++;

		//If we still arent at total, there is a problem
		//Note that this can happen if a stagnant Species
		//dominates the population and then gets killed off by its age
		//Then the whole population plummets in fitness
		//If the average fitness is allowed to hit 0, then we no longer have 
		//an average we can use to assign offspring.
		if (finalExpected < populationSize) 
		{
			//      cout<<"Population died!"<<endl;
			//cin>>pause;
			for (std::vector<Species>::iterator it = species.begin(); it != species.end(); ++it)
			{
				it->setExpectedOffspring(0);
			}

			bestSpecies->setExpectedOffspring(populationSize);
		}
	}

	//Official implementation re-sorts the species list by their fitness
	//but the fitness doesn't seem to have changed, maybe I missed something
	//Seems like the sorting functions are not the same, strange...
	//It seems that genomes in species were sorted by fitness at some point
	//but didn't see where

	bestSpecies = (*sortedSpecies.rbegin());

	//Official implementation compute incrment the value of the last time the population as improved or reset it, here
	//It's more optimal for us to do it in the setScore function

	//Check for stagnation- if there is stagnation, perform delta-coding
	if (highestLastChanged >= neatParam.dropOffAge + 5) 
	{

		//    cout<<"PERFORMING DELTA CODING"<<endl;

		highestLastChanged = 0;

		int halfPop = populationSize / 2;

		std::list<Species*>::reverse_iterator it = sortedSpecies.rbegin();

		(*it)->getChamp()->setSuperChampOffspring(halfPop);
		(*it)->setExpectedOffspring(halfPop);
		(*it)->lastImprove = (*it)->age;

		++it;

		if (it != sortedSpecies.rend())
		{

			(*it)->getChamp()->setSuperChampOffspring(populationSize - halfPop);
			(*it)->setExpectedOffspring(populationSize - halfPop);
			(*it)->lastImprove = (*it)->age;

			++it;

			//Get rid of all species under the first 2
			while (it != sortedSpecies.rend())
			{
				(*it)->setExpectedOffspring(0);
				++it;
			}
		}
		else {
			it = sortedSpecies.rbegin();
			(*it)->getChamp()->setSuperChampOffspring(populationSize);
			(*it)->setExpectedOffspring(populationSize);
		}

	}//else{ Official implementation has stolen babies functionnality here }

	//Remove from species that are meant to die and not reproduce
	for (int i = 0; i < populationSize; i++)
	{
		if (genomes[i].getEliminate() == true)
		{
			std::list<Species*>::iterator it = sortedSpecies.begin();
			while (it != sortedSpecies.end() && (*it)->remove(&genomes[i]) == false)
			{
				++it;
			}
		}
	}


	Genome* newPop = new Genome[populationSize];

	//Perform reproduction.  Reproduction is done on a per-Species
	//basis.  (So this could be paralellized potentially.)
	int newBornIndex = 0;
	std::vector<std::thread> threads;
	unsigned int cpus = std::thread::hardware_concurrency();

	std::list<Species*>::iterator itSortedSpecies = sortedSpecies.begin();
	std::mutex lock;

	/*lock.lock();

	while (cpus > threads.size()+1)
	{
		threads.push_back(std::thread(&NeatAlgoGen::reproduce, this, std::ref(itSortedSpecies), std::ref(lock), std::ref(newBornIndex), std::ref(sortedSpecies), newPop));
	}

	lock.unlock();*/
	reproduce(itSortedSpecies, lock, newBornIndex, sortedSpecies, newPop);

	for (int i = 0; i < threads.size(); i++)
	{
		threads[i].join();
	}

	threads.clear();

	//Adding to species is done using the pointer champ of the species so we can empty the species now
	for (std::vector<Species>::iterator itSpecies = species.begin(); itSpecies != species.end(); ++itSpecies)
	{
		itSpecies->getSpecies()->clear();
	}

	//Add new born to species
	for (int i = 0; i < populationSize; i++)
	{
		addToSpecies(&newPop[i]);
	}

	//Remove old generation
	delete[] genomes;
	genomes = newPop;

	//Remove extinct species and age those that survive
	int cpt = 0;

	std::vector<Species>::iterator it = species.end();
	int count = 0;
	int check = 0;

	while(cpt < species.size())
	{
		if (species[cpt].getSpecies()->size() == 0)
		{
			if(it == species.end()) it = species.begin() + cpt;
			
			count++;
		}
		else {
			//Try to do erasing by band
			if (it != species.end())
			{
				species.erase(it, species.begin() + cpt);
				it = species.end();
				
				cpt -= count;
				count = 0;
			}

			species[cpt].age++;
			species[cpt].setChamp(nullptr);
		}

		cpt++;
		check++;
	}

	if (it != species.end())
	{
		species.erase(it, species.begin() + cpt);
	}

	std::cout << "size: " << species.size() << std::endl;

	//Official implmentation removes old innovations here not sure if that's really usefull
	
	generateNetworks();
}

void NeatAlgoGen::reproduce(std::list<Species*>::iterator& it, std::mutex& lock, int& sharedNewBornIndex, std::list<Species*>& sortedSpecies, Genome* newPop)
{
	lock.lock();

	while (it != sortedSpecies.end())
	{
		Species* curSpecies = *it;
		++it;

		int newBornIndex = sharedNewBornIndex;//Reservation of a group of index
		sharedNewBornIndex += curSpecies->getExpectedOffspring();

		lock.unlock();

		bool champDone = false;

		for (int count = 0; count < curSpecies->getExpectedOffspring(); count++, newBornIndex++)
		{
			if(curSpecies->getChamp()->getSuperChampOffspring() > 0)
			{
				newPop[newBornIndex] = *curSpecies->getChamp();
				
				if (curSpecies->getChamp()->getSuperChampOffspring() > 1)
				{
					if ((randFloat() < 0.8) || (neatParam.pbMutateLink == 0.0))
					{
						//ABOVE LINE IS FOR:
						//Make sure no links get added when the system has link adding disabled
						//CEDRIC NOTES: why is 0.8 isn't a parameter value ?
						if (randFloat() > 0.5)
						{
							newPop[newBornIndex].mutateWeightRandom(neatParam.weightRandomStrength);
						}
						else {
							newPop[newBornIndex].mutateWeightShift(neatParam.weightShiftStrength);
						}
					}
					else {
						//Sometimes we add a link to a superchamp
						newPop[newBornIndex].mutateLink(allConnections);
					}
				}//else{ official implementation does some stuff }

				curSpecies->getChamp()->decrementSuperChampOffspring();
			}
			//If we have a Species champion, just clone it 
			else if ((champDone == false) && (curSpecies->getExpectedOffspring() > 5)) 
			{
				newPop[newBornIndex] = *curSpecies->getChamp();

				champDone = true;
			}
			//First, decide whether to mate or mutate
			//If there is only one organism in the pool, then always mutate
			else if ((randFloat() < neatParam.pbMutateOnly) || curSpecies->getSpecies()->size() == 1)
			{
				//According to official implementation you don't gain much from roulette selection because of the size of the species
				//No error here, official implmentation picks a random guy in species of size 1, above comment should probably be moved though
				newPop[newBornIndex] = *(*curSpecies->getSpecies())[0];

				mutate(newPop[newBornIndex]);
			}//Otherwise we should mate 
			else {
				Genome* gen1 = (*curSpecies->getSpecies())[randInt(0, curSpecies->getSpecies()->size() - 1)];
				Genome* gen2;

				if ((randFloat() > neatParam.interspeciesMateRate))
				{//Mate within Species
					
					do {
						gen2 = (*curSpecies->getSpecies())[randInt(0, curSpecies->getSpecies()->size() - 1)];
					} while (gen2 == gen1);

				}
				else {
					//Mate outside Species  

					//Select a random species
					int giveUp = 0;  //Give up if you cant find a different Species
					Species* randspecies = nullptr;
					
					
					do{

						//This old way just chose any old species
						//randspeciesnum=randInt(0,(pop->species).size()-1);

						//Choose a random species tending towards better species
						float randmult = gaussRand() / 4;
						if (randmult > 1.0) randmult = 1.0;
						//This tends to select better species
						int randSpeciesNum = (int)floor((randmult * (sortedSpecies.size() - 1.0)) + 0.5);
						std::list<Species*>::iterator randSpecies = (sortedSpecies.begin());
						for (int i = 0; i < randSpeciesNum; i++)
							++randSpecies;

						randspecies = (*randSpecies);

						++giveUp;
					} while ((randspecies == curSpecies) && (giveUp < 5));

					gen2 = (*randspecies->getSpecies())[randInt(0, randspecies->getSpecies()->size() - 1)];
				}

				//Perform mating based on probabilities of differrent mating types
				if (randFloat() < neatParam.pbMateMultipoint) 
				{
					newPop[newBornIndex].crossover(*gen1, *gen2, Genome::CROSSOVER::RANDOM);
				}
				else if (randFloat() < neatParam.pbMateMultipoint / (neatParam.pbMateMultipoint + neatParam.pbMateSinglepoint))
				{
					newPop[newBornIndex].crossover(*gen1, *gen2, Genome::CROSSOVER::AVERAGE);
				}
				else {
					newPop[newBornIndex].crossover(*gen1, *gen2, Genome::CROSSOVER::SINGLE_POINT);
				}

				//Determine whether to mutate the baby's Genome
				//This is done randomly or if the mom and dad are the same organism
				if (randFloat() > neatParam.pbMateOnly || gen1 == gen2 || distance(*gen1, *gen2) == 0.0)
				{
					mutate(newPop[newBornIndex]);
				}
			}

			newPop[newBornIndex].setScore(0);

			//Add the new born to species later
			//To not disturb the reproduction
		}

		lock.lock();
	}

	lock.unlock();
}

float NeatAlgoGen::gaussRand() 
{
	static int iset = 0;
	static float gset;
	float fac, rsq, v1, v2;

	if (iset == 0) 
	{
		do {
			v1 = 2.0f * (randFloat()) - 1.0f;
			v2 = 2.0f * (randFloat()) - 1.0f;
			rsq = v1 * v1 + v2 * v2;
		} while (rsq >= 1.0 || rsq == 0.0f);
		fac = sqrt(-2.0f * log(rsq) / rsq);
		gset = v1 * fac;
		iset = 1;
		return v2 * fac;
	}
	else {
		iset = 0;
		return gset;
	}
}

void NeatAlgoGen::adjustFitness()
{
	for (std::vector<Species>::iterator itSpecies = species.begin(); itSpecies != species.end(); ++itSpecies)
	{
		int ageDebt = (itSpecies->age - itSpecies->lastImprove + 1) - neatParam.dropOffAge;

		if (ageDebt == 0) ageDebt = 1;

		for (std::vector<Genome *>::iterator itGen = itSpecies->getSpecies()->begin(); itGen != itSpecies->getSpecies()->end(); ++itGen)
		{
			if ((ageDebt >= 1) || itSpecies->obliterate) 
			{
				(*itGen)->setSpeciesScore(((*itGen)->getScore()) * 0.01);
			}

			if (itSpecies->age <= 10)
			{
				(*itGen)->setSpeciesScore(((*itGen)->getScore()) * neatParam.ageSignificance);
			}

			//Do not allow negative fitness
			if ((*itGen)->getSpeciesScore() < 0.0001f) (*itGen)->setSpeciesScore(0.0001f);

			//Share fitness with the species
			(*itGen)->setSpeciesScore((*itGen)->getSpeciesScore() / itSpecies->getSpecies()->size());
		}

		//Sort the population and mark for death those after survival_thresh*pop_size
		if (neatParam.bestHigh == true)
		{
			std::sort(itSpecies->getSpecies()->begin(), itSpecies->getSpecies()->end(), genomeSortAsc);
		}
		else {
			std::sort(itSpecies->getSpecies()->begin(), itSpecies->getSpecies()->end(), genomeSortDesc);
		}

		//Decide how many get to reproduce based on survival_thresh*pop_size
		//Adding 1.0 ensures that at least one will survive
		int numParents = (int)floor(neatParam.killRate * (float)itSpecies->getSpecies()->size() + 1.0);

		//Mark for death those who are ranked too low to be parents
		itSpecies->setChamp(itSpecies->getSpecies()->back());  //Mark the champ as such

		int i = 0;

		for (std::vector<Genome*>::iterator itGen = itSpecies->getSpecies()->begin(); itGen != itSpecies->getSpecies()->end() && numParents > i; ++itGen, ++i)
		{
			(*itGen)->setEliminate(true);//Mark for elimination
		}
	}
}

void NeatAlgoGen::addToSpecies(Genome* gen)
{
	bool found = false;
	for (std::vector<Species>::iterator it = species.begin(); it != species.end() && found == false; ++it)
	{
		if (distance(*it->getChamp(), *gen) < neatParam.speciationDistance)
		{
			found = true;
			it->Add(gen);
		}
	}

	if (found == false)
	{
		species.push_back(Species(gen));
	}

}

float NeatAlgoGen::distance(Genome& genomeA, Genome& genomeB)
{
	int highestInnovationGene1 = 0;
	if (genomeA.getConnections()->size() != 0) 
	{
		highestInnovationGene1 = genomeA.getConnections()->crbegin()->first;
	}

	int highestInnovationGene2 = 0;
	if (genomeB.getConnections()->size() != 0) 
	{
		highestInnovationGene2 = genomeB.getConnections()->crbegin()->first;
	}

	Genome* genome1, *genome2;

	if (highestInnovationGene1 < highestInnovationGene2) 
	{
		genome1 = &genomeB;
		genome2 = &genomeA;
	}
	else {
		genome1 = &genomeA;
		genome2 = &genomeB;
	}

	std::map<unsigned int, GeneConnection>::const_iterator it1 = genome1->getConnections()->cbegin();
	std::map<unsigned int, GeneConnection>::const_iterator it2 = genome2->getConnections()->cbegin();
	int count = 0;

	float disjoint = 0;
	float excess = 0;
	float similar = 0;


	while (it1 != genome1->getConnections()->cend() && it2 != genome2->getConnections()->cend()) 
	{

		if (it1->first == it2->first) 
		{
			//similargene
			similar++;
			++it1; count++;
			++it2;
		}
		else if (it1->first > it2->first) 
		{
			//disjoint gene of b
			disjoint++;
			++it2;
		}
		else {
			//disjoint gene of a
			disjoint++;
			++it1;
			count++;
		}
	}

	excess = genome1->getConnections()->size() - count;

	if (similar == 0) similar = 1;

	//Official implmentation does some division by 1.0 here...
	return neatParam.disjointCoeff * disjoint + neatParam.excessCoeff * excess + neatParam.mutDiffCoeff * ((disjoint + excess) / similar);
}

void NeatAlgoGen::setScore(std::vector < float > newScores)
{
	if (newScores.size() != populationSize)
	{
		std::cout << "New score size different from population size, aborting." << std::endl;

		return;
	}

	unsigned int bestIndex = 0;
	float bestScore = newScores[0];

	for (int i = 1; i < newScores.size(); i++)
	{
		if (newScores[i] >= 0)
		{
			genomes[i].setScore(newScores[i]);

			if ((neatParam.bestHigh == true && newScores[i] > bestScore) || (neatParam.bestHigh == false && newScores[i] < bestScore))
			{
				bestScore = newScores[i];
				bestIndex = i;
			}
		}
		else {
			genomes[i].setScore(0);//Don't want to tank the score of scpecies with negative score
		}

	}

	if (lastBestScore > bestScore)
	{
		highestLastChanged++;
	}
	else {
		lastBestScore = bestScore;
		highestLastChanged = 0;
	}

	if(neatParam.saveHistory == true) history.push_back(bestScore);

	goat = genomes[bestIndex];
}

bool NeatAlgoGen::saveHistory()
{
	if (neatParam.saveHistory == false)
	{
		std::cout << "History not saved, aborting." << std::endl;

		return false;
	}

	int n = 1;
	std::ifstream f;

	do {
		if (f.is_open() == true) f.close();

		f = std::ifstream(neatParam.fileSave + "_" + std::to_string(n) + ".csv");
		n++;
	}while(f.good() && n < 100);

	f.close();

	if (n >= 100)
	{
		std::cout << "Could not find path, or files already exist." << std::endl;

		return false;
	}

	n--;

	std::fstream csvFile;
	csvFile.open(neatParam.fileSave + "_" + std::to_string(n) + ".csv", std::fstream::in | std::fstream::out | std::fstream::trunc);

	if (csvFile.is_open() == false)
	{
		std::cout << "Fstream failed to create file." << std::endl;

		return false;
	}

	for (std::vector<float>::iterator it = history.begin(); it != history.end(); ++it)
	{
		std::string str = std::to_string(*it);
		size_t pos = str.find(".");
		
		if (pos == -1)
		{
			csvFile << str << std::endl;
		}
		else {
			csvFile << str.substr(0, pos) << "," << str.substr(pos+1, str.size()) << std::endl;
		}
	}

	csvFile.close();
	return true;
}

