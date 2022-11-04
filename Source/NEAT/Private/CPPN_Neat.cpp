// Fill out your copyright notice in the Description page of Project Settings.


#include "CPPN_Neat.h"
#include <algorithm>
#include <chrono>
#include <random>

CPPN_Neat::CPPN_Neat(unsigned int _populationSize, unsigned int _input, unsigned int _output, NeatParameters _neatParam): NeatAlgoGen(_populationSize, _input, _output, _neatParam)
{
	for (int i = 0; i < genomes.size(); i++)
	{
		genomes[i].nodesToConnection.clear();
		genomes[i].connections.clear();

		int extra = input - output;

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

		for (; itInput != inputList.end() && itOutput != outputList.end(); ++itInput, ++itOutput)
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

			int innov;

			if (allConnections.find(std::pair(nodeA, nodeB)) != allConnections.end())
			{
				innov = allConnections[std::pair(nodeA, nodeB)];
			}
			else {
				innov = allConnections.size();
				allConnections[std::pair(nodeA, nodeB)] = innov;
			}
			
			genomes[i].connections[innov] = GeneConnection(innov, nodeA, nodeB);
			genomes[i].connections[innov].weight = (static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 2)) - 1) * neatParam.weightRandomStrength;

			genomes[i].nodesToConnection[std::pair(nodeA, nodeB)] = innov;

		}
	}
}

CPPN_Neat::~CPPN_Neat()
{
	for (int i = 0; i < neatParam.activationFunctions.size(); i++)
	{
		delete neatParam.activationFunctions[i];
	}
}

void CPPN_Neat::mutate(Genome& genome)
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

	if (neatParam.pbMutateActivation > rand() % 1) {
		unsigned int index = rand() % neatParam.activationFunctions.size();
	}
}

float CPPN_Neat::distance(Genome& genomeA, Genome& genomeB)
{
	const std::deque<GeneNode>* nodesA = genomeA.getNodes();
	const std::deque<GeneNode>* nodesB = genomeB.getNodes();

	int count = 0;

	for (int i = 0; i < nodesA->size() && i < nodesB->size(); i++)
	{
		if ((*nodesA)[0].getActivation()->getId() != (*nodesA)[0].getActivation()->getId())
		{
			count++;
		}
	}

	int N = std::min(nodesA->size(), nodesB->size());

	if (N < 20)
	{
		N = 1;
	}

	return NeatAlgoGen::distance(genomeA, genomeB) + neatParam.C4 * count / N;
}