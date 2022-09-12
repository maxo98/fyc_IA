// Fill out your copyright notice in the Description page of Project Settings.


#include "Node.h"

Node::Node()
{
}

Node::~Node()
{
}

void Node::wipeConnections()
{
	previousNodes.clear();
}

void Node::removeConnection(Node* node)
{
	previousNodes.erase(node);
}

void Node::addConnection(Node* node, float weight)
{
	previousNodes.insert(std::pair<Node*, float>(node, weight));
}

void Node::changeWeight(Node* node, float weight)
{
	previousNodes.at(node) = weight;
}

float Node::compute()
{
	if (computed == false)
	{
		value = 0;

		for (std::map<Node*, float>::iterator nodes = previousNodes.begin(); nodes != previousNodes.end(); ++nodes)
		{
			value += nodes->first->compute() * nodes->second;
		}

		value = sigmoidActivation(value);
	}

	return value;
}