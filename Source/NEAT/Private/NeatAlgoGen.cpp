// Fill out your copyright notice in the Description page of Project Settings.


#include "NeatAlgoGen.h"

NeatAlgoGen::NeatAlgoGen(int _populationSize, int _input, int _output, float _pbMutateLink, float _pbMutateNode, float _pbWeightShift, float _pbWeightRandom, float _pbToggleLink, float _weightShiftStrength, float _weightRandomStrength)
{
	populationSize = _populationSize;
	input = _input;
	output = _output;

	pbMutateLink = _pbMutateLink;
	pbMutateNode = _pbMutateNode;
	pbWeightShift = _pbWeightShift;
	pbWeightRandom = _pbWeightRandom;
	pbToggleLink = _pbToggleLink;

	weightShiftStrength = _weightShiftStrength;
	weightRandomStrength = _weightRandomStrength;

	networks.resize(populationSize);
	genomes.reserve(populationSize);


	for (int i = 0; i < populationSize; i++)
	{
		genomes.push_back(Genome(input, output));
		Genome* genome = &genomes.back();
		genome->mutateLink(allConnections);//Minimum structure
	}
}

NeatAlgoGen::NeatAlgoGen()
{

	populationSize = 3;
	input = 3;
	output = 3;

	pbMutateLink = 0;
	pbMutateNode = 0;
	pbWeightShift = 0;
	pbWeightRandom = 0;
	pbToggleLink = 0;

	weightShiftStrength = 0;
	weightRandomStrength = 0;

	networks.resize(populationSize);
	genomes.reserve(populationSize);


	for (int i = 0; i < populationSize; i++)
	{
		genomes.push_back(Genome(input, output));
		Genome* genome = &genomes.back();
		genome->mutateLink(allConnections);
		genome->mutateNode(allConnections);
		genome->mutateLink(allConnections);
		genome->mutateNode(allConnections);
		genome->mutateLink(allConnections);
		genome->mutateNode(allConnections);
		genome->mutateLink(allConnections);
		genome->mutateNode(allConnections);
		genome->mutateLink(allConnections);
		genome->mutateNode(allConnections);
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
}

NeatAlgoGen::~NeatAlgoGen()
{
}

void NeatAlgoGen::mutate(Genome& genome)
{
	if (pbMutateLink > rand() % 1) {
		genome.mutateLink(allConnections);
	}
	
	if (pbMutateNode > rand() % 1) {
		genome.mutateNode(allConnections);
	}
	
	if (pbWeightShift > rand() % 1) {
		genome.mutateWeightShift(pbWeightShift);
	}
	
	if (pbWeightRandom > rand() % 1) {
		genome.mutateWeightRandom(pbWeightShift);
	}
	
	if (pbToggleLink > rand() % 1) {
		genome.mutateLinkToggle();
	}
}

void NeatAlgoGen::generateNetworks()
{

	for (int cpt = 0; cpt < genomes.size(); cpt++)
	{
		networks[cpt].clear();

		std::deque<GeneNode>* nodes = genomes[cpt].getNodes();
		std::vector<std::pair<int, int>> nodePosition;//Stores postion of the nodes in the network
		nodePosition.reserve(nodes->size());

		//Add the nodes to the layer
		for (std::deque<GeneNode>::iterator node = nodes->begin(); node != nodes->end(); ++node)
		{
			int layer = node->getLayer();

			switch(node->getType())
			{
			case NODE_TYPE::HIDDEN:
				nodePosition.push_back(std::pair<int, int>(layer, networks[cpt].getNHiddenNode(layer)));
				networks[cpt].addHiddenNode(layer);
				break;

			case NODE_TYPE::INPUT:
				nodePosition.push_back(std::pair<int, int>(0, networks[cpt].getNInputNode()));
				networks[cpt].addInputNode();
				break;
				
			case NODE_TYPE::OUTPUT:
				nodePosition.push_back(std::pair<int, int>(-1, networks[cpt].getNOutputNode()));
				networks[cpt].addOutputNode();
				break;
			}
		}

		//Need to find on which layer the output nodes really are
		for (int i = 0; i < nodePosition.size(); i++)
		{
			if (nodePosition[i].first == -1)
			{
				nodePosition[i].first = networks[cpt].getLayerSize() - 1;
			}
		}

		std::unordered_map<int, GeneConnection>* connections = genomes[cpt].getConnections();

		for (std::unordered_map<int, GeneConnection>::iterator connection = connections->begin(); connection != connections->end(); ++connection)
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