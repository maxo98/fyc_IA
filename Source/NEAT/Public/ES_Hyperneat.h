// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HyperNeat.h"

/**
 * 
 */
template<unsigned int S>
class NEAT_API ES_Hyperneat: public Hyperneat<S>
{
public:
	ES_Hyperneat(unsigned int _populationSize, NeatParameters _neatParamters, ActivationFunction _activationFunction, ThresholdFunction _thresholdFunction, unsigned int _nDecomposition);
	~ES_Hyperneat();

	virtual void generateNetworks();

protected:
	unsigned int nDecomposition;
};
