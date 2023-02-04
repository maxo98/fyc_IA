#include "Neat.h"
#include "ThreadPool.h"

#define NEAT_EXPORT __declspec(dllexport)

template<typename type>
std::vector<type> wrapperArrayToVector(const type* data, const size_t length)
{
	std::vector<type> newVector = std::vector<type>(length);

	for (int i = 0; i < length; i++)
	{
		newVector[i] = data[i];
	}

	return newVector;
}

extern "C"
{
	NEAT_EXPORT NeatParameters* InitParameterInstance();
	NEAT_EXPORT void DeleteInstance(void* instance);
	NEAT_EXPORT Neat* InstantiateNeat(size_t popSize, unsigned int input, unsigned int output, NeatParameters* neatParam);
	NEAT_EXPORT NeuralNetwork* GetNeuralNetwork(Neat* neat, unsigned int popSize);
	NEAT_EXPORT void GetNeuralNetworkCompute(NeuralNetwork* neuralNetwork, float* outputArray, size_t lengthOutPut);
	NEAT_EXPORT void SetScore(Neat* neat, float* score, const unsigned int length);
	NEAT_EXPORT void SaveGoat(Neat* neat, char* fileName);
	NEAT_EXPORT Genome* LoadGoat(char* fileName);
	NEAT_EXPORT void Evolve(Neat* neat);
	NEAT_EXPORT void NetworkFromGenome(Genome* genome, NeuralNetwork* neuralNetwork);
	NEAT_EXPORT ThreadPool* InitThreadPool();
}

ThreadPool* InitThreadPool()
{
	ThreadPool* pool = ThreadPool::getInstance();
	pool->start();
	return pool;
}

void GetNeuralNetworkCompute(NeuralNetwork* neuralNetwork, float* outputArray, size_t lengthOutPut)
{
	std::vector<float> inputs = std::vector<float>(lengthOutPut + 1);
	std::vector<float> outputsConverted = std::vector<float>(lengthOutPut);

	neuralNetwork->compute(inputs, outputsConverted);

	for (int i = 0; i < lengthOutPut; i++)
	{
		outputArray[i] = outputsConverted[i];
	}
}

NeuralNetwork* GetNeuralNetwork(Neat* neat, unsigned int idx)
{
	return neat->getNeuralNetwork(idx);
}

void SaveGoat(Neat* neat, char* fileName)
{
	std::string s = fileName;
	neat->getGoat()->saveCurrentGenome(s);
}

Genome* LoadGoat(char* fileName)
{
	return Genome::loadGenome(fileName);
}

void NetworkFromGenome(Genome* genome, NeuralNetwork* neuralNetwork)
{
	Neat::genomeToNetwork(*genome, *neuralNetwork);
}

Genome* GetGoat(Neat* neat)
{
	return neat->getGoat();
}

void Evolve(Neat* neat)
{
	neat->evolve();
	return;
}

Neat* InstantiateNeat(size_t popSize, unsigned int input,unsigned int output, NeatParameters* neatParam)
{
	return new Neat(popSize, input, output, *neatParam, Neat::INIT::ONE);
}

void SetScore(Neat* neat, float* score, const unsigned int length)
{
	std::vector<float> newScore = wrapperArrayToVector(score, length);
	neat->setScore(newScore);
}

NeatParameters* InitParameterInstance()
{
	NeatParameters* newNeatParameters = new NeatParameters();

	newNeatParameters->activationFunctions.push_back(new SigmoidActivation());
	newNeatParameters->pbMutateLink = 0.05;// 0.05;
	newNeatParameters->pbMutateNode = 0.03;//0.03;
	newNeatParameters->pbWeight = 1;// 0.9;
	newNeatParameters->pbToggleLink = 0.05;// 0.05;
	newNeatParameters->weightMuteStrength = 2.5;// 2.5;
	newNeatParameters->disjointCoeff = 1.0;
	newNeatParameters->excessCoeff = 1.0;
	newNeatParameters->mutDiffCoeff = 0.4;
	newNeatParameters->weightCoeff = 0.5;
	newNeatParameters->killRate = 0.2;
	newNeatParameters->keepChamp = true;
	newNeatParameters->elistism = true;
	newNeatParameters->rouletteMultiplier = 2.0;
	newNeatParameters->champFileSave = "champ";
	newNeatParameters->avgFileSave = "avg";//Without extension type file
	newNeatParameters->saveChampHistory = false;
	newNeatParameters->saveAvgHistory = false;
	newNeatParameters->pbMateMultipoint = 0.6;
	newNeatParameters->pbMateSinglepoint = 0.0;
	newNeatParameters->interspeciesMateRate = 0.001;
	newNeatParameters->dropOffAge = 15;
	newNeatParameters->ageSignificance = 1.0;
	newNeatParameters->pbMutateOnly = 0.25;
	newNeatParameters->pbMateOnly = 0.2;
	newNeatParameters->speciationDistance = 4.0;
	newNeatParameters->speciationDistanceMod = 0.3;
	newNeatParameters->minExpectedSpecies = 4;
	newNeatParameters->maxExpectedSpecies = 8;
	newNeatParameters->adaptSpeciation = false;

	return newNeatParameters;
}

void DeleteInstance(void* instance)
{
	delete instance;
}