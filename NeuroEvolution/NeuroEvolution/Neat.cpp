// Fill out your copyright notice in the Description page of Project Settings.


#include "Neat.h"
#include <algorithm>
#include <chrono>
#include <random>
#include "ThreadPool.h"


Neat::Neat(unsigned int _populationSize, unsigned int _input, unsigned int _output, const NeatParameters& _neatParam, INIT init)
{
	if (this->init(_populationSize, _input, _output, _neatParam) == false) return;

	for (unsigned int i = 0; i < populationSize; i++)
	{
		genomes[i] = (Genome(input, output, neatParam.activationFunctions));

		if (init != INIT::NONE)
		{

			if (init == INIT::ONE)
			{
				oneConnectionInit(genomes[i]);
			}
			else if (init == INIT::FULL)
			{
				fullConnectInit(genomes[i]);
			}
		}
	}

	generateNetworks();
}

Neat::Neat(unsigned int _populationSize, unsigned int _input, unsigned int _output, const NeatParameters& _neatParam, std::vector<Genome>& initPop)
{
	if (this->init(_populationSize, _input, _output, _neatParam) == false) return;

	for (int i = 0; i < initPop.size(); i++)
	{
		for (std::map<unsigned int, GeneConnection>::iterator it = initPop[i].getConnections()->begin(); it != initPop[i].getConnections()->end(); ++it)
		{
			allConnections.emplace(std::pair<int, int>(it->second.getNodeA(), it->second.getNodeB()), it->first);
		}
	}

	if (initPop.size() == _populationSize)
	{
		for (int i = 0; i < initPop.size(); i++)
		{
			genomes[i] = initPop[i];
		}
	}
	else {
		for (int i = 0; i < _populationSize; i++)
		{
			genomes[i] = initPop[randInt(0, initPop.size() - 1 )];
			mutate(genomes[i]);
		}
	}

	generateNetworks();
}

