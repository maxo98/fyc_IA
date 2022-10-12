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
	ES_Hyperneat(unsigned int _populationSize, NeatParameters _neatParamters, HyperneatParameters _hyperparam, unsigned int _nDecomposition);
	~ES_Hyperneat();

	virtual void generateNetworks();

protected:
	unsigned int nDecomposition;
};
