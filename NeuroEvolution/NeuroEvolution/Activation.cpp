// Fill out your copyright notice in the Description page of Project Settings.


#include "Activation.h"

std::vector<Activation> Activation::activationRegister = std::vector<Activation>();
bool Activation::init = false;

void Activation::initialize()
{
	Activation::activationRegister.push_back(SigmoidActivation());
	Activation::activationRegister.push_back(ReluActivation());
	Activation::activationRegister.push_back(LinearActivation());
	Activation::activationRegister.push_back(ThresholdActivation());
	Activation::activationRegister.push_back(SinActivation());
	Activation::activationRegister.push_back(TanhActivation());
	Activation::activationRegister.push_back(SincActivation());
	Activation::activationRegister.push_back(GaussianActivation());
	Activation::activationRegister.push_back(SwishActivation());
	Activation::activationRegister.push_back(HyperbolSecantActivation());
	Activation::activationRegister.push_back(CedricSpikeActivation());
	Activation::activationRegister.push_back(InvPyramidActivation()); 
	Activation::activationRegister.push_back(JigsawActivation()); 
	Activation::activationRegister.push_back(AbsActivation());
	Activation::activationRegister.push_back(SquareActivation());
	Activation::activationRegister.push_back(SquareRootActivation());
}
