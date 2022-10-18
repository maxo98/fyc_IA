// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GeneNode.h"

class CPPN_Neat;

/**
 * 
 */
class NEAT_API GeneConnection
{
public:
	GeneConnection();//Necessary to put it in a unordered_map or something else
	GeneConnection(unsigned int _innovation, unsigned int _nodeA, unsigned int _nodeB);
	~GeneConnection();

	inline unsigned int getNodeA() { return nodeA; };
	inline unsigned int getNodeB() { return nodeB; };
	inline float getWeight() { return weight; };
	inline bool isEnabled() { return enabled; };

	friend class Genome;
	friend class CPPN_Neat;

private:
	unsigned int innovation;
	unsigned int nodeA, nodeB;
	bool enabled = true;
	float weight = 1;
};
