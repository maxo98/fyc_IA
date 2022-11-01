// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <map>
#include "Activation.h"

/**
 * 
 */
class NEAT_API Node
{
public:
	Node(Activation* activation);
	~Node();

	void wipeConnections();
	void removeConnection(Node*);
	void addConnection(Node*, float, bool);
	void changeWeight(Node*, float);

	inline float getOldValue() { return oldValue; }

	inline void setValue(float x) { value = x; computed = true; };

	float compute();
	inline void reset() { computed = false; oldValue = 0; value = 0; };
	inline void next() { computed = false; oldValue = value; };

	friend class ANeuralNetworkDisplayHUD;
	friend class NeuralNetwork;
	

private:

	Activation* activation;
	std::map<Node*, float> previousNodes;
	std::map<Node*, float> recursionNodes;
	bool computed;
	float value;
	float oldValue;
};
