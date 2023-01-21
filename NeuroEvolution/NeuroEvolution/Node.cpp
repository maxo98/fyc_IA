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

Node::Node(const Node& other)
{
	activation = other.activation;
	previousNodes = other.previousNodes;
	recursionNodes = other.recursionNodes;
	computed = false;
	id = -1;
}

Node::Node(Node&& other) noexcept
{
	activation = std::move(other.activation);
	previousNodes = std::move(other.previousNodes);
	recursionNodes = std::move(other.recursionNodes);
	computed = false;
	id = std::move(other.id);
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

		backpropValue = value;
		value = activation->activate(value);

		computed = true;
	}

	return value;
}