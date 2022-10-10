// Fill out your copyright notice in the Description page of Project Settings.


#include "ES_Hyperneat.h"

template<unsigned int S>
ES_Hyperneat<S>::ES_Hyperneat(unsigned int _populationSize, NeatParameters _neatParamters, ActivationFunction _activationFunction, ThresholdFunction _thresholdFunction, unsigned int _nDecomposition): Hyperneat(_populationSize, _neatParamters, _activationFunction, _thresholdFunction)
{
	nDecomposition = _nDecomposition;
}

template<unsigned int S>
ES_Hyperneat<S>::~ES_Hyperneat()
{
}

template<unsigned int S>
void ES_Hyperneat<S>::generateNetworks()
{

}