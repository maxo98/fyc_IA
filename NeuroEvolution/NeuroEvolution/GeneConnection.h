// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <iostream>

#include "GeneNode.h"

class CPPN_Neat;

/**
 * 
 */
class GeneConnection
{
public:
	GeneConnection();//Necessary to put it in a unordered_map or something else
	GeneConnection(unsigned int _innovation, unsigned int _nodeA, unsigned int _nodeB);
	~GeneConnection();

	inline unsigned int getNodeA() const { return nodeA; };
	inline unsigned int getNodeB() const { return nodeB; };
	inline float getWeight() const { return weight; };
	inline bool isEnabled() const { return enabled; };

	inline void setWeight(float _weight) { weight = _weight; };
	inline void setEnabled(bool _enabled) { enabled = _enabled; };

	friend class Genome;
	friend class CPPN_Neat;

	friend std::ostream& operator<<(std::ostream& os, const GeneConnection& geneConnection) {
		os << geneConnection.innovation;
		os << " ";
		os << geneConnection.nodeA;
		os << " ";
		os << geneConnection.nodeB;
		os << " ";
		os << geneConnection.enabled;
		os << " ";
		os << geneConnection.weight;
		return os;
	};

private:
	unsigned int innovation;
	unsigned int nodeA, nodeB;
	bool enabled = true;
	float weight = 1;
};
