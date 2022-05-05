// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LobbyManager.h"
#include "RTS_FPSGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class RTS_FPS_API ARTS_FPSGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
private:
#ifdef UE_BUILD_DEBUG
	/*
	UNIT TESTS********************************************
	*/


#endif

protected:


public:

	//Public Server-Side Function to Launch a Match (ONLY USABLE POST LOBBY CREATION)
	UFUNCTION(BlueprintCallable, Category = "Match")
		bool CreateMatch(FString MapName, int NumPlayers);

};
