// Fill out your copyright notice in the Description page of Project Settings.


#include "Genome.h"
#include <stack>

Genome::Genome(int input, int output)
{
    for (int i = 0; i < input; i++)
    {
        nodes.push_back(GeneNode(NODE_TYPE::INPUT, 0));
    }
 
    for (int i = 0; i < output; i++)
    {
        nodes.push_back(GeneNode(NODE_TYPE::OUTPUT, 999999));
    }
}

Genome::~Genome()
{
}

/**
*Check if the connection already exist, if so use it's innovation number
* otherwise add it to the register
*/
void Genome::addConnection(int nodeA, int nodeB, std::unordered_map<std::pair<int, int>, int>& allConnections)
{
    std::unordered_map<std::pair<int, int>, int>::iterator found = allConnections.find(std::pair(nodeA, nodeB));

    int innovationNumber = -1;

    if (found == allConnections.end())
    {
        innovationNumber = allConnections.size();
        allConnections[std::pair(nodeA, nodeB)] = innovationNumber;
    }
    else {
        innovationNumber = found->second;
    }

    connections[innovationNumber] = GeneConnection(innovationNumber, nodeA, nodeB);
    nodesToConnection[std::pair(nodeA, nodeB)] = innovationNumber;
}

bool Genome::mutateLink(std::unordered_map<std::pair<int, int>, int>& allConnections)
{
    int i = 0;
    bool foundMutation = false;

    int nodeA;
    int nodeB;

    //Try to find a non-existant connection to create a 100 times
    while(i < 100 && foundMutation == false)
    {
        i++;

        //Pick two random nodes
        nodeA = rand() % nodes.size();
        nodeB = rand() % nodes.size();

        if (nodeA == nodeB || nodes[nodeA].layer == nodes[nodeB].layer || (nodes[nodeA].type == NODE_TYPE::OUTPUT && nodes[nodeB].type == NODE_TYPE::OUTPUT))
        {
            continue;
        }
            
        //Make sure we connect them in the right order
        if(nodes[nodeA].layer > nodes[nodeB].layer)
        {
            int c = nodeA;
            nodeA = nodeB;
            nodeB = c;
        }

        //Check that the nodes they are not already connected
        if (nodesToConnection.find(std::pair(nodeA, nodeB)) == nodesToConnection.end())
        {
            foundMutation = true;
        }
    }

    if (foundMutation == true)
    {
        addConnection(nodeA, nodeB, allConnections);
        return true;
    }

    return false;
}

bool Genome::mutateNode(std::unordered_map<std::pair<int, int>, int>& allConnections)
{
    int i = 0;
    bool foundMutation = false;
    int geneConnectionIndex;
    GeneConnection* connection = nullptr;

    //Try to find an enabled connection to create a 100 times
    while (i < 100 && foundMutation == false)
    {
        geneConnectionIndex = rand() % (connections.size());

        connection = &std::next(connections.begin(), geneConnectionIndex)->second;

        if (connection->enabled == true)
        {
            foundMutation = true;
        }

        i++;
    }

    if (foundMutation == false) return false;

    //Create the new node, disable old connection and gather info
    int nodeA = connection->nodeA;
    int nodeB = connection->nodeB;
    float oldWeight = connection->weight;
    connection->enabled = false;
    nodes.push_back(GeneNode(NODE_TYPE::HIDDEN, nodes[connection->getNodeA()].layer + 1));
    int nodeC = nodes.size()-1;

    //Shift node's layer
    if (nodes[nodeA].layer + 1 == nodes[nodeB].layer)
    {
        shiftNodes(nodeB, nodes[nodeB].layer);
    }

    //Create the new connections
    addConnection(nodeA, nodeC, allConnections);
    addConnection(nodeC, nodeB, allConnections);
    return true;
}

void Genome::mutateWeightShift(float weightShiftStrength)
{
    bool foundMutation = false;
    int geneConnectionIndex;
    int i = 0;
    GeneConnection* connection = nullptr;

    //Try to find an enabled connection to create a 100 times
    while (i < 100 && foundMutation == false)
    {
        geneConnectionIndex = rand() % connections.size();
        connection = &std::next(connections.begin(), geneConnectionIndex)->second;

        if (connection->enabled == true)
        {
            foundMutation = true;
        }

        i++;
    }

    if (foundMutation == true)
    {
        //Shift the weight by a random offset
        connection->weight += (static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 2)) - 1) * weightShiftStrength;
    }
}

