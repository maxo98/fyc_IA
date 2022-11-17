// NeuroEvolution.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//
#define SPECIES_DEBUG

#include "XorTask.h"
#include "TestHyperneat.h"

int main()
{
	auto seed = time(NULL);

	std::cout << "seed " << seed << std::endl;

	srand(seed);

	//return launchXor();
	return launchHypeneatTest();
}

