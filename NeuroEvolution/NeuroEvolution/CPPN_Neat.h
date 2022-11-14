// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Neat.h"

/**
 * 
 */
class CPPN_Neat: public Neat
{
public:
	inline CPPN_Neat() {};
	CPPN_Neat(unsigned int _populationSize, unsigned int _input, unsigned int _output, const NeatParameters& _neatParamters);
	~CPPN_Neat();

	virtual void mutate(Genome& genome, std::mutex* lock = nullptr);

	virtual float distance(Genome& genomeA, Genome& genomeB);
};
