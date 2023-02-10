// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#define WARNING

#include <list>
#include "Node.h"
#include <utility>
#include <deque>
#include "Genome.h"
#include <atomic>

/**
 * Refer to the nodes by there index starting from 0
 */
class NeuralNetwork
{
public:
	NeuralNetwork();
	~NeuralNetwork();

	inline unsigned int getLayerSize() { return hiddenNodes.size() + 2; };
	unsigned int getNHiddenNode(unsigned int layer);
	inline unsigned int getNInputNode() { return inputNodes.size(); }
	inline unsigned int getNOutputNode() { return outputNodes.size(); }

	inline std::deque<Node>* getOutputNodes() { return &outputNodes; }

	std::pair<unsigned int, unsigned int> addHiddenNode(unsigned int layer, Activation* activation, int id = -1);
	void addHiddenNode(int n, unsigned int layer, Activation* activation);
	std::pair<unsigned int, unsigned int> addInputNode(int id = -1);
	void addMultipleInputNode(int n);
	std::pair<unsigned int, unsigned int> addOutputNode(Activation* activation, int id = -1);
	void addOutputNode(int n, Activation* activation);
	void removeHiddenNode(unsigned int layer);
	inline void removeInputNode() { inputNodes.pop_back(); };
	inline void removeOutputNode() { outputNodes.pop_back(); };

	void connectNodes(unsigned int layerA, unsigned int nodeA, unsigned int layerB, unsigned int nodeB, float weight);
	void connectNodes(std::pair<unsigned int, unsigned int> nodeA, std::pair<unsigned int, unsigned int> nodeB, float weight);

	bool compute(const std::vector<float>& inputs, std::vector<float>& outputs);
	bool computeSpecificOuputs(const std::vector<float>& inputs, std::vector<float>& outputs, const std::vector<int>& indices);
	bool prepareComputation(const std::vector<float>& inputs);
	void splitLayerComputing(std::deque<Node>::iterator it, int size, bool output = false, std::vector<float>* outputs = nullptr);
	void concurrentComputing(int workload, int startIndex, std::deque<Node>::iterator it, bool output, std::vector<float>* outputs, std::atomic<bool>* ticket = nullptr);

	bool backprop(const std::vector<float>& inputs, const std::vector<float>& outputs, const float& learnRate, bool multitrhead = true);
	void splitBackpropThread(std::deque<Node>::iterator it, int size, const float& learnRate, const std::vector<float>* outputs = nullptr);
	void backpropThread(int workload, int startIndex, std::deque<Node>::iterator it, const float& learnRate, const std::vector<float>* outputs = nullptr, std::atomic<bool>* ticket = nullptr);
	void applyBackprop(Genome& gen);

	void clear();
	void clearConnections();
	void clearHidden();

	inline void setRecursion(bool value) { recursive = value; };
	inline bool isRecursive() { return recursive; }

	Node* getNode(unsigned int layer, unsigned int node);
	Node* getNodeFromLayer(std::deque<Node>& layer, unsigned int node);
	inline Node* getOutputNode(unsigned int node) { return &outputNodes[node]; };

	bool warningRecursive = true;

private:

	std::deque<std::deque<Node>> hiddenNodes;
	std::deque<Node> inputNodes;
	std::deque<Node> outputNodes;
	bool recursive = false;

	Activation dummyActivation;

	friend class ANeuralNetworkDisplayHUD;
};
