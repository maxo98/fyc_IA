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
	Node(ActivationFunction activation);
	~Node();

	void wipeConnections();
	void removeConnection(Node*);
	void addConnection(Node*, float);
	void changeWeight(Node*, float);

	inline void setValue(float x) { value = x; computed = true; };

	float compute();
	void reset() { computed = false; };

	friend class ANeuralNetworkDisplayHUD;
	friend class NeuralNetwork;
	

private:

	ActivationFunction activation;
	std::map<Node*, float> previousNodes;
	bool computed;
	float value;
};
