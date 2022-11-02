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
	GeneNode(NODE_TYPE _type, Activation* _activationFunction, unsigned int _layer = 0);
	~GeneNode();

	inline NODE_TYPE getType() { return type; };
	inline unsigned int getLayer() { return layer; };
	inline void setActivation(Activation* _activationFunction) { activationFunction = _activationFunction; };
	inline Activation* const getActivation() const { return activationFunction; };

	friend class Genome;

private:

	Activation* activationFunction;
	unsigned int layer;
	NODE_TYPE type;
};
