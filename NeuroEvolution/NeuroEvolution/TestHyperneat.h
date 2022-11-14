#pragma once
#include <iostream>
#include "Hyperneat.h"

int launchHypeneatTest();

bool hypeneatTest(const std::vector<std::vector<float>>& grid, std::vector<float>& output, const std::vector<std::vector<float>>& centers, Hyperneat& hyper);

void writeSquares(std::vector<float>& grid, std::vector<float>& center, int n);