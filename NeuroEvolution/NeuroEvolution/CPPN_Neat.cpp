// Fill out your copyright notice in the Description page of Project Settings.


#include "CPPN_Neat.h"
#include <algorithm>


CPPN_Neat::CPPN_Neat(unsigned int _populationSize, unsigned int _input, unsigned int _output, const NeatParameters& _neatParam): Neat(_populationSize, _input, _output, _neatParam, INIT::NONE)
{
	for (int i = 0; i < populationSize; i++)
	{
		genomes[i] = Genome(_input, output, neatParam.activationFunctions, true);
		fullConnectInit(genomes[i]);
	}

	generateNetworks();
}

CPPN_Neat::~CPPN_Neat()
{
	//delete[] genomes;
}

void CPPN_Neat::mutate(Genome& genome, std::mutex* lock)
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

		if (neatParam.pbMutateActivation > randFloat())
		{
			unsigned int index = randInt(0, neatParam.activationFunctions.size() - 1);
		}
	//}

}

float CPPN_Neat::distance(Genome& genomeA, Genome& genomeB)
{
	const std::vector<GeneNode>* nodesA = genomeA.getNodes();
	const std::vector<GeneNode>* nodesB = genomeB.getNodes();

	int count = 0;

	for (int i = 0; i < nodesA->size() && i < nodesB->size(); i++)
	{
		if ((*nodesA)[0].getActivation()->getId() != (*nodesA)[0].getActivation()->getId())
		{
			count++;
		}
	}

	return Neat::distance(genomeA, genomeB) + neatParam.activationDiffCoeff * count;
}