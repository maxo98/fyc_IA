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

	inline int getLayerSize() { return hiddenNodes.size() + 2; };
	int getNHiddenNode(int layer);
	inline int getNInputNode() { return inputNodes.size(); }
	inline int getNOutputNode() { return outputNodes.size(); }

	void addHiddenNode(int layer, ActivationFunction activation);
	inline void addInputNode() { inputNodes.push_back(Node(nullptr)); };
	inline void addOutputNode(ActivationFunction activation) { outputNodes.push_back(Node(activation)); };
	void removeHiddenNode(int layer);
	inline void removeInputNode() { inputNodes.pop_back(); };
	inline void removeOutputNode() { outputNodes.pop_back(); };

	void connectNodes(int layerA, int nodeA, int layerB, int nodeB, float weight);
	void connectNodes(std::pair<int, int> nodeA, std::pair<int, int> nodeB, float weight);

	void compute(std::vector<float> &inputs , std::vector<float> &outputs);

	void clear();

private:
	Node* getNode(int layer, int node);
	Node* getNodeFromLayer(std::list<Node> &layer, int node);

	std::list<std::list<Node>> hiddenNodes;
	std::list<Node> inputNodes;
	std::list<Node> outputNodes;

	friend class ANeuralNetworkDisplayHUD;
};
