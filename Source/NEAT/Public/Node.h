// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <map>

/**
 * 
 */
class NEAT_API Node
{
public:
	Node();
	~Node();

	void wipeConnections();
	void removeConnection(Node*);
	void addConnection(Node*, float);
	void changeWeight(Node*, float);

	inline void setValue(float x) { value = x; computed = true; };

	inline float sigmoidActivation(float x) { return 1 / (1 + exp(-x)); };

	float compute();
	void reset() { computed = false; };

	friend class ANeuralNetworkDisplayHUD;
	friend class NeuralNetwork;
	

private:
	std::map<Node*, float> previousNodes;
	bool computed;
	float value;
};
