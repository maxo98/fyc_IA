// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include <vector>
#include "NeuralNetwork.h"
#include "Species.h"
#include "Activation.h"
#include <mutex>

typedef struct {
	float pbMutateOnly;
	float pbMutateLink;//Probability of each mutation
	float pbMutateNode;
	float pbWeightShift;
	float pbWeightRandom;
	float pbToggleLink;
	float weightShiftStrength;//Max values for weight shift and new random one 
	float weightRandomStrength;
	float pbMutateActivation;

	float pbMateMultipoint;
	float pbMateSinglepoint;
	float interspeciesMateRate;
	float pbMateOnly;
	
	float killRate;
	int dropOffAge;
	float ageSignificance;
	

	std::vector<Activation*> activationFunctions;

	float disjointCoeff, excessCoeff, mutDiffCoeff, activationDiffCoeff;
	float speciationDistance;
	float speciationDistanceMod;
	int numSpeciesTarget;
	bool adaptSpeciation;
	
	bool bestHigh;//False doesn't currently work

	std::string fileSave;//Without extension type file
	bool saveHistory;

	float scoreMultiplier;

} NeatParameters;


struct speciesSortDesc
{
	bool operator ()(const Species* i, const Species* j)
	{
		return (i->getMaxFitness() > j->getMaxFitness());
	}
};

struct speciesSortAsc
{
	bool operator ()(const Species* i, const Species* j)
	{
		return (i->getMaxFitness() < j->getMaxFitness());
	}
};

inline bool genomeSortDesc(Genome* i, Genome* j) { return (i->getScore() > j->getScore()); };

inline bool genomeSortAsc(Genome* i, Genome* j) { return (i->getScore() < j->getScore()); };


/**
 * 
 */
class NeatAlgoGen
{
public:
	NeatAlgoGen();
	NeatAlgoGen(unsigned int _populationSize, unsigned int _input, unsigned int _output, NeatParameters _neatParam);
	~NeatAlgoGen();

	inline NeuralNetwork* getNeuralNetwork(int i) { return &networks[i]; };
	inline int getPopulationSize() { return networks.size(); };

	void evolve();
	void adjustFitness();
	void addToSpecies(Genome* gen);
	void reproduce(std::list<Species*>::iterator& it, std::mutex& lock, int& sharedNewBornIndex, std::list<Species*>& sortedSpecies, Genome* newPop);
	
	void kill();
	void removeExtinctSpecies();
	virtual void mutate(Genome& genome);
	void generateNetworks();

	virtual float distance(Genome& genomeA, Genome& genomeB);

	void setScore(std::vector < float > newScores);
	bool saveHistory();

	friend class ANeuralNetworkDisplayHUD;

	//Taken from official implementation, no idea how it works
	float gaussRand();

protected:
	std::vector<NeuralNetwork> networks;
	Genome* genomes;
	std::unordered_map<std::pair<unsigned int, unsigned int>, unsigned int> allConnections;//Innovation number starts at 0
	unsigned int populationSize, input, output;
	NeatParameters neatParam;
	unsigned int highestLastChanged;
	float lastBestScore = 0;

	std::vector<Species> species;
	int generation = 0;

	std::vector<float> history;
	Genome goat;
};
