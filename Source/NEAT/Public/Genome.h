// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GeneConnection.h"
#include "GeneNode.h"
#include <deque>
#include <unordered_map>

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
	Genome(int input, int output);
	~Genome();

	bool mutateLink(std::unordered_map<std::pair<int, int>, int>& allConnections);
	bool mutateNode(std::unordered_map<std::pair<int, int>, int>& allConnections);
	void mutateWeightShift(float weightShiftStrength);
	void mutateWeightRandom(float weightRandomStrength);
	void mutateLinkToggle();

	inline std::unordered_map<int, GeneConnection>* const getConnections() { return &connections; };
	inline std::deque<GeneNode>* const getNodes() { return &nodes; };


private:
	void addConnection(int nodeA, int nodeB, std::unordered_map<std::pair<int, int>, int>& allConnections);

	std::unordered_map<int, GeneConnection> connections;
	std::unordered_map<std::pair<int, int>, GeneConnection*> nodesToConnection;
	std::deque<GeneNode> nodes;
};
