// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "NeatAlgoGen.h"
#include "NeuralNetworkDisplayHUD.generated.h"//Should always be the last one

/**
 * 
 */
UCLASS()
class NEAT_API ANeuralNetworkDisplayHUD : public AHUD
{
	GENERATED_BODY()

	virtual void drawHUD();

	void drawNetwork(NeuralNetwork* network, float xOffset, float yOffset);

	//Searches for a node in the previous layers and returns its position, first argument should be set to the position of the current layer
	//It also searches in the input layer, if it didn't find it
	bool findNodePos(int& x, int& y, const std::list <std::list<Node>>::iterator& itLayer, const std::pair<Node*, float>& connection, NeuralNetwork* network);
	//Searches for a node in a layer and returns its position
	bool findNodePosInLayer(int& y, std::list<Node>::iterator nodes, std::list<Node>::iterator nodesEnd, const std::pair<Node*, float>& connection);

public:
	NeatAlgoGen neat;
};