bool Neat::init(unsigned int _populationSize, unsigned int _input, unsigned int _output, const NeatParameters& _neatParam)
{
	if (_neatParam.activationFunctions.size() == 0)
	{
		std::cerr << "Neat init failed, zero activation function given" << std::endl;

		return false;//Avoid Unreal crash, should probably put a debug message
	}

	populationSize = _populationSize;
	input = _input;
	output = _output;

	neatParam = _neatParam;

	networks.resize(populationSize);
	genomes = new Genome[populationSize];
	futureGen = new Genome[populationSize];

	initialized = true;

	return true;
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
	delete[] futureGen;
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
	int i = 0;
	for (std::vector<GeneNode>::iterator node = nodes->begin(); node != nodes->end(); ++node, ++i)
	{
		unsigned int layer = node->getLayer();

		switch (node->getType())
		{
		case NODE_TYPE::HIDDEN:
			nodePosition.push_back(std::pair<unsigned int, unsigned int>(layer, network.getNHiddenNode(layer)));
			network.addHiddenNode(layer, node->getActivation(), i);
			break;

		case NODE_TYPE::INPUT:
			nodePosition.push_back(std::pair<unsigned int, unsigned int>(0, network.getNInputNode()));
			network.addInputNode(i);
			break;

		case NODE_TYPE::OUTPUT:
			nodePosition.push_back(std::pair<unsigned int, unsigned int>(-1, network.getNOutputNode()));
			network.addOutputNode(node->getActivation(), i);
			break;
		}
	}

	//Need to find on which layer the output nodes really are
	for (unsigned int i = 0; i < nodePosition.size(); i++)
	{
		if (nodePosition[i].first == -1)
		{
			nodePosition[i].first = network.getLayerSize() - 1;
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

	//if (neatParam.adaptSpeciation == true && generation > 1)
	//{
	//	if (species.size() < neatParam.numSpeciesTarget)
	//		neatParam.speciationDistance -= neatParam.speciationDistanceMod;
	//	else if (species.size() > neatParam.numSpeciesTarget)
	//		neatParam.speciationDistance += neatParam.speciationDistanceMod;

	//	if (neatParam.speciationDistance < 0.3) neatParam.speciationDistance = 0.3;
	//}

	//Start CEDRIC Speciation threshold adaption
	if (neatParam.adaptSpeciation == true && generation > 1)
	{
		if (species.size() < neatParam.minExpectedSpecies)
			neatParam.speciationDistance -= (neatParam.speciationDistanceMod * neatParam.minExpectedSpecies / species.size()) * 1;
		else if (species.size() > neatParam.maxExpectedSpecies)
			neatParam.speciationDistance += (neatParam.speciationDistanceMod * species.size() / neatParam.maxExpectedSpecies) * 0.5;

		if (neatParam.speciationDistance < 0.3) neatParam.speciationDistance = 0.3;
	}
	//End CEDRIC Speciation threshold adaption


	std::list<Species*> sortedSpecies;

	for (std::vector<Species>::iterator it = species.begin(); it != species.end(); ++it)
	{
		it->computeScore();
		sortedSpecies.push_back(&*it);

		if (neatParam.elistism == true)
		{
			it->sort();
		}
	}

	sortedSpecies.sort(speciesSortAsc());

	//Flag the lowest performing species over age 20 every 30 generations 
	//NOTE: THIS IS FOR COMPETITIVE COEVOLUTION STAGNATION DETECTION
	if ((generation % 30) == 0)
	{
#ifdef DEBUG
		std::cout << "flag for obliterate" << std::endl;
#endif //DEBUG

		std::list<Species*>::iterator currSpecies = sortedSpecies.begin();

		while (currSpecies != sortedSpecies.end() && (*currSpecies)->age < 20)
		{
			++currSpecies;
		}

		if (currSpecies != sortedSpecies.end())
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

	if (neatParam.saveAvgHistory == true) avgHistory.push_back(totalFitness/(float)populationSize);

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
		int finalExpected = 0;
		int maxExpected = 0;

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

	bestSpecies = (*sortedSpecies.rbegin());

	bestSpecies->setPopChamp(true);

	//START Cedric Modif
	//We don't have stolen babies like official implementation,
	//But we should give a slight edge to the champion species, if it doesn't already have it
	if(neatParam.keepChamp == true && bestSpecies->getExpectedOffspring() < 2 && (bestSpecies->age - bestSpecies->lastImprove) < neatParam.dropOffAge)
	{
		//Steal one offspring from a bottom species that is not new
		int ageThreshold = 5;
		int offspringThreshold = 2;

		do{
			std::list<Species*>::iterator it = sortedSpecies.begin();

			do {

				for (it; it != sortedSpecies.end() && (*it)->age < ageThreshold && (*it)->getExpectedOffspring() < offspringThreshold; ++it)
				{}

				while ((*it)->getExpectedOffspring() > 1)
				{
					(*it)->decrementExpectedOffspring();

					bestSpecies->incrementExpectedOffspring();
				}

				++it;

			} while (bestSpecies->getExpectedOffspring() < 2 && it != sortedSpecies.end());

			if (ageThreshold == 0)
			{
				offspringThreshold = 1;
			}

			ageThreshold = 0;
		} while (bestSpecies->getExpectedOffspring() < 2);

#ifdef DEBUG
		std::cout << "offspring added to champ" << std::endl;
#endif DEBUG

	}
#ifdef DEBUG
	else if (bestSpecies->getExpectedOffspring() < 2 && neatParam.keepChamp == true)
	{
		std::cout << "champ dies of age " << (bestSpecies->age - bestSpecies->lastImprove) << std::endl;
	}
	else {
		std::cout << "champ offspring: " << bestSpecies->getExpectedOffspring() << std::endl;
	}
#endif DEBUG

	//END Cedric Modif

	//Official implementation re-sorts the species list by their fitness
	//but the fitness doesn't seem to have changed, maybe I missed something
	//Seems like the sorting functions are not the same, strange...
	//It seems that genomes in species were sorted by fitness at some point
	//but didn't see where

	

	//Official implementation compute incrment the value of the last time the population as improved or reset it, here
	//It's more optimal for us to do it in the setScore function

	//Check for stagnation- if there is stagnation, perform delta-coding
	if (highestLastChanged >= neatParam.dropOffAge + 5) 
	{
#ifdef DEBUG
		std::cout << "PERFORMING DELTA CODING" << std::endl;
#endif // DEBUG

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

#ifdef DEBUG
	for (std::vector<Species>::iterator it = species.begin(); it != species.end(); ++it)
	{
		if (it->getSpecies()->size() == 0)
		{
			std::cout << "Species killed" << std::endl;
		}
	}
#endif //DEBUG


	//Perform reproduction.  Reproduction is done on a per-Species
	//basis.  (So this could be paralellized potentially.)
	int newBornIndex = 0;
	std::mutex lock;
	std::list<Species*>::iterator itSortedSpecies = sortedSpecies.begin();

	int threads = 1;
	ThreadPool* pool = ThreadPool::getInstance();
	size_t taskLaunched = pool->getTasksTotal();
	unsigned int cpus = (pool->getThreadPoolSize() > taskLaunched ? pool->getThreadPoolSize() - taskLaunched : 0);

	float totalWorkload = sortedSpecies.size();
	float workload = (cpus > 1 ? totalWorkload / cpus : totalWorkload);
	float restWorkload = 0;
	int currentWorkload = totalWorkload;
	int count = 0;

	if (totalWorkload == 1)
	{
		cpus = 1;
	}

	std::deque<std::atomic<bool>> tickets;

#ifdef MULTITHREAD
	while (workload < 1 && cpus > 2)
	{
		cpus--;
		workload = totalWorkload / cpus;
	}

	if (workload < 1.f)
	{
		cpus = 0;
	}

	while (cpus > threads)
	{
		currentWorkload = floor(workload);
		float workloadFrac = fmod(workload, 1.0f);
		restWorkload = workloadFrac;

		tickets.emplace_back(false);
		pool->queueJob(&Neat::reproduce, this, currentWorkload + floor(restWorkload), itSortedSpecies, newBornIndex, std::ref(sortedSpecies), futureGen, &lock, &tickets.back());
		++threads;

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
#endif // MULTITHREAD

	currentWorkload = totalWorkload - count;

	count += currentWorkload;

	reproduce(currentWorkload, itSortedSpecies, newBornIndex, sortedSpecies, futureGen, &lock);

	for (std::deque<std::atomic<bool>>::iterator itTicket = tickets.begin(); itTicket != tickets.end(); ++itTicket)
	{
		itTicket->wait(false);
	}

	//Adding to species is done using the pointer champ of the species so we can empty the species now
	for (std::vector<Species>::iterator itSpecies = species.begin(); itSpecies != species.end(); ++itSpecies)
	{
		itSpecies->getSpecies()->clear();
	}

	//Add new born to species
	//This could probably benifit from concurrency
	for (int i = 0; i < populationSize; i++)
	{
		addToSpecies(&futureGen[i]);
	}

	//Remove old generation
	Genome* genHolder = genomes;
	genomes = futureGen;
	futureGen = genHolder;

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

#ifdef DEBUG
	std::cout << "species: " << species.size() << std::endl;
#endif //DEBUG
	//Official implmentation removes old innovations here not sure if that's really usefull
	
	generateNetworks();
}

void Neat::reproduce(int workload, std::list<Species*>::iterator it, int newBornIndex, std::list<Species*>& sortedSpecies, Genome* newPop, std::mutex* lock, std::atomic<bool>* ticket)
{
	for(int i = 0; i < workload; ++i, ++it)
	{
		Species* curSpecies = *it;

		bool champDone = false;

		for (int count = 0; count < curSpecies->getExpectedOffspring(); count++, newBornIndex++)
		{
			//If we have a Species champion and lots of offspring or pouplation champion, just clone it 
			//Cedric Modif clone the pop champ
			if ((champDone == false && curSpecies->getExpectedOffspring() > 5) || curSpecies->isPopChamp() == true)
			{
				newPop[newBornIndex] = *curSpecies->getChamp();

#ifdef DEBUG
				if (curSpecies->isPopChamp() == true)
				{
					std::cout << "DONE" << std::endl;
				}
#endif
				champDone = true;
				curSpecies->setPopChamp(false);
			}
			else if (curSpecies->getChamp()->getSuperChampOffspring() > 0)
			{
				newPop[newBornIndex] = *curSpecies->getChamp();
				newPop[newBornIndex].setSuperChampOffspring(0);

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

				int index = -1;

				if (neatParam.elistism == true)
				{
					index = randGeoDist(std::min(neatParam.rouletteMultiplier / curSpecies->getSpecies()->size(), 0.9999f), curSpecies->getSpecies()->size() - 1);
				}
				else {
					index = randInt(0, curSpecies->getSpecies()->size() - 1);
				}
				

				Genome* gen1 = (*curSpecies->getSpecies())[index];
				Genome* gen2;

				if ((randFloat() > neatParam.interspeciesMateRate))
				{//Mate within Species
					
					index = -1;
					int giveUp = 0;

					do {
						if (neatParam.elistism == true)
						{
							index = randGeoDist(std::min(neatParam.rouletteMultiplier / curSpecies->getSpecies()->size(), 0.9999f), curSpecies->getSpecies()->size() - 1);
						}
						else {
							index = randInt(0, curSpecies->getSpecies()->size() - 1);
						}

					
						gen2 = (*curSpecies->getSpecies())[index];
						giveUp++;
					} while (gen2 == gen1 && giveUp < 10);

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

					index = -1;

					if (neatParam.elistism == true)
					{
						index = randGeoDist(std::min(neatParam.rouletteMultiplier / randspecies->getSpecies()->size(), 0.9999f), randspecies->getSpecies()->size() - 1);
					}
					else {
						index = randInt(0, randspecies->getSpecies()->size() - 1);
					}

					gen2 = (*randspecies->getSpecies())[index];
				}

				if (gen2 != gen1)
				{
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
				}else{
					newPop[newBornIndex] = *gen1;
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

	if (ticket != nullptr)
	{
		(*ticket) = true;
		ticket->notify_one();
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

		//itSpecies->setChamp(itSpecies->getSpecies()->back());  //Mark the champ as such
		//Should have already been done

		//Mark for death those who are ranked too low to be parents
		int i = 1;

		for (std::vector<Genome*>::iterator itGen = itSpecies->getSpecies()->begin(); itGen != itSpecies->getSpecies()->end() && numParents > i; ++itGen, ++i)
		{
			(*itGen)->setEliminate(true);//Mark for elimination
		}
	}
}

void Neat::addToSpecies(Genome* gen)
{
	Species* bestSpecies = nullptr;
	float bestDistance = 9999999;

	for (std::vector<Species>::iterator it = species.begin(); it != species.end() && bestSpecies == nullptr; ++it)
	{
		float dist = distance(*it->getChamp(), *gen);

		if (dist < neatParam.speciationDistance && bestDistance >= dist)
		{
			bestSpecies = &*it;
			bestDistance = dist;
		}
	}

	if (bestSpecies != nullptr)
	{
		bestSpecies->Add(gen);

	}else{

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

	float weightDiff = 0;


	while (it1 != genome1->getConnections()->cend() && it2 != genome2->getConnections()->cend()) 
	{

		if (it1->first == it2->first) 
		{
			//similargene
			similar++;
			
			weightDiff = abs(it1->second.getWeight() - it2->second.getWeight()) * neatParam.weightCoeff;
			
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
	return neatParam.disjointCoeff * disjoint + neatParam.excessCoeff * excess + neatParam.mutDiffCoeff * ((disjoint + excess) / similar) + weightDiff;
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

	if (lastBestScore >= bestScore)
	{
		highestLastChanged++;
	}
	else {
#ifdef DEBUG
		std::cout << "new best score " << bestScore << std::endl;
#endif // DEBUG

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

