// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "CarMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class NEAT_API UCarMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()

private:
	bool dead = false;
	
public:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
