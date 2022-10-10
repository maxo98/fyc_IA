// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Activation.h"

enum class NODE_TYPE
{
	INPUT,
	OUTPUT,
	HIDDEN
};

/**
 * 
 */
class NEAT_API GeneNode
{
public:
	GeneNode(NODE_TYPE _type, ActivationFunction _activationFunction, int _layer = 0);
	~GeneNode();

	inline NODE_TYPE getType() { return type; };
	inline int getLayer() { return layer; };
	inline void setActivation(ActivationFunction _activationFunction) { activationFunction = _activationFunction; };
	inline ActivationFunction getActivation() { return activationFunction; };

	friend class Genome;

private:

	ActivationFunction activationFunction;
	int layer;
	NODE_TYPE type;
};
