// Fill out your copyright notice in the Description page of Project Settings.


#include "ES_Hyperneat.h"
#include <algorithm>
#include <queue>

ES_Hyperneat::ES_Hyperneat(unsigned int _populationSize, NeatParameters _neatParam, HyperneatParameters _hyperParam, ES_Parameters _esParam):
	Hyperneat(_populationSize, _neatParam, _hyperParam)
{
	esParam = _esParam;
}

ES_Hyperneat::~ES_Hyperneat()
{
	delete hyperParam.activationFunction;
}

ES_Hyperneat::SubstrateTree::SubstrateTree(std::vector<float> _pos, float _width, float _level)
{
	pos = _pos;
	width = _width;
	level = _level;
}

ES_Hyperneat::Connection::Connection(std::vector<float> _pos1, std::vector<float> _pos2, float _weight)
{
	pos1 = _pos1;
	pos2 = _pos2;
	weight = _weight;
}

void ES_Hyperneat::generateNetworks()
{

}

void ES_Hyperneat::divAndInit(int index, const std::vector<float>& pos, SubstrateTree* root, bool outgoing)
{
	
	std::queue<SubstrateTree*> q;
	q.push(root);

	while (!q.empty())
	{
		SubstrateTree* point = q.front();
		q.pop();

		std::vector<float> newPos;

		float newWidth = point->width / 2;

		//Genereate the new leaves
		// x + newWidth, y + newWidth, etc
		// x + newWidth, y - newWidth, etc
		// x - newWidth, y + newWidth, etc
		// x - newWidth, y - newWidth, etc
		for (int i = 0; i < pos.size(); i++)
		{
			if (i == 0)
			{
				SubstrateTree first(std::vector<float>(), newWidth, point->level+1);
				first.pos.push_back(pos[0] + newWidth);
				point->leaves.push_back(first);

				SubstrateTree second(std::vector<float>(), newWidth, point->level + 1);
				second.pos.push_back(pos[0] - newWidth);
				point->leaves.push_back(second);
			}
			else {
				int size = point->leaves.size();
				std::list<SubstrateTree>::iterator leaf = point->leaves.begin();

				for(int cpt = 0; cpt < size; ++cpt, ++leaf)
				{
					point->leaves.push_back(*leaf);

					leaf->pos.push_back(pos[i] + newWidth);

					point->leaves.back().pos.push_back(pos[i] - newWidth);
				}
			}
		}

		//For each leaf check if the connection is active
		//And compute weight
		int nActive = 0;

		for (std::list<SubstrateTree>::iterator leaf = point->leaves.begin(); leaf != point->leaves.end(); ++leaf)
		{
			std::vector<float> input, output;

			//Create input vector
			if (outgoing == true)
			{
				input = hyperParam.cppnInputFunction(hyperParam.inputVariables, pos, point->pos);
			}
			else {
				input = hyperParam.cppnInputFunction(hyperParam.inputVariables, point->pos, pos);
			}

			//Compute
			cppns.getNeuralNetwork(index)->compute(input, output);

			//Set weight and activation
			std::vector<float> const *p1, *p2;

			if (outgoing == true)
			{
				p1 = &pos;
				p2 = &point->pos;
			}
			else {
				p1 = &point->pos;
				p2 = &pos;
			}

			point->weight = hyperParam.weightModifierFunction(hyperParam.weightVariables, output[0], *p1, *p2);

			if (hyperParam.thresholdFunction(hyperParam.thresholdVariables, output, *p1, *p2) == true)
			{
				nActive++;
			}
		}

		point->variance = std::min(nActive, int(nActive - pos.size()));

		//Divide until initial resolution or if variance is still high
		if ((point->level < esParam.initialDepth) || (point->level < esParam.maxDepth && point->variance > esParam.divThreshold))
		{
			for (std::list<SubstrateTree>::iterator leaf = point->leaves.begin(); leaf != point->leaves.end(); ++leaf)
			{
				q.push(&*leaf);
			}
		}
	}
}


