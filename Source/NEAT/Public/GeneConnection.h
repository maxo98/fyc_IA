// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GeneNode.h"

/**
 * 
 */
class NEAT_API GeneConnection
{
public:
	GeneConnection();//Necessary to put it in a unordered_map or something else
	GeneConnection(int _innovation, int _nodeA, int _nodeB);
	~GeneConnection();

	friend class Genome;

private:
	int innovation;
	int nodeA, nodeB;
	bool enabled = true;
	float weight = 1;
};
