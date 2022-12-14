// Fill out your copyright notice in the Description page of Project Settings.


#include "NeuralNetworkDisplayHUD.h"

void ANeuralNetworkDisplayHUD::drawHUD()
{
	//drawNetwork(&neat.networks[0], 0, 0); 
	//drawNetwork(&neat.networks[1], 0, 200);
	//drawNetwork(&neat.networks[2], 500, 0);
}

void ANeuralNetworkDisplayHUD::drawNetwork(NeuralNetwork* network, float xOffset, float yOffset)
{
	int ySpace = 30;
	int xSpace = 60;
	int squareSize = 10;
	FLinearColor nodeColor = FLinearColor(0.2, 0.2, 1, 1);
	FLinearColor textColor = FLinearColor(1, 1, 1, 1);
	FLinearColor connectionColor = FLinearColor(0, 1, 0, 1);

	int x = 0;
	int y = 0;

	//Draw the input nodes
	for (std::list<Node>::iterator itInput = network->inputNodes.begin(); itInput != network->inputNodes.end(); ++itInput)
	{
		DrawRect(nodeColor, 20 + xSpace * x + xOffset, 20 + ySpace * y + yOffset, squareSize, squareSize);
		y++;
	}

	x++;
	y = 0;

	//Draw the hidden layers and their connections
	for (std::list <std::list<Node>>::iterator itLayer = network->hiddenNodes.begin(); itLayer != network->hiddenNodes.end(); ++itLayer)
	{
		for (std::list<Node>::iterator itHidden = itLayer->begin(); itHidden != itLayer->end(); ++itHidden)
		{
			DrawRect(nodeColor, 20 + xSpace * x + xOffset, 20 + ySpace * y + yOffset, squareSize, squareSize);

			//GEngine->AddOnScreenDebugMessage(-1, 60.f, FColor::Red, FString::Printf(TEXT("%i"), itHidden->previousNodes.size()));

			for (std::map<Node*, float>::iterator nodes = itHidden->previousNodes.begin(); nodes != itHidden->previousNodes.end(); ++nodes)
			{
				int x2 = x;
				int y2 = 0;

				bool found = findNodePos(x2, y2, itLayer, *nodes, network);

				if (found == false)
				{
					//GEngine->AddOnScreenDebugMessage(-1, 60.f, FColor::Red, "Can't find second node to draw connection");
				}

				DrawLine(20 + xSpace * x2 + squareSize / 2 + xOffset, 20 + ySpace * y2 + squareSize / 2 + yOffset, 20 + xSpace * x + squareSize / 2 + xOffset, 20 + ySpace * y + squareSize / 2 + yOffset, connectionColor, 2);

				float textY = (y < y2 ? y : y2) + abs(y - y2) / 2;

				DrawText(FString::Printf(TEXT("%.2f"), nodes->second), textColor, 20 + xSpace * x - (x - x2) / 2.f * 55 + squareSize / 2.f - 10 + xOffset, 20 + ySpace * textY + squareSize / 2 + yOffset);
			}

			y++;
		}

		x++;
		y = 0;
	}

	//Draw the the output nodes and their connections
	for (std::list<Node>::iterator itOutput = network->outputNodes.begin(); itOutput != network->outputNodes.end(); ++itOutput)
	{
		DrawRect(nodeColor, 20 + xSpace * x + xOffset, 20 + ySpace * y + yOffset, 10, 10);

		for (std::map<Node*, float>::iterator nodes = itOutput->previousNodes.begin(); nodes != itOutput->previousNodes.end(); ++nodes)
		{
			int x2 = x;
			int y2 = 0;

			std::list <std::list<Node>>::iterator itLayer = network->hiddenNodes.end();

			bool found = findNodePos(x2, y2, itLayer, *nodes, network);

			if (found == false)
			{
				//GEngine->AddOnScreenDebugMessage(-1, 60.f, FColor::Red, "Can't find second node to draw connection");
			}

			DrawLine(20 + xSpace * x2 + squareSize / 2 + xOffset, 20 + ySpace * y2 + squareSize / 2 + yOffset, 20 + xSpace * x + squareSize / 2 + xOffset, 20 + ySpace * y + squareSize / 2 + yOffset, connectionColor, 2);

			float textY = (y < y2 ? y : y2) + abs(y - y2) / 2;

			DrawText(FString::Printf(TEXT("%.2f"), nodes->second), textColor, 20 + xSpace * x - (x - x2) / 2.f * 55 + squareSize / 2.f - 10 + xOffset, 20 + ySpace * textY + squareSize / 2 + yOffset);

		}

		y++;
	}
}

bool ANeuralNetworkDisplayHUD::findNodePos(int& x, int& y, const std::list <std::list<Node>>::iterator& itLayer, const std::pair<Node*, float>& connection, NeuralNetwork* network)
{
	//Function automatically walks one step backward
	std::list <std::list<Node>>::reverse_iterator itPrevLayer = std::make_reverse_iterator(itLayer);
	bool found = false;

	//Search in the hidden layers
	while (itPrevLayer != network->hiddenNodes.rend() && found == false)
	{
		found = findNodePosInLayer(y, itPrevLayer->begin(), itPrevLayer->end(), connection);

		x--;
		++itPrevLayer;
	}

	//Search in the input layer 
	if (found == false)
	{
		x = 0;
		return findNodePosInLayer(y, network->inputNodes.begin(), network->inputNodes.end(), connection);
	}
	else {
		return true;
	}

	//return false;
}


bool ANeuralNetworkDisplayHUD::findNodePosInLayer(int& y, std::list<Node>::iterator nodes, std::list<Node>::iterator nodesEnd, const std::pair<Node*, float>& connection)
{
	y = 0;

	while (nodes != nodesEnd)
	{
		if (&*nodes == connection.first)
		{
			return true;
		}

		y++;
		++nodes;
	}

	return false;
}