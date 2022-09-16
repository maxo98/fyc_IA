// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

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
	GeneNode(NODE_TYPE _type, int _layer = 0);
	~GeneNode();

	friend class Genome;

private:
	int layer;
	NODE_TYPE type;
};
