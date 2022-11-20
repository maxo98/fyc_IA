// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GeneConnection.h"
#include "GeneNode.h"
#include <map>
#include <unordered_map>
#include <mutex>
#include "Utils.h"

class CPPN_Neat;

/**
 * 
 */
class Genome
{
public:
	Genome();
	Genome(unsigned int input, unsigned int output, std::vector<Activation*> activationFunctions, bool cppn = false);
	~Genome();

	enum class WEIGHT_MUTATOR 
	{
		GAUSSIAN = 0,
		COLDGAUSSIAN = 1
	};

	enum class CROSSOVER { RANDOM, AVERAGE, SINGLE_POINT };

	bool mutateLink(std::unordered_map<std::pair<unsigned int, unsigned int>, unsigned int>& allConnections, std::mutex* lock = nullptr);
	bool mutateNode(std::unordered_map<std::pair<unsigned int, unsigned int>, unsigned int>& allConnections, Activation* activationFunction, std::mutex* lock = nullptr);
	//void mutateWeightShift(float weightShiftStrength);
	//void mutateWeightRandom(float weightRandomStrength);
	void mutateLinkToggle();
	void mutateActivation(std::vector<Activation*>& activationFunctions);
	void mutateWeights(float power, float rate, WEIGHT_MUTATOR mutType);

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

	void addConnection(unsigned int nodeA, unsigned int nodeB, std::unordered_map<std::pair<unsigned int, unsigned int>, unsigned int>& allConnections, float weight = 1, std::mutex* lock = nullptr);

	std::string toString();

private:
	void shiftNodes(unsigned int node, unsigned int layerMin);

	unsigned int input;

	std::map<unsigned int, GeneConnection> connections;
	std::unordered_map<std::pair<unsigned int, unsigned int>, unsigned int> nodesToConnection;//From a pair of nodes to innovation number of connection
	std::vector<unsigned int> orderAddedCon;
	std::vector<GeneNode> nodes;
	float score = 0;
	float speciesScore = 0;
	int superChampOffspring = 0;
	bool eliminate = false;
};