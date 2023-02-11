// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CPPN_Neat.h"
#include <deque>
#include <unordered_set>
#include <cstring>
#include <algorithm>

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

	Activation* activationFunction;
	ThresholdFunction thresholdFunction; 
	CppnInputFunction cppnInputFunction; 
	WeightModifierFunction weightModifierFunction;
	WeightModifierFunction inverseWeightModifierFunction;//Only for backprop

	//Optionnal
	std::vector<void*> thresholdVariables;
	std::vector<void*> inputVariables;
	std::vector<void*> weightVariables;

} HyperneatParameters;

/**
 * This implementation supposes that we use the same activation function as for all the substrate
 */
class Hyperneat
{
public:
	Hyperneat(unsigned int _populationSize, const NeatParameters& _neatParam, const HyperneatParameters& _hyperParam, Neat::INIT init = Neat::INIT::ONE);
	Hyperneat(unsigned int _populationSize, const NeatParameters& _neatParam, const HyperneatParameters& _hyperParam, std::vector<Genome>& initPop);

	~Hyperneat();

	inline bool isInit() { return cppns->isInit(); }

	void addInput(const std::vector<float>& node);
	void addOutput(const std::vector<float>& node);
	void addHiddenNode(unsigned int layer, const std::vector<float>& node);

	inline void evolve() { cppns->evolve(); generateNetworks(); };
	inline void setScore(const std::vector < float >& newScores) { cppns->setScore(newScores); }
	inline bool saveHistory() { return cppns->saveHistory(); }
	inline Genome* getGoat() { return cppns->getGoat(); };

	void clear();

	void generateNetworks();
	void generateNetworksThread(int startIndex, int worlkload, std::atomic<bool>* ticket = nullptr);
	void genomeToNetwork(Genome& gen, NeuralNetwork& net);
	virtual void createNetwork(NeuralNetwork& hypernet, NeuralNetwork& net);
	void initNetworks();

	//Only compute backprop over connections of output nodes
	bool backprop(const std::vector<float>& inputs, const std::vector<float>& outputs, float learnRate);
	void applyBackprop();

	inline NeuralNetwork* getNeuralNetwork(int i) { return &networks[i]; };

protected:
	virtual void initNetwork(NeuralNetwork& net);

	/**
	* Connect layer to the previous layer
	*/
	void connectLayer(unsigned int layer, NeuralNetwork& hypernet, NeuralNetwork& net, std::vector<std::vector<float>>::iterator itNode,
		std::vector<std::vector<float>>::iterator itNodeEnd,
		std::vector<std::vector<float>>::iterator beginPreviousLayer, std::vector<std::vector<float>>::iterator endPreviousLayer);

	std::vector<std::vector<float>> inputSubstrate;
	std::vector<std::vector<float>> outputSubstrate;
	std::vector<std::vector<std::vector<float>>> hiddenSubstrates;

	std::vector<NeuralNetwork> networks;

	HyperneatParameters hyperParam;

	CPPN_Neat* cppns;
};

//CPPN Input Functions
std::vector<float> basicCppnInput(std::vector<void*> variables, std::vector<float> p1, std::vector<float> p2);
std::vector<float> biasCppnInput(std::vector<void*> variables, std::vector<float> p1, std::vector<float> p2);
std::vector<float> sqrDistCppnInput(std::vector<void*> variables, std::vector<float> p1, std::vector<float> p2);
std::vector<float> invDistCppnInput(std::vector<void*> variables, std::vector<float> p1, std::vector<float> p2);
std::vector<float> deltaDistCppnInput(std::vector<void*> variables, std::vector<float> p1, std::vector<float> p2);

//Threshold functions
inline bool fixedThreshold(std::vector<void*> variables, std::vector<float> values, const std::vector<float>& p1, const std::vector<float>& p2)
{
	return (*(float*)variables[0] < abs(values[0]));
}

inline bool noThreshold(std::vector<void*> variables, std::vector<float> values, const std::vector<float>& p1, const std::vector<float>& p2)
{
	return true;
}

inline bool leoThreshold(std::vector<void*> variables, std::vector<float> values, const std::vector<float>& p1, const std::vector<float>& p2)
{
	return (0 < abs(values[1]));
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

inline float inverseProportionnalWeight(std::vector<void*> variables, float weight, const std::vector<float>& p1, const std::vector<float>& p2)
{
	return weight  / *(float*)variables[0];
}

inline float substractWeight(std::vector<void*> variables, float weight, const std::vector<float>& p1, const std::vector<float>& p2)
{
	float tmp = (abs(weight) - abs(*(float*)variables[0]));
	return (tmp > 0 ? tmp * (signbit(weight) == true ? 1 : -1) : 0);
}

inline float addWeight(std::vector<void*> variables, float weight, const std::vector<float>& p1, const std::vector<float>& p2)
{
	return (weight == 0 ? 0 : weight + (*(float*)variables[0]) * (signbit(weight) == true ? 1 : -1));
}

inline float absWeight(std::vector<void*> variables, float weight, const std::vector<float>& p1, const std::vector<float>& p2)
{
	return abs(weight);
}