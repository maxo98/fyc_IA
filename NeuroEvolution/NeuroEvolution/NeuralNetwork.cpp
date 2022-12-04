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
	std::deque<Node>::iterator itOutput;

	for (itOutput = outputNodes.begin(); itOutput != outputNodes.end(); ++itOutput)
	{
		std::deque<Node>::iterator itPrevious;

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
		std::deque<Node>::iterator it_input;
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
	std::deque<std::deque<Node>>::iterator it;
	for(it = hiddenNodes.begin(); it != hiddenNodes.end() && i != layer; ++it, ++i);

	return it->size();
}

void NeuralNetwork::addHiddenNode(int n, unsigned int layer, Activation* activation)
{
	if (layer > hiddenNodes.size())
	{
		while (layer > hiddenNodes.size())
		{
			hiddenNodes.push_back(std::deque<Node>());
		}

		hiddenNodes.back().resize(n + hiddenNodes.size(), Node(activation));
	}
	else {

		layer--;

		int i = 0;
		std::deque<std::deque<Node>>::iterator it;
		for (it = hiddenNodes.begin(); it != hiddenNodes.end() && i != layer; ++it, ++i);

		it->resize(n + hiddenNodes.size(), Node(activation));
	}
}

std::pair<unsigned int, unsigned int> NeuralNetwork::addHiddenNode(unsigned int layer, Activation* activation)
{
	if (layer > hiddenNodes.size())
	{
		while (layer > hiddenNodes.size())
		{
			hiddenNodes.push_back(std::deque<Node>());
		}
		
		hiddenNodes.back().push_back(Node(activation));

		return std::pair<unsigned int, unsigned int>(layer, hiddenNodes.back().size()-1);
	}

	layer--;

	int i = 0;
	std::deque<std::deque<Node>>::iterator it;
	for (it = hiddenNodes.begin(); it != hiddenNodes.end() && i != layer; ++it, ++i);

	it->push_back(Node(activation));

	return std::pair<unsigned int, unsigned int>(layer, it->size() - 1);
}

std::pair<unsigned int, unsigned int> NeuralNetwork::addInputNode()
{ 
	inputNodes.push_back(Node(&dummyActivation));
	return std::pair<unsigned int, unsigned int>(0, inputNodes.size() - 1);
}

void NeuralNetwork::addInputNode(int n)
{
	inputNodes.resize(inputNodes.size() + n, Node(&dummyActivation));
}

std::pair<unsigned int, unsigned int> NeuralNetwork::addOutputNode(Activation* activation)
{ 
	outputNodes.push_back(Node(activation)); 
	return std::pair<unsigned int, unsigned int>(std::numeric_limits<unsigned int>::max(), outputNodes.size() - 1);
}

void NeuralNetwork::addOutputNode(int n, Activation* activation)
{
	outputNodes.resize(n + outputNodes.size(), Node(activation));
}

void NeuralNetwork::removeHiddenNode(unsigned int layer)
{
	layer--;
	int i = 0;
	std::deque<std::deque<Node>>::iterator it;
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
		if (warningRecursive == true)
		{
			std::cout << "Error connecting nodes, layerA is superior or equal to layerB, recursion deactivated\n";
		}

		return;
	}

	if (layerB >= getLayerSize() && layerB != std::numeric_limits<unsigned int>::max())
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
	else if ((layer-1) == hiddenNodes.size() || layer == std::numeric_limits<unsigned int>::max())
	{
		return getNodeFromLayer(outputNodes, node);
	}
	else if((layer - 1) < hiddenNodes.size()){
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
	outputs.clear();

	if (inputs.size() >= inputNodes.size())
	{
#ifdef WARNING
		if (inputs.size() > inputNodes.size())
		{
			std::cout << "Inputs given larger than expected\n";
		}
#endif // WARNING

		//Reset the hidden nodes
		for (std::deque<std::deque<Node>>::iterator it = hiddenNodes.begin(); it != hiddenNodes.end(); ++it)
		{
			for (std::deque<Node>::iterator itNode = it->begin(); itNode != it->end(); ++itNode)
			{
				itNode->next();
			}
		}

		//Reset output nodes
		for (std::deque<Node>::iterator itNode = outputNodes.begin(); itNode != outputNodes.end(); ++itNode)
		{
			itNode->next();
		}

		//Set the input values
		unsigned int i = 0;
		for (std::deque<Node>::iterator it = inputNodes.begin(); it != inputNodes.end(); ++it, ++i)
		{
			it->setValue(inputs[i]);
		}

		//Parallel computing hidden nodes value
		for (std::deque<std::deque<Node>>::iterator it = hiddenNodes.begin(); it != hiddenNodes.end(); ++it)
		{
			std::deque<Node>::iterator itNode = it->begin();

			splitLayerComputing(itNode, it->size());
		}

		outputs.resize(outputNodes.size(), 0);

		//Compute the result
		splitLayerComputing(outputNodes.begin(), outputNodes.size(), true, &outputs);
	}
	else {
		std::cout << "Inputs given smaller than expected\n";
	}
}

void NeuralNetwork::clear()
{
	hiddenNodes.clear();
	inputNodes.clear();
	outputNodes.clear();
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

	if (totalWorkload >= 40)
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

	if (currentWorkload == 0)
	{
		std::cout << "error workload" << std::endl;
	}

	concurrentComputing(currentWorkload, startIndex, it, output, outputs);

	//std::cout << std::endl;

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
			//std::cout << i << " " << it->compute();
		}else{
			(*outputs)[i] = it->compute();
		}
	}
}