// Fill out your copyright notice in the Description page of Project Settings.


#include "ES_Hyperneat.h"

ES_Hyperneat::ES_Hyperneat(unsigned int _populationSize, unsigned int _nDimensions, NeatParameters _neatParamters, unsigned int _cppnInput, unsigned int _cppnOutput, 
	unsigned int _nDecomposition, ActivationFunction _activationFunction, ThresholdFunction _thresholdFunction, CppnInputFunction _cppnInputFunction, 
	WeightModifierFunction _weightModifierFunction, std::vector<float> _thresholdConstants, std::vector<float> _inputConstants, std::vector<float> _weightConstants): 
	Hyperneat(_populationSize, _nDimensions, _neatParamters, _cppnInput, _cppnOutput, _activationFunction, _thresholdFunction, _cppnInputFunction, _weightModifierFunction, _thresholdConstants, _inputConstants, _weightConstants)
{
	nDecomposition = _nDecomposition;
}

ES_Hyperneat::~ES_Hyperneat()
{
}

void ES_Hyperneat::generateNetworks()
{

}