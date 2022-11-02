// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <string>
#include <vector>

class Activation {
public:
	virtual float activate(float x) const { return 0; };

	inline virtual std::string getId() const { return "none"; };

	static void initialize();

	inline static bool isInitialized() { return Activation::init; };

	inline static std::vector<Activation> getRegister() { return Activation::activationRegister; };

private:
	static std::vector<Activation> activationRegister;

	static bool init;
};

class sigmoidActivation : public Activation {
public:
	virtual float activate(float x) const { return 1 / (1 + exp(-x)); };

	inline virtual std::string getId() const  { return "sigmoid"; };
};

class reluActivation : public Activation {
public:
	virtual float activate(float x) const { return (x > 0 ? x : 0); };

	inline virtual std::string getId() const { return "relu"; };
};

class linearActivation : public Activation {
public:
	virtual float activate(float x) const { return x; };

	inline virtual std::string getId() const { return "linear"; };
};

class thresholdActivation : public Activation {
public:
	virtual float activate(float x) const { return (x >= 0 ? 1 : 0); };

	inline virtual std::string getId() const { return "threshold"; };
};

class sinActivation : public Activation {
public:
	virtual float activate(float x) const { return sin(x); };

	inline virtual std::string getId() const { return "sin"; };
};

//Hyperbolic tangent
class hyperTanActivation : public Activation {
public:
	virtual float activate(float x) const { return (1 - exp(-x * 2)) / (1 + exp(-x * 2)); };

	inline virtual std::string getId() const { return "hyperTan"; };
};

class tanhActivation : public Activation {
public:
	virtual float activate(float x) const { return tanh(x); };

	inline virtual std::string getId() const { return "tanh"; };
};

class sincActivation : public Activation {
public:
	virtual float activate(float x) const { return sin(x) / x; };

	inline virtual std::string getId() const { return "sinc"; };
};

class gaussianActivation : public Activation {
public:
	virtual float activate(float x) const { return exp(-(x * x)); };

	inline virtual std::string getId() const { return "gaussian"; };
};

class swishActivation : public Activation {
public:
	virtual float activate(float x) const { return x / (1 + exp(-x)); };

	inline virtual std::string getId() const { return "swish"; };
};

class hyperbolSecantActivation : public Activation {
public:
	virtual float activate(float x) const { return 2 / (exp(x) + exp(-x)); };

	inline virtual std::string getId() const { return "hyperbolSecant"; };
};

class cedricSpikeActivation : public Activation {
public:
	virtual float activate(float x) const { return (1 / (abs(x) + 0.2)) / 5; };

	inline virtual std::string getId() const { return "cedricSpike"; };
};

class invPyramidActivation : public Activation {
public:
	virtual float activate(float x) const { return (abs(x) < 1 ? 1 / (-abs(x)) + 1 : 0); };

	inline virtual std::string getId() const { return "invPyramid"; };
};

class jigsawActivation : public Activation {
public:
	virtual float activate(float x) const { return x - floor(x); };

	inline virtual std::string getId() const { return "jigsaw"; };
};

