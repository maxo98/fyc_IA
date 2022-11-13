// Fill out your copyright notice in the Description page of Project Settings.


#include "Genome.h"
#include <stack>

Genome::Genome()
{
    input = 0;
}

Genome::Genome(unsigned int _input, unsigned int output, std::vector<Activation*> activationFunctions, bool cppn)
{
    input = _input;

    for (unsigned int i = 0; i < input; i++)
    {
        nodes.push_back(GeneNode(NODE_TYPE::INPUT, activationFunctions[0]));
    }
 
    for (unsigned int i = 0; i < output; i++)
    {
        unsigned int activationIndex;
        
        if (cppn == true)
        {
            activationIndex = randInt(0, activationFunctions.size() - 1);
        }
        else {
            activationIndex = 0;
        }

        nodes.push_back(GeneNode(NODE_TYPE::OUTPUT, activationFunctions[activationIndex], 999999));
    }
}

Genome::~Genome()
{
}

/**
*Check if the connection already exist, if so use it's innovation number
* otherwise add it to the register
*/
void Genome::addConnection(unsigned int nodeA, unsigned int nodeB, std::unordered_map<std::pair<unsigned int, unsigned int>, unsigned int>& allConnections, float weight)
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
    connections[innovationNumber].weight = weight;
    nodesToConnection[std::pair<unsigned int, unsigned int>(nodeA, nodeB)] = innovationNumber;
    orderAddedCon.push_back(innovationNumber);
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
        nodeA = randInt(0, nodes.size() - 1);
        nodeB = randInt(0, nodes.size() - 1);

        if (nodeA >= nodes.size())
        {
            int err;
        }

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
        geneConnectionIndex = randInt(0, connections.size() - 1);

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

    if (nodeA >= nodes.size())
    {
        int err;
    }

    if ((nodes[connection->getNodeA()].layer + 1) > 100)
    {
        int err;
    }

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

//void Genome::mutateWeightShift(float weightShiftStrength)
//{
//    bool foundMutation = false;
//    unsigned int geneConnectionIndex;
//    unsigned int i = 0;
//    GeneConnection* connection = nullptr;
//
//    //Try to find an enabled connection to create a 100 times
//    while (i < 100 && foundMutation == false)
//    {
//        geneConnectionIndex = randInt(0, connections.size() - 1);
//        connection = &std::next(connections.begin(), geneConnectionIndex)->second;
//
//        if (connection->enabled == true)
//        {
//            foundMutation = true;
//        }
//
//        i++;
//    }
//
//    if (foundMutation == true)
//    {
//        //Shift the weight by a random offset
//        connection->weight += (static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 2)) - 1) * weightShiftStrength;
//    }
//}
//
//void Genome::mutateWeightRandom(float weightRandomStrength)
//{
//    bool foundMutation = false;
//    unsigned int geneConnectionIndex;
//    unsigned int i = 0;
//    GeneConnection* connection = nullptr;
//
//    //Try to find an enabled connection to create a 100 times
//    while (i < 100 && foundMutation == false)
//    {
//        geneConnectionIndex = randInt(0, connections.size() - 1);
//        connection = &std::next(connections.begin(), geneConnectionIndex)->second;
//
//        if (connection->enabled == true)
//        {
//            foundMutation = true;
//        }
//
//        i++;
//    }
//
//    if (foundMutation == true)
//    {
//        //Set a new random weight
//        connection->weight = (static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 2)) -1 ) * weightRandomStrength;
//    }
//}

void Genome::mutateLinkToggle()
{
    unsigned int geneConnectionIndex = randInt(0, connections.size() - 1);;
    GeneConnection* connection = &std::next(connections.begin(), geneConnectionIndex)->second;

    //Switches the activation of the connection
    connection->enabled = !connection->enabled;
}

void Genome::mutateActivation(Activation* activationFunction)
{
    unsigned int nodeIndex = randInt(input+1, nodes.size() - 1);
    nodes[nodeIndex].setActivation(activationFunction);
}

