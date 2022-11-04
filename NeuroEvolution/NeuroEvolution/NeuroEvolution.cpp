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
	neatparam.pbMutateNode = 0.1;
	neatparam.pbWeightShift = 0.15;
	neatparam.pbWeightRandom = 0.05;
	neatparam.pbToggleLink = 0.05;
	neatparam.weightShiftStrength = 2.5;
	neatparam.weightRandomStrength = 2.5;

	neatparam.C1 = 2.0;
	neatparam.C2 = 2.0;
	neatparam.C3 = 1.0;
	neatparam.speciationDistance = 6.0;
	neatparam.survivors = 0.8;

	neatparam.bestHigh = true;

	neatparam.fileSave = "save";
	neatparam.saveHistory = true;

	neatparam.scoreMultiplier = 1000;

	int n = 3;

	NeatAlgoGen neat(100, n*2+1, n, neatparam);

	std::vector<uint8_t> inputs;

	for (uint8_t i = 0; i < pow(2, n-1); i++)
	{
		inputs.push_back(i);
	}

	
	std::vector<float> correct(100, 0), mistake(100, 0);

	std::cout << "start\n";

	for (int i3 = 0; i3 < 100; i3++)
	{
		

		for (int i = 0; i < 100; i++)
		{
			correct[i] = pow(n, n);
			mistake[i] = pow(n, n);
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

				for (int i = 0; i < 100; i++)
				{
					NeuralNetwork* network = neat.getNeuralNetwork(i);

					network->compute(networkInputs, networkOutputs);

					for (int i2 = 0; i2 < networkOutputs.size(); i2++)
					{
						float multiplier = 0.5;

						if (((result >> i2) & 1) == 1)
						{
							if (networkOutputs[i2] <= 0.5)
							{
								mistake[i]--;
								multiplier = 1;
							}

							correct[i] -= (1 - networkOutputs[i2]) * multiplier;
						}
						else {
							if (networkOutputs[i2] >= 0.5)
							{
								mistake[i]--;
								multiplier = 1;
							}

							correct[i] -= networkOutputs[i2] * multiplier;
						}
					}
				}
			}
		}

		neat.setScore(mistake);

		float leastMistake = 0;

		for (int i = 0; i < 100; i++)
		{
			if (mistake[i] > leastMistake)
			{
				leastMistake = mistake[i];
			}

			if (mistake[i] == pow(n, n))
			{
				std::cout << "success !\n";
				neat.saveHistory();
				return 0;
			}
		}

		//std::cout << i3 << " " << leastMistake << std::endl;

		neat.evolve();
	}

	//std::cout << leastMistake << std::endl;
	neat.saveHistory();
}

