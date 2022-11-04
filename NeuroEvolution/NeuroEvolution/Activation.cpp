// Fill out your copyright notice in the Description page of Project Settings.


#include "Activation.h"

std::vector<Activation> Activation::activationRegister = std::vector<Activation>();
bool Activation::init = false;

void Activation::initialize()
{
	Activation::activationRegister.push_back(sigmoidActivation());
	Activation::activationRegister.push_back(reluActivation());
	Activation::activationRegister.push_back(linearActivation());
	Activation::activationRegister.push_back(thresholdActivation());
	Activation::activationRegister.push_back(sinActivation());
	Activation::activationRegister.push_back(hyperTanActivation());
	Activation::activationRegister.push_back(tanhActivation());
	Activation::activationRegister.push_back(sincActivation());
	Activation::activationRegister.push_back(gaussianActivation());
	Activation::activationRegister.push_back(swishActivation());
	Activation::activationRegister.push_back(hyperbolSecantActivation());
	Activation::activationRegister.push_back(cedricSpikeActivation());
	Activation::activationRegister.push_back(invPyramidActivation());
	Activation::activationRegister.push_back(jigsawActivation());
}
