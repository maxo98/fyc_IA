#pragma once
#include "ES_Hyperneat.h"

int launchESHypeneatTest();

bool esHypeneatTest(int popSize, ES_Hyperneat& esHyper);

int snakeTest(NeuralNetwork* network, bool display);

void snakeEvaluate(int startIndex, int currentWorkload, std::vector<float>& fitness, ES_Hyperneat& esHyper);