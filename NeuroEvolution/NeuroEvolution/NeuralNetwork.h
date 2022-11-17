// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include <list>
#include "Node.h"
#include <utility>
#include <deque>

/**
 * Refer to the nodes by there index starting from 0
 */
class NeuralNetwork
{
public:
	NeuralNetwork();
	~NeuralNetwork();

	//void fullyConnect();

	inline unsigned int getLayerSize() { return hiddenLayerSize + 2; };
	unsigned int getNHiddenNode(unsigned int layer);
	inline unsigned int getNInputNode() { return inputNodesSize; }
	inline unsigned int getNOutputNode() { return outputNodesSize; }

	std::pair<unsigned int, unsigned int> addHiddenNode(unsigned int layer, Activation* activation);
	std::pair<unsigned int, unsigned int> addInputNode();
	std::pair<unsigned int, unsigned int> addOutputNode(Activation* activation);

	void reserveHidden(int n, unsigned int layer, Activation* activation);
	void reserveInput(int n);
	void reserveOutput(int n, Activation* activation);

	//void removeHiddenNode(unsigned int layer);
	inline void removeInputNode() { inputNodes.pop_back(); };
	inline void removeOutputNode() { outputNodes.pop_back(); };

	void connectNodes(unsigned int layerA, unsigned int nodeA, unsigned int layerB, unsigned int nodeB, float weight);
	void connectNodes(std::pair<unsigned int, unsigned int> nodeA, std::pair<unsigned int, unsigned int> nodeB, float weight);

	void compute(const std::vector<float> &inputs , std::vector<float> &outputs);
	void splitLayerComputing(std::deque<Node>::iterator it, int size, bool output = false, std::vector<float>* outputs = nullptr);
	void concurrentComputing(int workload, int startIndex, std::deque<Node>::iterator it, bool output, std::vector<float>* outputs);

	void clear();

	inline void setRecursion(bool value) { recursive = value; };
	inline bool isRecursive() { return recursive; }

	Node* getNode(unsigned int layer, unsigned int node);
	Node* getNodeFromLayer(std::deque<Node>& layer, unsigned int node);
	inline Node* getOutputNode(unsigned int node) { return &outputNodes[node]; };

private:
	//Nodes should never move in the memory
	std::deque<std::deque<Node>> hiddenNodes;
	std::deque<Node> inputNodes;
	std::deque<Node> outputNodes;

	int hiddenLayerSize = 0;
	std::vector<int> hiddenNodesSize;
	int inputNodesSize = 0;
	int outputNodesSize = 0;

	bool recursive = false;

	Activation dummyActivation;

	//friend class ANeuralNetworkDisplayHUD;
};
