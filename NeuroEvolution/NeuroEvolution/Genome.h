// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <map>
#include <mutex>
#include <fstream>
#include <unordered_map>

#include "Utils.h"
#include "GeneNode.h"
#include "GeneConnection.h"

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

	enum class DataToSaveEnum {
		GENECONNECTION = 0,
		GENENODE = 1
	};

	enum class CROSSOVER { RANDOM, AVERAGE, SINGLE_POINT };

	bool mutateLink(std::unordered_map<std::pair<unsigned int, unsigned int>, unsigned int>& allConnections, std::mutex* lock = nullptr);
	bool mutateNode(std::unordered_map<std::pair<unsigned int, unsigned int>, unsigned int>& allConnections, Activation* activationFunction, std::mutex* lock = nullptr);
	void mutateLinkToggle();
	void mutateActivation(std::vector<Activation*>& activationFunctions);
	void mutateWeights(float power, float rate, WEIGHT_MUTATOR mutType);

	inline std::map<unsigned int, GeneConnection>* const getConnections() { return &connections; };
	inline std::unordered_map<std::pair<unsigned int, unsigned int>, unsigned int>* getNodesToConn() { return &nodesToConnection; };
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
	void saveCurrentGenome(const std::string& fileName = "saveGenome.txt");
	void addHiddenNode(Activation* activation, unsigned int layer);
	static Genome loadGenome(const std::string& fileName = "saveGenome.txt");

	//Parent A should be the fittest
	void crossover(Genome& parentA, Genome& parentB, CROSSOVER type);

	void addConnection(unsigned int nodeA, unsigned int nodeB, std::unordered_map<std::pair<unsigned int, unsigned int>, unsigned int>& allConnections, float weight = 1, std::mutex* lock = nullptr);

	std::string toString();

private:

	GeneNode loadGeneNode(const std::vector<std::string> data);
	GeneConnection loadGeneConnection(const std::vector<std::string> data);

	struct DataToSaveStruct {
		DataToSaveEnum dataTypeEnum;
		const GeneNode* geneNode;
		const GeneConnection* geneConnection;
		int idGeneConnection;

		DataToSaveStruct(const GeneNode* geneNode)
			: geneNode(geneNode),
			dataTypeEnum(DataToSaveEnum::GENENODE),
			idGeneConnection(-1),
			geneConnection(nullptr)
		{};

		DataToSaveStruct(const GeneConnection* geneConnection, const unsigned int id)
			: geneConnection(geneConnection),
			dataTypeEnum(DataToSaveEnum::GENECONNECTION),
			idGeneConnection(id),
			geneNode(nullptr)
		{};

		~DataToSaveStruct()
		{
		}

		friend std::ostream& operator<<(std::ostream& os, const DataToSaveStruct& data)
		{
			os << (int)data.dataTypeEnum;
			os << " ";

			if (data.dataTypeEnum == Genome::DataToSaveEnum::GENECONNECTION)
				os << *data.geneConnection;
			else
				os << *data.geneNode;

			os << " ";
			if (data.idGeneConnection >= 0)
				os << data.idGeneConnection;
			os << " ";
			os << '\t' << '\n';

			return os;
		};
	};

	void shiftNodes(unsigned int node, unsigned int layerMin);

	unsigned int input, output;

	std::map<unsigned int, GeneConnection> connections;
	std::unordered_map<std::pair<unsigned int, unsigned int>, unsigned int> nodesToConnection;//From a pair of nodes to innovation number of connection
	std::vector<unsigned int> orderAddedCon;
	std::vector<GeneNode> nodes;
	float score = 0;
	float speciesScore = 0;
	int superChampOffspring = 0;
	bool eliminate = false;
};

inline bool genomeSortDesc(Genome* i, Genome* j) { return (i->getScore() > j->getScore()); };

inline bool genomeSortAsc(Genome* i, Genome* j) { return (i->getScore() < j->getScore()); };