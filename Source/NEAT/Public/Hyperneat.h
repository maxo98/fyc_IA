// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CPPN_Neat.h"
#include <deque>

typedef bool (*ThresholdFunction) (std::vector<float> constants, std::vector<float> values, std::vector<float> p1, std::vector<float> p2);

//Returns the values to pass in input of the CPPN
typedef std::vector<float> (*CppnInputFunction) (std::vector<float> constants, std::vector<float> p1, std::vector<float> p2);

//Returns the weight to apply to the connection
typedef float (*WeightModifierFunction) (std::vector<float> constants, float weight, std::vector<float> p1, std::vector<float> p2);

/**
 * This implementation supposes that we use the same activation function as for all the substrate
 */
class NEAT_API Hyperneat
{
public:
	Hyperneat(unsigned int _populationSize, unsigned int _nDimensions, NeatParameters _neatParamters, unsigned int _cppnInput, unsigned int _cppnOutput,
		ActivationFunction _activationFunction, ThresholdFunction _thresholdFunction, CppnInputFunction _cppnInputFunction, WeightModifierFunction _weightModifierFunction,
		std::vector<float> _thresholdConstants = std::vector<float>(), std::vector<float> _inputConstants = std::vector<float>(), std::vector<float> _weightConstants = std::vector<float>());

	~Hyperneat();

	void addInput(std::vector<float> node);
	void addOutput(std::vector<float> node);
	void addHiddenNode(unsigned int layer, std::vector<float> node);

	void clear();

	virtual void generateNetworks();

	inline NeuralNetwork* getNeuralNetwork(int i) { return &networks[i]; };

protected:
	void addLayerAndConnect(unsigned int layer, unsigned int networkIndex, std::list<std::vector<float>>::iterator itNode, std::list<std::vector<float>>::iterator itNodeEnd,
		std::list<std::vector<float>>::iterator beginPreviousLayer, std::list<std::vector<float>>::iterator endPreviousLayer);

	std::list<std::vector<float>> inputSubstrate;
	std::list<std::vector<float>> outputSubstrate;
	std::deque<std::list<std::vector<float>>> hiddenSubstrates;

	std::vector<NeuralNetwork> networks;

	ActivationFunction activationFunction;
	ThresholdFunction thresholdFunction;
	CppnInputFunction cppnInputFunction;
	WeightModifierFunction weightModifierFunction;

	CPPN_Neat cppns;

	unsigned int cppnInput, cppnOutput, nDimensions;
	std::vector<float> thresholdConstants, inputConstants, weightConstants;
};

//CPPN Input Functions
std::vector<float> basicCppnInput(std::vector<float> constants, std::vector<float> p1, std::vector<float> p2);
std::vector<float> sqrDistCppnInput(std::vector<float> constants, std::vector<float> p1, std::vector<float> p2);
std::vector<float> deltaDistCppnInput(std::vector<float> constants, std::vector<float> p1, std::vector<float> p2);

//Threshold functions
inline bool fixedThreshold(std::vector<float> constants, std::vector<float> values, std::vector<float> p1, std::vector<float> p2)
{
	return (constants[0] < abs(values[0]));
}


inline bool leoThreshold(std::vector<float> constants, std::vector<float> values, std::vector<float> p1, std::vector<float> p2)
{
	return (constants[0] < abs(values[1]));
}

//Weight modifiers functions
inline float noChangeWeight(std::vector<float> constants, float weight, std::vector<float> p1, std::vector<float> p2)
{
	return weight;
}

inline float proportionnalWeight (std::vector<float> constants, float weight, std::vector<float> p1, std::vector<float> p2)
{
	return constants[0] * weight;
}