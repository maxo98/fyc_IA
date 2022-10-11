// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CPPN_Neat.h"
#include <deque>

typedef bool (*ThresholdFunction) (std::vector<float> constants, std::vector<float> values, std::vector<float> p1, std::vector<float> p2);

//Returns the values to pass in input of the CPPN
typedef std::deque<float> (*CppnInputFunction) (std::vector<float> constants, std::vector<float> p1, std::vector<float> p2);

//Returns the weight to apply to the connection
typedef float (*WeightModifierFunction) (std::vector<float> constants, float weight, std::vector<float> p1, std::vector<float> p2);

/**
 * This implementation supposes that we use the same activation function as for all the substrate
 * S: geometric dimension
 */
template<unsigned int S>
class NEAT_API Hyperneat
{
public:
	Hyperneat(unsigned int _populationSize, NeatParameters _neatParamters, int _cppnInput, int _cppnOutput,
		ActivationFunction _activationFunction, ThresholdFunction _thresholdFunction, CppnInputFunction _cppnInputFunction, WeightModifierFunction _weightModifierFunction,
		std::vector<float> _thresholdConstants = std::vector<float>(), std::vector<float> _inputConstants = std::vector<float>(), std::vector<float> _weightConstants = std::vector<float>());

	~Hyperneat();

	inline void addInput(std::array<float, S> node) { inputSubstrate.push_back(node); };
	inline void addOutput(std::array<float, S> node) { inputSubstrate.push_back(node); };
	inline void addHiddenNode(unsigned int layer, std::array<float, S> node) { inputSubstrate[layer].push_back(node); };

	void clear();

	virtual void generateNetworks();

protected:
	std::list<std::array<float, S>> inputSubstrate;
	std::list<std::array<float, S>> outputSubstrate;
	std::list<std::list<std::array<float, S>>> hiddenSubstrates;

	std::vector<NeuralNetwork> networks;

	ActivationFunction activationFunction;
	ThresholdFunction thresholdFunction;
	CppnInputFunction cppnInputFunction;
	WeightModifierFunction weightModifierFunction;

	CPPN_Neat cppns;

	int cppnInput, cppnOutput;
	std::vector<float> thresholdConstants, inputConstants, weightConstants;
};

//CPPN Input Functions
std::deque<float> basicCppnInput(std::vector<float> constants, std::vector<float> p1, std::vector<float> p2);
std::deque<float> sqrDistCppnInput(std::vector<float> constants, std::vector<float> p1, std::vector<float> p2);
std::deque<float> deltaDistCppnInput(std::vector<float> constants, std::vector<float> p1, std::vector<float> p2);

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