#pragma once
#include <iostream>
#include "Hyperneat.h"

int launchHypeneatTest();

bool hypeneatTest(int popSize, int test, int n, const std::vector<std::vector<float>>& grid, const std::vector<std::vector<float>>& centers, Hyperneat& hyper);

void evaluate(int test, int startIndex, int n, int workload, const std::vector<std::vector<float>>& grid, const std::vector<std::vector<float>>& centers, Hyperneat& hyper, std::vector<float>& fitness);

void writeSquares(std::vector<float>& grid, std::vector<float>& center, int n, int i);