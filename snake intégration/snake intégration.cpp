#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <iomanip>

#include "ThreadPool.h"
#include "Neat.h"

#define MULTITHREAD
#define SCREEN_SIZE 20

int snakeTest(NeuralNetwork* network, bool display, bool& validation)
{
	std::vector<float> scoreArray;
	int wins = 0;

	scoreArray.resize(20, 0);

	for (int test = 0; (test < 20 && display == false) || (test < 1 && display == true); test++)
	{
		if (display == true)
		{
			std::cout << "new snake \n";
		}

		std::pair<int, int> fruits;
		char screen[SCREEN_SIZE][SCREEN_SIZE];

		std::deque<std::pair<int, int>> snake;

		int direction = 2, score = 0, input = 0, vie = 1, cpt, found = 0, mange = 0;

		for (int i = 0; i < SCREEN_SIZE; i++)
		{
			for (cpt = 0; cpt < SCREEN_SIZE; cpt++)
			{
				screen[i][cpt] = 0;
			}
		}

		for (int i = 0; i < 3; i++)
		{
			screen[SCREEN_SIZE / 2 + i - 1][SCREEN_SIZE / 2] = -1;
			snake.push_back(std::pair<int, int>(SCREEN_SIZE / 2 + i - 1, SCREEN_SIZE / 2));
		}

		do
		{
			fruits.first = rand() % (SCREEN_SIZE);
			fruits.second = rand() % (SCREEN_SIZE);

			if (screen[fruits.first][fruits.second] != -1)
			{
				screen[fruits.first][fruits.first] = 5;
				found = 1;
			}

		} while (found == 0);

		int timer = 0;

		std::vector<float> networkInput, output;
		int countChangeDir = 0;

		networkInput.resize(8, 0);

		do
		{
			timer++;

			if (display == true)
			{
				std::cout << snake.back() << " " << fruits << std::endl;
			}

			networkInput[0] = (sqrt(pow(SCREEN_SIZE, 2) * 2) - sqrt(pow(snake.back().first - fruits.first - 0.5, 2) + pow(snake.back().second - fruits.second, 2))) / (SCREEN_SIZE * 2);
			networkInput[1] = (sqrt(pow(SCREEN_SIZE, 2) * 2) - sqrt(pow(snake.back().first - fruits.first + 0.5, 2) + pow(snake.back().second - fruits.second, 2))) / (SCREEN_SIZE * 2);
			networkInput[2] = (sqrt(pow(SCREEN_SIZE, 2) * 2) - sqrt(pow(snake.back().first - fruits.first, 2) + pow(snake.back().second - fruits.second - 0.5, 2))) / (SCREEN_SIZE * 2);
			networkInput[3] = (sqrt(pow(SCREEN_SIZE, 2) * 2) - sqrt(pow(snake.back().first - fruits.first, 2) + pow(snake.back().second - fruits.second + 0.5, 2))) / (SCREEN_SIZE * 2);

			for (int n = 0; n < 4; n++)
			{
				networkInput[n + 4] = 0;
			}

			float result = 0;

			std::pair<int, int> pos;

			for (int i = -1; i <= SCREEN_SIZE; i++)
			{
				pos.first = snake.back().first - i;

				for (int j = -1; j <= SCREEN_SIZE; j++)
				{
					pos.second = snake.back().second - j;

					if (i == -1 || j == -1 || i == SCREEN_SIZE || j == SCREEN_SIZE ||
						(i > -1 && i < SCREEN_SIZE && j > -1 && j << SCREEN_SIZE && screen[i][j] == -1
							&& (pos.first != 0 || pos.second != 0)))
					{
						float dist = -sqrt((pow(SCREEN_SIZE, 2) * 2) - (pow(float(pos.first) - 0.5f, 2) + pow(float(pos.second), 2))) / (SCREEN_SIZE * 2);
						if (dist < networkInput[4]) networkInput[4] = dist;

						dist = -sqrt((pow(SCREEN_SIZE, 2) * 2) - (pow(float(pos.first) + 0.5f, 2) + pow(float(pos.second), 2))) / (SCREEN_SIZE * 2);
						if (dist < networkInput[5]) networkInput[5] = dist;

						dist = -sqrt((pow(SCREEN_SIZE, 2) * 2) - (pow(float(pos.first), 2) + pow(float(pos.second) - 0.5f, 2))) / (SCREEN_SIZE * 2);
						if (dist < networkInput[6]) networkInput[6] = dist;

						dist = -sqrt((pow(SCREEN_SIZE, 2) * 2) - (pow(float(pos.first), 2) + pow(float(pos.second) + 0.5f, 2))) / (SCREEN_SIZE * 2);
						if (dist < networkInput[7]) networkInput[7] = dist;
					}
				}
			}

			if (display == true)
			{
				std::cout << "netinput" << networkInput << std::endl;
			}

			network->compute(networkInput, output);

			// up, down, left, right
			int directionIndex = 0;
			float outputScore = output[0];

			if (display == true)
			{
				std::cout << 0 << " " << output[0] << std::endl;
			}

			for (int i = 1; i < output.size(); i++)
			{
				if (output[i] > outputScore)
				{
					directionIndex = i;
					outputScore = output[i];
				}

				if (display == true)
				{
					std::cout << i << " " << output[i] << std::endl;
				}
			}

			if (outputScore == 0)
			{
				directionIndex = -1;
			}

			switch (directionIndex)
			{
			case 0:
				if (display == true) std::cout << "UP\n";
				direction = 8;
				countChangeDir++;
				break;
			case 1:
				if (display == true) std::cout << "DOWN\n";
				direction = 2;
				countChangeDir++;
				break;
			case 2:
				if (display == true) std::cout << "LEFT\n";
				direction = 4;
				countChangeDir++;
				break;
			case 3:
				if (display == true) std::cout << "RIGHT\n";
				direction = 6;
				countChangeDir++;
				break;
			}

			input = 0;

			switch (direction)
			{
			case 8:
				snake.push_back(std::pair<int, int>(snake.back().first - 1, snake.back().second));
				break;
			case 2:
				snake.push_back(std::pair<int, int>(snake.back().first + 1, snake.back().second));
				break;
			case 4:
				snake.push_back(std::pair<int, int>(snake.back().first, snake.back().second - 1));
				break;
			case 6:
				snake.push_back(std::pair<int, int>(snake.back().first, snake.back().second + 1));
				break;
			}

			if (screen[snake.back().first][snake.back().second] == 5)
			{
				do
				{
					fruits.first = rand() % (SCREEN_SIZE);
					fruits.second = rand() % (SCREEN_SIZE);

					if (screen[fruits.first][fruits.second] != -1 && abs(snake.back().first - fruits.first) > 1 && abs(snake.back().second - fruits.second))
					{
						screen[fruits.first][fruits.second] = 5;
						mange = 1;
					}

				} while (mange == 0);

				score++;
			}

			if (snake.back().first >= SCREEN_SIZE || snake.back().first < 0 || snake.back().first >= SCREEN_SIZE || snake.back().first < 0
				|| screen[snake.back().first][snake.back().second] == -1)
			{
				vie = 0;
			}
			else
			{
				screen[snake.back().first][snake.back().first] = -1;

				if (mange == 0)
				{
					screen[snake[0].first][snake[0].second] = 0;
					snake.pop_front();
				}
				else
				{
					mange = 0;
				}

				if (display == true)
				{
					std::cout << "Score " << score << std::endl;

					for (int i = 0; i < SCREEN_SIZE; i++)
					{
						for (cpt = 0; cpt < SCREEN_SIZE; cpt++)
						{
							switch (screen[i][cpt])
							{
							case 0:
								std::cout << "_";
								break;
							case -1:
								std::cout << "o";
								break;
							case 5:
								std::cout << "x";
								break;
							}
						}
						std::cout << std::endl;
					}
				}

				if (sqrt(pow(SCREEN_SIZE, 2) * 2) - sqrt(pow(snake.back().first - fruits.first - 0.5, 2) + pow(snake.back().second - fruits.second, 2) <
					sqrt(pow(SCREEN_SIZE, 2) * 2) - sqrt(pow(snake[snake.size() - 2].first - fruits.second, 2) + pow(snake[snake.size() - 2].second - fruits.second, 2))))
				{
					scoreArray[test] += 0.25;
				}
				else {
					scoreArray[test] -= 0.25;
				}
			}

		} while (vie >= 1 && score < 10 && timer < 200 && timer < ((SCREEN_SIZE + 5) * (score + 1)));

		scoreArray[test] += score * SCREEN_SIZE * 4 + 1;

		if (countChangeDir == 0)
		{
			scoreArray[test] = 0;
		}

		if (score == 10)
		{
			wins++;
		}
	}

	if (wins == 20)
	{
		validation = true;
	}

	float finalScore = 0;

	for (int i = 0; i < scoreArray.size(); i++)
	{
		finalScore += scoreArray[i];
	}

	finalScore /= scoreArray.size();

	return finalScore;
}

