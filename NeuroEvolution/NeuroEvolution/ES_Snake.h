#pragma once
#include "ES_Hyperneat.h"

int launchESHypeneatTest();

bool esHypeneatTest(int popSize, Hyperneat& esHyper);

int snakeTest(NeuralNetwork* network, bool display, bool& validated);

void snakeEvaluate(int startIndex, int currentWorkload, std::vector<float>& fitness, Hyperneat& esHyper, bool& validated, std::atomic<bool>* ticket = nullptr);