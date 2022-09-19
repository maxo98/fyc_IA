// Fill out your copyright notice in the Description page of Project Settings.


#include "CarPawn.h"


#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "RaceCheckPoint.h"
//#include "ConstructorHelpers.h"

// Sets default values
ACarPawn::ACarPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // Our root component will be a sphere that reacts to physics
    UBoxComponent* BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("RootComponent"));
    RootComponent = BoxComponent;
    BoxComponent->InitBoxExtent(FVector(1));
    BoxComponent->SetCollisionProfileName(TEXT("Pawn"));

    // Create and position a mesh component so we can see where our sphere is
    UStaticMeshComponent* BoxVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualRepresentation"));
    BoxVisual->SetupAttachment(RootComponent);
    static ConstructorHelpers::FObjectFinder<UStaticMesh> BoxVisualAsset(TEXT("/Game/Cube"));
    if (BoxVisualAsset.Succeeded())
    {
        BoxVisual->SetStaticMesh(BoxVisualAsset.Object);
        BoxVisual->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
        BoxVisual->SetWorldScale3D(FVector(1.0f));
        BoxVisual->SetCollisionProfileName(TEXT("Car"));
    }
    else {
        GEngine->AddOnScreenDebugMessage(-1, 60.f, FColor::Red, "Can't create cube.");
    }

    // Create an instance of our movement component, and tell it to update our root component.
    ourMovementComponent = CreateDefaultSubobject<UCarMovementComponent>(TEXT("CustomMovementComponent"));
    ourMovementComponent->UpdatedComponent = RootComponent;

    //Register Events
    OnActorBeginOverlap.AddDynamic(this, &ACarPawn::OnOverlapBegin);

    UpdateOverlaps(true);
}

// Called when the game starts or when spawned
void ACarPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACarPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACarPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ACarPawn::MoveForward(float AxisValue)
{
    if (ourMovementComponent != NULL && (ourMovementComponent->UpdatedComponent == RootComponent))
    {
        ourMovementComponent->AddInputVector(FVector(0, AxisValue, 0));
    }
}

void ACarPawn::MoveRight(float AxisValue)
{
    if (ourMovementComponent != NULL && (ourMovementComponent->UpdatedComponent == RootComponent))
    {
        ourMovementComponent->AddInputVector(FVector(AxisValue, 0, 0));
    }
}

void ACarPawn::OnOverlapBegin(AActor* myOverlappedActor, AActor* otherActor)
{
    GEngine->AddOnScreenDebugMessage(-1, 60.f, FColor::Red, FString::Printf(TEXT("aaa")));

    GEngine->AddOnScreenDebugMessage(-1, 60.f, FColor::Red, FString::Printf(TEXT("%i"), ((ARaceCheckPoint*)myOverlappedActor)->getNumber()));
     
}