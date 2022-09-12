// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <list>
#include "Node.h"

/**
 * 
 */
class NEAT_API NeuralNetwork
{
public:
	NeuralNetwork();
	~NeuralNetwork();

	void fullyConnect();

	inline int getLayerSize() { return hiddenNodes.size() + 2; };
	int getNHiddenNode(int layer);
	inline int getNInputNode() { return inputNodes.size(); }
	inline int getNOutputNode() { return outputNodes.size(); }

	/**
	 * if layer parameter is bigger than the current number of layer, just one layer is appended at the end regardless of the layer value
	 * and a node is added to that layer
	 */
	void addHiddenNode(int layer);
	inline void addInputNode() { inputNodes.push_back(Node()); };
	inline void addOutputNode() { outputNodes.push_back(Node()); };
	void removeHiddenNode(int layer);
	inline void removeInputNode() { inputNodes.pop_back(); };
	inline void removeOutputNode() { outputNodes.pop_back(); };

	void connectNodes(int layerA, int nodeA, int layerB, int nodeB, float weight);

	void compute(std::vector<float> &inputs , std::vector<float> &outputs);

private:
	Node* getNode(int layer, int node);
	Node* getNodeFromLayer(std::list<Node> &layer, int node);

	std::list<std::list<Node>> hiddenNodes;
	std::list<Node> inputNodes;
	std::list<Node> outputNodes;

	friend class ANeuralNetworkDisplayHUD;
};
