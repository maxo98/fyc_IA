// Fill out your copyright notice in the Description page of Project Settings.


#include "NeuralNetworkDisplayHUD.h"

void ANeuralNetworkDisplayHUD::DrawHUD()
{
	int ySpace = 30;
	int xSpace = 60;
	int squareSize = 10;

	int x = 0;
	int y = 0;

	for (std::list<Node>::iterator itInput = network.inputNodes.begin(); itInput != network.inputNodes.end(); ++itInput)
	{
		DrawRect(FLinearColor(0, 1, 0, 1), 20 + xSpace * x, 20 + ySpace * y, squareSize, squareSize);
		y++;
	}

	x++;
	y = 0;

	for (std::list <std::list<Node>>::iterator itLayer = network.hiddenNodes.begin(); itLayer != network.hiddenNodes.end(); ++itLayer)
	{
		for (std::list<Node>::iterator itHidden = itLayer->begin(); itHidden != itLayer->end(); ++itHidden)
		{
			DrawRect(FLinearColor(0, 1, 0, 1), 20 + xSpace * x, 20 + ySpace * y, squareSize, squareSize);
			
			//GEngine->AddOnScreenDebugMessage(-1, 60.f, FColor::Red, FString::Printf(TEXT("%i"), itHidden->previousNodes.size()));

			for (std::map<Node*, float>::iterator nodes = itHidden->previousNodes.begin(); nodes != itHidden->previousNodes.end(); ++nodes)
			{
				std::list<Node>::iterator itPrev;
				std::list<Node>::iterator itPrevEnd;
				int y2 = 0;

				if (itLayer == network.hiddenNodes.begin())
				{
					itPrev = network.inputNodes.begin();
					itPrevEnd = network.inputNodes.end();
				}
				else {
					itPrev = std::prev(itLayer)->begin();
					itPrevEnd = std::prev(itLayer)->end();
				}

				for (itPrev; itPrev != itPrevEnd; ++itPrev)
				{

					if (&*itPrev == nodes->first)
					{
						DrawLine(20 + xSpace * (x - 1) + squareSize / 2, 20 + ySpace * y2 + squareSize / 2, 20 + xSpace * x + squareSize / 2, 20 + ySpace * y + squareSize / 2, FLinearColor(0, 1, 0, 1), 2);
						
						float textY = (y < y2 ? y : y2) + abs(y - y2) / 2;
						
						DrawText(FString::Printf(TEXT("%.2f"), nodes->second), FColor::Black, 20 + xSpace * (x - 0.5) + squareSize / 2 - 10, 20 + ySpace * textY + squareSize / 2);
						
						break;
					}

					y2++;
				}
			}

			y++;
		}

		x++;
		y = 0;
	}

	for (std::list<Node>::iterator itOutput = network.outputNodes.begin(); itOutput != network.outputNodes.end(); ++itOutput)
	{
		DrawRect(FLinearColor(0, 1, 0, 1), 20 + xSpace * x, 20 + ySpace * y, 10, 10);
		
		for (std::map<Node*, float>::iterator nodes = itOutput->previousNodes.begin(); nodes != itOutput->previousNodes.end(); ++nodes)
		{
			std::list<Node>::iterator itPrev = network.hiddenNodes.back().begin();
			std::list<Node>::iterator itPrevEnd = network.hiddenNodes.back().end();

			int y2 = 0;

			for (itPrev; itPrev != itPrevEnd; ++itPrev)
			{

				if (&*itPrev == nodes->first)
				{
					DrawLine(20 + xSpace * (x - 1) + squareSize / 2, 20 + ySpace * y2 + squareSize / 2, 20 + xSpace * x + squareSize / 2, 20 + ySpace * y + squareSize / 2, FLinearColor(0, 1, 0, 1), 2);
					
					float textY = (y < y2 ? y : y2) + abs(y - y2)/2;

					DrawText(FString::Printf(TEXT("%.2f"), nodes->second), FColor::Black, 20 + xSpace * (x - 0.5) + squareSize / 2 - 10, 20 + ySpace * textY + squareSize / 2);

					break;
				}

				y2++;
			}

		}

		y++;
	}

	

	/*DrawRect(FLinearColor(0, 1, 0, 1), 20, 20, 10, 10);
	DrawLine(100, 100, 300, 300, FLinearColor(0, 1, 0, 1), 2);
	DrawRect(FLinearColor(0, 1, 0, 1), 300, 300, 50, 50);*/
}

void ANeuralNetworkDisplayHUD::BeginPlay()
{
    Super::BeginPlay();
}