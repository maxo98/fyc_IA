// Fill out your copyright notice in the Description page of Project Settings.


#include "NeuralNetwork.h"

NeuralNetwork::NeuralNetwork()
{
	//Just for testing purpose
	//addInputNode();
	//addInputNode();
	//addInputNode();

	//addOutputNode();
	//addOutputNode();
	//addOutputNode();

	//addHiddenNode(0);
	//addHiddenNode(0);
	//addHiddenNode(1);
	//addHiddenNode(1);
	//addHiddenNode(1);

	/*connectNodes(0, 0, 1, 1, 0.5);
	connectNodes(0, 1, 1, 1, 0.5);
	connectNodes(1, 1, 2, 0, 1.5);
	connectNodes(2, 0, 3, 2, 1);

	connectNodes(0, 2, 1, 0, 1.5);
	connectNodes(1, 0, 2, 2, 0.75);
	connectNodes(2, 2, 3, 0, 3);

	connectNodes(0, 2, 3, 0, 1.5);

	std::vector<float> inputs, ouputs;

	inputs.push_back(1);
	inputs.push_back(1);
	inputs.push_back(2);

	compute(inputs, ouputs);*/

	//fullyConnect();
}

NeuralNetwork::~NeuralNetwork()
{
}

void NeuralNetwork::fullyConnect()
{
	//Connect ouput nodes to previous layer
	std::list<Node>::iterator itOutput;

	for (itOutput = outputNodes.begin(); itOutput != outputNodes.end(); ++itOutput)
	{
		std::list<Node>::iterator itPrevious;

		if (hiddenNodes.size() > 0)
		{
			for (itPrevious = hiddenNodes.back().begin(); itPrevious != hiddenNodes.back().end(); ++itPrevious)
			{
				itOutput->addConnection(&(*itPrevious), 0.5);
			}
		}
		else {
			for (itPrevious = inputNodes.begin(); itPrevious != inputNodes.end(); ++itPrevious)
			{
				itOutput->addConnection(&(*itPrevious), 0.5);
			}
		}
	}

	//Connect hidden layers to previous hidden layers
	std::list <std::list<Node>>::reverse_iterator itLayer = hiddenNodes.rbegin();
	std::list<Node>::iterator itNext = itLayer->begin();
	std::list<Node>::iterator itNextEnd = itLayer->end();
	
	while ((++itLayer) != hiddenNodes.rend())
	{
		for (itNext; itNext != itNextEnd; ++itNext)
		{
			std::list<Node>::iterator itPrevious = itLayer->begin();
			std::list<Node>::iterator itPreviousEnd = itLayer->end();

			for (itPrevious; itPrevious != itLayer->end(); ++itPrevious)
			{
				itNext->addConnection(&(*itPrevious), 0.5);
			}
		}

		itNext = itLayer->begin();
		itNextEnd = itLayer->end();
	}

	//Connect first hidden layer to input layer 
	//std::list<Node>::iterator itNext;
	for (itNext = hiddenNodes.front().begin(); itNext != hiddenNodes.front().end(); ++itNext)
	{
		std::list<Node>::iterator it_input;
		for (it_input = inputNodes.begin(); it_input != inputNodes.end(); ++it_input)
		{
			itNext->addConnection(&(*it_input), 0.5);
		}
	}
}

int NeuralNetwork::getNHiddenNode(int layer)
{
	layer--;

	if (layer >= hiddenNodes.size())
	{
		return 0;
	}

	int i = 0;
	std::list<std::list<Node>>::iterator it;
	for(it = hiddenNodes.begin(); it != hiddenNodes.end() && i != layer; ++it, ++i);

	return it->size();
}

