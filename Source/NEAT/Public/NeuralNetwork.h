// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <list>
#include "Node.h"

/**
 * Refer to the nodes by there index starting from 0
 */
class NEAT_API NeuralNetwork
{
public:
	NeuralNetwork();
	~NeuralNetwork();

	void fullyConnect();

	inline unsigned int getLayerSize() { return hiddenNodes.size() + 2; };
	unsigned int getNHiddenNode(unsigned int layer);
	inline unsigned int getNInputNode() { return inputNodes.size(); }
	inline unsigned int getNOutputNode() { return outputNodes.size(); }

	std::pair<unsigned int, unsigned int> addHiddenNode(unsigned int layer, ActivationFunction activation);
	std::pair<unsigned int, unsigned int> addInputNode();
	std::pair<unsigned int, unsigned int> addOutputNode(ActivationFunction activation);
	void removeHiddenNode(unsigned int layer);
	inline void removeInputNode() { inputNodes.pop_back(); };
	inline void removeOutputNode() { outputNodes.pop_back(); };

	void connectNodes(unsigned int layerA, unsigned int nodeA, unsigned int layerB, unsigned int nodeB, float weight);
	void connectNodes(std::pair<unsigned int, unsigned int> nodeA, std::pair<unsigned int, unsigned int> nodeB, float weight);

	void compute(std::vector<float> &inputs , std::vector<float> &outputs);

	void clear();

private:
	Node* getNode(unsigned int layer, unsigned int node);
	Node* getNodeFromLayer(std::list<Node> &layer, unsigned int node);

	std::list<std::list<Node>> hiddenNodes;
	std::list<Node> inputNodes;
	std::list<Node> outputNodes;

	friend class ANeuralNetworkDisplayHUD;
};