void ES_Hyperneat::prunAndExtract(int index, const std::vector<float>& pos, SubstrateTree* tree, bool outgoing, std::unordered_set<Connection, HyperConnectionHash, HyperConnectionEqual>& connections)
{
	//Traverse tree depth-first
	for (std::list<SubstrateTree>::iterator leaf = tree->leaves.begin(); leaf != tree->leaves.end(); ++leaf)
	{
		if (leaf->variance >= esParam.varianceThreshold)
		{
			prunAndExtract(index, pos, &*leaf, outgoing, connections);
		}
		else {
			//Determine if point is in a band by checking neighbor CPPN values

			//Prepare to compute band value
			float value;
			std::vector<float> input, output;
			std::vector<float> const *p1 , *p2;
			
			//Critical part with pointer
			std::vector<float> posValue;
			posValue.resize(leaf->pos.size());

			if (outgoing == true)
			{
				p1 = &pos;
				p2 = &posValue;
			}
			else {
				p1 = &posValue;
				p2 = &pos;
			}

			//Compute band value
			for (unsigned int i = hyperParam.nDimensions / 2; i < hyperParam.nDimensions; i++)
			{
				float valueA, valueB;

				posValue = leaf->pos;
				posValue[i] -= leaf->width;

				//Start Debug critical part
				if (&posValue != p1)
				{
					if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 60.f, FColor::Red, "Error pointer prunnAndExtract");
				}
				//End Debug critical part
				
				input = hyperParam.cppnInputFunction(hyperParam.inputVariables, *p1, *p2);
				cppns.getNeuralNetwork(index)->compute(input, output);

				valueA = hyperParam.weightModifierFunction(hyperParam.weightVariables, output[0], *p1, *p2);

				posValue = leaf->pos;
				posValue[i] += leaf->width;

				input = hyperParam.cppnInputFunction(hyperParam.inputVariables, *p1, *p2);
				cppns.getNeuralNetwork(index)->compute(input, output);

				valueB = hyperParam.weightModifierFunction(hyperParam.weightVariables, output[0], *p1, *p2);

				value = std::max(value, std::min(valueA, valueB));
			}

			if (value > esParam.bandThreshold)
			{
				//Create new connection
				posValue = leaf->pos;

				input = hyperParam.cppnInputFunction(hyperParam.inputVariables, *p1, *p2);
				cppns.getNeuralNetwork(index)->compute(input, output);

				connections.emplace(*p1, *p2, hyperParam.weightModifierFunction(hyperParam.weightVariables, output[0], *p1, *p2));
			}
		}
	}
}

