// Fill out your copyright notice in the Description page of Project Settings.


#include "NeuralNetwork.h"
#include <limits>
#include <thread>

NeuralNetwork::NeuralNetwork()
{
}

NeuralNetwork::~NeuralNetwork()
{
}

/*void NeuralNetwork::fullyConnect()
{
	//Connect ouput nodes to previous layer
	std::deque<Node>::iterator itOutput;

	for (itOutput = outputNodes.begin(); itOutput != outputNodes.end(); ++itOutput)
	{
		std::deque<Node>::iterator itPrevious;

		if (hiddenNodes.size() > 0)
		{
			for (itPrevious = hiddenNodes.back().begin(); itPrevious != hiddenNodes.back().end(); ++itPrevious)
			{
				itOutput->addPreviousNode(&(*itPrevious), 0.5);
			}
		}
		else {
			for (itPrevious = inputNodes.begin(); itPrevious != inputNodes.end(); ++itPrevious)
			{
				itOutput->addPreviousNode(&(*itPrevious), 0.5);
			}
		}
	}

	//Connect hidden layers to previous hidden layers
	std::deque <std::deque<Node>>::reverse_iterator itLayer = hiddenNodes.rbegin();
	std::deque<Node>::iterator itNext = itLayer->begin();
	std::deque<Node>::iterator itNextEnd = itLayer->end();
	
	while ((++itLayer) != hiddenNodes.rend())
	{
		for (itNext; itNext != itNextEnd; ++itNext)
		{
			std::deque<Node>::iterator itPrevious = itLayer->begin();
			std::deque<Node>::iterator itPreviousEnd = itLayer->end();

			for (itPrevious; itPrevious != itLayer->end(); ++itPrevious)
			{
				itNext->addPreviousNode(&(*itPrevious), 0.5);
			}
		}

		itNext = itLayer->begin();
		itNextEnd = itLayer->end();
	}

	//Connect first hidden layer to input layer 
	//std::list<Node>::iterator itNext;
	for (itNext = hiddenNodes.front().begin(); itNext != hiddenNodes.front().end(); ++itNext)
	{
		std::deque<Node>::iterator it_input;
		for (it_input = inputNodes.begin(); it_input != inputNodes.end(); ++it_input)
		{
			itNext->addPreviousNode(&(*it_input), 0.5);
		}
	}
}*/

unsigned int NeuralNetwork::getNHiddenNode(unsigned int layer)
{
	layer--;

	if (layer >= hiddenLayerSize)
	{
		return 0;
	}

	return hiddenNodesSize[layer];
}

std::pair<unsigned int, unsigned int> NeuralNetwork::addHiddenNode(unsigned int layer, Activation* activation)
{
	while (layer > hiddenLayerSize)
	{
		hiddenLayerSize++;
	}

	if (hiddenNodes.size() < hiddenLayerSize)
	{
		
		do{
			hiddenNodes.push_back(std::deque<Node>());
			hiddenNodesSize.push_back(0);
			
		}while (hiddenNodes.size() < hiddenLayerSize);
		
		hiddenNodes.back().push_back(Node(activation));
		hiddenNodesSize[layer-1]++;

		return std::pair<unsigned int, unsigned int>(layer, hiddenNodes.back().size()-1);
	}

	layer--;

	if (hiddenNodes[layer].size() > hiddenNodesSize[layer])
	{
		hiddenNodes[layer][hiddenNodesSize[layer]].setActivation(activation);
		hiddenNodes[layer][hiddenNodesSize[layer]].wipeConnections();
	}
	else {
		hiddenNodes[layer].push_back(Node(activation));
	}

	hiddenNodesSize[layer]++;

	return std::pair<unsigned int, unsigned int>(layer+1, hiddenNodesSize[layer] - 1);
}

