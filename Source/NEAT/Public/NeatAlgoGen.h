// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <vector>
#include "NeuralNetwork.h"
#include "Genome.h"
#include "Activation.h"

typedef struct {
	float pbMutateLink;//Probability of each mutation
	float pbMutateNode;
	float pbWeightShift;
	float pbWeightRandom;
	float pbToggleLink;
	float weightShiftStrength;//Max values for weight shift and new random one 
	float weightRandomStrength;
	float pbMutateActivation;
	std::vector<ActivationFunction> activationFunctions;

} NeatParameters;

/**
 * 
 */
class NEAT_API NeatAlgoGen
{
public:
	NeatAlgoGen();//Just for testing purposes
	NeatAlgoGen(unsigned int _populationSize, unsigned int _input, unsigned int _output, NeatParameters _neatParamters);
	~NeatAlgoGen();

	virtual void mutate(Genome& genome);

	void generateNetworks();

	friend class ANeuralNetworkDisplayHUD;

protected:
	std::vector<NeuralNetwork> networks;
	std::vector<Genome> genomes;
	std::unordered_map<std::pair<unsigned int, unsigned int>, unsigned int> allConnections;//Innovation number starts at 0
	unsigned int populationSize, input, output;
	NeatParameters neatParamters;
};
