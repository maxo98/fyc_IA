// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "NeuralNetwork.h"
#include "NeuralNetworkDisplayHUD.generated.h"//Should always be the last one

/**
 * 
 */
UCLASS()
class NEAT_API ANeuralNetworkDisplayHUD : public AHUD
{
	GENERATED_BODY()

	virtual void DrawHUD();

	virtual void BeginPlay();

public:
	NeuralNetwork network;
};
