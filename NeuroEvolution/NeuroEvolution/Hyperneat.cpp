// Fill out your copyright notice in the Description page of Project Settings.


#include "Hyperneat.h"

Hyperneat::Hyperneat(unsigned int _populationSize, const NeatParameters& _neatParam, const HyperneatParameters& _hyperParam, Neat::INIT init)
{
	hyperParam = _hyperParam;

	cppns = new CPPN_Neat(_populationSize, hyperParam.cppnInput, hyperParam.cppnOutput, _neatParam, init);

	networks.resize(_populationSize);
}

Hyperneat::Hyperneat(unsigned int _populationSize, const NeatParameters& _neatParam, const HyperneatParameters& _hyperParam, std::vector<Genome>& initPop)
{
	hyperParam = _hyperParam;

	cppns = new CPPN_Neat(_populationSize, hyperParam.cppnInput, hyperParam.cppnOutput, _neatParam, initPop);

	networks.resize(_populationSize);
}

Hyperneat::~Hyperneat()
{
	delete cppns;
}

void Hyperneat::addInput(const std::vector<float>& node)
{ 
	if (node.size() != hyperParam.nDimensions)
	{
		std::cout << "Error adding input of wrong dimension" << std::endl;

		return;
	}

	inputSubstrate.push_back(node); 
}

void Hyperneat::addOutput(const std::vector<float>& node)
{ 
	if (node.size() != hyperParam.nDimensions)
	{
		std::cout << "Error adding input of wrong dimension" << std::endl;

		return;
	}

	outputSubstrate.push_back(node);
}

void Hyperneat::addHiddenNode(unsigned int layer, const std::vector<float>& node)
{ 
	if (node.size() != hyperParam.nDimensions)
	{
		std::cout << "Error adding input of wrong dimension\n";

		return;
	}

	hiddenSubstrates[layer].push_back(node);
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
	int count = 0;

#ifdef MULTITHREAD
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

		count += currentWorkload + floor(restWorkload);
		startIndex += currentWorkload + floor(restWorkload);

		restWorkload -= floor(restWorkload);
		restWorkload += workloadFrac;
	}

	while (restWorkload > 0)
	{
		restWorkload--;
		currentWorkload++;
	}
#endif // MULTITHREAD

	count += currentWorkload;

	while (count > totalWorkload)
	{
		currentWorkload--;
		count--;
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
	initNetwork(net);
	createNetwork(hyperNet, net);
}

void Hyperneat::initNetworks()
{
	for (int i = 0; i < networks.size(); i++)
	{
		initNetwork(networks[i]);
	}
}

void Hyperneat::initNetwork(NeuralNetwork& net)
{
	//Add the input layer
	net.addMultipleInputNode(inputSubstrate.size());

	for (std::vector<std::vector<std::vector<float>>>::iterator itLayer = hiddenSubstrates.begin(); itLayer != hiddenSubstrates.end(); ++itLayer)
	{
		net.addHiddenNode(itLayer->size(), hyperParam.activationFunction);
	}

	//Add and connect the output layer
	net.addOutputNode(outputSubstrate.size(), hyperParam.activationFunction);
}

void Hyperneat::createNetwork(NeuralNetwork& hypernet, NeuralNetwork& net)
{
	net.clearConnections();

	std::vector<std::vector<float>>::iterator beginPreviousLayer = inputSubstrate.begin();
	std::vector<std::vector<float>>::iterator endPreviousLayer = inputSubstrate.end();

	int layer = 1;

	//Connect the hidden layers
	for (std::vector<std::vector<std::vector<float>>>::iterator itLayer = hiddenSubstrates.begin(); itLayer != hiddenSubstrates.end(); ++itLayer)
	{
		connectLayer(layer, hypernet, net, itLayer->begin(), itLayer->end(), beginPreviousLayer, endPreviousLayer);

		beginPreviousLayer = itLayer->begin();
		endPreviousLayer = itLayer->end();

		layer++;
	}

	connectLayer(layer, hypernet, net, outputSubstrate.begin(), outputSubstrate.end(), beginPreviousLayer, endPreviousLayer);
}

