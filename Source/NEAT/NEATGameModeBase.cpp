// Copyright Epic Games, Inc. All Rights Reserved.


#include "NEATGameModeBase.h"
#include <stdlib.h>     
#include <time.h>       

ANEATGameModeBase::ANEATGameModeBase()
{
	HUDClass = ANeuralNetworkDisplayHUD::StaticClass();

	srand(time(NULL));
}