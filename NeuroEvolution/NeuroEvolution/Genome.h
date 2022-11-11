// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "GeneConnection.h"
#include "GeneNode.h"
#include <deque>
#include <map>
#include <unordered_map>
#include <iostream>
#include <utility>

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

inline float randFloat() { return ((double)rand()) / RAND_MAX; }

inline int randInt(int x, int y) { return rand() % (y - x + 1) + x; };

class CPPN_Neat;

/**
 * 
 */
class Genome
{
public:
	Genome();
	Genome(unsigned int input, unsigned int output, std::vector<Activation*> activationFunctions);
	~Genome();

	enum class CROSSOVER { RANDOM, AVERAGE, SINGLE_POINT };

	bool mutateLink(std::unordered_map<std::pair<unsigned int, unsigned int>, unsigned int>& allConnections);
	bool mutateNode(std::unordered_map<std::pair<unsigned int, unsigned int>, unsigned int>& allConnections, Activation* activationFunction);
	void mutateWeightShift(float weightShiftStrength);
	void mutateWeightRandom(float weightRandomStrength);
	void mutateLinkToggle();
	void mutateActivation(Activation* activationFunction);

	inline std::map<unsigned int, GeneConnection>* const getConnections() { return &connections; };
	inline std::vector<GeneNode>* const getNodes() { return &nodes; };
	inline void setScore(float _score) { score = _score; };
	inline float getScore() { return score; };
	inline void setSpeciesScore(float _score) { speciesScore = _score; };
	inline float getSpeciesScore() { return speciesScore; };
	inline void setSuperChampOffspring(int value) { superChampOffspring = value; };
	inline int getSuperChampOffspring() { return superChampOffspring; };
	inline void decrementSuperChampOffspring() { superChampOffspring--; };
	inline void setEliminate(bool value) { eliminate = value; };
	inline bool getEliminate() { return eliminate; };

	//Parent A should be the fittest
	void crossover(Genome& parentA, Genome& parentB, CROSSOVER type);

	friend class CPPN_Neat;

private:
	void addConnection(unsigned int nodeA, unsigned int nodeB, std::unordered_map<std::pair<unsigned int, unsigned int>, unsigned int>& allConnections);

	void shiftNodes(unsigned int node, unsigned int layerMin);

	unsigned int input;

	std::map<unsigned int, GeneConnection> connections;
	std::unordered_map<std::pair<unsigned int, unsigned int>, unsigned int> nodesToConnection;//From a pair of nodes to innovation number of connection
	std::vector<GeneNode> nodes;
	float score = 0;
	float speciesScore = 0;
	int superChampOffspring = 0;
	bool eliminate = false;
};