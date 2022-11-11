// NeuroEvolution.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//

#include <iostream>
#include "NeatAlgoGen.h"

int main()
{
	srand(time(NULL));

	NeatParameters neatparam;

	neatparam.activationFunctions.push_back(new sigmoidActivation());

	neatparam.pbMutateLink = 0.05;
	neatparam.pbMutateNode = 0.03;
	neatparam.pbWeightShift = 0.7;
	neatparam.pbWeightRandom = 0.2;
	neatparam.pbToggleLink = 0.05;
	neatparam.weightShiftStrength = 2.5;
	neatparam.weightRandomStrength = 2.5;

	neatparam.disjointCoeff = 1.0;
	neatparam.excessCoeff = 1.0;
	neatparam.mutDiffCoeff = 0.4;
	
	neatparam.killRate = 0.2;

	neatparam.bestHigh = true;

	neatparam.fileSave = "save";
	neatparam.saveHistory = true;

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
	neatparam.adaptSpeciation = true;

	int n = 1;

	int popSize = 150;

	NeatAlgoGen neat(popSize, n*2+1, n, neatparam);

	std::vector<uint8_t> inputs;

	for (uint8_t i = 0; i < pow(2, n-1); i++)
	{
		inputs.push_back(i);
	}
	
	std::vector<float> correct(popSize, 0), mistake(popSize, 0);

	std::cout << "start\n";

	float leastMistake = 999999;

	for (int i3 = 0; i3 < 100; i3++)
	{
		for (int i = 0; i < popSize; i++)
		{
			correct[i] = 0;
			mistake[i] = 0;
		}

		for (int cpt = 0; cpt < inputs.size(); cpt++)
		{
			for (int cpt2 = cpt; cpt2 < inputs.size(); cpt2++)
			{
				std::vector<float> networkInputs;
				std::vector<float> networkOutputs;
				networkInputs.resize(n*2+1);

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
								multiplier = 1;
							}
							else {
								//correct[i]+=1;
							}

							correct[i] += networkOutputs[i2] * multiplier;
							//mistake[i] += (1 - networkOutputs[i2]) * multiplier;
						}
						else {
							if (networkOutputs[i2] >= 0.5)
							{
								mistake[i]++;
								multiplier = 1;
							}
							else {
								//correct[i] += 1;
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
			//correct[i] = pow(4 - mistake[i], 2);
		}

		neat.setScore(correct);

		for (int i = 0; i < 100; i++)
		{
			if (mistake[i] < leastMistake)
			{
				leastMistake = mistake[i];
			}

			if (mistake[i] == 0)
			{
				/*std::cout << "success !\n";
				neat.saveHistory();
				return 0;*/
			}
		}

		//std::cout << i3 << " " << leastMistake << std::endl;

		neat.evolve();
	}

	std::cout << leastMistake << std::endl;
	neat.saveHistory();
}

