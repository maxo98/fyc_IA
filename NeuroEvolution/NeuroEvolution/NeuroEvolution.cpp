// NeuroEvolution.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//
#include "XorTask.h"
#include "TestHyperneat.h"
#include "ES_Snake.h"
#include "BackpropTest.h"
#include "HyperBackprop.h"
#include "ThreadPool.h"

int main()
{
	auto seed = time(NULL);

	std::cout << "seed " << seed << std::endl;

	srand(seed);//Some random parts don't use this seed 

	ThreadPool* pool = ThreadPool::getInstance();
	pool->start();

	//return launchXor();
	//return launchHypeneatTest();
	//return launchESHypeneatTest();
	//backpropTest();
	testHyperBackprop();

	return 0;
}

