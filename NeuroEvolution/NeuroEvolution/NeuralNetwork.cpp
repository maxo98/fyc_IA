// Fill out your copyright notice in the Description page of Project Settings.


#include "NeuralNetwork.h"
#include <limits>
#include "ThreadPool.h"

NeuralNetwork::NeuralNetwork()
{
}

NeuralNetwork::~NeuralNetwork()
{
}

//Might be deprecated
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

std::pair<unsigned int, unsigned int> NeuralNetwork::addHiddenNode(unsigned int layer, Activation* activation, int id)
{
	if (layer > hiddenNodes.size())
	{
		while (layer > hiddenNodes.size())
		{
			hiddenNodes.push_back(std::deque<Node>());
		}
		
		hiddenNodes.back().push_back(Node(activation, id));

		return std::pair<unsigned int, unsigned int>(layer, hiddenNodes.back().size()-1);
	}

	layer--;

	int i = 0;
	std::deque<std::deque<Node>>::iterator it;
	for (it = hiddenNodes.begin(); it != hiddenNodes.end() && i != layer; ++it, ++i);

	it->push_back(Node(activation, id));

	return std::pair<unsigned int, unsigned int>(layer+1, it->size() - 1);
}

std::pair<unsigned int, unsigned int> NeuralNetwork::addInputNode(int id)
{ 
	inputNodes.push_back(Node(&dummyActivation, id));
	return std::pair<unsigned int, unsigned int>(0, inputNodes.size() - 1);
}

void NeuralNetwork::addMultipleInputNode(int n)
{
	inputNodes.resize(inputNodes.size() + n, Node(&dummyActivation));
}

