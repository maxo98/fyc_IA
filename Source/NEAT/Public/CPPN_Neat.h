// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NeatAlgoGen.h"

/**
 * 
 */
class NEAT_API CPPN_Neat: public NeatAlgoGen
{
public:
	CPPN_Neat(unsigned int _populationSize, unsigned int _input, unsigned int _output, NeatParameters _neatParamters);
	~CPPN_Neat();

	virtual void mutate(Genome& genome);
};
