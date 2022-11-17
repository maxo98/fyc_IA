// Fill out your copyright notice in the Description page of Project Settings.


#include "Node.h"

Node::Node(Activation* _activation)
{
	activation = _activation;
}

Node::~Node()
{
}

void Node::wipeConnections()
{
	previousNodesSize = 0;
	recursionNodesSize = 0;
}

/*void Node::removeConnection(Node* node)
{
	previousNodes.erase(node);
}*/

/*void Node::changeWeight(Node* node, float weight)
{
	previousNodes.at(node) = weight;
}*/

float Node::compute()
{
	if (computed == false)
	{
		value = 0;

		int i = 0;
		for (std::vector<std::pair<Node*, float>>::iterator nodes = previousNodes.begin(); i < previousNodesSize; ++nodes, ++i)
		{
			value += nodes->first->compute() * nodes->second;
		}

		i = 0;
		for (std::vector<std::pair<Node*, float>>::iterator nodes = recursionNodes.begin(); i < recursionNodesSize; ++nodes, ++i)
		{
			value += nodes->first->getOldValue() * nodes->second;
		}

		if (activation == nullptr)
		{
			std::cout << "Activation function is nullptr." << std::endl;

			return 0;
		}

		value = activation->activate(value);
		computed = true;
	}

	return value;
}

void Node::addPreviousNode(Node* node, float weight) 
{
	if (previousNodesSize < previousNodes.size())
	{
		previousNodes[previousNodesSize] = std::pair(node, weight);
	}
	else {
		previousNodes.push_back(std::pair(node, weight));
	}

	previousNodesSize++;
}

void Node::addRecursionNode(Node* node, float weight)
{
	if (recursionNodesSize < recursionNodes.size())
	{
		recursionNodes[recursionNodesSize] = std::pair(node, weight);
	}
	else {
		recursionNodes.push_back(std::pair(node, weight));
	}

	recursionNodesSize++;
}