std::pair<unsigned int, unsigned int> NeuralNetwork::addInputNode()
{ 
	if (inputNodes.size() <= inputNodesSize)
	{
		inputNodes.push_back(Node(&dummyActivation));
	}

	inputNodesSize++;

	return std::pair<unsigned int, unsigned int>(std::numeric_limits<int>::max(), outputNodesSize - 1);
}

std::pair<unsigned int, unsigned int> NeuralNetwork::addOutputNode(Activation* activation)
{ 
	if (outputNodes.size() < outputNodesSize)
	{
		outputNodes[outputNodesSize].setActivation(activation);
		outputNodes[outputNodesSize].wipeConnections();
	}
	else {
		outputNodes.push_back(Node(activation));
	}

	outputNodesSize++;

	return std::pair<unsigned int, unsigned int>(std::numeric_limits<int>::max(), outputNodesSize - 1);
}

/*void NeuralNetwork::removeHiddenNode(unsigned int layer)
{
	layer--;
	int i = 0;
	std::deque<std::deque<Node>>::iterator it;
	for (it = hiddenNodes.begin(); it != hiddenNodes.end() && i != layer; ++it, ++i);

	it->pop_back();
}*/

void NeuralNetwork::connectNodes(std::pair<unsigned int, unsigned int> nodeA, std::pair<unsigned int, unsigned int> nodeB, float weight)
{
	connectNodes(nodeA.first, nodeA.second, nodeB.first, nodeB.second, weight);
}

void NeuralNetwork::connectNodes(unsigned int layerA, unsigned int nodeA, unsigned int layerB, unsigned int nodeB, float weight)
{
	if (layerA >= layerB && recursive == false && layerB != UINT_MAX)
	{
		std::cout << "Error connecting nodes, layerA is superior or equal to layerB, recursion deactivated\n";

		return;
	}

	if (layerB >= getLayerSize() && layerB != UINT_MAX)
	{
		std::cout << "Error connecting nodes, layerB doesn't exist." << std::endl;
	}

	Node *previousNode, *nextNode;

	previousNode = getNode(layerA, nodeA);
	nextNode = getNode(layerB, nodeB);

	if (previousNode == nullptr || nextNode == nullptr)
	{
		std::cout << "Error connecting nodes, one node doesn't exist." << std::endl;
		return;
	}

	if (layerA < layerB)
	{
		nextNode->addPreviousNode(previousNode, weight);
	}
	else {
		nextNode->addRecursionNode(previousNode, weight);
	}
}

Node* NeuralNetwork::getNode(unsigned int layer, unsigned int node)
{
	if (layer == 0)
	{
		return getNodeFromLayer(inputNodes, node);
	}
	else if (layer == UINT_MAX || (layer-1) == hiddenNodes.size())
	{
		return getNodeFromLayer(outputNodes, node);
	}
	else if((layer - 1) < hiddenNodes.size())
	{
		layer--;

		return getNodeFromLayer(hiddenNodes[layer], node);
	}
	else {
		std::cout << "Trying to get a node layer " << layer << " while max layer is " << getLayerSize() << std::endl;
		return nullptr;
	}
}

Node* NeuralNetwork::getNodeFromLayer(std::deque<Node>& layer, unsigned int node)
{
	if (node < layer.size())
	{
		return &layer[node];
	}
	else {
		std::cout << "Trying to get a node that doesn't exist" << std::endl;

		return nullptr;
	}
}

