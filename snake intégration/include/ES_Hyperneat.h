// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HyperNeat.h"


//Hyperneat configuration
typedef struct {
	float width;
	int initialDepth, maxDepth, divThreshold;
	//The initial resolution for the division phase of ES-HyperNEAT
	//maxDepth: The maximum resolution for the division phase of ES-HyperNEAT.
	//divThreshold: The variance value that a quadtree node in ES - HyperNEAT must exceed to be further divided.
	std::vector<float> center;
	float varianceThreshold;//The variance value that determines how far the depth-first search in the pruning and extraction phase in ES-HyperNEAT should traverse the quadtree.
	float bandThreshold;//The value that the band level of a connection must exceed to be expressed in ES-HyperNEAT.
	int iterationLevel;//The parameter that determines how many times the quadtree extraction algorithm in ES-HyperNEAT should be iteratively applied to discovering hidden neurons.

	bool allowRecurisvity;

} ES_Parameters;

struct HyperNodeHash {
public:
	size_t operator()(const std::vector<float>& node) const {
		if (node.size() == 0) return 0;

		std::size_t value = 0;
		std::memcpy(&value, &node[0], std::min(sizeof(float), sizeof(std::size_t)));

		for (int i = 1; i < node.size(); i++)
		{
			std::size_t tmp = 0;
			std::memcpy(&value, &node[i], std::min(sizeof(float), sizeof(std::size_t)));

			value = value ^ (tmp << i);
		}

		return value; // or use boost::hash_combine
	}
};


/**
 * 
 */
class ES_Hyperneat: public Hyperneat
{
public:
	ES_Hyperneat(unsigned int _populationSize, const NeatParameters& _neatParam, const HyperneatParameters& _hyperParam, const ES_Parameters& _esParam, Neat::INIT init = Neat::INIT::ONE);
	~ES_Hyperneat();

	virtual void createNetwork(NeuralNetwork& hypernet, NeuralNetwork& net);

	inline void setCenter(std::vector<float> _center) { esParam.center = _center; }
	inline void setWidth(float _width) { esParam.width = _width; }

protected:
	virtual void initNetwork(NeuralNetwork& net);

	ES_Parameters esParam;
	std::unordered_map<std::vector<float>, std::pair<unsigned int, unsigned int>, HyperNodeHash> nodesPosInput, nodesPosOutput;

	class Connection {
	public:
		Connection(std::vector<float> _pos1, std::vector<float> _pos2, float _weight);

		std::vector<float> pos1;
		std::vector<float> pos2;
		float weight;
		bool recursive = false;
	};

	class SubstrateTree {
	public:
		SubstrateTree(std::vector<float> _pos, float _width, float _level);

		std::vector<float> pos;
		float level;
		float weight;
		int variance;
		float width;
		bool active;

		std::list<SubstrateTree> leaves;
	};

	struct HyperConnectionHash {
	public:
		size_t operator()(const Connection& conn) const {
			if (conn.pos1.size() == 0) return 0;

			std::size_t value = 0;
			std::memcpy(&value, &conn.pos1[0], std::min(sizeof(float), sizeof(std::size_t)));

			for (int i = 1; i < conn.pos1.size(); i++)
			{
				std::size_t tmp = 0;
				std::memcpy(&value, &conn.pos1[i], std::min(sizeof(float), sizeof(std::size_t)));

				value = value ^ (tmp << i);
			}

			for (int i = 0; i < conn.pos2.size(); i++)
			{
				std::size_t tmp = 0;
				std::memcpy(&value, &conn.pos2[i], std::min(sizeof(float), sizeof(std::size_t)));

				value = value ^ (tmp << i);
			}

			return value; // or use boost::hash_combine
		}
	};

	struct HyperConnectionEqual {
	public:
		bool operator()(const Connection& first, const Connection& second) const {
			return (first.pos1 == second.pos1 && first.pos2 == second.pos2);
		}
	};

	void divAndInit(NeuralNetwork& hypernet, const std::vector<float>& pos, SubstrateTree* root, bool outgoing);
	void prunAndExtract(NeuralNetwork& hypernet, const std::vector<float>& pos, SubstrateTree* tree, bool outgoing, std::unordered_set<Connection, HyperConnectionHash,
		HyperConnectionEqual>& connections);

	void connectNodes(std::unordered_map<std::vector<float>, std::pair<unsigned int, unsigned int>, HyperNodeHash>& nodesPos,
		std::unordered_multimap<std::vector<float>, const Connection*, HyperNodeHash>& connectionMap, NeuralNetwork& hypernet, NeuralNetwork& net,
		std::unordered_map<std::vector<float>, std::pair<unsigned int, unsigned int>, HyperNodeHash>* prevNodesPos = nullptr);
};

