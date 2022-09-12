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

	//Searches for a node in the previous layers and returns its position, first argument should be set to the position of the current layer
	bool findNodePos(int& x, int& y, const std::list <std::list<Node>>::iterator& itLayer, const std::pair<Node*, float>& connection);
	//Searches for a node in a layer and returns its position
	bool findNodePosInLayer(int& y, std::list<Node>::iterator nodes, std::list<Node>::iterator nodesEnd, const std::pair<Node*, float>& connection);

public:
	NeuralNetwork network;
};
