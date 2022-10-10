// Fill out your copyright notice in the Description page of Project Settings.


#include "Hyperneat.h"

template<unsigned int S>
Hyperneat<S>::Hyperneat(unsigned int _populationSize, NeatParameters _neatParamters, ActivationFunction _activationFunction, ThresholdFunction _thresholdFunction)
{
	activationFunction = _activationFunction;
	thresholdFunction = _thresholdFunction;

	cppns = CPPN_Neat(_populationSize, S, 1, _neatParamters);

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