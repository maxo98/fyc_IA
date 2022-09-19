// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "CarMovementComponent.h"

#include "CarPawn.generated.h"



UCLASS()
class NEAT_API ACarPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACarPawn();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveForward(float AxisValue);

	void MoveRight(float AxisValue);

	// declare overlap begin function
	void OnOverlapBegin(AActor* myOverlappedActor, AActor* otherActor);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UCarMovementComponent* ourMovementComponent;

};
