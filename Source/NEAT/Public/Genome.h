// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GeneConnection.h"
#include "GeneNode.h"
#include <deque>
#include <unordered_map>

//Needed for unordered map
// custom specialization of std::hash can be injected in namespace std
template<>
struct std::hash<std::pair<int, int>>
{
	std::size_t operator()(std::pair<int, int> const& pair) const noexcept
	{
		return pair.first ^ (pair.second << 1); // or use boost::hash_combine
	}
};


/**
 * 
 */
class NEAT_API Genome
{
public:
	Genome(int input, int output, std::vector<ActivationFunction> activationFunctions);
	~Genome();

	bool mutateLink(std::unordered_map<std::pair<int, int>, int>& allConnections);
	bool mutateNode(std::unordered_map<std::pair<int, int>, int>& allConnections, ActivationFunction activationFunction);
	void mutateWeightShift(float weightShiftStrength);
	void mutateWeightRandom(float weightRandomStrength);
	void mutateLinkToggle();
	void mutateActivation(ActivationFunction activationFunction);

	inline std::unordered_map<int, GeneConnection>* const getConnections() { return &connections; };
	inline std::deque<GeneNode>* const getNodes() { return &nodes; };

	//Parent A should be the fittest
	void crossover(Genome& parentA, Genome& parentB);

private:
	void addConnection(int nodeA, int nodeB, std::unordered_map<std::pair<int, int>, int>& allConnections);

	void shiftNodes(int node, int layerMin);

	int input;

	std::unordered_map<int, GeneConnection> connections;
	std::unordered_map<std::pair<int, int>, int> nodesToConnection;//From a pair of nodes to innovation number of connection
	std::deque<GeneNode> nodes;
};
