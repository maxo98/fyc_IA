// Fill out your copyright notice in the Description page of Project Settings.


#include "Hyperneat.h"

template<unsigned int S>
Hyperneat<S>::Hyperneat(unsigned int _populationSize, NeatParameters _neatParamters, int _cppnInput, int _cppnOutput,
	ActivationFunction _activationFunction, ThresholdFunction _thresholdFunction, CppnInputFunction _cppnInputFunction, WeightModifierFunction _weightModifierFunction,
	std::vector<float> _thresholdConstants, std::vector<float> _inputConstants, std::vector<float> _weightConstants)
{
	activationFunction = _activationFunction;
	thresholdFunction = _thresholdFunction;
	cppnInputFunction = _cppnInputFunction;
	weightModifierFunction = _weightModifierFunction;

	thresholdConstants = _thresholdConstants;
	inputConstants = _inputConstants;
	weightConstants = _weightConstants;

	cppnInput = _cppnInput;
	cppnOutput = _cppnOutput;

	cppns = CPPN_Neat(_populationSize, cppnInput, cppnOutput, _neatParamters);

	networks.clear();
}

template<unsigned int S>
Hyperneat<S>::~Hyperneat()
{
}

template<unsigned int S>
void Hyperneat<S>::clear()
{
	inputSubstrate.clear();
	outputSubstrate.clear();
	hiddenSubstrate.clear();
}

template<unsigned int S>
void Hyperneat<S>::generateNetworks()
{

}

std::deque<float> basicCppnInput(std::vector<float> constants, std::vector<float> p1, std::vector<float> p2)
{
	std::deque<float> input;
	input.insert(input.end(), p1.begin(), p1.end());
	input.insert(input.end(), p2.begin(), p2.end());
	return input;
}

std::deque<float> sqrDistCppnInput(std::vector<float> constants, std::vector<float> p1, std::vector<float> p2)
{
	std::deque<float> input;
	input.insert(input.end(), p1.begin(), p1.end());
	input.insert(input.end(), p2.begin(), p2.end());
	input.push_back(0);

	for (int i = 0; i < p1.size(); i++)
	{
		input[input.size() - 1] += (p2[i] - p1[i]) * (p2[i] - p1[i]);
	}

	return input;
}

std::deque<float> deltaDistCppnInput(std::vector<float> constants, std::vector<float> p1, std::vector<float> p2)
{
	std::deque<float> input;
	input.insert(input.end(), p1.begin(), p1.end());
	input.insert(input.end(), p2.begin(), p2.end());

	for (int i = 0; i < p1.size(); i++)
	{
		input.push_back((p2[i] - p1[i]));
	}

	return input;
}