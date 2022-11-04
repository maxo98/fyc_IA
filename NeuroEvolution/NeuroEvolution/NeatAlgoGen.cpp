// Fill out your copyright notice in the Description page of Project Settings.


#include "NeatAlgoGen.h"
#include <algorithm>
#include <fstream>

NeatAlgoGen::NeatAlgoGen(unsigned int _populationSize, unsigned int _input, unsigned int _output, NeatParameters _neatParam)
{
	if (_neatParam.activationFunctions.size() == 0) return;//Avoid Unreal crash, should probably put a debug message

	populationSize = _populationSize;
	input = _input;
	output = _output;

	neatParam = _neatParam;

	networks.resize(populationSize);
	scores.resize(populationSize);
	genomes.reserve(populationSize);


	for (unsigned int i = 0; i < populationSize; i++)
	{
		genomes.push_back(Genome(input, output, neatParam.activationFunctions, &scores[i]));
		Genome* genome = &genomes.back();
		genome->mutateLink(allConnections);//Minimum structure
		genome->mutateWeightRandom(neatParam.pbWeightRandom);
	}

	generateNetworks();
}

NeatAlgoGen::NeatAlgoGen()
{
	//Outdated
	/*neatParam.activationFunctions.push_back(sigmoidActivation());

	populationSize = 3;
	input = 3;
	output = 3;

	networks.resize(populationSize);
	genomes.reserve(populationSize);


	for (unsigned int i = 0; i < populationSize; i++)
	{
		genomes.push_back(Genome(input, output, neatParam.activationFunctions));
		Genome* genome = &genomes.back();
		genome->mutateLink(allConnections);
		genome->mutateNode(allConnections, neatParam.activationFunctions[0]);
		genome->mutateLink(allConnections);
		genome->mutateNode(allConnections, neatParam.activationFunctions[0]);
		genome->mutateLink(allConnections);
		genome->mutateNode(allConnections, neatParam.activationFunctions[0]);
		genome->mutateLink(allConnections);
		genome->mutateNode(allConnections, neatParam.activationFunctions[0]);
		genome->mutateLink(allConnections);
		genome->mutateNode(allConnections, neatParam.activationFunctions[0]);
		genome->mutateWeightRandom(2);
		genome->mutateWeightShift(0.5);
		genome->mutateWeightRandom(2);
		genome->mutateWeightShift(0.5);
		genome->mutateWeightRandom(2);
		genome->mutateWeightShift(0.5);
		genome->mutateLinkToggle();
		genome->mutateLinkToggle();
	}

	genomes[2].crossover(genomes[0], genomes[1]);

	generateNetworks();

	std::vector<float> inputNet;

	inputNet.push_back(0);
	inputNet.push_back(0);
	inputNet.push_back(0);

	std::vector<float> outputNet;

	outputNet.push_back(0);
	outputNet.push_back(0);
	outputNet.push_back(0);

	networks[0].compute(inputNet, outputNet);*/
}

NeatAlgoGen::~NeatAlgoGen()
{
	for (int i = 0; i < neatParam.activationFunctions.size(); i++)
	{
		delete neatParam.activationFunctions[i];
	}
}

void NeatAlgoGen::mutate(Genome& genome)
{
	if (neatParam.activationFunctions.size() == 0)
		return;

	if (neatParam.pbMutateLink > rand() % 1) {
		genome.mutateLink(allConnections);
	}
	
	if (neatParam.pbMutateNode > rand() % 1) {

		unsigned int index = rand() % neatParam.activationFunctions.size();
		genome.mutateNode(allConnections, neatParam.activationFunctions[index]);
	}
	
	if (neatParam.pbWeightShift > rand() % 1) {
		genome.mutateWeightShift(neatParam.pbWeightShift);
	}
	
	if (neatParam.pbWeightRandom > rand() % 1) {
		genome.mutateWeightRandom(neatParam.pbWeightRandom);
	}
	
	if (neatParam.pbToggleLink > rand() % 1) {
		genome.mutateLinkToggle();
	}
}

