// Fill out your copyright notice in the Description page of Project Settings.


#include "CPPN_Neat.h"

CPPN_Neat::CPPN_Neat()
{
}

CPPN_Neat::~CPPN_Neat()
{
}

void CPPN_Neat::mutate(Genome& genome)
{
	if (neatParamters.activationFunctions.size() == 0)
		return;

	if (neatParamters.pbMutateLink > rand() % 1) {
		genome.mutateLink(allConnections);
	}

	if (neatParamters.pbMutateNode > rand() % 1) {

		int index = rand() % neatParamters.activationFunctions.size();
		genome.mutateNode(allConnections, neatParamters.activationFunctions[index]);
	}

	if (neatParamters.pbWeightShift > rand() % 1) {
		genome.mutateWeightShift(neatParamters.pbWeightShift);
	}

	if (neatParamters.pbWeightRandom > rand() % 1) {
		genome.mutateWeightRandom(neatParamters.pbWeightRandom);
	}

	if (neatParamters.pbToggleLink > rand() % 1) {
		genome.mutateLinkToggle();
	}

	if (neatParamters.pbMutateActivation > rand() % 1) {
		int index = rand() % neatParamters.activationFunctions.size();
	}
}