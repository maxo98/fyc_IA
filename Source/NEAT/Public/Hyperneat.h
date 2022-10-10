// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CPPN_Neat.h"

typedef bool (*ThresholdFunction) (std::vector<float> values);

/**
 * 
 * This implementation supposes that we use the same activation function as for all the substrate
 */
template<unsigned int S>
class NEAT_API Hyperneat
{
public:
	Hyperneat(unsigned int _populationSize, NeatParameters _neatParamters, ActivationFunction _activationFunction, ThresholdFunction _thresholdFunction);

	~Hyperneat();

	inline void addInput(std::array<float, S> node) { inputSubstrate.push_back(node); };
	inline void addOutput(std::array<float, S> node) { inputSubstrate.push_back(node); };
	inline void addHiddenNode(unsigned int layer, std::array<float, S> node) { inputSubstrate[layer].push_back(node); };

	void clear();

	virtual void generateNetworks();

protected:
	std::list<std::array<float, S>> inputSubstrate;
	std::list<std::array<float, S>> outputSubstrate;
	std::vector<std::list<std::array<float, S>>> hiddenSubstrates;

	std::vector<NeuralNetwork> networks;

	ActivationFunction activationFunction;
	ThresholdFunction thresholdFunction;

	CPPN_Neat cppns;
};


//Threshold functions

//To test if it really works
template<unsigned int T>
bool fixedThreshold(std::vector<float> values)
{
	return (values[0] > T);
}