void ES_Hyperneat::generateSubstrate()
{
	cppns.generateNetworks();

	for (int index = 0; index < cppns.getPopulationSize(); index++)
	{
		networks[index].clear();

		std::unordered_set<Connection, HyperConnectionHash, HyperConnectionEqual> connections;
		std::unordered_set<std::vector<float>, HyperNodeHash> hiddenNodes;
		std::queue<std::vector<float>> unexploredNodes;
		std::unordered_multimap<std::vector<float>, const Connection*, HyperNodeHash> inConnectionMap, outConnectionMap;
		std::unordered_map<std::vector<float>, int, HyperNodeHash> nodesLayerMap;//Holds the layer of each node
		std::unordered_map<std::vector<float>, std::pair<int, int>, HyperNodeHash> nodesPos;//Holds the layer and pos of each node

		//Input to hidden nodes connections
		for (std::unordered_set<std::vector<float>, HyperNodeHash>::iterator itSubstrate = inputSubstrate.begin(); itSubstrate != inputSubstrate.end(); ++itSubstrate)
		{
			SubstrateTree root(esParam.center, esParam.width, 1);

			divAndInit(index, *itSubstrate, &root, true);
			prunAndExtract(index, *itSubstrate, &root, true, connections);

			nodesLayerMap.emplace(*itSubstrate, 0);
			nodesPos.emplace(*itSubstrate, networks[index].addInputNode());

			for (std::unordered_set<Connection, HyperConnectionHash, HyperConnectionEqual>::iterator itConnection = connections.begin(); itConnection != connections.end(); ++itConnection)
			{
				inConnectionMap.emplace(itConnection->pos1, &*itConnection);

				if (hiddenNodes.emplace(itConnection->pos1).second == true)
				{
					unexploredNodes.push(itConnection->pos1);
				}
				
				if (hiddenNodes.emplace(itConnection->pos2).second == true)
				{
					unexploredNodes.push(itConnection->pos2);
				}
			}
		}

		//Hidden to hidden nodes connections
		for (int level = 1; level < esParam.iterationLevel; level++)
		{
			while (unexploredNodes.empty() == false)
			{
				std::unordered_set<Connection, HyperConnectionHash, HyperConnectionEqual> newConnections;
				SubstrateTree root(esParam.center, esParam.width, 1);

				divAndInit(index, unexploredNodes.front(), &root, true);
				prunAndExtract(index, unexploredNodes.front(), &root, true, newConnections);

				for (std::unordered_set<Connection, HyperConnectionHash, HyperConnectionEqual>::iterator itConnection = newConnections.begin(); itConnection != newConnections.end(); ++itConnection)
				{
					inConnectionMap.emplace(itConnection->pos1, &*itConnection);
					outConnectionMap.emplace(itConnection->pos2, &*itConnection);

					if (hiddenNodes.emplace(itConnection->pos1).second == true)
					{
						unexploredNodes.push(itConnection->pos1);
					}

					if (hiddenNodes.emplace(itConnection->pos2).second == true)
					{
						unexploredNodes.push(itConnection->pos2);
					}

					connections.emplace(*itConnection);
				}

				unexploredNodes.pop();
			}
		}

		//Hidden to output nodes connections
		for (std::unordered_set<std::vector<float>, HyperNodeHash>::iterator itSubstrate = outputSubstrate.begin(); itSubstrate != outputSubstrate.end(); ++itSubstrate)
		{
			SubstrateTree root(esParam.center, esParam.width, 1);

			divAndInit(index, *itSubstrate, &root, true);
			prunAndExtract(index, *itSubstrate, &root, true, connections);

			nodesPos.emplace(*itSubstrate, networks[index].addOutputNode(hyperParam.activationFunction));

			//Nodes not created here because all the hidden nodes that are connected to an output node are already expressed
		}

		//Remove all the nodes and their connections that don't have a path to input and an ouput neurons

		std::unordered_set<Connection, HyperConnectionHash, HyperConnectionEqual> outputReachableConn, inputReachableConn;

		reachableConnections(outConnectionMap, outputSubstrate, outputReachableConn);
		reachableConnections(inConnectionMap, inputSubstrate, inputReachableConn);

		connections.clear();
		hiddenNodes.clear();

		
		std::unordered_multimap<std::vector<float>, const Connection*, HyperNodeHash> connectionMap;

		//Search common connections
		for (std::unordered_set<Connection, HyperConnectionHash, HyperConnectionEqual>::iterator itOutputConn = outputReachableConn.begin(); itOutputConn != outputReachableConn.end(); ++itOutputConn)
		{
			//If connection is common keep it
			if (inputReachableConn.find(*itOutputConn) != inputReachableConn.end())
			{
				connections.emplace(*itOutputConn);

				//Attribute nodes to a layer
				//If n1 already exist
				if (nodesLayerMap.find(itOutputConn->pos1) != nodesLayerMap.end())
				{
					//If n2 doesn't exist yet
					if (nodesLayerMap.find(itOutputConn->pos2) == nodesLayerMap.end())
					{
						nodesLayerMap.emplace(itOutputConn->pos2, nodesLayerMap[itOutputConn->pos1] + 1);
					}
					//If n2 isn't part of the input substrate
					else if (nodesLayerMap[itOutputConn->pos2] <= nodesLayerMap[itOutputConn->pos1] || inputSubstrate.find(itOutputConn->pos2) == inputSubstrate.end())
					{
						//check for recursion first
						if (bfs(itOutputConn->pos2, itOutputConn->pos1, connectionMap) == false)
						{
							//If there's no recursion just shift the nodes
							shiftNodes(itOutputConn->pos1, connectionMap, nodesLayerMap);
						}
						else {//Otherwise tag the connection as recursive
							Connection tmp = *itOutputConn;

							tmp.recursive = true;

							itOutputConn = outputReachableConn.erase(itOutputConn);
							itOutputConn = outputReachableConn.emplace_hint(itOutputConn, tmp);
						}
					}
				}
				else {//If n1 doesn't exist yet
					nodesLayerMap.emplace(itOutputConn->pos1, 1);

					//If n2 doesn't exist yet
					if (nodesLayerMap.find(itOutputConn->pos2) == nodesLayerMap.end())
					{
						nodesLayerMap.emplace(itOutputConn->pos2, nodesLayerMap[itOutputConn->pos1] + 1);
					}
				}

				connectionMap.emplace(itOutputConn->pos1, &*itOutputConn);
			}
		}

		//Add the hidden nodes to the network
		for (std::unordered_map<std::vector<float>, int, HyperNodeHash>::iterator itHiddenNodes = nodesLayerMap.begin(); itHiddenNodes != nodesLayerMap.end(); ++itHiddenNodes)
		{
			nodesPos.emplace(itHiddenNodes->first, networks[index].addHiddenNode(itHiddenNodes->second, hyperParam.activationFunction));
		}

		//Connect the nodes
		for (std::unordered_set<Connection, HyperConnectionHash, HyperConnectionEqual>::iterator itConn = connections.begin(); itConn != connections.end(); ++itConn)
		{
			std::vector<float> input, output;
			input = hyperParam.cppnInputFunction(hyperParam.inputVariables, itConn->pos1, itConn->pos2);
			cppns.getNeuralNetwork(index)->compute(input, output);
			networks[index].connectNodes(nodesPos[itConn->pos1], nodesPos[itConn->pos2], hyperParam.weightModifierFunction(hyperParam.weightVariables, output[0], itConn->pos1, itConn->pos2));
		}
	}
}