void NeuralNetwork::compute(const std::vector<float>& inputs, std::vector<float>& outputs)
{
	//outputs.clear();

	if (inputs.size() >= inputNodesSize)
	{
		//Reset the hidden nodes
		unsigned int i = 0;
		for (std::deque<std::deque<Node>>::iterator it = hiddenNodes.begin(); i < hiddenLayerSize; ++it, ++i)
		{
			unsigned int cpt = 0;
			for (std::deque<Node>::iterator itNode = it->begin(); cpt < hiddenNodesSize[i]; ++itNode, ++cpt)
			{
				itNode->next();
			}
		}

		//Set the input values
		i = 0;
		for (std::deque<Node>::iterator it = inputNodes.begin(); i < inputNodesSize; ++it, ++i)
		{
			it->setValue(inputs[i]);
		}

		//Parallel computing hidden nodes value
		i = 0;
		for (std::deque<std::deque<Node>>::iterator it = hiddenNodes.begin(); i < hiddenLayerSize; ++i, ++it)
		{
			std::deque<Node>::iterator itNode = it->begin();

			splitLayerComputing(itNode, hiddenNodesSize[i]);
		}

		outputs.resize(outputNodes.size(), 0);

		//Compute the result
		splitLayerComputing(outputNodes.begin(), outputNodesSize, true, &outputs);
	}
}

void NeuralNetwork::clear()
{
	for (int i = 0; i < hiddenNodesSize.size(); i++)
	{
		hiddenNodesSize[i] = 0;
	}
	
	hiddenLayerSize = 0;
	inputNodesSize = 0;
	outputNodesSize = 0;
}

void NeuralNetwork::splitLayerComputing(std::deque<Node>::iterator it, int size, bool output, std::vector<float>* outputs)
{
	std::vector<std::thread> threads;
	unsigned int cpus = std::thread::hardware_concurrency();

	float totalWorkload = size;
	float workload = totalWorkload / cpus;
	float restWorkload = 0;
	int currentWorkload = totalWorkload;
	int startIndex = 0;
	int count = 0;

	if (totalWorkload >= 20)
	{
		while (workload < 1)
		{
			cpus--;
			workload = totalWorkload / cpus;
		}

		currentWorkload = floor(workload);
		float workloadFrac = fmod(workload, 1.0f);
		restWorkload = workloadFrac;

		while (cpus > threads.size() + 1)
		{
			threads.push_back(std::thread(&NeuralNetwork::concurrentComputing, this, currentWorkload + floor(restWorkload), startIndex, it, output, outputs));

			count += currentWorkload + floor(restWorkload);

			for (int i = 0; i < currentWorkload + floor(restWorkload); i++)
			{
				++it;
			}

			startIndex += currentWorkload + floor(restWorkload);

			restWorkload -= floor(restWorkload);
			restWorkload += workloadFrac;
		}

		while (restWorkload > 0)
		{
			restWorkload--;
			currentWorkload++;
		}

		count += currentWorkload;

		while (count > totalWorkload)
		{
			currentWorkload--;
			count--;
		}
	}

	concurrentComputing(currentWorkload, startIndex, it, output, outputs);

	for (int i = 0; i < threads.size(); i++)
	{
		threads[i].join();
	}
}

void NeuralNetwork::concurrentComputing(int workload, int startIndex, std::deque<Node>::iterator it, bool output, std::vector<float>* outputs)
{
	for (int i = startIndex; i < (workload + startIndex); ++i, ++it)
	{
		if (output == false)
		{
			it->compute();
		}else{
			it->next();
			(*outputs)[i] = it->compute();
		}
	}
}

void NeuralNetwork::reserveHidden(int n, unsigned int layer, Activation* activation)
{
	layer--;

	int size = hiddenNodes[layer].size();

	for (int i = 0; i < size && i < n; i++)
	{
		hiddenNodes[layer][i].setActivation(activation);
	}

	if (size < n)
	{
		hiddenNodes[layer].resize(n, Node(activation));
	}
}

void NeuralNetwork::reserveInput(int n)
{

	if (inputNodes.size() < n)
	{
		inputNodes.resize(n, Node(&dummyActivation));
	}
}

void NeuralNetwork::reserveOutput(int n, Activation* activation)
{
	for (int i = 0; i < outputNodes.size() && i < n; i++)
	{
		outputNodes[i].setActivation(activation);
	}

	if (outputNodes.size() < n)
	{
		outputNodes.resize(n, Node(activation));
	}
}