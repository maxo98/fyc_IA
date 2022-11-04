// Fill out your copyright notice in the Description page of Project Settings.


#include "Genome.h"
#include <stack>

Genome::Genome()
{

}

Genome::Genome(unsigned int _input, unsigned int output, std::vector<Activation*> activationFunctions, float* _score)
{
    input = _input;

    for (unsigned int i = 0; i < input; i++)
    {
        nodes.push_back(GeneNode(NODE_TYPE::INPUT, activationFunctions[0]));
    }
 
    for (unsigned int i = 0; i < output; i++)
    {
        unsigned int activationIndex = rand() % activationFunctions.size();

        nodes.push_back(GeneNode(NODE_TYPE::OUTPUT, activationFunctions[activationIndex], 999999));
    }

    score = _score;
}

Genome::~Genome()
{
}

/**
*Check if the connection already exist, if so use it's innovation number
* otherwise add it to the register
*/
void Genome::addConnection(unsigned int nodeA, unsigned int nodeB, std::unordered_map<std::pair<unsigned int, unsigned int>, unsigned int>& allConnections)
{
    std::unordered_map<std::pair<unsigned int, unsigned int>, unsigned int>::iterator found = allConnections.find(std::pair<unsigned int, unsigned int>(nodeA, nodeB));

    unsigned int innovationNumber = -1;

    if (found == allConnections.end())
    {
        innovationNumber = allConnections.size();
        allConnections[std::pair<unsigned int, unsigned int>(nodeA, nodeB)] = innovationNumber;
    }
    else {
        innovationNumber = found->second;
    }

    connections[innovationNumber] = GeneConnection(innovationNumber, nodeA, nodeB);
    nodesToConnection[std::pair<unsigned int, unsigned int>(nodeA, nodeB)] = innovationNumber;
}

bool Genome::mutateLink(std::unordered_map<std::pair<unsigned int, unsigned int>, unsigned int>& allConnections)
{
    unsigned int i = 0;
    bool foundMutation = false;

    unsigned int nodeA;
    unsigned int nodeB;

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
            unsigned int c = nodeA;
            nodeA = nodeB;
            nodeB = c;
        }

        //Check that the nodes they are not already connected
        if (nodesToConnection.find(std::pair<unsigned int, unsigned int>(nodeA, nodeB)) == nodesToConnection.end())
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

bool Genome::mutateNode(std::unordered_map<std::pair<unsigned int, unsigned int>, unsigned int>& allConnections, Activation* activationFunction)
{
    unsigned int i = 0;
    bool foundMutation = false;
    unsigned int geneConnectionIndex;
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
    unsigned int nodeA = connection->nodeA;
    unsigned int nodeB = connection->nodeB;
    float oldWeight = connection->weight;
    connection->enabled = false;
    nodes.push_back(GeneNode(NODE_TYPE::HIDDEN, activationFunction, nodes[connection->getNodeA()].layer + 1));
    unsigned int nodeC = nodes.size()-1;

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
    unsigned int geneConnectionIndex;
    unsigned int i = 0;
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
    unsigned int geneConnectionIndex;
    unsigned int i = 0;
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
    unsigned int geneConnectionIndex = rand() % connections.size();
    GeneConnection* connection = &std::next(connections.begin(), geneConnectionIndex)->second;

    //Switches the activation of the connection
    connection->enabled = !connection->enabled;
}

void Genome::mutateActivation(Activation* activationFunction)
{
    unsigned int nodeIndex = (rand() % (nodes.size() - input)) + input;
    nodes[nodeIndex].setActivation(activationFunction);
}

//Parent A should be the fittest
void Genome::crossover(Genome& parentA, Genome& parentB)
{
    nodes.clear();
    connections.clear();
    nodesToConnection.clear();

    //Insert nodes
    std::deque<GeneNode>* nodesA = parentA.getNodes();
    std::deque<GeneNode>* nodesB = parentB.getNodes();

    for (unsigned int i = 0; i < nodesA->size(); i++)
    {
        unsigned int layer = (*nodesA)[i].getLayer();

        nodes.push_back(GeneNode((*nodesA)[i].getType(), (*nodesA)[i].getActivation(), layer));
    }

    //Insert connections of fittest parent (aka parentA)
    std::map<unsigned int, GeneConnection>* parentAConnections = parentA.getConnections();
    std::map<unsigned int, GeneConnection>* parentBConnections = parentB.getConnections();
    

    for (std::map<unsigned int, GeneConnection>::iterator itA = (*parentAConnections).begin(); itA != (*parentAConnections).end(); ++itA)
    {
        std::map<unsigned int, GeneConnection>::iterator found = parentBConnections->find(itA->first);

        //If both parent have the same gene pick one randomly
        if (found != parentBConnections->end())
        {
            uint8_t aOrB = rand() % 2;

            if (aOrB == 0)
            {
                connections[itA->first] = itA->second;
            }
            else {
                connections[found->first] = found->second;
            }
        }
        else {
            connections[itA->first] = itA->second;
        }

        nodesToConnection[std::pair<unsigned int, unsigned int>(connections[itA->first].getNodeA(), connections[itA->first].getNodeB())] = itA->first;
    }
}

void Genome::shiftNodes(unsigned int node, unsigned int layerMin)
{
    std::stack<unsigned int> nodeToShift;

    nodeToShift.push(node);
    nodes[node].layer = layerMin + 1;

    while (nodeToShift.empty() == false)
    {
        unsigned int node = nodeToShift.top();
        nodeToShift.pop();

        //Search connection and check if it needs a shift
        for (int i = 0; i < nodes.size(); i++)
        {
            if (i == node) continue;

            std::unordered_map<std::pair<unsigned int, unsigned int>, unsigned int>::iterator it = nodesToConnection.find(std::pair<unsigned int, unsigned int>(node, i));

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