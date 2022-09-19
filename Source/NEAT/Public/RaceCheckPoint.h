// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "RaceCheckPoint.generated.h"

/**
 * 
 */
UCLASS()
class NEAT_API ARaceCheckPoint : public ATriggerBox
{
	GENERATED_BODY()

public:

	// constructor sets default values for this actor's properties
	ARaceCheckPoint();

	inline unsigned int getNumber() { return number; };

	void OnOverlapBegin(AActor* myOverlappedActor, AActor* otherActor);

private:
	UPROPERTY(EditAnywhere);
	unsigned int number;
};
