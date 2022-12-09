// Fill out your copyright notice in the Description page of Project Settings.


#include "ES_Hyperneat.h"
#include <algorithm>
#include <queue>

ES_Hyperneat::ES_Hyperneat(unsigned int _populationSize, const NeatParameters& _neatParam, const HyperneatParameters& _hyperParam, const ES_Parameters& _esParam):
	Hyperneat(_populationSize, _neatParam, _hyperParam)
{
	esParam = _esParam;

	//for (int i = 0; i < _populationSize; i++)
	//{
	//	networks[i].warningRecursive = false;
	//}
}

ES_Hyperneat::~ES_Hyperneat()
{
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

void ES_Hyperneat::divAndInit(NeuralNetwork& hypernet, const std::vector<float>& pos, SubstrateTree* root, bool outgoing)
{
	
	std::queue<SubstrateTree*> q;
	q.push(root);

	//std::cout << root->pos << std::endl;

	while (!q.empty())
	{
		SubstrateTree* point = q.front();
		q.pop();

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
				first.pos.push_back(point->pos[0] + newWidth);
				point->leaves.push_back(first);

				SubstrateTree second(std::vector<float>(), newWidth, point->level + 1);
				second.pos.push_back(point->pos[0] - newWidth);
				point->leaves.push_back(second);
			}
			else {
				int size = point->leaves.size();
				std::list<SubstrateTree>::iterator leaf = point->leaves.begin();

				for(int cpt = 0; cpt < size; ++cpt, ++leaf)
				{
					point->leaves.push_back(*leaf);

					leaf->pos.push_back(point->pos[i] + newWidth);

					point->leaves.back().pos.push_back(point->pos[i] - newWidth);
				}
			}
		}

		//std::cout << point->leaves.size() << std::endl;
		//

		//For each leaf check if the connection is active
		//And compute weight
		int nActive = 0;

		for (std::list<SubstrateTree>::iterator leaf = point->leaves.begin(); leaf != point->leaves.end(); ++leaf)
		{
			std::vector<float> input, output;

			//std::cout << leaf->pos << std::endl;

			//Create input vector
			if (outgoing == true)
			{
				input = hyperParam.cppnInputFunction(hyperParam.inputVariables, pos, point->pos);
			}
			else {
				input = hyperParam.cppnInputFunction(hyperParam.inputVariables, point->pos, pos);
			}

			//Compute
			hypernet.compute(input, output);

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

		point->variance = abs(std::min(nActive, int(nActive - pos.size())));

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


void ES_Hyperneat::prunAndExtract(NeuralNetwork& hypernet, const std::vector<float>& pos, SubstrateTree* tree, bool outgoing, 
	std::unordered_set<Connection, HyperConnectionHash, HyperConnectionEqual>& connections)
{

	bool done = false;

	//Traverse tree depth-first
	for (std::list<SubstrateTree>::iterator leaf = tree->leaves.begin(); leaf != tree->leaves.end(); ++leaf)
	{
		//std::cout << "variance " << leaf->variance << " " << esParam.varianceThreshold << std::endl;
		if (leaf->leaves.size() != 0 && leaf->variance >= esParam.varianceThreshold)
		{
			prunAndExtract(hypernet, pos, &*leaf, outgoing, connections);
		}
		else {
			//Determine if point is in a band by checking neighbor CPPN values

			//Prepare to compute band value
			float value = 0;
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
				
				input = hyperParam.cppnInputFunction(hyperParam.inputVariables, *p1, *p2);
				hypernet.compute(input, output);

				valueA = abs(hyperParam.weightModifierFunction(hyperParam.weightVariables, output[0], *p1, *p2));

				posValue = leaf->pos;
				posValue[i] += leaf->width;

				input = hyperParam.cppnInputFunction(hyperParam.inputVariables, *p1, *p2);
				hypernet.compute(input, output);

				valueB = abs(hyperParam.weightModifierFunction(hyperParam.weightVariables, output[0], *p1, *p2));

				value = std::max(value, std::min(valueA, valueB));
			}

			//std::cout << "band " << value << " " << esParam.bandThreshold << std::endl;

			if (value > esParam.bandThreshold)
			{
				//Create new connection
				posValue = leaf->pos;

				input = hyperParam.cppnInputFunction(hyperParam.inputVariables, *p1, *p2);
				hypernet.compute(input, output);

				if (connections.emplace(*p1, *p2, hyperParam.weightModifierFunction(hyperParam.weightVariables, output[0], *p1, *p2)).second == true)
				{
					//std::cout << *p1 << " >> " << *p2 << std::endl;
				}
			}
		}
	}
}

void ES_Hyperneat::createNetwork(NeuralNetwork& hypernet, NeuralNetwork& net)
{
	net.clear();

	//std::cout << "new network\n\n\n";

	std::unordered_set<Connection, HyperConnectionHash, HyperConnectionEqual> connections;
	std::unordered_set<std::vector<float>, HyperNodeHash> hiddenNodes;//To change to a map with layer index
	std::queue<std::vector<float>>* unexploredNodes, * futureUnexploredNodes, unexploredNodesA, unexploredNodesB;//Nodes to explore swaped in and out
	std::unordered_multimap<std::vector<float>, const Connection*, HyperNodeHash> hiddenConnectionMap, outConnectionMap;//Links nodes to connections entering them
	std::unordered_map<std::vector<float>, unsigned int, HyperNodeHash> nodesLayerMap;//Holds the layer of each node
	std::unordered_map<std::vector<float>, std::pair<unsigned int, unsigned int>, HyperNodeHash> nodesPosInput, nodesPosOutput, nodesPosHidden;//Holds the layer and pos of each node

	//Input to hidden nodes connections
	for (std::unordered_set<std::vector<float>, HyperNodeHash>::iterator itSubstrate = inputSubstrate.begin(); itSubstrate != inputSubstrate.end(); ++itSubstrate)
	{
		SubstrateTree root(esParam.center, esParam.width, 1);
		//SubstrateTree root(*itSubstrate, esParam.width, 1);

		divAndInit(hypernet, *itSubstrate, &root, true);
		prunAndExtract(hypernet, *itSubstrate, &root, true, connections);

		//std::cout << esParam.center << std::endl;

		//nodesLayerMap.emplace(*itSubstrate, 0);
		nodesPosInput.emplace(*itSubstrate, net.addInputNode());
	}

	//std::cout << "conn " << connections.size() << std::endl;

	for (std::unordered_set<Connection, HyperConnectionHash, HyperConnectionEqual>::iterator itConnection = connections.begin(); itConnection != connections.end(); ++itConnection)
	{
		//std::cout << itConnection->pos1 << std::endl;

		hiddenConnectionMap.emplace(itConnection->pos2, &*itConnection);

		if (nodesLayerMap.emplace(itConnection->pos2, 1).second == true)
		{
			unexploredNodesA.push(itConnection->pos2);
		}
	}

	unexploredNodes = &unexploredNodesA;
	futureUnexploredNodes = &unexploredNodesB;

	//Hidden to hidden nodes connections
	for (int level = 1; level < esParam.iterationLevel; level++)
	{
		//std::cout << "unexplored " << unexploredNodes->size() << std::endl;

		while (unexploredNodes->empty() == false)
		{
			std::unordered_set<Connection, HyperConnectionHash, HyperConnectionEqual> newConnections;
			SubstrateTree root(esParam.center, esParam.width, 1);
			//SubstrateTree root(unexploredNodes->front(), esParam.width, 1);

			divAndInit(hypernet, unexploredNodes->front(), &root, true);
			prunAndExtract(hypernet, unexploredNodes->front(), &root, true, newConnections);

			for (std::unordered_set<Connection, HyperConnectionHash, HyperConnectionEqual>::iterator itConnection = newConnections.begin(); itConnection != newConnections.end(); ++itConnection)
			{
				Connection connTmp = *itConnection;//We cannot change value of keys inside an unordered set

				//if (nodesPosInput.find(connTmp.pos2) != nodesPosInput.end())
				{
					if (nodesLayerMap.emplace(connTmp.pos2, level + 1).second == true)
					{
						futureUnexploredNodes->push(connTmp.pos2);
					}
					else if (nodesLayerMap[connTmp.pos2] <= level)
					{
						connTmp.recursive = true;
					}

					std::pair<std::unordered_set<Connection, HyperConnectionHash, HyperConnectionEqual>::iterator, bool> result = connections.emplace(*itConnection);

					hiddenConnectionMap.emplace(result.first->pos2, &*result.first);
				}
			}

			unexploredNodes->pop();
		}

		if (&unexploredNodesA == unexploredNodes)
		{
			unexploredNodes = &unexploredNodesB;
			futureUnexploredNodes = &unexploredNodesA;

		}
		else {

			unexploredNodes = &unexploredNodesA;
			futureUnexploredNodes = &unexploredNodesB;
		}
	}

	unexploredNodes = futureUnexploredNodes;
	std::unordered_set<Connection, HyperConnectionHash, HyperConnectionEqual> newConnections;

	//Hidden to output nodes connections
	for (std::unordered_set<std::vector<float>, HyperNodeHash>::iterator itSubstrate = outputSubstrate.begin(); itSubstrate != outputSubstrate.end(); ++itSubstrate)
	{
		SubstrateTree root(esParam.center, esParam.width, 1);
		//SubstrateTree root(*itSubstrate, esParam.width, 1);

		divAndInit(hypernet, *itSubstrate, &root, false);
		prunAndExtract(hypernet, *itSubstrate, &root, false, newConnections);

		nodesPosOutput.emplace(*itSubstrate, net.addOutputNode(hyperParam.activationFunction));

		//Nodes not created here because all the hidden nodes that are connected to an output node are already expressed
	}

	//Search for nodes connecting the output and existing in our list of nodes, before graph traversal
	for (std::unordered_set<Connection, HyperConnectionHash, HyperConnectionEqual>::iterator itConnection = newConnections.begin(); itConnection != newConnections.end(); ++itConnection)
	{
		std::unordered_map<std::vector<float>, unsigned int, HyperNodeHash>::iterator itNode = nodesLayerMap.find(itConnection->pos1);

		if (itNode != nodesLayerMap.end()/* && itNode->second != 0*/)
		{
			std::pair<std::unordered_set<Connection, HyperConnectionHash, HyperConnectionEqual>::iterator, bool> result = connections.emplace(*itConnection);
			outConnectionMap.emplace(result.first->pos2, &*result.first);

			if (nodesPosHidden.emplace(itNode->first, net.addHiddenNode(itNode->second, hyperParam.activationFunction)).second == true)
			{
				//std::cout << result.first->pos1 << std::endl;

				unexploredNodes->push(result.first->pos1);
			}
		}
	}

	//for (std::unordered_multimap<std::vector<float>, const Connection*, HyperNodeHash>::const_iterator it = hiddenConnectionMap.begin(); it != hiddenConnectionMap.end(); ++it)
	//{
	//	std::cout << it->second->pos1 << " >> " << it->second->pos2 << std::endl;
	//}

	//Graph traversal, add hidden nodes found in nodePosHidden
	while (unexploredNodes->empty() == false)
	{
		std::pair<std::unordered_multimap<std::vector<float>, const Connection*, HyperNodeHash>::const_iterator,
			std::unordered_multimap<std::vector<float>, const Connection*, HyperNodeHash>::const_iterator>
			range = hiddenConnectionMap.equal_range(unexploredNodes->front());

		while (range.first != range.second)
		{
			//std::cout << range.first->second->pos1 << std::endl;
			//std::cout << range.first->second->pos2 << std::endl;

			std::unordered_map<std::vector<float>, unsigned int, HyperNodeHash>::iterator layer = nodesLayerMap.find(range.first->second->pos1);

			if (layer != nodesLayerMap.end() && range.first->second->recursive == false)
			{
				if (nodesPosHidden.emplace(range.first->second->pos1, net.addHiddenNode(layer->second, hyperParam.activationFunction)).second == true)
				{
					unexploredNodes->push(range.first->second->pos1);
				}
			}

			++range.first;
		}

		unexploredNodes->pop();
	}

	//Connect the nodes
	connectNodes(nodesPosOutput, outConnectionMap, hypernet, net, &nodesPosHidden);
	
	connectNodes(nodesPosHidden, hiddenConnectionMap, hypernet, net, &nodesPosInput);

}

void ES_Hyperneat::connectNodes(std::unordered_map<std::vector<float>, std::pair<unsigned int, unsigned int>, HyperNodeHash>& nodesPos,
	std::unordered_multimap<std::vector<float>, const Connection*, HyperNodeHash>& connectionMap, NeuralNetwork& hypernet, NeuralNetwork& net,
	std::unordered_map<std::vector<float>, std::pair<unsigned int, unsigned int>, HyperNodeHash>* prevNodesPos)
{
	if (prevNodesPos == nullptr)
	{
		prevNodesPos = &nodesPos;
	}

	for (std::unordered_map<std::vector<float>, std::pair<unsigned int, unsigned int>, HyperNodeHash>::iterator itNodes = nodesPos.begin(); itNodes != nodesPos.end(); ++itNodes)
	{

		if (itNodes->second.first != 0)
		{
			std::pair<std::unordered_multimap<std::vector<float>, const Connection*, HyperNodeHash>::const_iterator,
				std::unordered_multimap<std::vector<float>, const Connection*, HyperNodeHash>::const_iterator>
				range = connectionMap.equal_range(itNodes->first);

			while (range.first != range.second)
			{

				if (range.first->second->recursive == false || esParam.allowRecurisvity == true)
				{

					std::vector<float> input, output;
					input = hyperParam.cppnInputFunction(hyperParam.inputVariables, range.first->second->pos1, itNodes->first);
					hypernet.compute(input, output);

					std::unordered_map<std::vector<float>, std::pair<unsigned int, unsigned int>, HyperNodeHash>::iterator pos = prevNodesPos->find(range.first->second->pos1);

					if (pos == prevNodesPos->end())
					{
						pos = nodesPos.find(range.first->second->pos1);
						//std::cout << "check it out \n";
					}

					net.connectNodes(pos->second, itNodes->second, hyperParam.weightModifierFunction(hyperParam.weightVariables, output[0], range.first->second->pos1, itNodes->first));

					//std::cout << range.first->second->pos1 << " >> " << itNodes->first << std::endl;
				}

				++range.first;
			}
		}
	}
}