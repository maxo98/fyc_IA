// Fill out your copyright notice in the Description page of Project Settings.


#include "Genome.h"
#include <stack>

Genome::Genome()
{
    input = 0;
}

Genome::Genome(unsigned int _input, unsigned int _output, std::vector<Activation*> activationFunctions, bool cppn)
{
    input = _input;
    output = _output;

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
void Genome::addConnection(unsigned int nodeA, unsigned int nodeB, std::unordered_map<std::pair<unsigned int, unsigned int>, unsigned int>& allConnections, float weight, std::mutex* lock)
{
    if (lock != nullptr)
    {
        lock->lock();
    }

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

    if (lock != nullptr)
    {
        lock->unlock();
    }
}

bool Genome::mutateLink(std::unordered_map<std::pair<unsigned int, unsigned int>, unsigned int>& allConnections, std::mutex* lock)
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
        if (nodes.size() == (input + output))
        {
            nodeA = randInt(0, nodes.size() - 1);
            nodeB = randInt(0, nodes.size() - 1);
        }
        else {
            nodeA = randInt(0, nodes.size() - 1 - output);
            nodeB = randInt(input, nodes.size() - 1);
        }

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
        addConnection(nodeA, nodeB, allConnections, 1, lock);
        return true;
    }

    return false;
}

bool Genome::mutateNode(std::unordered_map<std::pair<unsigned int, unsigned int>, unsigned int>& allConnections, Activation* activationFunction, std::mutex* lock)
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

    nodes.push_back(GeneNode(NODE_TYPE::HIDDEN, activationFunction, nodes[connection->getNodeA()].layer + 1));
    unsigned int nodeC = nodes.size()-1;

    //Shift node's layer
    if (nodes[nodeA].layer + 1 == nodes[nodeB].layer)
    {
        shiftNodes(nodeB, nodes[nodeB].layer);
    }

    //Create the new connections
    addConnection(nodeA, nodeC, allConnections, 1, lock);
    addConnection(nodeC, nodeB, allConnections, 1, lock);
    return true;
}

void Genome::mutateLinkToggle()
{
    unsigned int geneConnectionIndex = randInt(0, connections.size() - 1);
    GeneConnection* connection = &std::next(connections.begin(), geneConnectionIndex)->second;

    //Switches the activation of the connection
    connection->enabled = !connection->enabled;
}

