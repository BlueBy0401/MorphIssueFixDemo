// Copyright Epic Games, Inc. All Rights Reserved.


#include "PatcherDemoGameModeBase.h"

APatcherDemoGameModeBase::APatcherDemoGameModeBase()
{
	bUseSeamlessTravel = true;
}

void APatcherDemoGameModeBase::TryServerTravel(const FString& URL)
{
	GetWorld()->ServerTravel(URL, true, true);
}