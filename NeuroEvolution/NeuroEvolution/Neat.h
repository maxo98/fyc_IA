// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include <vector>
#include "NeuralNetwork.h"
#include "Species.h"
#include "Activation.h"

//#define REPRO_MULTITHREAD

typedef struct {
	float pbMutateOnly;
	float pbMutateLink;//Probability of each mutation
	float pbMutateNode;
	//float pbWeightShift;
	//float pbWeightRandom;
	float pbWeight;
	float pbToggleLink;
	//float weightShiftStrength;//Max values for weight shift and new random one 
	//float weightRandomStrength;
	float weightMuteStrength;
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

	std::string champFileSave, avgFileSave;//Without extension type file
	bool saveChampHistory;
	bool saveAvgHistory;

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
class Neat
{
public:
	
	enum class INIT{NONE, ONE, FULL};

	Neat();
	Neat(unsigned int _populationSize, unsigned int _input, unsigned int _output, const NeatParameters& _neatParam, INIT init = INIT::ONE);
	~Neat();

	void fullConnectInit(Genome& gen);
	void oneConnectionInit(Genome& gen);

	inline NeuralNetwork* getNeuralNetwork(int i) { return &networks[i]; };
	inline int getPopulationSize() { return networks.size(); };

	void evolve();
	void adjustFitness();
	void addToSpecies(Genome* gen);
	void reproduce(int workload, std::list<Species*>::iterator it, int newBornIndex, std::list<Species*>& sortedSpecies, Genome* newPop, std::mutex* lock);
	
	virtual void mutate(Genome& genome, std::mutex* lock = nullptr);
	void generateNetworks();
	void genomeToNetwork(Genome& genome, NeuralNetwork& network);

	virtual float distance(Genome& genomeA, Genome& genomeB);

	void setScore(const std::vector < float >& newScores);
	bool saveHistory();

	inline Genome* getGenomes() { return genomes; };
	inline unsigned int getOutput() { return output; };

	friend class ANeuralNetworkDisplayHUD;

	Genome* getGoat() { return &goat; };

	//Taken from official implementation, no idea how it works
	float gaussRand();

protected:
	std::vector<NeuralNetwork> networks;
	Genome* genomes = nullptr;
	//unordered_map seems bugged
	std::unordered_map<std::pair<unsigned int, unsigned int>, unsigned int> allConnections;//Innovation number starts at 0
	unsigned int populationSize = 0, input = 0, output = 0;
	NeatParameters neatParam;
	unsigned int highestLastChanged = 0;
	float lastBestScore = 0;

	std::vector<Species> species;
	int generation = 0;

	std::vector<float> champHistory;
	std::vector<float> avgHistory;
	Genome goat;
};
