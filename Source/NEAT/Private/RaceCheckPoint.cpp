// Fill out your copyright notice in the Description page of Project Settings.


#include "RaceCheckPoint.h"

ARaceCheckPoint::ARaceCheckPoint()
{
    OnActorBeginOverlap.AddDynamic(this, &ARaceCheckPoint::OnOverlapBegin);

}

void ARaceCheckPoint::OnOverlapBegin(AActor* myOverlappedActor, AActor* otherActor)
{
    GEngine->AddOnScreenDebugMessage(-1, 60.f, FColor::Red, FString::Printf(TEXT("aaa")));

    GEngine->AddOnScreenDebugMessage(-1, 60.f, FColor::Red, FString::Printf(TEXT("%i"), ((ARaceCheckPoint*)myOverlappedActor)->getNumber()));

}