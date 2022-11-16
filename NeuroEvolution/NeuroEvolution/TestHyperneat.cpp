#include "TestHyperneat.h"

#define MAXIME

int launchHypeneatTest()
{
	int test = 10;
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
	neatparam.activationFunctions.push_back(new tanhActivation());
	neatparam.activationFunctions.push_back(new gaussianActivation());
	neatparam.activationFunctions.push_back(new absActivation());
	neatparam.activationFunctions.push_back(new reluActivation());
	neatparam.activationFunctions.push_back(new linearActivation());

	neatparam.pbMutateLink = 0.5;// 0.05;
	neatparam.pbMutateNode = 0.3;//0.03;
	//neatparam.pbWeightShift = 0.7;
	//neatparam.pbWeightRandom = 0.2;
	neatparam.pbWeight = 1;// 0.9;
	neatparam.pbToggleLink = 0.05;// 0.05;
	//neatparam.weightShiftStrength = 2.5;
	//neatparam.weightRandomStrength = 2.5;
	neatparam.weightMuteStrength = 5;// 2.5;
	neatparam.pbMutateActivation = 0.3;

	neatparam.disjointCoeff = 1.0;
	neatparam.excessCoeff = 1.0;
	neatparam.mutDiffCoeff = 0.4;

	neatparam.killRate = 0.2;

	neatparam.champFileSave = "champ";
	neatparam.avgFileSave = "avg";//Without extension type file
	neatparam.saveChampHistory = true;
	neatparam.saveAvgHistory = true;

	neatparam.scoreMultiplier = 1000;

	neatparam.pbMateMultipoint = 0.6;
	neatparam.pbMateSinglepoint = 0.0;
	neatparam.interspeciesMateRate = 0.001;
	neatparam.dropOffAge = 15;
	neatparam.ageSignificance = 1.0;
	neatparam.pbMutateOnly = 0.25;
	neatparam.pbMateOnly = 0.2;

#ifdef CEDRIC
	neatparam.speciationDistance = 5.0;
#endif CEDRIC

#ifdef MAXIME
	neatparam.speciationDistance = 7.0;
#endif MAXIME
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

	int popSize = 150;

	int result = 0;

	std::vector<float> pos;
	pos.resize(2);

	int count = 0;

	//for (int i = 0; i < 100; i++)
	{
		Hyperneat hyper(popSize, neatparam, hyperneatParam);

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

		if (hypeneatTest(popSize, test, n, grid, centers, hyper) == true)
		{
			result++;
		}

		hyper.saveHistory();
		
		std::vector<float> output;

		Neat neat;
		NeuralNetwork network;
		hyper.genomeToNetwork(*hyper.getGoat(), network);

		Genome* gen = hyper.getGoat();

		std::vector<float> gridTest;
		std::vector<float> centerTest;

		gridTest.resize(n * n);
		centerTest.resize(2);

		writeSquares(gridTest, centerTest, n);

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

		x = maxIndex % 10;
		y = floor(maxIndex / 10);

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
		std::cout << "gen " << i3 << std::endl;

		for (int i = 0; i < popSize; i++)
		{
			fitness[i] = n*10;// 4000;//(10*10 + 10*10)*20
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

			x = maxIndex % n;
			y = floor(maxIndex / n);

			//std::cout << centers[cpt][0] << " " << centers[cpt][1] << " " << x << " " << y << std::endl;

			fitness[i] -= sqrtf((centers[cpt][0] - x) * (centers[cpt][0] - x) + (centers[cpt][1] - y) * (centers[cpt][1] - y));
		}
	}
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