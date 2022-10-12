// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HyperNeat.h"

/**
 * 
 */
class NEAT_API ES_Hyperneat: public Hyperneat
{
public:
	ES_Hyperneat(unsigned int _populationSize, unsigned int _nDimensions, NeatParameters _neatParamters, unsigned int _cppnInput, unsigned int _cppnOutput, unsigned int _nDecomposition,
		ActivationFunction _activationFunction, ThresholdFunction _thresholdFunction, CppnInputFunction _cppnInputFunction, WeightModifierFunction _weightModifierFunction,
		std::vector<float> _thresholdConstants = std::vector<float>(), std::vector<float> _inputConstants = std::vector<float>(), 
		std::vector<float> _weightConstants = std::vector<float>());
	~ES_Hyperneat();

	virtual void generateNetworks();

protected:
	unsigned int nDecomposition;
};
