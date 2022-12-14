// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <iostream>

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
class GeneNode
{
public:
	GeneNode(NODE_TYPE _type, Activation* _activationFunction, unsigned int _layer = 0);
	~GeneNode();

	inline NODE_TYPE getType() { return type; };
	inline unsigned int getLayer() { return layer; };
	inline void setActivation(Activation* _activationFunction) { activationFunction = _activationFunction; };
	inline Activation* const getActivation() const { return activationFunction; };

	friend class Genome;

	friend std::ostream& operator<<(std::ostream& os, const GeneNode& geneNode) 
	{	
		os << *geneNode.activationFunction;
		os << " ";
		os << geneNode.layer;
		os << " ";
		os << (int)geneNode.type;

		return os;
	};

private:

	Activation* activationFunction;
	unsigned int layer;
	NODE_TYPE type;
};