void Genome::mutateActivation(std::vector<Activation*>& activationFunctions)
{
    if (nodes.size() != 0)
    {
        int indexNode = 1;

        if (nodes.size() - input > 1)
        {
            indexNode = randGeoDist(0.1, (nodes.size() - input - 1)) + 1;
        }

        unsigned int index = randInt(0, activationFunctions.size() - 1);
        nodes[nodes.size() - indexNode].setActivation(activationFunctions[index]);
    }
    else {
        std::cout << "Error mutating genome with 0 nodes\n";
    }

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

void Genome::saveCurrentGenome(const std::string& fileName) 
{
    std::list<DataToSaveStruct> saveBuffer;

    for (const auto& node : nodes)
    {
        saveBuffer.push_back(DataToSaveStruct(&node));
    }

    for (const auto& connection : connections)
    {
        // first is the id from the map, second is GeneConnection
        saveBuffer.push_back(DataToSaveStruct(&connection.second, connection.first));
    }

    std::fstream file;
    file.open(fileName, std::fstream::out | std::fstream::trunc);

    for (const auto& data : saveBuffer)
    {
        file << data;
    }

    file.close();
}

Genome Genome::loadGenome(const std::string& fileName)
{
    std::list<DataToSaveStruct> loadBuffer;
    std::fstream file;
    Genome loadedGenome = Genome();

    file.open(fileName, std::fstream::in);

    if (!file.is_open())
    {
        throw("file has not been opened");
        file.close();
        return loadedGenome;
    }

    std::string line;
    
    while (getline(file, line))
    {
#ifdef DEBUG
        std::cout << line << std::endl;
#endif // DEBUG

        std::vector<std::string> stringSplited;
        int idxStart, idxEnd;
        idxStart = idxEnd = 0;

        while ((idxStart = line.find_first_not_of(' ', idxEnd)) != std::string::npos) 
        { 
            idxEnd = line.find(' ', idxStart);
            stringSplited.push_back(line.substr(idxStart, idxEnd - idxStart));
        }

        if (std::stoi(stringSplited[0]) == (int)DataToSaveEnum::GENECONNECTION)
        {
            loadedGenome.connections.insert(std::make_pair(std::stoi(stringSplited[stringSplited.size() - 2]), loadedGenome.loadGeneConnection(stringSplited)));
        }
        else
        {
            loadedGenome.nodes.push_back(loadedGenome.loadGeneNode(stringSplited));
        }
    }

    file.close();
    return loadedGenome;
}

GeneConnection Genome::loadGeneConnection(const std::vector<std::string> data)
{
    auto loadedGeneConnection = GeneConnection(std::stoi(data[1]), std::stoi(data[2]), std::stoi(data[3]));
    loadedGeneConnection.setWeight(std::stof(data[5]));
    loadedGeneConnection.setEnabled(std::stoi(data[4]));
    return loadedGeneConnection;
}

GeneNode Genome::loadGeneNode(const std::vector<std::string> data)
{
    return GeneNode(static_cast<NODE_TYPE>(std::stoi(data[3])), Activation::initActivation(data[1]), std::stoi(data[2]));
}

std::string Genome::toString()
{
    std::string str = "";

    str += "input " + std::to_string(input) + " score " + std::to_string(score) + " speciesScore " + std::to_string(speciesScore) + " superChampOffspring " + std::to_string(superChampOffspring)
        + " eliminate " + std::to_string(eliminate) + " connection size " + std::to_string(connections.size()) + " nodesToConnection size " + std::to_string(nodesToConnection.size())
        + " orderAddedCon size " + std::to_string(orderAddedCon.size()) + " nodes size " + std::to_string(nodes.size());

    return str;
}

void Genome::addHiddenNode(Activation* activation, unsigned int layer)
{
    nodes.push_back(GeneNode(NODE_TYPE::HIDDEN, activation, layer));
}

void Genome::fullyConnect(int nLayer, int nNode, Activation* hiddenActivation, Activation* outputActivation, std::unordered_map<std::pair<unsigned int, unsigned int>, unsigned int>& allConnections,
    WeightInitFunction weightInit, const long long& seed)
{
    nodes.clear();

    connections.clear();
    nodesToConnection.clear();
    orderAddedCon.clear();

    for (unsigned int i = 0; i < input; i++)
    {
        nodes.push_back(GeneNode(NODE_TYPE::INPUT, hiddenActivation));
    }

    if (nLayer != 0)
    {
        //Create and connect hidden layers
        for (unsigned int i = 0; i < nLayer; i++)
        {
            for (unsigned int cpt = 0; cpt < nNode; cpt++)
            {
                int in = (i == 0 ? input-1 : nNode);
                int out = (i == (nLayer - 1) ? output : nNode);

                nodes.push_back(std::move(GeneNode(NODE_TYPE::HIDDEN, hiddenActivation, i + 1)));

                if (i == 0)
                {
                    for (unsigned int i2 = 0; i2 < input - 1; i2++)
                    {
                        addConnection(i2, cpt + i * nNode + input, allConnections, weightInit(in, out, seed));
                    }
                }
                else {
                    for (unsigned int i2 = 0; i2 < nNode; i2++)
                    {
                        addConnection(i2 + (i - 1) * nNode + input, cpt + i * nNode + input, allConnections, weightInit(in, out, seed));
                    }
                }

                addConnection(input - 1, cpt + i * nNode + input, allConnections, 0);//Bias connection
            }
        }

        //Create and connect output nodes
        for (unsigned int i = 0; i < output; i++)
        {
            nodes.push_back(GeneNode(NODE_TYPE::OUTPUT, outputActivation, 999999));

            for (unsigned int i2 = 0; i2 < nNode; i2++)
            {
                addConnection(i2 + (nLayer - 1) * nNode + input, i + nLayer * nNode + input, allConnections, weightInit(nNode, 0, seed));
            }

            addConnection(input - 1, i + nLayer * nNode + input, allConnections, 0);//Bias connection
        }
    }
    else {
        for (unsigned int i = 0; i < output; i++)
        {
            nodes.push_back(GeneNode(NODE_TYPE::OUTPUT, outputActivation, 999999));

            for (unsigned int i2 = 0; i2 < input; i2++)
            {
                addConnection(i2, i + input, allConnections, weightInit(input, 0, seed));
            }

            addConnection(input - 1, i + nLayer * nNode + input, allConnections, 0);//Bias connection
        }
    }

}