void NeatAlgoGen::generateNetworks()
{

	for (unsigned int cpt = 0; cpt < genomes.size(); cpt++)
	{
		networks[cpt].clear();

		std::deque<GeneNode>* nodes = genomes[cpt].getNodes();
		std::vector<std::pair<unsigned int, unsigned int>> nodePosition;//Stores postion of the nodes in the network
		nodePosition.reserve(nodes->size());

		//Add the nodes to the layer
		for (std::deque<GeneNode>::iterator node = nodes->begin(); node != nodes->end(); ++node)
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
	genSpecies();
	kill();
	removeExtinctSpecies();
	reproduce();
	
	for (int i = 0; i < genomes.size(); i++)
	{
		if (genomes[i].getInSpecies() == true)
		{
			mutate(genomes[i]);
		}
	}

	generateNetworks();
}

void NeatAlgoGen::genSpecies()
{
	for (unsigned int cpt = 0; cpt < genomes.size(); cpt++)
	{
		genomes[cpt].setInSpecies(false);
	}

	for (std::list<std::deque<Genome*>>::iterator it = species.begin(); it != species.end(); ++it)
	{
		Genome* r = (*it)[rand() % it->size()];
		it->clear();
		it->push_back(r);
		r->setInSpecies(true);
	}

	for (unsigned int cpt = 0; cpt < genomes.size(); cpt++) 
	{
		if (genomes[cpt].getInSpecies() == true) continue;


		bool found = false;
		for (std::list<std::deque<Genome*>>::iterator it = species.begin(); it != species.end() && found == false; ++it)
		{
			if (distance(*(*it)[0], genomes[cpt]) < neatParam.speciationDistance)
			{
				found = true;
				it->push_back(&genomes[cpt]);
			}
		}

		if (found == false) 
		{
			species.push_back(std::deque<Genome*>());
			speciesScore.push_back(0);
			species.back().push_back(&genomes[cpt]);
		}

		genomes[cpt].setInSpecies(true);
	}

	std::list<std::deque<Genome*>>::iterator itSpe = species.begin();
	std::list<float>::iterator itScore = speciesScore.begin();

	for (; itSpe != species.end(); ++itSpe, ++itScore)
	{
		*itScore = 0;

		for (unsigned int cpt = 0; cpt < itSpe->size(); cpt++)
		{
			*itScore += (*itSpe)[cpt]->getScore();
		}

		*itScore /= itSpe->size();
	}
}

void NeatAlgoGen::kill()
{
	for (std::list<std::deque<Genome*>>::iterator it = species.begin(); it != species.end(); ++it)
	{
		float percentage = 1 - neatParam.survivors;

		if (neatParam.bestHigh == true)
		{
			std::sort(it->begin(), it->end(), inSpeciesSortWeakOrder);
		}
		else {
			std::sort(it->begin(), it->end(), inSpeciesSortStrongOrder);
		}


		float amount = percentage * it->size();
		for (int i = 0; i < amount; i++) {
			(*it)[i]->setInSpecies(false);
			it->pop_front();
		}
	}
}

void NeatAlgoGen::removeExtinctSpecies() 
{
	std::list<std::deque<Genome*>>::iterator itSpe = species.begin();
	std::list<float>::iterator itScore = speciesScore.begin();

	while(itSpe != species.end())
	{
		if (itSpe->size() <= 1)
		{
			if (itSpe->size() == 1) (*itSpe)[0]->setInSpecies(false);

			itSpe = species.erase(itSpe);
			itScore = speciesScore.erase(itScore);
		}
		else {
			++itSpe;
			++itScore;
		}
	}

	std::cout << species.size() << std::endl;
}

void NeatAlgoGen::reproduce()
{
	float totalScore = 0;
	std::list<float> inverseScores;

	for (std::list<float>::iterator itScore = speciesScore.begin(); itScore != speciesScore.end(); ++itScore)
	{
		totalScore += *itScore;
	}

	if (neatParam.bestHigh == false)
	{
		float tmp = 0;
		
		for (std::list<float>::iterator itScore = speciesScore.begin(); itScore != speciesScore.end(); ++itScore)
		{
			inverseScores.push_back(totalScore - *itScore);
			tmp += (totalScore - *itScore);
		}

		totalScore = tmp;
	}

	for (unsigned int cpt = 0; cpt < genomes.size(); cpt++)
	{
		if (genomes[cpt].getInSpecies() == false)
		{
			float r = 0;
			if((int)(totalScore * neatParam.scoreMultiplier) != 0) r = rand() % (int)(totalScore * neatParam.scoreMultiplier);

			float score = 0;

			std::list<float>::iterator itScore;

			if (neatParam.bestHigh == true)
			{
				itScore = speciesScore.begin();
			}
			else {
				itScore = inverseScores.begin();
			}

			for (std::list<std::deque<Genome*>>::iterator itSpe = species.begin(); itSpe != species.end(); ++itSpe, ++itScore)
			{
				score += *itScore;

				if (score >= r)
				{
					breed(&genomes[cpt], &*itSpe);
					//genomes[cpt].setInSpecies(true); technically inside a species but maybe don't mutate newborns
					itSpe->push_back(&genomes[cpt]);
				}
			}
		}
	}
}

void NeatAlgoGen::breed(Genome* child, std::deque<Genome*>* currentSpecies)
{
	Genome* parentA = nullptr, * parentB = nullptr;;

	if (currentSpecies->size() > 2)
	{
		int a = -1, b = -1;
		pick(currentSpecies, a, b);

		parentA = (*currentSpecies)[a];
		parentB = (*currentSpecies)[b];
	}
	else {
		parentA = (*currentSpecies)[0];
		parentB = (*currentSpecies)[1];
	}

	if ((neatParam.bestHigh == true && parentA->getScore() < parentB->getScore()) || (neatParam.bestHigh == false && parentA->getScore() > parentB->getScore()))
	{
		Genome* tmp = parentA;
		parentA = parentB;
		parentB = tmp;
	}

	child->crossover(*parentA, *parentB);
}

void NeatAlgoGen::pick(std::deque<Genome*>* currentSpecies, int& parentA, int& parentB)
{
	float totalScore = 0;
	std::list<float> listScores;
	std::list<float> inverseScores;

	for (std::deque<Genome*>::iterator it = currentSpecies->begin(); it != currentSpecies->end(); ++it)
	{
		totalScore += (*it)->getScore();
		listScores.push_back((*it)->getScore());
	}

	if (neatParam.bestHigh == false)
	{
		float tmp = 0;

		for (std::list<float>::iterator itScore = listScores.begin(); itScore != listScores.end(); ++itScore)
		{
			inverseScores.push_back(totalScore - *itScore);
			tmp += (totalScore - *itScore);
		}

		totalScore = tmp;
	}


	for (std::deque<Genome*>::iterator it = currentSpecies->begin(); it != currentSpecies->end(); ++it)
	{
		if ((*it)->getInSpecies() == false)
		{
			float rA = rand() % (int)(totalScore * neatParam.scoreMultiplier);
			float rB = rand() % (int)(totalScore * neatParam.scoreMultiplier);
			float score = 0;

			std::list<float>::iterator itScore;

			if (neatParam.bestHigh == true)
			{
				itScore = listScores.begin();
			}
			else {
				itScore = inverseScores.begin();
			}

			int i = 0;

			for (std::deque<Genome*>::iterator itGen = currentSpecies->begin(); itGen != currentSpecies->end() && (parentA == -1 || parentB == -1);
				++itGen, ++itScore, ++i)
			{
				score += *itScore;

				if (score >= rA && parentA == -1)
				{
					parentA = i;
				}else if (score >= rB && parentB == -1)
				{
					parentB = i;
				}
			}
		}
	}

	if (parentA == -1)
	{
		parentA = currentSpecies->size() - 1;
	}

	if (parentB == -1)
	{
		parentB = currentSpecies->size()-2;
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
	double weight_diff = 0;
	float similar = 0;


	while (it1 != genome1->getConnections()->cend() && it2 != genome2->getConnections()->cend()) 
	{

		if (it1->first == it2->first) 
		{
			//similargene
			similar++;
			weight_diff += fabs(it1->second.getWeight() - it2->second.getWeight());
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

	weight_diff /= std::max(1, (int)similar);
	excess = genome1->getConnections()->size() - count;

	float N = std::max(genome1->getConnections()->size(), genome2->getConnections()->size());
	if (N < 20) 
	{
		N = 1;
	}

	return neatParam.C1 * disjoint / N + neatParam.C2 * excess / N + neatParam.C3 * weight_diff;
}

void NeatAlgoGen::setScore(std::vector < float > newScores)
{
	if (newScores.size() != scores.size())
	{
		std::cout << "New score size different from current score size, aborting." << std::endl;

		return;
	}

	unsigned int bestIndex = 0;
	float bestScore = newScores[0];

	for (int i = 1; i < newScores.size(); i++)
	{
		scores[i] = newScores[i];

		if ((neatParam.bestHigh == true && newScores[i] > bestScore) || (neatParam.bestHigh == false && newScores[i] < bestScore))
		{
			bestScore = newScores[i];
			bestIndex = i;
		}
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

	for (std::list<float>::iterator it = history.begin(); it != history.end(); ++it)
	{
		std::string str = std::to_string(*it);
		int pos = str.find(".");
		
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

