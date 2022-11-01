// Fill out your copyright notice in the Description page of Project Settings.


#include "NeatAlgoGen.h"

NeatAlgoGen::NeatAlgoGen(unsigned int _populationSize, unsigned int _input, unsigned int _output, NeatParameters _neatParam)
{
	populationSize = _populationSize;
	input = _input;
	output = _output;

	neatParam = _neatParam;

	networks.resize(populationSize);
	genomes.reserve(populationSize);


	for (unsigned int i = 0; i < populationSize; i++)
	{
		genomes.push_back(Genome(input, output, neatParam.activationFunctions));
		Genome* genome = &genomes.back();
		genome->mutateLink(allConnections);//Minimum structure
		genome->mutateWeightRandom(neatParam.pbWeightRandom);
	}
}

NeatAlgoGen::NeatAlgoGen()
{
	Activation* test = new sigmoidActivation();

	if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 60.f, FColor::Red, FString::Printf(TEXT("%f"), test->activate(1)));

	delete test;

	/*neatParam.activationFunctions.push_back(sigmoidActivation());

	populationSize = 3;
	input = 3;
	output = 3;

	networks.resize(populationSize);
	genomes.reserve(populationSize);


	for (unsigned int i = 0; i < populationSize; i++)
	{
		genomes.push_back(Genome(input, output, neatParam.activationFunctions));
		Genome* genome = &genomes.back();
		genome->mutateLink(allConnections);
		genome->mutateNode(allConnections, neatParam.activationFunctions[0]);
		genome->mutateLink(allConnections);
		genome->mutateNode(allConnections, neatParam.activationFunctions[0]);
		genome->mutateLink(allConnections);
		genome->mutateNode(allConnections, neatParam.activationFunctions[0]);
		genome->mutateLink(allConnections);
		genome->mutateNode(allConnections, neatParam.activationFunctions[0]);
		genome->mutateLink(allConnections);
		genome->mutateNode(allConnections, neatParam.activationFunctions[0]);
		genome->mutateWeightRandom(2);
		genome->mutateWeightShift(0.5);
		genome->mutateWeightRandom(2);
		genome->mutateWeightShift(0.5);
		genome->mutateWeightRandom(2);
		genome->mutateWeightShift(0.5);
		genome->mutateLinkToggle();
		genome->mutateLinkToggle();
	}

	genomes[2].crossover(genomes[0], genomes[1]);

	generateNetworks();

	std::vector<float> inputNet;

	inputNet.push_back(0);
	inputNet.push_back(0);
	inputNet.push_back(0);

	std::vector<float> outputNet;

	outputNet.push_back(0);
	outputNet.push_back(0);
	outputNet.push_back(0);

	networks[0].compute(inputNet, outputNet);*/
}

NeatAlgoGen::~NeatAlgoGen()
{
}

void NeatAlgoGen::mutate(Genome& genome)
{
	if (neatParam.activationFunctions.size() == 0)
		return;

	if (neatParam.pbMutateLink > rand() % 1) {
		genome.mutateLink(allConnections);
	}
	
	if (neatParam.pbMutateNode > rand() % 1) {

		unsigned int index = rand() % neatParam.activationFunctions.size();
		genome.mutateNode(allConnections, neatParam.activationFunctions[index]);
	}
	
	if (neatParam.pbWeightShift > rand() % 1) {
		genome.mutateWeightShift(neatParam.pbWeightShift);
	}
	
	if (neatParam.pbWeightRandom > rand() % 1) {
		genome.mutateWeightRandom(neatParam.pbWeightRandom);
	}
	
	if (neatParam.pbToggleLink > rand() % 1) {
		genome.mutateLinkToggle();
	}
}

void NeatAlgoGen::generateNetworks()
{

	for (unsigned int cpt = 0; cpt < genomes.size(); cpt++)
	{
		networks[cpt].clear();

		std::deque<GeneNode>* nodes = genomes[cpt].getNodes();
		std::vector<std::pair<unsigned int, unsigned int>> nodePosition;//Stores postion of the nodes in the network
		nodePosition.reserve(nodes->size());

		//Add the nodes to the layer
		for (std::deque<GeneNode>::iterator node = nodes->begin(); node != nodes->end(); ++node)
		{
			unsigned int layer = node->getLayer();

			switch(node->getType())
			{
			case NODE_TYPE::HIDDEN:
				nodePosition.push_back(std::pair<unsigned int, unsigned int>(layer, networks[cpt].getNHiddenNode(layer)));
				networks[cpt].addHiddenNode(layer, node->getActivation());
				break;

			case NODE_TYPE::INPUT:
				nodePosition.push_back(std::pair<unsigned int, unsigned int>(0, networks[cpt].getNInputNode()));
				networks[cpt].addInputNode();
				break;
				
			case NODE_TYPE::OUTPUT:
				nodePosition.push_back(std::pair<unsigned int, unsigned int>(-1, networks[cpt].getNOutputNode()));
				networks[cpt].addOutputNode(node->getActivation());
				break;
			}
		}

		//Need to find on which layer the output nodes really are
		for (unsigned int i = 0; i < nodePosition.size(); i++)
		{
			if (nodePosition[i].first == -1)
			{
				nodePosition[i].first = networks[cpt].getLayerSize() - 1;
			}
		}

		std::unordered_map<unsigned int, GeneConnection>* connections = genomes[cpt].getConnections();

		for (std::unordered_map<unsigned int, GeneConnection>::iterator connection = connections->begin(); connection != connections->end(); ++connection)
		{
			if (connection->second.isEnabled() == true)
			{
				//if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 60.f, FColor::Red, FString::Printf(TEXT("%i %i %i %i"), nodePosition[connection->second.getNodeA()].first, nodePosition[connection->second.getNodeA()].second, nodePosition[connection->second.getNodeB()].first, nodePosition[connection->second.getNodeB()].second));
				//GEngine->AddOnScreenDebugMessage(-1, 60.f, FColor::Red, FString::Printf(TEXT("%i %i"), connection->second.getNodeA(), nodes->size()));

				networks[cpt].connectNodes(nodePosition[connection->second.getNodeA()], nodePosition[connection->second.getNodeB()], connection->second.getWeight());
			}
		}

		//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 60.f, FColor::Red, "End network");
	}
}