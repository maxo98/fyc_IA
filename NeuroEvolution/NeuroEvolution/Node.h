// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include <map>
#include "Activation.h"
#include <iostream>

/**
 * 
 */
class Node
{
public:
	Node(Activation* activation);
	~Node();

	void wipeConnections();
	//void removeConnection(Node*);
	//void changeWeight(Node*, float);

	inline float getOldValue() { return oldValue; }

	inline void setValue(float x) { value = x; computed = true; };

	void addPreviousNode(Node*, float);
	void addRecursionNode(Node*, float);

	inline void clear() { previousNodesSize = 0; recursionNodesSize = 0; }
	
	inline void setActivation(Activation* _activation) { activation = _activation; }

	float compute();
	inline void reset() { computed = false; oldValue = 0; value = 0; };
	inline void next() { computed = false; oldValue = value; };

	//friend class ANeuralNetworkDisplayHUD;
	//friend class NeuralNetwork;

	inline std::vector<std::pair<Node*, float>>* getPreviousNode() { return &previousNodes; };
	inline Activation* getActivation() { return activation; };
	
private:
	Activation* activation;
	std::vector<std::pair<Node*, float>> previousNodes;
	std::vector<std::pair<Node*, float>> recursionNodes;
	int previousNodesSize = 0;
	int recursionNodesSize = 0;
	bool computed = false;
	float value;
	float oldValue;
};
