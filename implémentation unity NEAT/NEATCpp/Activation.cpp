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

Activation* Activation::initActivation(const std::string& id)
{
	if (strcmp(id.c_str(), "none") == 0)
	{
		return new Activation();
	}

	if (strcmp(id.c_str(), "sigmoid") == 0)
	{
		return new SigmoidActivation();
	}

	if (strcmp(id.c_str(), "relu") == 0)
	{
		return new ReluActivation();
	}

	if (strcmp(id.c_str(), "linear") == 0)
	{
		return new LinearActivation();
	}

	if (strcmp(id.c_str(), "absolute") == 0)
	{
		return new AbsActivation();
	}

	if (strcmp(id.c_str(), "threshold") == 0)
	{
		return new ThresholdActivation();
	}

	if (strcmp(id.c_str(), "sin") == 0)
	{
		return new SinActivation();
	}

	if (strcmp(id.c_str(), "tanh") == 0)
	{
		return new TanhActivation();
	}

	if (strcmp(id.c_str(), "sinc") == 0)
	{
		return new SincActivation();
	}

	if (strcmp(id.c_str(), "posSinc") == 0)
	{
		return new PosSincActivation();
	}

	if (strcmp(id.c_str(), "gaussian") == 0)
	{
		return new GaussianActivation();
	}

	if (strcmp(id.c_str(), "swish") == 0)
	{
		return new SwishActivation();
	}

	if (strcmp(id.c_str(), "hyperbolSecant") == 0)
	{
		return new HyperbolSecantActivation();
	}

	if (strcmp(id.c_str(), "cedricSpike") == 0)
	{
		return new CedricSpikeActivation();
	}

	if (strcmp(id.c_str(), "invPyramid") == 0)
	{
		return new InvPyramidActivation();
	}

	if (strcmp(id.c_str(), "jigsaw") == 0)
	{
		return new JigsawActivation();
	}

	if (strcmp(id.c_str(), "square") == 0)
	{
		return new SquareActivation();
	}

	if (strcmp(id.c_str(), "squareRoot") == 0)
	{
		return new SquareRootActivation();
	}
}
