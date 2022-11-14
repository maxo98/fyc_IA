// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "HyperNeat.h"
#include <cstring>

//Hyperneat configuration
typedef struct {
	unsigned int nDecomposition;
	float width;
	int initialDepth, maxDepth, divThreshold;
	std::vector<float> center;
	float varianceThreshold;
	float bandThreshold;
	int iterationLevel;

} ES_Parameters;


/**
 * 
 */
class ES_Hyperneat: public Hyperneat
{
public:
	ES_Hyperneat(unsigned int _populationSize, const NeatParameters& _neatParam, const HyperneatParameters& _hyperParam, const ES_Parameters& _esParam);
	~ES_Hyperneat();

	virtual void generateNetworks();
	void generateSubstrate();

	inline void setCenter(std::vector<float> _center) { esParam.center = _center; }
	inline void setWidth(float _width) { esParam.width = _width; }

protected:
	ES_Parameters esParam;

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

	void divAndInit(int index, const std::vector<float>& pos, SubstrateTree* root, bool outgoing);
	void prunAndExtract(int index, const std::vector<float>& pos, SubstrateTree* tree, bool outgoing, std::unordered_set<Connection, HyperConnectionHash,
		HyperConnectionEqual>& connections);

	void reachableConnections(const std::unordered_multimap<std::vector<float>, const Connection*, HyperNodeHash>& connections,
		std::unordered_set<std::vector<float>, HyperNodeHash> nodes,
		std::unordered_set<Connection, HyperConnectionHash, HyperConnectionEqual>& reachableConn);

	//breadth first search
	bool bfs(const std::vector<float>& pos, const std::vector<float>& endPos,
		const std::unordered_multimap<std::vector<float>, const Connection*, HyperNodeHash>& connectionMap);

	void shiftNodes(const std::vector<float>& startPos,
		const std::unordered_multimap<std::vector<float>, const Connection*, HyperNodeHash>& connectionMap,
		std::unordered_map<std::vector<float>, int, HyperNodeHash> nodesLayerMap);
};

