// Fill out your copyright notice in the Description page of Project Settings.


#include "Neat.h"
#include <algorithm>
#include <chrono>
#include <random>
#include <thread>


Neat::Neat(unsigned int _populationSize, unsigned int _input, unsigned int _output, const NeatParameters& _neatParam, INIT init)
{
	if (_neatParam.activationFunctions.size() == 0) return;//Avoid Unreal crash, should probably put a debug message

	populationSize = _populationSize;
	input = _input;
	output = _output;

	neatParam = _neatParam;

	networks.resize(populationSize);
	genomes = new Genome[populationSize];

	if (init != INIT::NONE)
	{
		for (unsigned int i = 0; i < populationSize; i++)
		{
			genomes[i] = (Genome(input, output, neatParam.activationFunctions));

			if (init == INIT::ONE)
			{
				oneConnectionInit(genomes[i]);
			}
			else if (init == INIT::FULL)
			{
				fullConnectInit(genomes[i]);
			}
		}


		generateNetworks();
	}
}

Neat::Neat()
{
}

void Neat::fullConnectInit(Genome& gen)
{

	std::vector<int> inputList, outputList;

	for (unsigned int cpt = 0; cpt < input; cpt++)
	{
		inputList.push_back(cpt);
	}

	for (unsigned int cpt = 0; cpt < output; cpt++)
	{
		outputList.push_back(cpt + input);
	}

	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::shuffle(inputList.begin(), inputList.end(), std::default_random_engine(seed));
	std::shuffle(outputList.begin(), outputList.end(), std::default_random_engine(seed));

	std::vector<int>::iterator itInput = inputList.begin();
	std::vector<int>::iterator itOutput = outputList.begin();

	bool inputDone = false, outputDone = false;

	do
	{
		int nodeA, nodeB;

		if (itOutput == outputList.end())
		{
			nodeA = *itInput;
			nodeB = outputList.back();

		}
		else if (itOutput == outputList.end())
		{
			nodeA = inputList.back();
			nodeB = *itOutput;
		}
		else {
			nodeA = *itInput;
			nodeB = *itOutput;
		}

		gen.addConnection(nodeA, nodeB, allConnections, randFloat() * randPosNeg() * neatParam.weightMuteStrength);


		++itInput; ++itOutput;

		if (itInput == inputList.end())
		{
			inputDone = true;
			itInput = inputList.begin();
		}

		if (itOutput == outputList.end())
		{
			outputDone = true;
			itOutput = outputList.begin();
		}

	} while (outputDone == false || inputDone == false);

	addToSpecies(&gen);
}

void Neat::oneConnectionInit(Genome& gen)
{
	gen.mutateLink(allConnections);//Minimum structure
	gen.mutateWeights(neatParam.weightMuteStrength, 1.0, Genome::WEIGHT_MUTATOR::COLDGAUSSIAN);

	addToSpecies(&gen);
}

Neat::~Neat()
{
	delete[] genomes;
}

void Neat::mutate(Genome& genome, std::mutex* lock)
{
	if (neatParam.activationFunctions.size() == 0)
		return;

	if (neatParam.pbMutateNode > randFloat())
	{

		unsigned int index = randInt(0, neatParam.activationFunctions.size() - 1);
		genome.mutateNode(allConnections, neatParam.activationFunctions[index], lock);
	}
	
	if (neatParam.pbMutateLink > randFloat())
	{
		genome.mutateLink(allConnections, lock);
	}
	//Official implementation says that a link can't be added after a node
	//Don't understand why
	//else {
		if (neatParam.pbWeight > randFloat())
		{
			genome.mutateWeights(neatParam.weightMuteStrength, 1.0, Genome::WEIGHT_MUTATOR::GAUSSIAN);
		}

		//if (neatParam.pbWeightShift > randFloat()) 
		//{
		//	genome.mutateWeightShift(neatParam.pbWeightShift);
		//}

		//if (neatParam.pbWeightRandom > randFloat()) 
		//{
		//	genome.mutateWeightRandom(neatParam.pbWeightRandom);
		//}

		if (neatParam.pbToggleLink > randFloat()) 
		{
			genome.mutateLinkToggle();
		}
	//}
}

void Neat::generateNetworks()
{
	for (unsigned int cpt = 0; cpt < populationSize; cpt++)
	{
		genomeToNetwork(genomes[cpt], networks[cpt]);
	}
}

void Neat::genomeToNetwork(Genome& genome, NeuralNetwork& network)
{
	network.clear();

	std::vector<GeneNode>* nodes = genome.getNodes();
	std::vector<std::pair<unsigned int, unsigned int>> nodePosition;//Stores postion of the nodes in the network
	nodePosition.reserve(nodes->size());

	//Add the nodes to the layer
	for (std::vector<GeneNode>::iterator node = nodes->begin(); node != nodes->end(); ++node)
	{
		unsigned int layer = node->getLayer();

		switch (node->getType())
		{
		case NODE_TYPE::HIDDEN:
			nodePosition.push_back(std::pair<unsigned int, unsigned int>(layer, network.getNHiddenNode(layer)));
			network.addHiddenNode(layer, node->getActivation());
			break;

		case NODE_TYPE::INPUT:
			nodePosition.push_back(std::pair<unsigned int, unsigned int>(0, network.getNInputNode()));
			network.addInputNode();
			break;

		case NODE_TYPE::OUTPUT:
			nodePosition.push_back(std::pair<unsigned int, unsigned int>(UINT_MAX, network.getNOutputNode()));
			network.addOutputNode(node->getActivation());
			break;
		}
	}

	std::map<unsigned int, GeneConnection>* connections = genome.getConnections();

	for (std::map<unsigned int, GeneConnection>::iterator connection = connections->begin(); connection != connections->end(); ++connection)
	{
		if (connection->second.isEnabled() == true)
		{
			network.connectNodes(nodePosition[connection->second.getNodeA()], nodePosition[connection->second.getNodeB()], connection->second.getWeight());
		}
	}
}

