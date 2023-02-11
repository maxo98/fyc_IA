#pragma once
#include "ES_Hyperneat.h"

int launchESHypeneatTest();

//#define NEAT

#ifndef NEAT
bool esHypeneatTest(int popSize, Hyperneat& esHyper);

void snakeEvaluate(int startIndex, int currentWorkload, std::vector<float>& fitness, Hyperneat& esHyper, bool& validated, std::atomic<bool>* ticket = nullptr);
#else
bool esHypeneatTest(int popSize, Neat& esHyper);

void snakeEvaluate(int startIndex, int currentWorkload, std::vector<float>& fitness, Neat& esHyper, bool& validated, std::atomic<bool>* ticket = nullptr);
#endif

int snakeTest(NeuralNetwork* network, bool display, bool& validated);

