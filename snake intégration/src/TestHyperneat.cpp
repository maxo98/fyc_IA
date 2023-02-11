#include "TestHyperneat.h"
#include <iomanip>


int launchHypeneatTest()
{
	int test = 4;
	int n = 20;

	std::vector<std::vector<float>> grid;
	std::vector<std::vector<float>> centers;

	grid.resize(test);
	centers.resize(test);

	for (int cpt = 0; cpt < test; cpt++)
	{
		grid[cpt].resize(n*n, 0);
		centers[cpt].resize(2);

		writeSquares(grid[cpt], centers[cpt], n, cpt);
	}

	NeatParameters neatparam;

	//neatparam.activationFunctions.push_back(new thresholdActivation());
	neatparam.activationFunctions.push_back(new SigmoidActivation());
	neatparam.activationFunctions.push_back(new SinActivation());
	neatparam.activationFunctions.push_back(new GaussianActivation());
	neatparam.activationFunctions.push_back(new AbsActivation());

	neatparam.pbMutateLink = 0.1;// 0.05;
	neatparam.pbMutateNode = 0.06;//0.03;
	//neatparam.pbWeightShift = 0.7;
	//neatparam.pbWeightRandom = 0.2;
	neatparam.pbWeight = 0.9;// 0.9;
	neatparam.pbToggleLink = 0.05;// 0.05;
	//neatparam.weightShiftStrength = 2.5;
	//neatparam.weightRandomStrength = 2.5;
	neatparam.weightMuteStrength = 3.0;// 2.5;
	neatparam.pbMutateActivation = 0.7;

	neatparam.disjointCoeff = 1.0;
	neatparam.excessCoeff = 1.0;
	neatparam.mutDiffCoeff = 0.4;
	neatparam.activationDiffCoeff = 1.0;
	neatparam.weightCoeff = 0;

	neatparam.killRate = 0.2;

	neatparam.champFileSave = "champ";
	neatparam.avgFileSave = "avg";//Without extension type file
	neatparam.saveChampHistory = true;
	neatparam.saveAvgHistory = true;

	neatparam.pbMateMultipoint = 0.6;
	neatparam.pbMateSinglepoint = 0.0;
	neatparam.interspeciesMateRate = 0.001;
	neatparam.dropOffAge = 15;
	neatparam.ageSignificance = 1.0;
	neatparam.pbMutateOnly = 0.25;
	neatparam.pbMateOnly = 0.2;

	neatparam.speciationDistance = 2.0;


	neatparam.speciationDistanceMod = 0.3;
	neatparam.minExpectedSpecies = 6;
	neatparam.maxExpectedSpecies = 12;
	neatparam.adaptSpeciation = true;

	neatparam.keepChamp = true;
	neatparam.elistism = true;
	neatparam.rouletteMultiplier = 2.0;

	HyperneatParameters hyperneatParam;

	hyperneatParam.activationFunction = new SigmoidActivation();
	hyperneatParam.cppnInput = 5;
	hyperneatParam.cppnInputFunction = biasCppnInput;
	hyperneatParam.cppnOutput = 1;
	hyperneatParam.nDimensions = 2;
	hyperneatParam.thresholdFunction = noThreshold;// leoThreshold;
	hyperneatParam.weightModifierFunction = noChangeWeight;

	int popSize = 150;

	int result = 0;

	std::vector<float> pos;
	pos.resize(2);

	int count = 0;

	//for (int i = 0; i < 100; i++)
	{
		Hyperneat hyper(popSize, neatparam, hyperneatParam);

		for (int i = 1; i <= n; i++)
		{
			pos[0] = i;

			for (int j = 1; j <= n; j++)
			{
				pos[1] = j;

				hyper.addInput(pos);
				hyper.addOutput(pos);
				count++;
			}
		}

		hyper.initNetworks();

		hyper.generateNetworks();

		if (hypeneatTest(popSize, test, n, grid, centers, hyper) == true)
		{
			result++;
		}

		hyper.saveHistory();
		
		std::vector<float> output;

		NeuralNetwork network;
		hyper.genomeToNetwork(*hyper.getGoat(), network);

		std::vector<float> gridTest;
		std::vector<float> centerTest;

		gridTest.resize(n * n);
		centerTest.resize(2);

		writeSquares(gridTest, centerTest, n, 0);

		network.compute(gridTest, output);

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

		x = maxIndex % n;
		y = floor(maxIndex / n);

		std::cout << centerTest[0] << ", " << centerTest[1] << std::endl;
		std::cout << x << ", " << y << std::endl;

	}

	for (int i = 0; i < neatparam.activationFunctions.size(); i++)
	{
		delete neatparam.activationFunctions[i];
	}

	delete hyperneatParam.activationFunction;

	return 0;
}

