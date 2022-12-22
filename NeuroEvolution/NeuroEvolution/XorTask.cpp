#include "XorTask.h"
#include <iostream>

int launchXor()
{
	NeatParameters neatparam;

	//neatparam.activationFunctions.push_back(new thresholdActivation());
	neatparam.activationFunctions.push_back(new SigmoidActivation());

	neatparam.pbMutateLink = 0.05;// 0.05;
	neatparam.pbMutateNode = 0.03;//0.03;
	neatparam.pbWeight = 1;// 0.9;
	neatparam.pbToggleLink = 0.05;// 0.05;
	neatparam.weightMuteStrength = 2.5;// 2.5;

	neatparam.disjointCoeff = 1.0;
	neatparam.excessCoeff = 1.0;
	neatparam.mutDiffCoeff = 0.4;
	neatparam.weightCoeff = 0.5;

	neatparam.killRate = 0.2;

	neatparam.keepChamp = true;
	neatparam.elistism = true;
	neatparam.rouletteMultiplier = 2.0;

	neatparam.champFileSave = "champ";
	neatparam.avgFileSave = "avg";//Without extension type file
	neatparam.saveChampHistory = false;
	neatparam.saveAvgHistory = false;

	neatparam.pbMateMultipoint = 0.6;
	neatparam.pbMateSinglepoint = 0.0;
	neatparam.interspeciesMateRate = 0.001;
	neatparam.dropOffAge = 15;
	neatparam.ageSignificance = 1.0;
	neatparam.pbMutateOnly = 0.25;
	neatparam.pbMateOnly = 0.2;

	neatparam.speciationDistance = 4.0;
	neatparam.speciationDistanceMod = 0.3;
	neatparam.minExpectedSpecies = 4;
	neatparam.maxExpectedSpecies = 8;
	neatparam.adaptSpeciation = false;

	int n = 1;

	int popSize = 150;

	int result = 0;

	std::vector<std::vector<float>> inputs;

	inputs.push_back(std::vector<float>());
	inputs[0].push_back(0);
	inputs[0].push_back(0);
	inputs[0].push_back(1);
	inputs.push_back(std::vector<float>());
	inputs[1].push_back(1);
	inputs[1].push_back(0);
	inputs[1].push_back(1);
	inputs.push_back(std::vector<float>());
	inputs[2].push_back(0);
	inputs[2].push_back(1);
	inputs[2].push_back(1);
	inputs.push_back(std::vector<float>());
	inputs[3].push_back(1);
	inputs[3].push_back(1);
	inputs[3].push_back(1);

	std::vector<std::vector<float>> outputs;

	outputs.push_back(std::vector<float>());
	outputs[0].push_back(0);
	outputs.push_back(std::vector<float>());
	outputs[1].push_back(1);
	outputs.push_back(std::vector<float>());
	outputs[2].push_back(1);
	outputs.push_back(std::vector<float>());
	outputs[3].push_back(0);

	for (int i = 0; i < 100; i++)
	{
		Neat neat(popSize, n * 2 + 1, n, neatparam, Neat::INIT::ONE);

		//! faire le test ici 
		std::cout << "iteration : " << i << std::endl;
		
		float leastMistake = 999999;

		if (xorTask(neat, n, popSize, leastMistake, inputs, outputs) == true)
		{
			result++;
		}

		neat.getGoat()->saveCurrentGenome();
	}

	std::cout << result << std::endl;

	for (int i = 0; i < neatparam.activationFunctions.size(); i++)
	{
		delete neatparam.activationFunctions[i];
	}

	return 0;
}

bool xorTask(Neat& neat, int n, int popSize, float& leastMistake, std::vector<std::vector<float>>& inputs, std::vector<std::vector<float>>& outputs)
{

	std::vector<float> correct(popSize, 0), mistake(popSize, 0);

	std::cout << "start\n";

	for (int i3 = 0; i3 < 100; i3++)
	{

		for (int i = 0; i < popSize; i++)
		{
			correct[i] = 0;
			mistake[i] = 0;
		}

		for (int cpt = 0; cpt < inputs.size(); cpt++)
		{
			std::vector<float> networkOutputs;

			for (int i = 0; i < popSize; i++)
			{
				NeuralNetwork* network = neat.getNeuralNetwork(i);

				network->compute(inputs[cpt], networkOutputs);

				float multiplier = 1;

				if (outputs[cpt][0] == 1)
				{
					if (networkOutputs[0] <= 0.5)
					{
						mistake[i]++;
					}
					else {
						correct[i] += 1;
					}

					correct[i] += networkOutputs[0] * multiplier;
					//mistake[i] += (1 - networkOutputs[i2]) * multiplier;
				}
				else {
					if (networkOutputs[0] > 0.5)
					{
						mistake[i]++;
					}
					else {
						correct[i] += 1;
					}

					correct[i] += (1 - networkOutputs[0]) * multiplier;
					//mistake[i] += (networkOutputs[i2]) * multiplier;
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
				std::cout << i3 << " generation success !\n";
				neat.saveHistory();
				return true;
			}
		}

		neat.evolve();
	}

	return false;
}