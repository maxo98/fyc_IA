#include "TestHyperneat.h"

int launchHypeneatTest()
{
	int test = 20;
	int n = 10;

	std::vector<std::vector<float>> grid;
	std::vector<std::vector<float>> centers;

	grid.resize(test);
	centers.resize(test);

	for (int cpt = 0; cpt < test; cpt++)
	{
		grid[cpt].resize(n*n, 0);
		centers[cpt].resize(2);

		writeSquares(grid[cpt], centers[cpt], n);
	}

	NeatParameters neatparam;

	//neatparam.activationFunctions.push_back(new thresholdActivation());
	neatparam.activationFunctions.push_back(new sigmoidActivation());
	neatparam.activationFunctions.push_back(new sinActivation());
	neatparam.activationFunctions.push_back(new hyperTanActivation());
	neatparam.activationFunctions.push_back(new gaussianActivation());

	neatparam.pbMutateLink = 0.5;// 0.05;
	neatparam.pbMutateNode = 0.3;//0.03;
	//neatparam.pbWeightShift = 0.7;
	//neatparam.pbWeightRandom = 0.2;
	neatparam.pbWeight = 1;// 0.9;
	neatparam.pbToggleLink = 0.05;// 0.05;
	//neatparam.weightShiftStrength = 2.5;
	//neatparam.weightRandomStrength = 2.5;
	neatparam.weightMuteStrength = 5;// 2.5;

	neatparam.disjointCoeff = 1.0;
	neatparam.excessCoeff = 1.0;
	neatparam.mutDiffCoeff = 0.4;

	neatparam.killRate = 0.2;

	neatparam.bestHigh = true;//Remenber false option doesn't currently work

	neatparam.champFileSave = "champ";
	neatparam.avgFileSave = "avg";//Without extension type file
	neatparam.saveChampHistory = true;
	neatparam.saveAvgHistory = false;

	neatparam.scoreMultiplier = 1000;

	neatparam.pbMateMultipoint = 0.6;
	neatparam.pbMateSinglepoint = 0.0;
	neatparam.interspeciesMateRate = 0.001;
	neatparam.dropOffAge = 15;
	neatparam.ageSignificance = 1.0;
	neatparam.pbMutateOnly = 0.25;
	neatparam.pbMateOnly = 0.2;

	neatparam.speciationDistance = 2.0;
	neatparam.speciationDistanceMod = 0.3;
	neatparam.numSpeciesTarget = 4;
	neatparam.adaptSpeciation = false;//Doesn't seem to have a lot of effect, despite being taken from official implementation

	HyperneatParameters hyperneatParam;

	hyperneatParam.activationFunction = new sigmoidActivation();
	hyperneatParam.cppnInput = 3;
	hyperneatParam.cppnInputFunction = basicCppnInput;
	hyperneatParam.cppnOutput = 2;
	hyperneatParam.nDimensions = 2;
	hyperneatParam.thresholdFunction = leoThreshold;
	hyperneatParam.weightModifierFunction = noChangeWeight;

	int popSize = 50;

	int result = 0;

	std::vector<float> pos, output;
	pos.resize(2);
	output.resize(n * n);

	int count = 0;

	//for (int i = 0; i < 100; i++)
	{
		Hyperneat hyper(50, neatparam, hyperneatParam);

		for (int i = 0; i < n; i++)
		{
			pos[0] = i;

			for (int j = 0; j < n; j++)
			{
				pos[1] = j;

				hyper.addInput(pos);
				hyper.addOutput(pos);
				count++;
			}
		}

		hyper.generateNetworks();

		if (hypeneatTest(grid, output, centers, hyper) == true)
		{
			result++;
		}

		hyper.saveHistory();
	}

	for (int i = 0; i < neatparam.activationFunctions.size(); i++)
	{
		delete neatparam.activationFunctions[i];
	}

	delete hyperneatParam.activationFunction;

	return 0;
}

bool hypeneatTest(const std::vector<std::vector<float>>& grid, std::vector<float>& output, const std::vector<std::vector<float>>& centers, Hyperneat& hyper)
{
	std::vector<float> fitness;

	fitness.resize(50);

	for (int i3 = 0; i3 < 100; i3++)
	{
		for (int i = 0; i < 50; i++)
		{
			fitness[i] = 4000;//(10*10 + 10*10)*20
		}

		for (int i = 0; i < 50; i++)
		{
			for (int cpt = 0; cpt < 20; cpt++)
			{
				hyper.getNeuralNetwork(i)->compute(grid[cpt], output);

				float max = output[0];
				int maxIndex = 0;

				for (int cpt2 = 1; cpt2 < 100; cpt2++)
				{
					if (max < output[cpt2])
					{
						max = output[cpt2];
						maxIndex = cpt2;
					}
				}

				int x, y;

				x = maxIndex % 10;
				y = floor(maxIndex / 10);

				fitness[i] -= ((centers[cpt][0] - x) * (centers[cpt][0] - x) + (centers[cpt][1] - y) * (centers[cpt][1] - y));
			}
		}

		hyper.setScore(fitness);

		hyper.evolve();
	}

	return false;
}

void writeSquares(std::vector<float>& grid, std::vector<float>& center, int n)
{
	int squareSize = 4;

	int x1 = randInt(0, n - squareSize);
	int y1 = randInt(0, n - squareSize);

	center[0] = x1 + squareSize / 2;
	center[1] = y1 + squareSize / 2;

	for (int i = 0; i < squareSize; i++)
	{
		for (int cpt = 0; cpt < squareSize; cpt++)
		{
			grid[x1 + i + (y1 + cpt) * n] = 1;
		}
	}

	int squareSize2 = 2;

	bool valid = false;
	int x2;
	int y2;

	do {
		x2 = randInt(0, n - squareSize2);
		y2 = randInt(0, n - squareSize2);

		if (((x2 + squareSize2) < x1) || (x2 > (x1 + squareSize)) || ((y2 + squareSize2) < y1) || (y2 > (y1 + squareSize)))
		{
			valid = true;
		}

	}while(valid == false);

	for (int i = 0; i < squareSize2; i++)
	{
		for (int cpt = 0; cpt < squareSize2; cpt++)
		{
			grid[x2 + i + (y2 + cpt) * n] = 1;
		}
	}
}