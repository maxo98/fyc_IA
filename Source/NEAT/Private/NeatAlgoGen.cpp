// Fill out your copyright notice in the Description page of Project Settings.


#include "NeatAlgoGen.h"
#include <algorithm>

NeatAlgoGen::NeatAlgoGen(unsigned int _populationSize, unsigned int _input, unsigned int _output, NeatParameters _neatParam)
{
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
}

NeatAlgoGen::NeatAlgoGen()
{
	Activation* test = new sigmoidActivation();

	if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 60.f, FColor::Red, FString::Printf(TEXT("%f"), test->activate(1)));

	delete test;

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
		mutate(genomes[i]);
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
			if (distance(*(*it)[0], genomes[cpt]) > neatParam.speciationDistance)
			{
				found = true;
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
	std::list<unsigned int>::iterator itScore = speciesScore.begin();

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
	std::list<unsigned int>::iterator itScore = speciesScore.begin();

	for (; itSpe != species.end(); ++itSpe, ++itScore)
	{
		if (itSpe->size() <= 1)
		{
			if (itSpe->size() == 1) (*itSpe)[0]->setInSpecies(false);

			itSpe = std::prev(species.erase(itSpe));
			itScore = std::prev(speciesScore.erase(itScore));
		}
	}
}

void NeatAlgoGen::reproduce()
{
	float totalScore = 0;
	std::list<unsigned int> inverseScores;

	for (std::list<unsigned int>::iterator itScore = speciesScore.begin(); itScore != speciesScore.end(); ++itScore)
	{
		totalScore += *itScore;
	}

	if (neatParam.bestHigh == false)
	{
		float tmp = 0;
		
		for (std::list<unsigned int>::iterator itScore = speciesScore.begin(); itScore != speciesScore.end(); ++itScore)
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
			float r = rand() % (int)totalScore;
			float score = 0;

			std::list<unsigned int>::iterator itScore;

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
					genomes[cpt].setInSpecies(true);
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
		pick(currentSpecies, parentA, parentB);
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

void NeatAlgoGen::pick(std::deque<Genome*>* currentSpecies, Genome* parentA, Genome* parentB)
{
	float totalScore = 0;
	std::list<unsigned int> listScores;
	std::list<unsigned int> inverseScores;

	for (std::deque<Genome*>::iterator it = currentSpecies->begin(); it != currentSpecies->end(); ++it)
	{
		totalScore += (*it)->getScore();
		listScores.push_back((*it)->getScore());
	}

	if (neatParam.bestHigh == false)
	{
		float tmp = 0;

		for (std::list<unsigned int>::iterator itScore = listScores.begin(); itScore != listScores.end(); ++itScore)
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
			float rA = rand() % (int)totalScore;
			float rB = rand() % (int)totalScore;
			float score = 0;

			std::list<unsigned int>::iterator itScore;

			if (neatParam.bestHigh == true)
			{
				itScore = listScores.begin();
			}
			else {
				itScore = inverseScores.begin();
			}

			for (std::deque<Genome*>::iterator itGen = currentSpecies->begin(); itGen != currentSpecies->end() && (parentA == nullptr || parentB == nullptr);
				++itGen, ++itScore)
			{
				score += *itScore;

				if (score >= rA)
				{
					parentA = *itGen;
				}else if (score >= rB)
				{
					parentB = *itGen;
				}
			}
		}
	}

	if (parentB == nullptr)
	{
		parentB = (*currentSpecies)[currentSpecies->size()];
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

	int disjoint = 0;
	int excess = 0;
	double weight_diff = 0;
	int similar = 0;


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

	weight_diff /= std::max(1, similar);
	excess = genome1->getConnections()->size() - count;

	double N = std::max(genome1->getConnections()->size(), genome2->getConnections()->size());
	if (N < 20) 
	{
		N = 1;
	}

	return neatParam.C1 * disjoint / N + neatParam.C2 * excess / N + neatParam.C3 * weight_diff;
}