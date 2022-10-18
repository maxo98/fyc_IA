// Fill out your copyright notice in the Description page of Project Settings.


#include "Hyperneat.h"

Hyperneat::Hyperneat(unsigned int _populationSize, NeatParameters _neatParam, HyperneatParameters _hyperParam)
{
	hyperParam = _hyperParam;

	cppns = CPPN_Neat(_populationSize, hyperParam.cppnInput, hyperParam.cppnOutput, _neatParam);

	networks.resize(_populationSize);
}

Hyperneat::~Hyperneat()
{
}

void Hyperneat::addInput(std::vector<float> node)
{ 
	if (node.size() != hyperParam.nDimensions)
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 60.f, FColor::Red, FString::Printf(TEXT("Error adding input of wrong dimension")));

		return;
	}

	inputSubstrate.emplace(node); 
}

void Hyperneat::addOutput(std::vector<float> node)
{ 
	if (node.size() != hyperParam.nDimensions)
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 60.f, FColor::Red, FString::Printf(TEXT("Error adding input of wrong dimension")));

		return;
	}

	outputSubstrate.emplace(node);
}

void Hyperneat::addHiddenNode(unsigned int layer, std::vector<float> node)
{ 
	if (node.size() != hyperParam.nDimensions)
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 60.f, FColor::Red, FString::Printf(TEXT("Error adding input of wrong dimension")));

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
	for (unsigned int cpt = 0; cpt < networks.size(); cpt++)
	{
		networks[cpt].clear();

		//Add the input layer
		for (int i = 0; i < inputSubstrate.size(); i++)
		{
			networks[cpt].addInputNode();
		}

		std::unordered_set<std::vector<float>, HyperNodeHash>::iterator beginPreviousLayer = inputSubstrate.begin();
		std::unordered_set<std::vector<float>, HyperNodeHash>::iterator endPreviousLayer = inputSubstrate.end();

		int layer = 1;
		
		//Add and connect the hidden layers
		for (std::deque<std::unordered_set<std::vector<float>, HyperNodeHash>>::iterator itLayer = hiddenSubstrates.begin(); itLayer != hiddenSubstrates.end(); ++itLayer)
		{
			addLayerAndConnect(layer, cpt, itLayer->begin(), itLayer->end(), beginPreviousLayer, endPreviousLayer, false);

			beginPreviousLayer = itLayer->begin();
			endPreviousLayer = itLayer->end();

			layer++;
		}

		//Add and connect the output layer
		addLayerAndConnect(layer, cpt, outputSubstrate.begin(), outputSubstrate.end(), beginPreviousLayer, endPreviousLayer, true);
	}
}

/**
* Add nodes of a layer to the neuralnetwork
* and connect it to the previous layer
*/
void Hyperneat::addLayerAndConnect(unsigned int layer, unsigned int networkIndex, std::unordered_set<std::vector<float>, HyperNodeHash>::iterator itNode,
	std::unordered_set<std::vector<float>, HyperNodeHash>::iterator itNodeEnd,
	std::unordered_set<std::vector<float>, HyperNodeHash>::iterator beginPreviousLayer, const std::unordered_set<std::vector<float>, HyperNodeHash>::iterator endPreviousLayer,
	bool outputLayer)
{
	int nodeB = 0;

	//For each node in the layer to add
	for (itNode; itNode != itNodeEnd; ++itNode)
	{
		if (outputLayer == false)
		{
			networks[networkIndex].addHiddenNode(layer, hyperParam.activationFunction);
		}
		else {
			networks[networkIndex].addOutputNode(hyperParam.activationFunction);
		}
		
		std::unordered_set<std::vector<float>, HyperNodeHash>::iterator prevLayer = beginPreviousLayer;

		int nodeA = 0;
		std::vector<float> p2 = std::vector<float>(itNode->begin(), itNode->end());
		

		//For each node from the previous layer
		for (prevLayer; prevLayer != endPreviousLayer; ++prevLayer)
		{
			std::vector<float> output, input;
			std::vector<float> p1 = std::vector<float>(prevLayer->begin(), prevLayer->end());
			input = hyperParam.cppnInputFunction(hyperParam.inputConstants, p1, p2);

			output.resize(hyperParam.cppnOutput);
			networks[networkIndex].compute(input, output);

			//Check if we should create a connection
			if (hyperParam.thresholdFunction(hyperParam.thresholdConstants, output, p1, p2) == true)
			{
				float weight = hyperParam.weightModifierFunction(hyperParam.weightConstants, output[0], p1, p2);
				networks[networkIndex].connectNodes(layer - 1, nodeA, layer, nodeB, weight);
			}

			nodeA++;
		}

		nodeB++;
	}
}

std::vector<float> basicCppnInput(std::vector<float> constants, std::vector<float> p1, std::vector<float> p2)
{
	p1.insert(p1.end(), p2.begin(), p2.end());
	return p1;
}

std::vector<float> sqrDistCppnInput(std::vector<float> constants, std::vector<float> p1, std::vector<float> p2)
{
	p1.insert(p1.end(), p2.begin(), p2.end());
	p1.push_back(0);

	for (int i = 0; i < p1.size(); i++)
	{
		p1[p1.size() - 1] += (p2[i] - p1[i]) * (p2[i] - p1[i]);
	}

	return p1;
}

std::vector<float> deltaDistCppnInput(std::vector<float> constants, std::vector<float> p1, std::vector<float> p2)
{
	p1.insert(p1.end(), p1.begin(), p1.end());

	for (int i = 0; i < p1.size(); i++)
	{
		p1.push_back((p2[i] - p1[i]));
	}

	return p1;
}