void Genome::mutateWeightRandom(float weightRandomStrength)
{
    bool foundMutation = false;
    int geneConnectionIndex;
    int i = 0;
    GeneConnection* connection = nullptr;

    //Try to find an enabled connection to create a 100 times
    while (i < 100 && foundMutation == false)
    {
        geneConnectionIndex = rand() % connections.size();
        connection = &std::next(connections.begin(), geneConnectionIndex)->second;

        if (connection->enabled == true)
        {
            foundMutation = true;
        }

        i++;
    }

    if (foundMutation == true)
    {
        //Set a new random weight
        connection->weight = (static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 2)) -1 ) * weightRandomStrength;
    }
}

void Genome::mutateLinkToggle()
{
    int geneConnectionIndex = rand() % connections.size();
    GeneConnection* connection = &std::next(connections.begin(), geneConnectionIndex)->second;

    //Switches the activation of the connection
    connection->enabled = !connection->enabled;
}

void Genome::crossover(Genome& parentA, Genome& parentB)
{
    nodes.clear();
    connections.clear();
    nodesToConnection.clear();

    //Insert nodes
    std::deque<GeneNode>* nodesA = parentA.getNodes();
    std::deque<GeneNode>* nodesB = parentB.getNodes();

    for (int i = 0; i < nodesA->size() || i < nodesB->size(); i++)
    {
        int layer;

        if (i < nodesA->size() && i < nodesB->size())
        {
            layer = (*nodesA)[i].getLayer() > (*nodesB)[i].getLayer() ? (*nodesA)[i].getLayer() : (*nodesB)[i].getLayer();

            nodes.push_back(GeneNode((*nodesA)[i].getType(), layer));
        }
        else if (i < nodesA->size())
        {
            layer = (*nodesA)[i].getLayer();

            nodes.push_back(GeneNode((*nodesA)[i].getType(), layer));
        }
        else if (i < nodesB->size())
        {
            layer = (*nodesB)[i].getLayer();

            nodes.push_back(GeneNode((*nodesB)[i].getType(), layer));
        }
    }

    //Insert connections parentA
    std::unordered_map<int, GeneConnection>* parentConnections = parentA.getConnections();

    for (std::unordered_map<int, GeneConnection>::iterator it = (*parentConnections).begin(); it != (*parentConnections).end(); ++it)
    {
        connections[it->first] = it->second;

        nodesToConnection[std::pair(connections[it->first].getNodeA(), connections[it->first].getNodeB())] = it->first;
    }

    //Insert connections parentB
    parentConnections = parentB.getConnections();

    for (std::unordered_map<int, GeneConnection>::iterator it = (*parentConnections).begin(); it != (*parentConnections).end(); ++it)
    {
        std::unordered_map<int, GeneConnection>::iterator found = connections.find(it->first);

        if (found != connections.end() && it->second.isEnabled() == false)
        {
            found->second.enabled = false;
        }
        else {

            connections[it->first] = it->second;

            nodesToConnection[std::pair(connections[it->first].getNodeA(), connections[it->first].getNodeB())] = it->first;
        }
    }

    for (std::unordered_map<std::pair<int, int>, int>::iterator it = nodesToConnection.begin(); it != nodesToConnection.end(); ++it)
    {
        if (nodes[it->first.first].getLayer() >= nodes[it->first.second].getLayer())
        {
            shiftNodes(it->first.second, nodes[it->first.first].getLayer());
        }
    }
}

void Genome::shiftNodes(int node, int layerMin)
{
    std::stack<int> nodeToShift;

    nodeToShift.push(node);
    nodes[node].layer = layerMin + 1;

    while (nodeToShift.empty() == false)
    {
        int node = nodeToShift.top();
        nodeToShift.pop();

        //Search connection and check if it needs a shift
        for (int i = 0; i < nodes.size(); i++)
        {
            if (i == node) continue;

            std::unordered_map<std::pair<int, int>, int>::iterator it = nodesToConnection.find(std::pair(node, i));

            if (it != nodesToConnection.end())
            {
                if (nodes[it->first.second].layer == nodes[node].layer)
                {
                    nodes[it->first.second].layer = nodes[node].layer + 1;

                    if (nodes[it->first.second].type != NODE_TYPE::OUTPUT)
                    {
                        nodeToShift.push(it->first.second);
                    }
                }
            }
        }
    }
}