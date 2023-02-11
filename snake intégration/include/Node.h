// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include <map>
#include "Activation.h"
#include <iostream>
#include <mutex>

/**
 * 
 */
class Node
{
public:
	Node(Activation* activation, int _id = -1);
	~Node();

	Node(const Node& other);
	Node(Node&& other) noexcept;

	void wipeConnections();
	void addConnection(Node*, float, bool);

	inline float getValue() { return value; }
	inline float getBackpropValue() { return backpropValue; }
	inline float getOldValue() { return oldValue; }
	inline Activation* getActivation() { return activation; }
	inline std::vector<std::pair<Node*, float>>* getPreviousNodes() { return &previousNodes; }
	inline std::vector<std::pair<Node*, float>>* getRecursionNodes() { return &recursionNodes; }

	inline void setValue(float x) { value = x; computed = true; delta = 0; };

	float compute();
	inline void reset() { computed = false; oldValue = 0; value = 0; delta = 0; };
	inline void next() { computed = false; oldValue = value; delta = 0; };

	friend class ANeuralNetworkDisplayHUD;
	friend class NeuralNetwork;
	
private:
	Activation* activation;
	std::vector<std::pair<Node*, float>> previousNodes;
	std::vector<std::pair<Node*, float>> recursionNodes;
	bool computed = false;
	float value;
	float oldValue;
	int id;
	float delta = 0;//Error bakcprop
	std::mutex deltaMtx;
	float backpropValue;
};