//Slightly modified version of mutate_link_weights from official implementation
void Genome::mutateWeights(float power, float rate, WEIGHT_MUTATOR mutType)
{
    float num;  //counts gene placement
    float geneTotal;
    float powerMod; //Modified power by gene number
    //The power of mutation will rise farther into the genome
    //on the theory that the older genes are more fit since
    //they have stood the test of time

    float randNum;
    float randChoice; //Decide what kind of mutation to do on a gene
    float endPart; //Signifies the last part of the genome
    float gaussPoint;
    float coldGaussPoint;

    bool severe;  //Once in a while really shake things up

    if (randFloat() > 0.5) severe = true;
    else severe = false;

    //Go through all the Genes and perturb their link's weights
    num = 0.0;
    geneTotal = connections.size();
    endPart = geneTotal * 0.8;
    //powermod=randposneg()*power*randfloat();  //Make power of mutation random
    //powermod=randfloat();
    powerMod = 1.0;

    //Loop on all genes  (ORIGINAL METHOD)
    for (int i = 0; i < connections.size(); i++) 
    {


        //Possibility: Have newer genes mutate with higher probability
        //Only make mutation power vary along genome if it's big enough
        //if (geneTotal>=10.0) {
        //This causes the mutation power to go up towards the end up the genome
        //powerMod=((power-0.7)/geneTotal)*num+0.7;
        //}
        //else {
        //    powerMod = power;
        //}

        //The following if determines the probabilities of doing cold gaussian
        //mutation, meaning the probability of replacing a link weight with
        //another, entirely random weight.  It is meant to bias such mutations
        //to the tail of a genome, because that is where less time-tested genes
        //reside.  The gausspoint and coldgausspoint represent values above
        //which a random float will signify that kind of mutation.  

        if (severe) 
        {
            gaussPoint = 0.3;
            coldGaussPoint = 0.1;
        }
        else if ((geneTotal >= 10.0) && (num > endPart)) 
        {
            gaussPoint = 0.5;  //Mutate by modification % of connections
            coldGaussPoint = 0.3; //Mutate the rest by replacement % of the time
        }
        else {
            //Half the time don't do any cold mutations
            if (randFloat() > 0.5) 
            {
                gaussPoint = 1.0 - rate;
                coldGaussPoint = 1.0 - rate - 0.1;
            }
            else {
                gaussPoint = 1.0 - rate;
                coldGaussPoint = 1.0 - rate;
            }
        }

        //Possible methods of setting the perturbation:
        //randnum=gaussrand()*powermod;
        //randnum=gaussrand();

        randNum = randPosNeg() * randFloat() * power * powerMod;
        //std::cout << "RANDOM: " << randnum << " " << randposneg() << " " << randfloat() << " " << power << " " << powermod << std::endl;
        if (mutType == WEIGHT_MUTATOR::GAUSSIAN) 
        {
            randChoice = randFloat();

            if (randChoice > gaussPoint)
            {
                connections[orderAddedCon[i]].weight += randNum;
            }
            else if (randChoice > coldGaussPoint)
            {
                connections[orderAddedCon[i]].weight = randNum;
            }
                
        }
        else if (mutType == WEIGHT_MUTATOR::COLDGAUSSIAN)
        {
        connections[orderAddedCon[i]].weight = randNum;
        }

        num += 1.0;
    }
}