void Neat::evolve()
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

	sortedSpecies.sort(speciesSortAsc());
	
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
	float totalFitness = 0;

	for (int i = 0; i < populationSize; i++)
	{
		totalFitness += genomes[i].getSpeciesScore();
	}

	if (neatParam.saveAvgHistory == true) avgHistory.push_back(totalFitness/populationSize);

	float skim = 0;
	int totalExpected = 0;

	for (std::vector<Species>::iterator it = species.begin(); it != species.end(); ++it)
	{
		it->countOffspring(skim, totalFitness, populationSize);
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
	else if (totalExpected > populationSize)
	{
		do {

			std::list<Species*>::iterator it = sortedSpecies.begin();

			while ((*it)->getExpectedOffspring() == 0)
			{
				++it;
			}

			(*it)->decrementExpectedOffspring();

		} while (totalExpected > populationSize);
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
	std::mutex lock;

	std::list<Species*>::iterator itSortedSpecies = sortedSpecies.begin();

	float totalWorkload = sortedSpecies.size();
	float workload = totalWorkload / cpus;
	int currentWorkload = 0;
	int count = 0;

//#ifndef REPRO_MULTITHREAD
//	currentWorkload = totalWorkload;//Multithreading is creating problem when we modify when modifying the all connections unordered map
//#else
	while (workload < 1)
	{
		cpus--;
		workload = totalWorkload / cpus;
	}

	currentWorkload = floor(workload);
	float workloadFrac = fmod(workload, 1.0f);
	float restWorkload = workloadFrac;

	while (cpus > threads.size()+1)
	{

		threads.push_back(std::thread(&Neat::reproduce, this, currentWorkload + floor(restWorkload), itSortedSpecies, newBornIndex, std::ref(sortedSpecies), newPop, &lock));

		count += currentWorkload + floor(restWorkload);

		for (int i = 0; i < currentWorkload + floor(restWorkload); i++)
		{
			newBornIndex += (*itSortedSpecies)->getExpectedOffspring();
			++itSortedSpecies;
		}

		restWorkload -= floor(restWorkload);
		restWorkload += workloadFrac;
	}

	while (restWorkload > 0)
	{
		restWorkload--;
		currentWorkload++;
	}

	count += currentWorkload;

	while (count > totalWorkload)
	{
		currentWorkload--;
		count--;
	}
//#endif
	reproduce(currentWorkload, itSortedSpecies, newBornIndex, sortedSpecies, newPop, &lock);

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
	count = 0;
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

			if (species[cpt].getNovel() == false)
			{
				species[cpt].age++;
			}
			else {
				species[cpt].setNovel(false);
			}


			species[cpt].setChamp(nullptr);
		}

		cpt++;
		check++;
	}

	if (it != species.end())
	{
		species.erase(it, species.begin() + cpt);
	}

#ifdef SPECIES_DEBUG
	std::cout << "species: " << species.size() << std::endl;
#endif
	//Official implmentation removes old innovations here not sure if that's really usefull
	
	generateNetworks();
}

void Neat::reproduce(int workload, std::list<Species*>::iterator it, int newBornIndex, std::list<Species*>& sortedSpecies, Genome* newPop, std::mutex* lock)
{
	for(int i = 0; i < workload; ++i, ++it)
	{
		Species* curSpecies = *it;

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
						newPop[newBornIndex].mutateWeights(neatParam.weightMuteStrength, 1, Genome::WEIGHT_MUTATOR::GAUSSIAN);
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

				mutate(newPop[newBornIndex], lock);
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
				else if (randFloat() <= neatParam.pbMateMultipoint / (neatParam.pbMateMultipoint + neatParam.pbMateSinglepoint))
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
					mutate(newPop[newBornIndex], lock);
				}
			}

			newPop[newBornIndex].setScore(0);

			//Add the new born to species later
			//To not disturb the reproduction
		}
	}
}

float Neat::gaussRand() 
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

void Neat::adjustFitness()
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
		std::sort(itSpecies->getSpecies()->begin(), itSpecies->getSpecies()->end(), genomeSortAsc);


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

void Neat::addToSpecies(Genome* gen)
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

float Neat::distance(Genome& genomeA, Genome& genomeB)
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

void Neat::setScore(const std::vector < float >& newScores)
{
	if (newScores.size() != populationSize)
	{
		std::cout << "New score size different from population size, aborting." << std::endl;

		return;
	}

	unsigned int bestIndex = 0;
	float bestScore = -1;

	for (int i = 0; i < newScores.size(); i++)
	{
		if (newScores[i] >= 0)
		{
			genomes[i].setScore(newScores[i]);

			if (newScores[i] > bestScore)
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
		goat = genomes[bestIndex];
	}

	if(neatParam.saveChampHistory == true) champHistory.push_back(bestScore);
}

bool Neat::saveHistory()
{
	bool result = true;

	if (neatParam.saveChampHistory == true)
	{
		if (saveVectorToCsv(neatParam.champFileSave, champHistory) == false)
		{
			result = false;
		}
	}

	if (neatParam.saveAvgHistory == true)
	{
		if (saveVectorToCsv(neatParam.avgFileSave, avgHistory) == false)
		{
			result = false;
		}
	}

	return result;
}