void NeuralNetwork::addHiddenNode(int layer)
{
	if (layer > hiddenNodes.size())
	{
		while (layer > hiddenNodes.size())
		{
			hiddenNodes.push_back(std::list<Node>());
		}
		
		hiddenNodes.back().push_back(Node());

		return;
	}

	layer--;

	int i = 0;
	std::list<std::list<Node>>::iterator it;
	for (it = hiddenNodes.begin(); it != hiddenNodes.end() && i != layer; ++it, ++i);

	it->push_back(Node());
}

void NeuralNetwork::removeHiddenNode(int layer)
{
	layer--;
	int i = 0;
	std::list<std::list<Node>>::iterator it;
	for (it = hiddenNodes.begin(); it != hiddenNodes.end() && i != layer; ++it, ++i);

	it->pop_back();
}

void NeuralNetwork::connectNodes(std::pair<int, int> nodeA, std::pair<int, int> nodeB, float weight)
{
	connectNodes(nodeA.first, nodeA.second, nodeB.first, nodeB.second, weight);
}

void NeuralNetwork::connectNodes(int layerA, int nodeA, int layerB, int nodeB, float weight)
{
	if (layerA >= layerB)
	{
		if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 60.f, FColor::Red, "Error connecting nodes, layerA is superior or equal to layerB");
	}

	if (layerB >= getLayerSize())
	{
		if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 60.f, FColor::Red, "Error connecting nodes, layerB doesn't exist.");
	}

	Node *previousNode, *nextNode;

	previousNode = getNode(layerA, nodeA);
	nextNode = getNode(layerB, nodeB);

	if (previousNode == nullptr || nextNode == nullptr)
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 60.f, FColor::Red, "Error connecting nodes, one node doesn't exist.");
		return;
	}

	nextNode->addConnection(previousNode, weight);
}

Node* NeuralNetwork::getNode(int layer, int node)
{
	if (layer == 0)
	{
		return getNodeFromLayer(inputNodes, node);
	}
	else if ((layer-1) == hiddenNodes.size()) {
		return getNodeFromLayer(outputNodes, node);
	}
	else if((layer - 1) < hiddenNodes.size()){
		layer--;
		int i = 0;
		std::list<std::list<Node>>::iterator it;
		for (it = hiddenNodes.begin(); it != hiddenNodes.end() && i != layer; ++it, ++i);

		return getNodeFromLayer(*it, node);
	}
	else {
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 60.f, FColor::Red, FString::Printf(TEXT("Trying to get a node layer %i while max layer is %i"), layer, getLayerSize()));
		return nullptr;
	}
}

Node* NeuralNetwork::getNodeFromLayer(std::list<Node>& layer, int node)
{
	if (node < layer.size())
	{
		int i = 0;
		std::list<Node>::iterator it;
		for (it = layer.begin(); it != layer.end() && i != node; ++it, ++i);

		return &*it;
	}
	else {
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 60.f, FColor::Red, "Trying to get a node that doesn't exist");

		return nullptr;
	}
}

void NeuralNetwork::compute(std::vector<float>& inputs, std::vector<float>& outputs)
{
	if (inputs.size() >= inputNodes.size())
	{
		//Reset the hidden nodes
		for (std::list<std::list<Node>>::iterator it = hiddenNodes.begin(); it != hiddenNodes.end(); ++it)
		{
			for (std::list<Node>::iterator itNode = it->begin(); itNode != it->end(); ++itNode)
			{
				itNode->reset();
			}
		}

		//Set the input values
		int i = 0;
		for (std::list<Node>::iterator it = inputNodes.begin(); it != inputNodes.end(); ++it, ++i)
		{
			it->setValue(inputs[i]);
		}

		//Compute the result
		for (std::list<Node>::iterator it = outputNodes.begin(); it != outputNodes.end(); ++it)
		{
			it->reset();
			outputs.push_back(it->compute());

			if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 60.f, FColor::Red, FString::Printf(TEXT("%.3f"), it->compute()));
			
		}
	}
}

void NeuralNetwork::clear()
{
	hiddenNodes.clear();
	inputNodes.clear();
	outputNodes.clear();
}