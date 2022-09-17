// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <vector>
#include "NeuralNetwork.h"
#include "Genome.h"

/**
 * 
 */
class NEAT_API NeatAlgoGen
{
public:
	NeatAlgoGen();//Just for testing purposes
	NeatAlgoGen(int _populationSize, int _input, int _output, float _pbMutateLink, float _pbMutateNode, float _pbWeightShift, float _pbWeightRandom, float _pbToggleLink, float _weightShiftStrength, float _weightRandomStrength);
	~NeatAlgoGen();

	void mutate(Genome& genome);

	void generateNetworks();

	friend class ANeuralNetworkDisplayHUD;

private:
	std::vector<NeuralNetwork> networks;
	std::vector<Genome> genomes;
	std::unordered_map<std::pair<int, int>, int> allConnections;//Innovation number starts at 0
	int populationSize, input, output;
	float pbMutateLink, pbMutateNode, pbWeightShift, pbWeightRandom, pbToggleLink;//Probability of each mutation
	float weightShiftStrength, weightRandomStrength;//Max values for weight shift and new random one 
};