/**
* Connect layer to the previous layer
*/
void Hyperneat::connectLayer(unsigned int layer, NeuralNetwork& hypernet, NeuralNetwork& net, std::vector<std::vector<float>>::iterator itNode,
	std::vector<std::vector<float>>::iterator itNodeEnd,
	std::vector<std::vector<float>>::iterator beginPreviousLayer, const std::vector<std::vector<float>>::iterator endPreviousLayer)
{
	int nodeB = 0;

	//For each node in the layer to add
	for (itNode; itNode != itNodeEnd; ++itNode)
	{
		std::vector<std::vector<float>>::iterator prevLayer = beginPreviousLayer;

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


//Conceived for fully connected, and no weight modification
bool Hyperneat::backprop(const std::vector<float>& inputs, const std::vector<float>& outputs, float learnRate)
{
	if (inputs.size() < inputSubstrate.size() || outputs.size() < outputSubstrate.size()) return false;

	for (int i = 0; i < networks.size(); i++)
	{
		std::vector<int> indices;
		std::vector<float> errors;

		indices.push_back(0);

		int cpt = 0;

		//Compute the error for each connection entering an output nodes
		//And correct it one by one
		for (std::deque<Node>::iterator it = networks[i].getOutputNodes()->begin(); it != networks[i].getOutputNodes()->end(); ++it, ++cpt)
		{
			indices[0] = cpt;
			networks[i].computeSpecificOuputs(inputs, errors, indices);
			
			float delta = (it->getValue() - outputs[i]) * it->getActivation()->derivate(it->getValue());

			std::vector<float> cppnInputs, cppnOutput;

			cppnOutput.push_back(0);

			int i2 = 0;
			for (std::vector<std::pair<Node*, float>>::iterator itPrev = it->getPreviousNodes()->begin(); itPrev != it->getPreviousNodes()->end(); ++itPrev, ++i2)
			{
				std::vector<float>* p1;

				if (hiddenSubstrates.size() == 0)
				{
					p1 = &inputSubstrate[i2];
				}
				else {
					p1 = &hiddenSubstrates.back()[i2];
				}

				cppnInputs = hyperParam.cppnInputFunction(hyperParam.inputVariables, *p1, outputSubstrate[cpt]);

				cppnOutput[0] = itPrev->second - learnRate * delta * itPrev->first->getValue();

				cppns->getNeuralNetwork(i)->backprop(cppnInputs, cppnOutput, learnRate);

				//networks[i].compute(cppnInputs, cppnOutput);

				//std::cout << "test2 " << cppnOutput[0] << std::endl;
			}
			
			//??? somethings missing here, should compute what the value of the weight should be
			//if (hiddenSubstrates.size() > 0)
			//{
			//	backprop(inputs, outputs, learnRate, hiddenSubstrates.back(), cpt, cppns->getNeuralNetwork(i));
			//}
			//else {
			//	backprop(inputs, outputs, learnRate, inputSubstrate, cpt, cppns->getNeuralNetwork(i));
			//}
		}
	}

	generateNetworks();

	return true;
}

void Hyperneat::backprop(const std::vector<float>& inputs, const std::vector<float>& outputs, float learnRate, std::vector<std::vector<float>> previousLayer, int outputIndex, NeuralNetwork* net)
{
	for (int j = 0; j < previousLayer.size(); j++)
	{
		std::vector<float> inputCppn, outputCppn;

		inputCppn.reserve(outputSubstrate[0].size() * 2);

		inputCppn.insert(inputCppn.end(), hiddenSubstrates.back()[j].begin(), hiddenSubstrates.back()[j].end());
		inputCppn.insert(inputCppn.end(), outputSubstrate[outputIndex].begin(), outputSubstrate[outputIndex].end());

		net->compute(inputCppn, outputCppn);

		if (hyperParam.thresholdFunction(hyperParam.thresholdVariables, outputCppn, hiddenSubstrates.back()[j], outputSubstrate[outputIndex]) == true)
		{
			float weight = hyperParam.weightModifierFunction(hyperParam.weightVariables, outputCppn[0], hiddenSubstrates.back()[j], outputSubstrate[outputIndex]);

			outputCppn[0] = weight;

			net->backprop(inputCppn, outputCppn, learnRate);
		}
	}
}

void Hyperneat::applyBackprop()
{
	for (int i = 0; i < networks.size(); i++)
	{
		networks[i].applyBackprop(cppns->getGenomes()[i]);
	}
}

std::vector<float> basicCppnInput(std::vector<void*> variables, std::vector<float> p1, std::vector<float> p2)
{
	p1.insert(p1.end(), p2.begin(), p2.end());
	return p1;
}

std::vector<float> biasCppnInput(std::vector<void*> variables, std::vector<float> p1, std::vector<float> p2)
{
	p1.insert(p1.end(), p2.begin(), p2.end());
	p1.push_back(1.0f);
	return p1;
}

std::vector<float> sqrDistCppnInput(std::vector<void*> variables, std::vector<float> p1, std::vector<float> p2)
{
	float dist = 0;

	for (int i = 0; i < p1.size(); i++)
	{
		
		dist += (p2[i] - p1[i]) * (p2[i] - p1[i]);
	}

	p1.insert(p1.end(), p2.begin(), p2.end());
	p1.push_back(dist);

	return p1;
}

std::vector<float> invDistCppnInput(std::vector<void*> variables, std::vector<float> p1, std::vector<float> p2)
{
	float dist = 0;

	for (int i = 0; i < p1.size(); i++)
	{
		dist += (p2[i] - p1[i]) * (p2[i] - p1[i]);
	}

	p1.insert(p1.end(), p2.begin(), p2.end());

	dist = *((float *)variables[0]) - sqrt(dist);

	p1.push_back(dist);

	return p1;
}

std::vector<float> deltaDistCppnInput(std::vector<void*> variables, std::vector<float> p1, std::vector<float> p2)
{
	p1.insert(p1.end(), p1.begin(), p1.end());

	for (int i = 0; i < p2.size(); i++)
	{
		p1.push_back((p2[i] - p1[i]));
	}

	return p1;
}