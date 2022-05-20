// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LobbyManager.h"
#include "RTS_FPSGameModeBase.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType) struct FMatchRequest
{
	GENERATED_BODY()

public:
	int Team;

	int MatchGameplayType;

	FMatchRequest() {
		Team = -1;
		MatchGameplayType = -1;
	}

	FMatchRequest(int inTeam, int inMatchGameplayType) {
		Team = inTeam;
		MatchGameplayType = inMatchGameplayType;
	}
};


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

private:
	int Team1Num = 0;

	int Team2Num = 0;

	bool bTeam1Commander = false;

	bool bTeam2Commander = false;

public:
	//Public Server-Side Function to Launch a Match (ONLY USABLE POST LOBBY CREATION)
	UFUNCTION(BlueprintCallable, Category = "Match")
		bool CreateMatch(FString MapName, int NumPlayers);

	bool RequestMatchPosition(FMatchRequest inRequest, APlayerController* RequestingPC);

};
