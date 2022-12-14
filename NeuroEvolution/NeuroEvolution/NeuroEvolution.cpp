// NeuroEvolution.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//
#include "XorTask.h"
#include "TestHyperneat.h"
#include "ES_Snake.h"
#include "BackpropTest.h"

int main()
{
	auto seed = time(NULL);

	std::cout << "seed " << seed << std::endl;

	srand(seed);//Some random parts don't use this seed 

	//return launchXor();
	//return launchHypeneatTest();
	return launchESHypeneatTest();
	//backpropTest();
	return 1;
}

