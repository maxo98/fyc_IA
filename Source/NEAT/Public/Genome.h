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
struct std::hash<std::pair<unsigned int, unsigned int>>
{
	std::size_t operator()(std::pair<unsigned int, unsigned int> const& pair) const noexcept
	{
		return pair.first ^ (pair.second << 1); // or use boost::hash_combine
	}
};

class CPPN_Neat;

/**
 * 
 */
class NEAT_API Genome
{
public:
	Genome(unsigned int input, unsigned int output, std::vector<ActivationFunction> activationFunctions);
	~Genome();

	bool mutateLink(std::unordered_map<std::pair<unsigned int, unsigned int>, unsigned int>& allConnections);
	bool mutateNode(std::unordered_map<std::pair<unsigned int, unsigned int>, unsigned int>& allConnections, ActivationFunction activationFunction);
	void mutateWeightShift(float weightShiftStrength);
	void mutateWeightRandom(float weightRandomStrength);
	void mutateLinkToggle();
	void mutateActivation(ActivationFunction activationFunction);

	inline std::unordered_map<unsigned int, GeneConnection>* const getConnections() { return &connections; };
	inline std::deque<GeneNode>* const getNodes() { return &nodes; };

	//Parent A should be the fittest
	void crossover(Genome& parentA, Genome& parentB);

	friend class CPPN_Neat;

private:
	void addConnection(unsigned int nodeA, unsigned int nodeB, std::unordered_map<std::pair<unsigned int, unsigned int>, unsigned int>& allConnections);

	void shiftNodes(unsigned int node, unsigned int layerMin);

	unsigned int input;

	std::unordered_map<unsigned int, GeneConnection> connections;
	std::unordered_map<std::pair<unsigned int, unsigned int>, unsigned int> nodesToConnection;//From a pair of nodes to innovation number of connection
	std::deque<GeneNode> nodes;
};
