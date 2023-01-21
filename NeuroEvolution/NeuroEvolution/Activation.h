// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <iostream>
#include <string>
#include <vector>

class Activation {
public:
	virtual float activate(const float& x) const { return 0; };

	virtual float derivate(const float& x, const float& y) const { return 0; };

	inline virtual std::string getId() const { return "none"; };

	static void initialize();

	inline static bool isInitialized() { return Activation::init; };

	inline static std::vector<Activation> getRegister() { return Activation::activationRegister; };

	static Activation* initActivation(const std::string& id);

	friend std::ostream& operator<<(std::ostream& os, const Activation& activation)
	{
		os << activation.getId();
		return os;
	}

private:
	static std::vector<Activation> activationRegister;

	static bool init;
};

class SigmoidActivation : public Activation {
public:
	virtual float activate(const float& x) const { return 1.f / (1.f + exp(-x)); };

	virtual float derivate(const float& x, const float& y) const { return y * (1 - y); };

	inline virtual std::string getId() const  { return "sigmoid"; };
};

class ReluActivation : public Activation {
public:
	virtual float activate(const float& x) const { return (x > 0.f ? x : 0.f); };

	virtual float derivate(const float& x, const float& y) const { return x > 0 ? 1 : 0; };

	inline virtual std::string getId() const { return "relu"; };
};

class LinearActivation : public Activation {
public:
	virtual float activate(const float& x) const { return x; };

	virtual float derivate(const float& x, const float& y) const { return 1; };

	inline virtual std::string getId() const { return "linear"; };
};

class AbsActivation : public Activation {
public:
	virtual float activate(const float& x) const { return abs(x); };

	virtual float derivate(const float& x, const float& y) const { return x >= 0 ? 1 : -1; };

	inline virtual std::string getId() const { return "absolute"; };
};

class ThresholdActivation : public Activation {
public:
	virtual float activate(const float& x) const { return (x >= 0.f ? 1.f : 0.f); };

	inline virtual std::string getId() const { return "threshold"; };
};

class SinActivation : public Activation {
public:
	virtual float activate(const float& x) const { return sin(x); };

	virtual float derivate(const float& x, const float& y) const { return cos(x); };

	inline virtual std::string getId() const { return "sin"; };
};

class TanhActivation : public Activation {
public:
	virtual float activate(const float& x) const { return tanh(x); };

	virtual float derivate(const float& x, const float& y) const { return 1 - pow(y, 2); };

	inline virtual std::string getId() const { return "tanh"; };
};

class SincActivation : public Activation {
public:
	virtual float activate(const float& x) const { return sin(x) / x; };

	virtual float derivate(const float& x, const float& y) const { return cos(x)/x - sin(x)/(x*x); };

	inline virtual std::string getId() const { return "sinc"; };
};

class PosSincActivation : public Activation {
public:
	virtual float activate(const float& x) const { return sin(x) / x + 0.25; };

	virtual float derivate(const float& x, const float& y) const { return cos(x) / x - sin(x) / (x * x); };

	inline virtual std::string getId() const { return "posSinc"; };
};

class GaussianActivation : public Activation {
public:
	virtual float activate(const float& x) const { return exp(-(x * x)); };

	virtual float derivate(const float& x, const float& y) const { return -2*x*exp(-x*x); };

	inline virtual std::string getId() const { return "gaussian"; };
};

class SwishActivation : public Activation {
public:
	virtual float activate(const float& x) const { return x / (1 + exp(-x)); };

	virtual float derivate(const float& x, const float& y) const { return (exp(x) * (exp(x) + x + 1))/pow((exp(x) + 1), 2); };

	inline virtual std::string getId() const { return "swish"; };
};

class HyperbolSecantActivation : public Activation {
public:
	virtual float activate(const float& x) const { return 2 / (exp(x) + exp(-x)); };

	virtual float derivate(const float& x, const float& y) const { return (exp(x) * (exp(x) + x + 1)) / pow((exp(x) + 1), 2); };

	inline virtual std::string getId() const { return "hyperbolSecant"; };
};

class CedricSpikeActivation : public Activation {
public:
	virtual float activate(const float& x) const { return (1 / (abs(x) + 0.2)) / 5; };

	inline virtual std::string getId() const { return "cedricSpike"; };
};

//Not sure what this is, I think there's an error here
class InvPyramidActivation : public Activation {
public:
	virtual float activate(const float& x) const { return (abs(x) < 1 ? 1 / (-abs(x)) + 1 : 0); };

	virtual float derivate(const float& x, const float& y) const { return -x/(5*abs(x)*pow((abs(x)+1/5), 2)); };

	inline virtual std::string getId() const { return "invPyramid"; };
};

class JigsawActivation : public Activation {
public:
	virtual float activate(const float& x) const { return x - floor(x); };

	inline virtual std::string getId() const { return "jigsaw"; };
};

class SquareActivation : public Activation {
public:
	virtual float activate(const float& x) const { return x * x; };

	virtual float derivate(const float& x, const float& y) const { return 2 * x; };

	inline virtual std::string getId() const { return "square"; };
};

class SquareRootActivation : public Activation {
public:
	virtual float activate(const float& x) const { return sqrt(abs(x)); };

	virtual float derivate(const float& x, const float& y) const { return 1 / (2 * sqrt(abs(x))); };

	inline virtual std::string getId() const { return "squareRoot"; };
};