void snakeEvaluate(int startIndex, int currentWorkload, std::vector<float>& fitness, Neat neat, bool& validated, std::atomic<bool>* ticket = nullptr)
{
	for (int i = startIndex; i < (startIndex + currentWorkload); i++)
	{
		bool tmp = false;

		auto network = neat.getNeuralNetwork(i);

		fitness[i] = snakeTest(network, false, tmp);

		if (tmp == true)
		{
			validated = true;
		}

	}

	if (ticket != nullptr)
	{
		(*ticket) = true;
		ticket->notify_one();
	}
}

bool neatTest(unsigned int popSize, Neat neat)
{
	std::vector<float> fitness;
	fitness.resize(popSize);

	bool validation = false;

	for (int i = 0; i < 100 && validation == false; i++)
	{
		std::cout << "gen " << i << std::endl;

		for (int j = 0; j < popSize; j++)
		{
			fitness[j] = 0;
		}

		int threads = 1;
		ThreadPool* pool = ThreadPool::getInstance();
		unsigned int cpus = std::thread::hardware_concurrency();

		float totalWorkload = popSize;
		float workload = totalWorkload / cpus;
		float restWorkload = 0;
		int currentWorkload = totalWorkload;
		int startIndex = 0;
		int count = 0;

		std::deque<std::atomic<bool>> tickets;

#ifdef MULTITHREAD

		while (workload < 1)
		{
			cpus--;
			workload = totalWorkload / cpus;
		}

		while (cpus > threads)
		{
			currentWorkload = floor(workload);
			float workloadFrac = fmod(workload, 1.f);
			restWorkload = workloadFrac;

			tickets.emplace_back(false);
			pool->queueJob(snakeEvaluate, startIndex, currentWorkload + floor(restWorkload), std::ref(fitness), std::ref(neat), std::ref(validation), &tickets.back());
			++threads;

			count += currentWorkload + floor(restWorkload);
			startIndex += currentWorkload + floor(restWorkload);

			restWorkload -= floor(restWorkload);
			restWorkload += workloadFrac;
		}
#endif //MULTITHREAD

		currentWorkload = totalWorkload - count;

		snakeEvaluate(startIndex, currentWorkload, fitness, neat, validation);

		for (std::deque<std::atomic<bool>>::iterator itTicket = tickets.begin(); itTicket != tickets.end(); ++itTicket)
		{
			itTicket->wait(false);
		}

		neat.setScore(fitness);

		neat.evolve();
	}

	std::cout << "done" << std::endl;

	return false;
}

