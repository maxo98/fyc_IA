// Fill out your copyright notice in the Description page of Project Settings.


#include "CarMovementComponent.h"

void UCarMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // Make sure that everything is still valid, and that we are allowed to move.
    if (!PawnOwner || !UpdatedComponent || ShouldSkipUpdate(DeltaTime) || dead == true)
    {
        return;
    }

    // Get (and then clear) the movement vector that we set in ACollidingPawn::Tick
    FVector rotationMovement = ConsumeInputVector();

    //GEngine->AddOnScreenDebugMessage(-1, 60.f, FColor::Red, rotationMovement.ToString());

    FVector DesiredMovementThisFrame = FVector(0, rotationMovement.Y, 0).GetClampedToMaxSize(1.0f) * DeltaTime * 600.0f;
    DesiredMovementThisFrame = DesiredMovementThisFrame.Y * GetOwner()->GetActorForwardVector();
    float rotationAngle = rotationMovement.X * DeltaTime * PI * 1.5;
    rotationMovement = FVector(0, 0, rotationMovement.X).GetClampedToMaxSize(1.0f);

    if ((!DesiredMovementThisFrame.IsNearlyZero() || rotationAngle != 0) && dead == false)
    {
        FHitResult hit;
        SafeMoveUpdatedComponent(DesiredMovementThisFrame, GetOwner()->GetActorQuat() * FQuat(GetOwner()->GetActorUpVector(), rotationAngle), true, hit);

        if (hit.GetActor() != nullptr)
        {
            dead = true;
        }

        // If we bumped into something, try to slide along it
        if (hit.IsValidBlockingHit())
        {
            SlideAlongSurface(DesiredMovementThisFrame, 1.f - hit.Time, hit.Normal, hit);
        }
    }
};