bool hypeneatTest(int popSize, int test, int n, const std::vector<std::vector<float>>& grid, const std::vector<std::vector<float>>& centers, Hyperneat& hyper)
{
	std::vector<float> fitness;

	fitness.resize(popSize);

	for (int i3 = 0; i3 < 100; i3++)
	{
		std::cout << std::endl << "gen " << i3 << std::endl;

		for (int i = 0; i < popSize; i++)
		{
			fitness[i] = pow(n*test, 2);// 4000;//(10*10 + 10*10)*20
		}

		std::vector<std::thread> threads;
		unsigned int cpus = std::thread::hardware_concurrency();

		float totalWorkload = popSize;
		float workload = totalWorkload / cpus;
		float restWorkload = 0;
		int currentWorkload = totalWorkload;
		int startIndex = 0;

		/*while (workload < 1)
		{
			cpus--;
			workload = totalWorkload / cpus;
		}

		currentWorkload = floor(workload);
		float workloadFrac = fmod(workload, 1.0f);
		restWorkload = workloadFrac;

		while (cpus > threads.size() + 1)
		{
			threads.push_back(std::thread(evaluate, test, startIndex, n, currentWorkload + floor(restWorkload), std::ref(grid), std::ref(centers), std::ref(hyper), std::ref(fitness)));

			startIndex += currentWorkload + floor(restWorkload);

			restWorkload -= floor(restWorkload);
			restWorkload += workloadFrac;
		}

		while (restWorkload > 0)
		{
			restWorkload--;
			currentWorkload++;
		}*/

		evaluate(test, startIndex, n, currentWorkload, grid, centers, hyper, fitness);

		for (int i = 0; i < threads.size(); i++)
		{
			threads[i].join();
		}

		hyper.setScore(fitness);

		hyper.evolve();
	}

	std::cout << "done" << std::endl;

	return false;
}

void evaluate(int test, int startIndex, int n, int workload, const std::vector<std::vector<float>>& grid, const std::vector<std::vector<float>>& centers, Hyperneat& hyper, std::vector<float>& fitness)
{

	for (int i = startIndex; i < workload; i++)
	{
		std::vector<float> output;

		for (int cpt = 0; cpt < test; cpt++)
		{
			hyper.getNeuralNetwork(i)->compute(grid[cpt], output);

			float max = output[0];
			int maxIndex = 0;

			for (int cpt2 = 1; cpt2 < (n*n)-1; cpt2++)
			{
				if (max < output[cpt2])
				{
					max = output[cpt2];
					maxIndex = cpt2;
				}
			}

			int x, y;

			x = maxIndex % n + 1;
			y = floor(maxIndex / n) + 1;

			//std::cout << centers[cpt][0] << " " << centers[cpt][1] << " " << x << " " << y << std::endl;

			fitness[i] -= (centers[cpt][0] - x) * (centers[cpt][0] - x) + (centers[cpt][1] - y) * (centers[cpt][1] - y);
		}
	}
}

void writeSquares(std::vector<float>& grid, std::vector<float>& center, int n, int i = 0)
{
	int squareSize = 5;

	int x1 = randInt(0, n - squareSize);
	int y1 = randInt(0, n - squareSize);

	center[0] = x1 + squareSize / 2;
	center[1] = y1 + squareSize / 2;

	switch (i)
	{
	case 0:
		x1 = 3;
		y1 = 3;
		center[0] = 5;
		center[1] = 5;
		break;

	case 1:
		x1 = 13;
		y1 = 14;
		center[0] = 15;
		center[1] = 16;
		break;

	case 2:
		x1 = 7;
		y1 = 11;
		center[0] = 9;
		center[1] = 13;
		break;

	case 3:
		x1 = 14;
		y1 = 2;
		center[0] = 16;
		center[1] = 4;
		break;
	}



	for (int i = 0; i < squareSize; i++)
	{
		for (int cpt = 0; cpt < squareSize; cpt++)
		{
			grid[x1 + i + (y1 + cpt) * n] = 1;
		}
	}

	int squareSize2 = 3;

	bool valid = false;
	int x2;
	int y2;

	switch (i)
	{
	case 0:
		x2 = 15;
		y2 = 16;
		break;

	case 1:
		x2 = 4;
		y2 = 3;
		break;

	case 2:
		x2 = 14;
		y2 = 4;
		break;

	case 3:
		x2 = 9;
		y2 = 11;
		break;
	}

	/*do {
		x2 = randInt(0, n - squareSize2);
		y2 = randInt(0, n - squareSize2);

		if (((x2 + squareSize2) < x1) || (x2 > (x1 + squareSize)) || ((y2 + squareSize2) < y1) || (y2 > (y1 + squareSize)))
		{
			valid = true;
		}

	}while(valid == false);*/

	for (int i = 0; i < squareSize2; i++)
	{
		for (int cpt = 0; cpt < squareSize2; cpt++)
		{
			grid[x2 + i + (y2 + cpt) * n] = 1;
		}
	}
}