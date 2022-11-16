// Fill out your copyright notice in the Description page of Project Settings.


#include "Hyperneat.h"

Hyperneat::Hyperneat(unsigned int _populationSize, const NeatParameters& _neatParam, const HyperneatParameters& _hyperParam)
{
	hyperParam = _hyperParam;

	cppns = new CPPN_Neat(_populationSize, hyperParam.cppnInput, hyperParam.cppnOutput, _neatParam);

	networks.resize(_populationSize);
}

Hyperneat::~Hyperneat()
{
	delete cppns;
//	delete hyperParam.activationFunction;
}

void Hyperneat::addInput(const std::vector<float>& node)
{ 
	if (node.size() != hyperParam.nDimensions)
	{
		std::cout << "Error adding input of wrong dimension" << std::endl;

		return;
	}

	inputSubstrate.emplace(node); 
}

void Hyperneat::addOutput(const std::vector<float>& node)
{ 
	if (node.size() != hyperParam.nDimensions)
	{
		std::cout << "Error adding input of wrong dimension" << std::endl;

		return;
	}

	outputSubstrate.emplace(node);
}

void Hyperneat::addHiddenNode(unsigned int layer, const std::vector<float>& node)
{ 
	if (node.size() != hyperParam.nDimensions)
	{
		std::cout << "Error adding input of wrong dimension\n";

		return;
	}

	hiddenSubstrates[layer].emplace(node);
};

void Hyperneat::clear()
{
	inputSubstrate.clear();
	outputSubstrate.clear();
	hiddenSubstrates.clear();
}

void Hyperneat::generateNetworks()
{
	std::vector<std::thread> threads;
	unsigned int cpus = std::thread::hardware_concurrency();

	float totalWorkload = networks.size();
	float workload = totalWorkload / cpus;
	float restWorkload = 0;
	int currentWorkload = totalWorkload;
	int startIndex = 0;

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
		threads.push_back(std::thread(&Hyperneat::generateNetworksThread, this, startIndex, currentWorkload + floor(restWorkload)));

		startIndex += currentWorkload + floor(restWorkload);

		restWorkload -= floor(restWorkload);
		restWorkload += workloadFrac;
	}

	while (restWorkload > 0)
	{
		restWorkload--;
		currentWorkload++;
	}

	generateNetworksThread(startIndex, currentWorkload);

	for (int i = 0; i < threads.size(); i++)
	{
		threads[i].join();
	}
}

void Hyperneat::generateNetworksThread(int startIndex, int worlkload)
{
	for (unsigned int cpt = startIndex; cpt < (startIndex + worlkload); cpt++)
	{
		createNetwork(*cppns->getNeuralNetwork(cpt), networks[cpt]);
	}
}

void Hyperneat::genomeToNetwork(Genome& gen, NeuralNetwork& net)
{
	NeuralNetwork hyperNet;
	cppns->genomeToNetwork(gen, hyperNet);
	createNetwork(hyperNet, net);
}

void Hyperneat::createNetwork(NeuralNetwork& hypernet, NeuralNetwork& net)
{
	net.clear();

	//Add the input layer
	net.addInputNode(inputSubstrate.size());

	std::unordered_set<std::vector<float>, HyperNodeHash>::iterator beginPreviousLayer = inputSubstrate.begin();
	std::unordered_set<std::vector<float>, HyperNodeHash>::iterator endPreviousLayer = inputSubstrate.end();

	int layer = 1;

	//Add and connect the hidden layers
	for (std::deque<std::unordered_set<std::vector<float>, HyperNodeHash>>::iterator itLayer = hiddenSubstrates.begin(); itLayer != hiddenSubstrates.end(); ++itLayer)
	{
		net.addOutputNode(itLayer->size(), hyperParam.activationFunction);

		connectLayer(layer, hypernet, net, itLayer->begin(), itLayer->end(), beginPreviousLayer, endPreviousLayer);

		beginPreviousLayer = itLayer->begin();
		endPreviousLayer = itLayer->end();

		layer++;
	}

	//Add and connect the output layer
	net.addOutputNode(outputSubstrate.size(), hyperParam.activationFunction);

	connectLayer(layer, hypernet, net, outputSubstrate.begin(), outputSubstrate.end(), beginPreviousLayer, endPreviousLayer);
}

/**
* Connect layer to the previous layer
*/
void Hyperneat::connectLayer(unsigned int layer, NeuralNetwork& hypernet, NeuralNetwork& net, std::unordered_set<std::vector<float>, HyperNodeHash>::iterator itNode,
	std::unordered_set<std::vector<float>, HyperNodeHash>::iterator itNodeEnd,
	std::unordered_set<std::vector<float>, HyperNodeHash>::iterator beginPreviousLayer, const std::unordered_set<std::vector<float>, HyperNodeHash>::iterator endPreviousLayer)
{
	int nodeB = 0;

	//For each node in the layer to add
	for (itNode; itNode != itNodeEnd; ++itNode)
	{
		std::unordered_set<std::vector<float>, HyperNodeHash>::iterator prevLayer = beginPreviousLayer;

		int nodeA = 0;
		std::vector<float> p2 = std::vector<float>(itNode->begin(), itNode->end());
		

		//For each node from the previous layer
		for (prevLayer; prevLayer != endPreviousLayer; ++prevLayer)
		{
			std::vector<float> output, input;
			std::vector<float> p1 = std::vector<float>(prevLayer->begin(), prevLayer->end());
			input = hyperParam.cppnInputFunction(hyperParam.inputVariables, p1, p2);

			hypernet.compute(input, output);

			//Check if we should create a connection
			if (hyperParam.thresholdFunction(hyperParam.thresholdVariables, output, p1, p2) == true)
			{
				float weight = hyperParam.weightModifierFunction(hyperParam.weightVariables, output[0], p1, p2);
				net.connectNodes(layer - 1, nodeA, layer, nodeB, weight);
			}

			nodeA++;
		}

		nodeB++;
	}
}

std::vector<float> basicCppnInput(std::vector<void*> variables, std::vector<float> p1, std::vector<float> p2)
{
	p1.insert(p1.end(), p2.begin(), p2.end());
	return p1;
}

std::vector<float> sqrDistCppnInput(std::vector<void*> variables, std::vector<float> p1, std::vector<float> p2)
{
	p1.insert(p1.end(), p2.begin(), p2.end());
	p1.push_back(0);

	for (int i = 0; i < p1.size(); i++)
	{
		p1[p1.size() - 1] += (p2[i] - p1[i]) * (p2[i] - p1[i]);
	}

	return p1;
}

std::vector<float> deltaDistCppnInput(std::vector<void*> variables, std::vector<float> p1, std::vector<float> p2)
{
	p1.insert(p1.end(), p1.begin(), p1.end());

	for (int i = 0; i < p1.size(); i++)
	{
		p1.push_back((p2[i] - p1[i]));
	}

	return p1;
}