//Parent A should be the fittest
void Genome::crossover(Genome& parentA, Genome& parentB, CROSSOVER type)
{
    nodes.clear();
    connections.clear();
    nodesToConnection.clear();

    //Insert nodes
    std::vector<GeneNode>* nodesA;

    int crossPoint = 999999;

    if (type == CROSSOVER::SINGLE_POINT)
    {
        if (parentA.getConnections()->size() < parentB.getConnections()->size())
        {
            crossPoint = randInt(0, parentA.getConnections()->size() - 1);
        }
        else {
            crossPoint = randInt(0, parentB.getConnections()->size() - 1);
        }

        //Take the most nodes to make sure that all the connections are valid
        if (parentA.getNodes()->size() > parentB.getNodes()->size())
        {
            nodesA = parentA.getNodes();
        }
        else {
            nodesA = parentB.getNodes();
        }
    }
    else {
        nodesA = parentA.getNodes();
    }

    for (unsigned int i = 0; i < nodesA->size(); i++)
    {
        unsigned int layer = (*nodesA)[i].getLayer();

        nodes.push_back(GeneNode((*nodesA)[i].getType(), (*nodesA)[i].getActivation(), layer));
    }

    //Insert connections of fittest parent (aka parentA)
    std::map<unsigned int, GeneConnection>* parentAConnections = parentA.getConnections();
    std::map<unsigned int, GeneConnection>* parentBConnections = parentB.getConnections();

    int cross = 0;

    for (std::map<unsigned int, GeneConnection>::iterator itA = (*parentAConnections).begin(); itA != (*parentAConnections).end() && cross < crossPoint; ++itA, ++cross)
    {
        std::map<unsigned int, GeneConnection>::iterator found = parentBConnections->find(itA->first);

        //If both parent have the same gene 
        if (found != parentBConnections->end())
        {
            //Pick one randomly
            if (type == CROSSOVER::RANDOM)
            {
                uint8_t aOrB = randInt(0, 1);

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

                if (type == CROSSOVER::AVERAGE)
                {
                    connections[itA->first].weight = (itA->second.getWeight() + found->second.getWeight()) / 2;
                }
            }
        }
        else {
            connections[itA->first] = itA->second;
        }

        if (connections[itA->first].getNodeA() >= nodes.size() || connections[itA->first].getNodeB() >= nodes.size())
        {
            int a;
        }

        nodesToConnection[std::pair<unsigned int, unsigned int>(connections[itA->first].getNodeA(), connections[itA->first].getNodeB())] = itA->first;
    }

    if (type != CROSSOVER::SINGLE_POINT)
    {
        orderAddedCon = parentA.orderAddedCon;
    }
    else
    {
        //Set the order in which each connection from parentA we're added
        for (int i = 0; i < parentA.orderAddedCon.size(); i++)
        {
            if (connections.find(parentA.orderAddedCon[i]) != connections.end())
            {
                orderAddedCon.push_back(parentA.orderAddedCon[i]);
            }
        }

        //Add connections of parentB
        std::map<unsigned int, GeneConnection>::iterator itB = (*parentBConnections).begin();

        for (int i = 0; i < crossPoint; ++itB, ++i);

        for (itB; itB != (*parentBConnections).end(); ++itB)
        {
            GeneConnection gene = itB->second;
            
            if (nodes[gene.nodeA].getLayer() <= nodes[gene.nodeB].getLayer())
            {
                if (nodes[gene.nodeA].getLayer() == nodes[gene.nodeB].getLayer())
                {
                    shiftNodes(gene.nodeB, nodes[gene.nodeA].getLayer());
                }

                connections[itB->first] = gene;
                nodesToConnection[std::pair<unsigned int, unsigned int>(connections[itB->first].getNodeA(), connections[itB->first].getNodeB())] = itB->first;
            }

            if (connections[itB->first].getNodeA() >= nodes.size() || connections[itB->first].getNodeB() >= nodes.size())
            {
                int a;
            }
        }

        //Set the order in which each connection from parentB we're added
        for (int i = 0; i < parentB.orderAddedCon.size(); i++)
        {
            if (connections.find(parentB.orderAddedCon[i]) != connections.end())
            {
                bool found = false;

                for (int cpt = 0; cpt < orderAddedCon.size() && found == false; cpt++)
                {
                    if (orderAddedCon[cpt] == parentB.orderAddedCon[i])
                    {
                        found = true;
                    }
                }

                if (found == false)
                {
                    orderAddedCon.push_back(parentA.orderAddedCon[i]);
                }
            }
        }
    }
}

void Genome::shiftNodes(unsigned int node, unsigned int layerMin)
{
    std::stack<unsigned int> nodeToShift;

    nodeToShift.push(node);
    nodes[node].layer = layerMin + 1;

    if ((layerMin + 1) > 100)
    {
        int err;
    }

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
                    if ((nodes[node].layer + 1) > 100)
                    {
                        int err;
                    }

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