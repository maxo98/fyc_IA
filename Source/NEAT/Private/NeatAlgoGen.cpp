// Fill out your copyright notice in the Description page of Project Settings.


#include "NeatAlgoGen.h"

NeatAlgoGen::NeatAlgoGen(int _populationSize, int _input, int _output, float _pbMutateLink, float _pbMutateNode, float _pbWeightShift, float _pbWeightRandom, float _pbToggleLink, float _weightShiftStrength, float _weightRandomStrength)
{
	populationSize = _populationSize;
	input = _input;
	output = _output;

	pbMutateLink = _pbMutateLink;
	pbMutateNode = _pbMutateNode;
	pbWeightShift = _pbWeightShift;
	pbWeightRandom = _pbWeightRandom;
	pbToggleLink = _pbToggleLink;

	weightShiftStrength = _weightShiftStrength;
	weightRandomStrength = _weightRandomStrength;

	for (int i = 0; i < populationSize; i++)
	{
		genomes.push_back(Genome(input, output));
		Genome* genome = &genomes.back();
	}
}

NeatAlgoGen::~NeatAlgoGen()
{
}

void NeatAlgoGen::mutate(Genome& genome)
{
	if (pbMutateLink > rand() % 1) {
		genome.mutateLink(allConnections);
	}if (pbMutateNode > rand() % 1) {
		genome.mutateNode(allConnections);
	}if (pbWeightShift > rand() % 1) {
		genome.mutateWeightShift(pbWeightShift);
	}if (pbWeightRandom > rand() % 1) {
		genome.mutateWeightRandom(pbWeightShift);
	}if (pbToggleLink > rand() % 1) {
		genome.mutateLinkToggle();
	}
}

