// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <vector>
#include "NeuralNetwork.h"

/**
 * 
 */
class NEAT_API NeatAlgoGen
{
public:
	NeatAlgoGen();
	~NeatAlgoGen();

private:
	std::vector<NeuralNetwork> networks;
};