//Searches for all the nodes and connections reachable from a set of node
void ES_Hyperneat::reachableConnections(
	const std::unordered_multimap<std::vector<float>, const Connection*, HyperNodeHash>& connections, std::unordered_set<std::vector<float>, HyperNodeHash> nodes,
	std::unordered_set<Connection, HyperConnectionHash, HyperConnectionEqual>& reachableConn)
{
	std::queue<std::vector<float>> unexploredNodes;

	for (std::unordered_set<std::vector<float>, HyperNodeHash>::const_iterator itNodes = nodes.cbegin(); itNodes != nodes.cend(); ++itNodes)
	{
		unexploredNodes.push(*itNodes);
	}

	while(unexploredNodes.empty() == false)
	{
		std::pair<std::unordered_multimap<std::vector<float>, const Connection*, HyperNodeHash>::const_iterator, 
			std::unordered_multimap<std::vector<float>, const Connection*, HyperNodeHash>::const_iterator> 
			range = connections.equal_range(unexploredNodes.front());

		while(range.first != range.second){

			if (reachableConn.emplace(*range.first->second).second == true)
			{
				if (nodes.emplace(range.first->second->pos2).second == true)
				{
					unexploredNodes.push(range.first->second->pos2);
				}
			}

			++range.first;
		}

		unexploredNodes.pop();
	}
}

//Breadth first search
bool ES_Hyperneat::bfs(const std::vector<float>& startPos, const std::vector<float>& endPos,
	const std::unordered_multimap<std::vector<float>, const Connection*, HyperNodeHash>& connectionMap)
{
	bool found = false;
	std::unordered_set<std::vector<float>, HyperNodeHash> discoveredNodes;
	std::queue<std::vector<float>> unexploredNodes;

	unexploredNodes.push(startPos);
	discoveredNodes.emplace(startPos);

	do {
		int i = 0;
		std::pair<std::unordered_multimap<std::vector<float>, const Connection*, HyperNodeHash>::const_iterator,
			std::unordered_multimap<std::vector<float>, const Connection*, HyperNodeHash>::const_iterator>
			range = connectionMap.equal_range(unexploredNodes.front());

		while (range.first != range.second && found == false)
		{
			if (range.first->second->pos2 == endPos)
			{
				found = true;
			}
			else {
				
				if (discoveredNodes.emplace(range.first->second->pos2).second == true)
				{
					unexploredNodes.push(range.first->second->pos2);
				}
			}

			++range.first;
		}

		unexploredNodes.pop();

	} while (unexploredNodes.empty() == false && found == false);

	return found;
}

//Moves the nodes that needs to be moved to another layer
void ES_Hyperneat::shiftNodes(const std::vector<float>& startPos,
	const std::unordered_multimap<std::vector<float>, const Connection*, HyperNodeHash>& connectionMap,
	std::unordered_map<std::vector<float>, int, HyperNodeHash> nodesLayerMap)
{
	bool found = false;
	std::queue<std::vector<float>> unexploredNodes;

	unexploredNodes.push(startPos);

	do {
		int i = 0;
		std::pair<std::unordered_multimap<std::vector<float>, const Connection*, HyperNodeHash>::const_iterator,
			std::unordered_multimap<std::vector<float>, const Connection*, HyperNodeHash>::const_iterator>
			range = connectionMap.equal_range(unexploredNodes.front());

		while (range.first != range.second && found == false)
		{
			if (range.first->second->recursive == false && nodesLayerMap[range.first->second->pos2] <= nodesLayerMap[range.first->second->pos1])
			{
				nodesLayerMap[range.first->second->pos2] = nodesLayerMap[range.first->second->pos1] + 1;

				unexploredNodes.push(range.first->second->pos2);
			}

			++range.first;
		}

		unexploredNodes.pop();

	} while (unexploredNodes.empty() == false);
}