std::pair<unsigned int, unsigned int> NeuralNetwork::addOutputNode(Activation* activation, int id)
{ 
	outputNodes.push_back(Node(activation, id)); 
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

bool NeuralNetwork::compute(const std::vector<float>& inputs, std::vector<float>& outputs)
{
	if (prepareComputation(inputs) == true)
	{
		//Parallel computing hidden nodes value
		for (std::deque<std::deque<Node>>::iterator it = hiddenNodes.begin(); it != hiddenNodes.end(); ++it)
		{
			std::deque<Node>::iterator itNode = it->begin();

			splitLayerComputing(itNode, it->size());
		}

		outputs.resize(outputNodes.size(), 0);

		//Compute the result
		splitLayerComputing(outputNodes.begin(), outputNodes.size(), true, &outputs);

		return true;
	}

	return false;
}

bool NeuralNetwork::computeSpecificOuputs(const std::vector<float>& inputs, std::vector<float>& outputs, const std::vector<int>& indices)
{
	outputs.clear();

	if (prepareComputation(inputs) == true)
	{
		//Compute the result
		for(int i = 0; i < indices.size(); i++)
			outputs.push_back(outputNodes[indices[i]].compute());

		return true;
	}

	return false;
}

bool NeuralNetwork::prepareComputation(const std::vector<float>& inputs)
{
	if (inputs.size() >= inputNodes.size())
	{
#ifdef WARNING
		if (inputs.size() > inputNodes.size())
		{
			std::cout << "Inputs given larger than expected, expecting " << inputNodes.size() << ", received " << inputs.size() << std::endl;
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

		return true;

	}
	else {
		std::cerr << "Inputs given smaller than expected\n";

		return false;
	}
}

void NeuralNetwork::clear()
{
	hiddenNodes.clear();
	inputNodes.clear();
	outputNodes.clear();
}

void NeuralNetwork::clearConnections()
{
	for (std::deque<std::deque<Node>>::iterator itLayer = hiddenNodes.begin(); itLayer != hiddenNodes.end(); ++itLayer)
	{
		for (std::deque<Node>::iterator itNode = itLayer->begin(); itNode != itLayer->end(); ++itNode)
		{
			itNode->previousNodes.clear();
			itNode->recursionNodes.clear();
		}
	}

	for (std::deque<Node>::iterator itNode = inputNodes.begin(); itNode != inputNodes.end(); ++itNode)
	{
		itNode->previousNodes.clear();
		itNode->recursionNodes.clear();
	}

	for (std::deque<Node>::iterator itNode = outputNodes.begin(); itNode != outputNodes.end(); ++itNode)
	{
		itNode->previousNodes.clear();
		itNode->recursionNodes.clear();
	}
}

void NeuralNetwork::clearHidden()
{
	hiddenNodes.clear();
}

void NeuralNetwork::splitLayerComputing(std::deque<Node>::iterator it, int size, bool output, std::vector<float>* outputs)
{
	int threads = 1;
	ThreadPool* pool = ThreadPool::getInstance();
	unsigned int cpus = std::thread::hardware_concurrency();

	float totalWorkload = size;
	float workload = totalWorkload / cpus;
	float restWorkload = 0;
	int currentWorkload = totalWorkload;
	int startIndex = 0;
	int count = 0;

	std::deque<std::atomic<bool>> tickets;

	while (workload < 10)
	{
		cpus--;
		workload = totalWorkload / cpus;
	}

	while (cpus > threads)
	{
		currentWorkload = floor(workload);
		float workloadFrac = fmod(workload, 1.0f);
		restWorkload = workloadFrac;

		tickets.emplace_back(false);
		pool->queueJob(&NeuralNetwork::concurrentComputing, this, currentWorkload + floor(restWorkload), startIndex, it, output, outputs, &tickets.back());
		++threads;

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

	if (currentWorkload == 0)
	{
		std::cout << "error workload" << std::endl;
	}

	concurrentComputing(currentWorkload, startIndex, it, output, outputs);

	for (std::deque<std::atomic<bool>>::iterator itTicket = tickets.begin(); itTicket != tickets.end(); ++itTicket)
	{
		itTicket->wait(false);
	}
}

void NeuralNetwork::concurrentComputing(int workload, int startIndex, std::deque<Node>::iterator it, bool output, std::vector<float>* outputs, std::atomic<bool>* ticket)
{
	for (int i = startIndex; i < (workload + startIndex); ++i, ++it)
	{
		if (output == false)
		{
			it->compute();
		}else{
			(*outputs)[i] = it->compute();
		}
	}

	if (ticket != nullptr)
	{
		(*ticket) = true;
		ticket->notify_one();
	}
}

bool NeuralNetwork::backprop(const std::vector<float>& inputs, const std::vector<float>& outputs, const float& learnRate)
{
	if (inputs.size() < inputNodes.size() || outputs.size() < outputNodes.size())
	{
		std::cerr << "Inputs or outputs given smaller than expected\n";
		return false;
	}

	std::vector<float> tmp;

	if (compute(inputs, tmp) == true)
	{
		splitBackpropThread(outputNodes.begin(), outputNodes.size(), learnRate, &outputs);

		for (std::deque<std::deque<Node>>::reverse_iterator itLayer = hiddenNodes.rbegin(); itLayer != hiddenNodes.rend(); ++itLayer)
		{
			splitBackpropThread(itLayer->begin(), itLayer->size(), learnRate);
		}

		return true;
	}

	return false;
}

void NeuralNetwork::splitBackpropThread(std::deque<Node>::iterator it, int size, const float& learnRate, const std::vector<float>* outputs)
{
	int threads = 1;
	ThreadPool* pool = ThreadPool::getInstance();
	unsigned int cpus = std::thread::hardware_concurrency();

	float totalWorkload = size;
	float workload = totalWorkload / cpus;
	float restWorkload = 0;
	int currentWorkload = totalWorkload;
	int startIndex = 0;
	int count = 0;

	std::deque<std::atomic<bool>> tickets;

	while (workload < 1)
	{
		cpus--;
		workload = totalWorkload / cpus;
	}

	while (cpus > threads)
	{
		currentWorkload = floor(workload);
		float workloadFrac = fmod(workload, 1.0f);
		restWorkload = workloadFrac;

		tickets.emplace_back(false);
		pool->queueJob(&NeuralNetwork::backpropThread, this, currentWorkload + floor(restWorkload), startIndex, it, std::ref(learnRate), outputs, &tickets.back());
		++threads;

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

	if (currentWorkload == 0)
	{
		std::cout << "error workload" << std::endl;
	}

	backpropThread(currentWorkload, startIndex, it, learnRate, outputs);

	for (std::deque<std::atomic<bool>>::iterator itTicket = tickets.begin(); itTicket != tickets.end(); ++itTicket)
	{
		itTicket->wait(false);
	}
}

void NeuralNetwork::backpropThread(int workload, int startIndex, std::deque<Node>::iterator it, const float& learnRate, const std::vector<float>* outputs, std::atomic<bool>* ticket)
{
	for (int i = startIndex; i < (workload + startIndex); ++i, ++it)
	{
		if (outputs != nullptr)
		{
			it->delta = (it->value - (*outputs)[i]) * it->activation->derivate(it->value);
		}
		else {
			it->delta *= it->activation->derivate(it->value);
		}
		
		if (it->delta == 0) continue;

		for (int cpt = 0; cpt < it->previousNodes.size(); cpt++)
		{
			it->previousNodes[cpt].first->delta += it->previousNodes[cpt].second * it->delta;
			it->previousNodes[cpt].second -= learnRate * it->delta * it->previousNodes[cpt].first->value;//Update weights
		}
	}

	if (ticket != nullptr)
	{
		(*ticket) = true;
		ticket->notify_one();
	}
}

void NeuralNetwork::applyBackprop(Genome& gen)
{
	std::unordered_map<std::pair<unsigned int, unsigned int>, unsigned int>* map = gen.getNodesToConn();
	std::map<unsigned int, GeneConnection>* conn = gen.getConnections();

	for (std::deque<Node>::iterator it = outputNodes.begin(); it != outputNodes.end(); ++it)
	{
		for (int cpt = 0; cpt < it->previousNodes.size(); cpt++)
		{
			(*conn)[(*map)[std::pair<unsigned int, unsigned int>(it->previousNodes[cpt].first->id, it->id)]].setWeight(it->previousNodes[cpt].second);
		}
	}

	for (std::deque<std::deque<Node>>::iterator itLayer = hiddenNodes.begin(); itLayer != hiddenNodes.end(); ++itLayer)
	{
		for (std::deque<Node>::iterator itNode = itLayer->begin(); itNode != itLayer->end(); ++itNode)
		{
			for (int cpt = 0; cpt < itNode->previousNodes.size(); cpt++)
			{
				(*conn)[(*map)[std::pair<unsigned int, unsigned int>(itNode->previousNodes[cpt].first->id, itNode->id)]].setWeight(itNode->previousNodes[cpt].second);
			}
		}
	}
}