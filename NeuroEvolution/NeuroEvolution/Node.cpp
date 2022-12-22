// Fill out your copyright notice in the Description page of Project Settings.


#include "Node.h"

Node::Node(Activation* _activation, int _id)
{
	activation = _activation;
	id = _id;
}

Node::~Node()
{
}

void Node::wipeConnections()
{
	previousNodes.clear();
}

void Node::addConnection(Node* node, float weight, bool recursive)
{
	if (recursive == false)
	{
		previousNodes.push_back(std::pair<Node*, float>(node, weight));
	}
	else {
		previousNodes.push_back(std::pair<Node*, float>(node, weight));
	}
	
}

float Node::compute()
{
	if (computed == false)
	{
		value = 0;

		for (std::vector<std::pair<Node*, float>>::iterator nodes = previousNodes.begin(); nodes != previousNodes.end(); ++nodes)
		{
			value += nodes->first->compute() * nodes->second;
		}

		for (std::vector<std::pair<Node*, float>>::iterator nodes = recursionNodes.begin(); nodes != recursionNodes.end(); ++nodes)
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