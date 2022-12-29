// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#define DEBUG
#define MULTITHREAD

#include <vector>
#include "NeuralNetwork.h"
#include "Species.h"
#include "Activation.h"

typedef struct {
	float pbMutateOnly;
	float pbMutateLink;//Probability of each mutation
	float pbMutateNode;
	float pbWeight;
	float pbToggleLink;
	float weightMuteStrength;//Max values for weight shift and new random one 
	float pbMutateActivation;

	float pbMateMultipoint;
	float pbMateSinglepoint;
	float interspeciesMateRate;
	float pbMateOnly;
	
	float killRate;
	int dropOffAge;
	float ageSignificance;
	
	bool keepChamp;//Ensures that the pop champ gets to reproduce unless he's to old
	bool elistism;//True => roulette favoring higher score, False => uniform rand
	float rouletteMultiplier;

	std::vector<Activation*> activationFunctions;

	float disjointCoeff, excessCoeff, mutDiffCoeff, activationDiffCoeff, weightCoeff;
	float speciationDistance;

	float speciationDistanceMod;
	int minExpectedSpecies;//Setting expected species to high values who'd probably enforce diversity
	int maxExpectedSpecies;//if you don't want to go through novelty search but still avoid deceptive local maximum
	bool adaptSpeciation;//Setting it to true would lead to more diversity I believe

	std::string champFileSave, avgFileSave;//Without extension type file
	bool saveChampHistory;
	bool saveAvgHistory;

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


/**
 * 
 */
class Neat
{
public:
	
	enum class INIT{NONE, ONE, FULL};

	Neat();
	Neat(unsigned int _populationSize, unsigned int _input, unsigned int _output, const NeatParameters& _neatParam, INIT init = INIT::ONE);
	Neat(unsigned int _populationSize, unsigned int _input, unsigned int _output, const NeatParameters& _neatParam, std::vector<Genome>& initPop);
	~Neat();

	bool init(unsigned int _populationSize, unsigned int _input, unsigned int _output, const NeatParameters& _neatParam);
	inline bool isInit() { return initialized; }

	void fullConnectInit(Genome& gen);
	void oneConnectionInit(Genome& gen);

	inline NeuralNetwork* getNeuralNetwork(int i) { return &networks[i]; };
	inline int getPopulationSize() { return networks.size(); };

	void evolve();
	void adjustFitness();
	void addToSpecies(Genome* gen);
	void reproduce(int workload, std::list<Species*>::iterator it, int newBornIndex, std::list<Species*>& sortedSpecies, Genome* newPop, std::mutex* lock, std::atomic<bool>* ticket = nullptr);
	
	virtual void mutate(Genome& genome, std::mutex* lock = nullptr);
	void generateNetworks();
	static void genomeToNetwork(Genome& genome, NeuralNetwork& network);

	virtual float distance(Genome& genomeA, Genome& genomeB);

	void setScore(const std::vector < float >& newScores);
	bool saveHistory();

	inline Genome* getGenomes() { return genomes; };
	inline unsigned int getOutput() { return output; };

	friend class ANeuralNetworkDisplayHUD;

	Genome* getGoat() { return &goat; };
	void setGoat(Genome _goat) { goat = _goat; };

	//Taken from official implementation, no idea how it works
	float gaussRand();

protected:
	std::vector<NeuralNetwork> networks;
	Genome* genomes = nullptr, *futureGen = nullptr;

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

	bool initialized = false;
};
