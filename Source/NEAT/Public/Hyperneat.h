// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CPPN_Neat.h"
#include <deque>
#include <unordered_set>

typedef bool (*ThresholdFunction) (std::vector<void*> variables, std::vector<float> values, const std::vector<float>& p1, const std::vector<float>& p2);

//Returns the values to pass in input of the CPPN
typedef std::vector<float> (*CppnInputFunction) (std::vector<void*> variables, std::vector<float> p1, std::vector<float> p2);

//Returns the weight to apply to the connection
typedef float (*WeightModifierFunction) (std::vector<void*> variables, float weight, const std::vector<float>& p1, const std::vector<float>& p2);

//Hyperneat configuration
typedef struct {
	unsigned int nDimensions;

	unsigned int cppnInput;
	unsigned int cppnOutput;

	ActivationFunction activationFunction;
	ThresholdFunction thresholdFunction; 
	CppnInputFunction cppnInputFunction; 
	WeightModifierFunction weightModifierFunction;

	//Optionnal
	std::vector<void*> thresholdVariables;
	std::vector<void*> inputVariables;
	std::vector<void*> weightVariables;

} HyperneatParameters;

struct HyperNodeHash {
public:
	size_t operator()(const std::vector<float>& node) const {
		if (node.size() == 0) return 0;

		std::size_t value = 0;
		std::memcpy(&value, &node[0], std::min(sizeof(float), sizeof(std::size_t)));

		for (int i = 1; i < node.size(); i++)
		{
			std::size_t tmp = 0;
			std::memcpy(&value, &node[i], std::min(sizeof(float), sizeof(std::size_t)));

			value = value ^ (tmp << i);
		}

		return value; // or use boost::hash_combine
	}
};

/**
 * This implementation supposes that we use the same activation function as for all the substrate
 */
class NEAT_API Hyperneat
{
public:
	Hyperneat(unsigned int _populationSize, NeatParameters _neatParamters, HyperneatParameters _hyperParam);

	~Hyperneat();

	void addInput(std::vector<float> node);
	void addOutput(std::vector<float> node);
	void addHiddenNode(unsigned int layer, std::vector<float> node);

	void clear();

	virtual void generateNetworks();

	inline NeuralNetwork* getNeuralNetwork(int i) { return &networks[i]; };

protected:
	/**
	* Add nodes of a layer to the neuralnetwork
	* and connect it to the previous layer
	*/
	void addLayerAndConnect(unsigned int layer, unsigned int networkIndex, std::unordered_set<std::vector<float>, HyperNodeHash>::iterator itNode, 
		std::unordered_set<std::vector<float>, HyperNodeHash>::iterator itNodeEnd,
		std::unordered_set<std::vector<float>, HyperNodeHash>::iterator beginPreviousLayer, std::unordered_set<std::vector<float>, HyperNodeHash>::iterator endPreviousLayer,
		bool outputLayer);

	std::unordered_set<std::vector<float>, HyperNodeHash> inputSubstrate;
	std::unordered_set<std::vector<float>, HyperNodeHash> outputSubstrate;
	std::deque<std::unordered_set<std::vector<float>, HyperNodeHash>> hiddenSubstrates;

	std::vector<NeuralNetwork> networks;

	HyperneatParameters hyperParam;

	CPPN_Neat cppns;
};

//CPPN Input Functions
std::vector<float> basicCppnInput(std::vector<void*> variables, std::vector<float> p1, std::vector<float> p2);
std::vector<float> sqrDistCppnInput(std::vector<void*> variables, std::vector<float> p1, std::vector<float> p2);
std::vector<float> deltaDistCppnInput(std::vector<void*> variables, std::vector<float> p1, std::vector<float> p2);

//Threshold functions
inline bool fixedThreshold(std::vector<void*> variables, std::vector<float> values, const std::vector<float>& p1, const std::vector<float>& p2)
{
	return (*(float*)variables[0] < abs(values[0]));
}


inline bool leoThreshold(std::vector<void*> variables, std::vector<float> values, const std::vector<float>& p1, const std::vector<float>& p2)
{
	return (*(float*)variables[0] < abs(values[1]));
}

//Weight modifiers functions
inline float noChangeWeight(std::vector<void*> variables, float weight, const std::vector<float>& p1, const std::vector<float>& p2)
{
	return weight;
}

inline float proportionnalWeight (std::vector<void*> variables, float weight, const std::vector<float>& p1, const std::vector<float>& p2)
{
	return *(float*)variables[0] * weight;
}