void initNeatParam(NeatParameters* neatParam)
{
	neatParam->pbMutateLink = 0.05;
	neatParam->pbMutateNode = 0.03;
	neatParam->pbWeight = 0.9;
	neatParam->pbToggleLink = 0.01;
	neatParam->weightMuteStrength = 0.5;
	neatParam->pbMutateActivation = 0.7;

	neatParam->disjointCoeff = 1.0;
	neatParam->excessCoeff = 1.0;
	neatParam->mutDiffCoeff = 0.4;
	neatParam->activationDiffCoeff = 1.0;
	neatParam->weightCoeff = 0.0f;

	neatParam->killRate = 0.2f;

	neatParam->champFileSave = "champ";
	neatParam->avgFileSave = "avg";
	neatParam->saveChampHistory = true;
	neatParam->saveAvgHistory = true;

	neatParam->pbMateMultipoint = 0.6;
	neatParam->pbMateSinglepoint = 0.0;
	neatParam->interspeciesMateRate = 0.001;
	neatParam->dropOffAge = 15;
	neatParam->ageSignificance = 1.0;
	neatParam->pbMutateOnly = 0.25;
	neatParam->pbMateOnly = 0.2;

	neatParam->speciationDistance = 5.5;

	neatParam->speciationDistanceMod = 0.3;
	neatParam->minExpectedSpecies = 15;
	neatParam->maxExpectedSpecies = 40;
	neatParam->adaptSpeciation = true;

	neatParam->keepChamp = true;
	neatParam->elistism = true;
	neatParam->rouletteMultiplier = 2.0f;

	neatParam->activationFunctions.push_back(new ThresholdActivation());
	neatParam->activationFunctions.push_back(new AbsActivation());
	neatParam->activationFunctions.push_back(new SinActivation());
	neatParam->activationFunctions.push_back(new HyperbolSecantActivation());
	neatParam->activationFunctions.push_back(new TanhActivation());
	neatParam->activationFunctions.push_back(new LinearActivation());
}

int main()
{
	auto seed = time(NULL);
	srand(seed);

	ThreadPool* pool = ThreadPool::getInstance();
	pool->start();

	NeatParameters neatParam;
	initNeatParam(&neatParam);

	unsigned int popSize = 300;

	Neat algo(popSize, 8, 4, neatParam, Neat::INIT::FULL);
	neatTest(popSize, algo);

	algo.saveHistory();

	std::vector<float> output;

	NeuralNetwork network;
	algo.genomeToNetwork(*algo.getGoat(), network);

	bool validated;

	snakeTest(&network, true, validated);

	algo.getGoat()->saveCurrentGenome("goatSnake.txt");

	for (int i = 0; i < neatParam.activationFunctions.size(); i++)
	{
		delete neatParam.activationFunctions[i];
	}

	return 0;
}