// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <vector>

typedef float (*ActivationFunction) (float value);

inline float sigmoidActivation(float x) { return 1 / (1 + exp(-x)); };

inline float reluActivation(float x) { return (x > 0 ? x : 0); };

inline float linearActivation(float x) { return x; };

inline float thresholdActivation(float x) { return (x >= 0 ? 1 : 0); };

inline float sinActivation(float x) { return sin(x); };

//Hyperbolic tangent
inline float hyperTanActivation(float x) { return (1 - exp(-x * 2)) / (1 + exp(-x*2)); };

inline float tanhActivation(float x) { return tanh(x); };

inline float sincActivation(float x) { return sin(x)/x; };

inline float gaussianActivation(float x) { return exp(-(x*x)); };

inline float swishActivation(float x) { return x / (1 + exp(-x)); };

inline float hyperbolSecantActivation(float x) { return 2 / (exp(x) + exp(-x)); };

inline float cedricSpikeActivation(float x) { return (1 / (abs(x) + 0.2))/5; };

inline float invPyramidActivation(float x) { return (abs(x) < 1 ? 1/(-abs(x)) + 1 : 0); };

inline float jigsawActivation(float x) { return x - floor(x); };

