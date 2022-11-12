// NeuroEvolution.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//

#include <iostream>
#include "Neat.h"

bool task(Neat& neat, int n, int popSize, float &leastMistake, std::vector<uint8_t>& inputs);

int main()
{
	auto seed = time(NULL);

	std::cout << "seed " << seed << std::endl;

	srand(seed);

	NeatParameters neatparam;

	//neatparam.activationFunctions.push_back(new thresholdActivation());
	neatparam.activationFunctions.push_back(new sigmoidActivation());

	neatparam.pbMutateLink = 0.05;
	neatparam.pbMutateNode = 0.03;
	//neatparam.pbWeightShift = 0.7;
	//neatparam.pbWeightRandom = 0.2;
	neatparam.pbWeight = 0.9;
	neatparam.pbToggleLink = 0.05;
	//neatparam.weightShiftStrength = 2.5;
	//neatparam.weightRandomStrength = 2.5;
	neatparam.weightMuteStrength = 2.5;

	neatparam.disjointCoeff = 1.0;
	neatparam.excessCoeff = 1.0;
	neatparam.mutDiffCoeff = 0.4;
	
	neatparam.killRate = 0.2;

	neatparam.bestHigh = true;//Remenber false option doesn't currently work

	neatparam.champFileSave = "champ";
	neatparam.avgFileSave = "avg";//Without extension type file
	neatparam.saveChampHistory = false;
	neatparam.saveAvgHistory = false;

	neatparam.scoreMultiplier = 1000;

	neatparam.pbMateMultipoint = 0.6;
	neatparam.pbMateSinglepoint = 0.0;
	neatparam.interspeciesMateRate = 0.001;
	neatparam.dropOffAge = 15;
	neatparam.ageSignificance = 1.0;
	neatparam.pbMutateOnly = 0.25;
	neatparam.pbMateOnly = 0.2;

	neatparam.speciationDistance = 3.0;
	neatparam.speciationDistanceMod = 0.3;
	neatparam.numSpeciesTarget = 4;
	neatparam.adaptSpeciation = false;//Doesn't seem to have a lot of effect, despite being taken from official implementation

	int n = 1;

	int popSize = 150;

	Neat neat(popSize, n*2+1, n, neatparam, Neat::INIT::FULL);

	std::vector<uint8_t> inputs;

	for (int i = -1; i < n; i++)
	{
		if (i == -1)
		{
			inputs.push_back(0);
		}
		else {
			inputs.push_back(1 << i);
		}
	}

	float leastMistake = 999999;

	task(neat, n, popSize, leastMistake, inputs);

	//std::cout << neat.getGoat()->getConnections()->size() << " connections\n";

	NeuralNetwork network;
	neat.genomeToNetwork(*neat.getGoat(), network);

	for (int cpt = 0; cpt < inputs.size(); cpt++)
	{
		for (int cpt2 = 0; cpt2 < inputs.size(); cpt2++)
		{
			std::vector<float> networkInputs;
			std::vector<float> networkOutputs;
			networkInputs.resize(n * 2 + 1);

			uint8_t result = inputs[cpt] ^ inputs[cpt2];

			for (int i2 = 0; i2 < n; i2++)
			{
				networkInputs[i2] = ((inputs[cpt] >> i2) & 1);
			}

			for (int i2 = 0; i2 < n; i2++)
			{
				networkInputs[i2 + n] = ((inputs[cpt2] >> i2) & 1);
			}

			networkInputs[networkInputs.size() - 1] = 1;
			
			network.compute(networkInputs, networkOutputs);

			std::cout << (int)inputs[cpt] << (int)inputs[cpt2] << " " << (inputs[cpt] ^ inputs[cpt2]) << networkOutputs[0] << std::endl;
		}
	}

	std::cout << leastMistake << std::endl;
	neat.saveHistory();

	return 0;
}

bool task(Neat& neat, int n, int popSize, float& leastMistake, std::vector<uint8_t>& inputs)
{

	std::vector<float> correct(popSize, 0), mistake(popSize, 0);

	std::cout << "start\n";

	sigmoidActivation activation;

	for (int i3 = 0; i3 < 200; i3++)
	{
		for (int i = 0; i < popSize; i++)
		{
			correct[i] = 0;
			mistake[i] = 0;
		}

		for (int cpt = 0; cpt < inputs.size(); cpt++)
		{
			for (int cpt2 = 0; cpt2 < inputs.size(); cpt2++)
			{
				std::vector<float> networkInputs;
				std::vector<float> networkOutputs;
				networkInputs.resize(n * 2 + 1);

				uint8_t result = inputs[cpt] ^ inputs[cpt2];

				for (int i2 = 0; i2 < n; i2++)
				{
					networkInputs[i2] = ((inputs[cpt] >> i2) & 1);
				}

				for (int i2 = 0; i2 < n; i2++)
				{
					networkInputs[i2 + n] = ((inputs[cpt2] >> i2) & 1);
				}

				networkInputs[networkInputs.size() - 1] = 1;

				for (int i = 0; i < popSize; i++)
				{
					NeuralNetwork* network = neat.getNeuralNetwork(i);

					network->compute(networkInputs, networkOutputs);

					for (int i2 = 0; i2 < networkOutputs.size(); i2++)
					{
						float multiplier = 1;

						if (((result >> i2) & 1) == 1)
						{
							if (networkOutputs[i2] <= 0.5)
							{
								mistake[i]++;
							}
							else {
								correct[i]+=1;
							}

							correct[i] += networkOutputs[i2] * multiplier;
							//mistake[i] += (1 - networkOutputs[i2]) * multiplier;
						}
						else {
							if (networkOutputs[i2] > 0.5)
							{
								mistake[i]++;
							}
							else {
								correct[i] += 1;
							}

							correct[i] += (1 - networkOutputs[i2]) * multiplier;
							//mistake[i] += (networkOutputs[i2]) * multiplier;
						}
					}
				}
			}
		}

		for (int i = 0; i < popSize; i++)
		{
			correct[i] = pow(correct[i], 2);
		}

		neat.setScore(correct);

		for (int i = 0; i < popSize; i++)
		{
			if (mistake[i] < leastMistake)
			{
				leastMistake = mistake[i];
			}

			if (mistake[i] == 0)
			{
				std::cout << "success !\n";
				neat.saveHistory();
				return true;
			}
		}

		//std::cout << i3 << " " << leastMistake << std::endl;

		neat.evolve();
	}

	return false;
}