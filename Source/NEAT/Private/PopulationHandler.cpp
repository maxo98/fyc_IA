// Fill out your copyright notice in the Description page of Project Settings.


#include "PopulationHandler.h"

// Sets default values
APopulationHandler::APopulationHandler()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void APopulationHandler::BeginPlay()
{
	Super::BeginPlay();

	//Mesh is created after spawn, so we need to rotate after spawn
	car = (GetWorld()->SpawnActor<ACarPawn>(spawnLoc->GetActorLocation(), FRotator()));
	car->SetActorScale3D(FVector3d(200, 100, 100));
	car->SetActorRotation(spawnLoc->GetActorRotation());
	//(GetWorld()->SpawnActor(ACarPawn::StaticClass(), NAME_None, NULL, NULL, NULL, false, false, NULL, NULL, false, NULL, true));

}

// Called every frame
void APopulationHandler::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	car->MoveForward(5);
	//car->MoveRight(5);
}

