// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <vector>

typedef float (*ActivationFunction) (float value);

inline float sigmoidActivation(float x) { return 1 / (1 + exp(-x)); };
