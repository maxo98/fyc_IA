// Fill out your copyright notice in the Description page of Project Settings.


#include "XorActor.h"
#include <cstdint>

// Sets default values
AXorActor::AXorActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	NeatParameters neatparam;

	neatparam.activationFunctions.push_back(new thresholdActivation());

	neatparam.pbMutateLink = 0.01;
	neatparam.pbMutateNode = 0.1;
	neatparam.pbWeightShift = 0.02;
	neatparam.pbWeightRandom = 0.02;
	neatparam.pbToggleLink = 0;
	neatparam.weightShiftStrength = 2.5;
	neatparam.weightRandomStrength = 2.5;

	neatparam.C1 = 2.0;
	neatparam.C2 = 2.0;
	neatparam.C3 = 1.0;
	neatparam.speciationDistance = 6.0;
	neatparam.survivors = 0.8;

	neatparam.bestHigh = true;

	neatparam.fileSave = "C:/save";
	neatparam.saveHistory = true;

	neat = NeatAlgoGen(100, 8, 4, neatparam);

	std::vector<uint8_t> inputs;
	inputs.push_back(0b00000000);
	inputs.push_back(0b00000001);
	inputs.push_back(0b00000010);
	inputs.push_back(0b00000011);
	inputs.push_back(0b00000100);
	inputs.push_back(0b00000101);
	inputs.push_back(0b00000111);
	inputs.push_back(0b00001000);
	inputs.push_back(0b00001001);
	inputs.push_back(0b00001011);
	inputs.push_back(0b00001111);

	std::vector<float> correct(100, 0), mistake(100, 0);

	/*for (int i3 = 0; i3 < 100; i3++)
	{
		for (int cpt = 0; cpt < inputs.size(); cpt++)
		{
			for (int cpt2 = cpt; cpt2 < inputs.size(); cpt++)
			{
				std::vector<float> networkInputs;
				std::vector<float> networkOutputs;
				networkInputs.resize(8);

				uint8_t result = inputs[cpt] ^ inputs[cpt2];

				for (int i2 = 0; i2 < 4; i2++)
				{
					//networkInputs[i2] = inputs[cpt] >> i2;
				}

				for (int i2 = 0; i2 < 4; i2++)
				{
					//networkInputs[i2 + 4] = inputs[cpt2] >> i2;
				}

				for (int i = 0; i < 100; i++)
				{
					NeuralNetwork* network = neat.getNeuralNetwork(i);

					//network->compute(networkInputs, networkOutputs);

					/*for (int i2 = 0; i2 < 8; i2++)
					{
						if (networkOutputs[i2] == ((result >> i2) & 1))
						{
							correct[i]++;
						}
						else {
							mistake[i]++;
						}
					}*
				}
			}
		}

		neat.setScore(correct);

		bool found = false;

		for (int i = 0; i < 100; i++)
		{
			if (mistake[i] == 0)
			{
				neat.saveHistory();
				return;
			}
		}
	}

	neat.saveHistory();*/
}

// Called when the game starts or when spawned
void AXorActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AXorActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

