// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <vector>
#include "NeuralNetwork.h"
#include "Genome.h"
#include "Activation.h"

typedef struct {
	float pbMutateLink;//Probability of each mutation
	float pbMutateNode;
	float pbWeightShift;
	float pbWeightRandom;
	float pbToggleLink;
	float weightShiftStrength;//Max values for weight shift and new random one 
	float weightRandomStrength;
	float pbMutateActivation;
	std::vector<Activation*> activationFunctions;

	float C1, C2, C3, C4;//C4 is for CPPN-Neat
	float speciationDistance;
	
	bool bestHigh;

	float survivors;

} NeatParameters;

inline bool inSpeciesSortWeakOrder(Genome* i, Genome* j) { return (i->getScore() < j->getScore()); };

inline bool inSpeciesSortStrongOrder(Genome* i, Genome* j) { return (i->getScore() > j->getScore()); };

/**
 * 
 */
class NEAT_API NeatAlgoGen
{
public:
	NeatAlgoGen();//Just for testing purposes
	NeatAlgoGen(unsigned int _populationSize, unsigned int _input, unsigned int _output, NeatParameters _neatParam);
	~NeatAlgoGen();

	inline NeuralNetwork* getNeuralNetwork(int i) { return &networks[i]; };
	inline int getPopulationSize() { return networks.size(); };

	virtual float distance(Genome& genomeA, Genome& genomeB);

	void evolve();
	void genSpecies();
	void kill();
	void removeExtinctSpecies();
	void reproduce();
	virtual void mutate(Genome& genome);
	void generateNetworks();

	void breed(Genome* child, std::deque<Genome*>* currentSpecies);
	void pick(std::deque<Genome*>* currentSpecies, Genome* parentA, Genome* parentB);

	friend class ANeuralNetworkDisplayHUD;

protected:
	std::vector<NeuralNetwork> networks;
	std::vector <Genome> genomes;
	std::vector < unsigned int > scores;
	std::unordered_map<std::pair<unsigned int, unsigned int>, unsigned int> allConnections;//Innovation number starts at 0
	unsigned int populationSize, input, output;
	NeatParameters neatParam;
	std::list<std::deque<Genome*>> species;//First genome of the vector is used as the representative of the species
	std::list<unsigned int> speciesScore;
};
