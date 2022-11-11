// Fill out your copyright notice in the Description page of Project Settings.


#include "NeuralNetwork.h"
#include <limits>
#include <thread>

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
				itOutput->addConnection(&(*itPrevious), 0.5, false);
			}
		}
		else {
			for (itPrevious = inputNodes.begin(); itPrevious != inputNodes.end(); ++itPrevious)
			{
				itOutput->addConnection(&(*itPrevious), 0.5, false);
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
				itNext->addConnection(&(*itPrevious), 0.5, false);
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
			itNext->addConnection(&(*it_input), 0.5, false);
		}
	}
}

unsigned int NeuralNetwork::getNHiddenNode(unsigned int layer)
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

std::pair<unsigned int, unsigned int> NeuralNetwork::addHiddenNode(unsigned int layer, Activation* activation)
{
	if (layer > hiddenNodes.size())
	{
		while (layer > hiddenNodes.size())
		{
			hiddenNodes.push_back(std::list<Node>());
		}
		
		hiddenNodes.back().push_back(Node(activation));

		return std::pair<unsigned int, unsigned int>(layer, hiddenNodes.back().size()-1);
	}

	layer--;

	int i = 0;
	std::list<std::list<Node>>::iterator it;
	for (it = hiddenNodes.begin(); it != hiddenNodes.end() && i != layer; ++it, ++i);

	it->push_back(Node(activation));

	return std::pair<unsigned int, unsigned int>(layer, it->size() - 1);
}

std::pair<unsigned int, unsigned int> NeuralNetwork::addInputNode()
{ 
	inputNodes.push_back(Node(&dummyActivation));
	return std::pair<unsigned int, unsigned int>(0, inputNodes.size() - 1);
}

std::pair<unsigned int, unsigned int> NeuralNetwork::addOutputNode(Activation* activation)
{ 
	outputNodes.push_back(Node(activation)); 
	return std::pair<unsigned int, unsigned int>(std::numeric_limits<int>::max(), outputNodes.size() - 1);
}

void NeuralNetwork::removeHiddenNode(unsigned int layer)
{
	layer--;
	int i = 0;
	std::list<std::list<Node>>::iterator it;
	for (it = hiddenNodes.begin(); it != hiddenNodes.end() && i != layer; ++it, ++i);

	it->pop_back();
}

void NeuralNetwork::connectNodes(std::pair<unsigned int, unsigned int> nodeA, std::pair<unsigned int, unsigned int> nodeB, float weight)
{
	connectNodes(nodeA.first, nodeA.second, nodeB.first, nodeB.second, weight);
}

void NeuralNetwork::connectNodes(unsigned int layerA, unsigned int nodeA, unsigned int layerB, unsigned int nodeB, float weight)
{
	if (layerA >= layerB && recursive == false)
	{
		std::cout << "Error connecting nodes, layerA is superior or equal to layerB, recursion deactivated\n";

		return;
	}

	if (layerB >= getLayerSize())
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

	nextNode->addConnection(previousNode, weight, layerA >= layerB);
}

Node* NeuralNetwork::getNode(unsigned int layer, unsigned int node)
{
	if (layer == 0)
	{
		return getNodeFromLayer(inputNodes, node);
	}
	else if ((layer-1) == hiddenNodes.size()) 
	{
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
		std::cout << "Trying to get a node layer " << layer << " while max layer is " << getLayerSize() << std::endl;
		return nullptr;
	}
}

Node* NeuralNetwork::getNodeFromLayer(std::list<Node>& layer, unsigned int node)
{
	if (node < layer.size())
	{
		unsigned int i = 0;
		std::list<Node>::iterator it;
		for (it = layer.begin(); it != layer.end() && i != node; ++it, ++i);

		if (it == layer.end()) return nullptr;

		return &*it;
	}
	else {
		std::cout << "Trying to get a node that doesn't exist" << std::endl;

		return nullptr;
	}
}

void NeuralNetwork::compute(std::vector<float>& inputs, std::vector<float>& outputs)
{
	outputs.clear();

	if (inputs.size() >= inputNodes.size())
	{
		//Reset the hidden nodes
		for (std::list<std::list<Node>>::iterator it = hiddenNodes.begin(); it != hiddenNodes.end(); ++it)
		{
			for (std::list<Node>::iterator itNode = it->begin(); itNode != it->end(); ++itNode)
			{
				itNode->next();
			}
		}

		//Set the input values
		unsigned int i = 0;
		for (std::list<Node>::iterator it = inputNodes.begin(); it != inputNodes.end(); ++it, ++i)
		{
			it->setValue(inputs[i]);
		}

		//Parallel computing hidden nodes value
		for (std::list<std::list<Node>>::iterator it = hiddenNodes.begin(); it != hiddenNodes.end(); ++it)
		{
			std::list<Node>::iterator itNode = it->begin();

			splitLayerComputing(itNode, it->size());
		}

		//Compute the result
		splitLayerComputing(outputNodes.begin(), outputNodes.size(), true, &outputs);
	}
}

void NeuralNetwork::clear()
{
	hiddenNodes.clear();
	inputNodes.clear();
	outputNodes.clear();
}

void NeuralNetwork::splitLayerComputing(std::list<Node>::iterator it, int size, bool output, std::vector<float>* outputs)
{
	std::vector<std::thread> threads;
	unsigned int cpus = std::thread::hardware_concurrency();

	float totalWorkload = size;
	float workload = totalWorkload / cpus;
	float restWorkload = 0;
	int currentWorkload = totalWorkload;

	if (totalWorkload > 10)
	{
		while (workload < 1)
		{
			cpus--;
			workload = totalWorkload / cpus;
		}

		int currentWorkload = floor(workload);
		float workloadFrac = fmod(workload, 1.0f);
		restWorkload = workloadFrac;

		while (cpus > threads.size() + 1)
		{
			threads.push_back(std::thread(&NeuralNetwork::concurrentComputing, this, currentWorkload + floor(restWorkload), it, output, outputs));

			for (int i = 0; i < currentWorkload + floor(restWorkload); i++)
			{
				++it;
			}

			restWorkload -= floor(restWorkload);
			restWorkload += workloadFrac;
		}

		while (restWorkload > 0)
		{
			restWorkload--;
			currentWorkload++;
		}
	}

	concurrentComputing(currentWorkload + floor(restWorkload), it, output, outputs);
}

void NeuralNetwork::concurrentComputing(int workload, std::list<Node>::iterator it, bool output, std::vector<float>* outputs)
{
	for (int i = 0; i < workload; ++i, ++it)
	{
		if (output == false)
		{
			it->compute();
		}else{
			it->next();
			outputs->push_back(it->compute());
		}
	}
}