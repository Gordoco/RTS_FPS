// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LobbyManager.h"
#include "TimerManager.h"
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

	FString PlayerName;

	FMatchRequest() {
		Team = -1;
		MatchGameplayType = -1;
		PlayerName = "";
	}

	FMatchRequest(int inTeam, int inMatchGameplayType, FString inPlayerName) {
		Team = inTeam;
		MatchGameplayType = inMatchGameplayType;
		PlayerName = inPlayerName;
	}
};

USTRUCT(BlueprintType) struct FPCData
{
	GENERATED_BODY()
public:
	APlayerController* Owner;

	int MatchData = 0;
	int Team = 0;

	FPCData() {}

	FPCData(APlayerController* inO, int inM, int inT) {
		Owner = inO;
		MatchData = inM;
		Team = inT;
	}
		
};

USTRUCT(BlueprintType) struct FRoles
{
	GENERATED_BODY()

		bool bTeam1Commander = false;

	bool bTeam2Commander = false;

	FRoles() {}
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

	FRoles Roles = FRoles();

	virtual void PostSeamlessTravel() override;

	virtual void SwapPlayerControllers(APlayerController* OldPC, APlayerController* NewPC) override;

	FTimerHandle CheckForPlayersLoadedHandle;

	FTimerHandle TimeoutHandle;

	void CheckForPlayersLoaded();

	bool bStarted = false;

	void StartGame();

	void TimeoutGame();

	TArray<FPCData> Data;

	TArray<AController*> OldControllerList;

	TArray<FMatchRequest> CurrRequests;

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Multiplayer")
		float TimeoutTime = 15;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Multiplayer")
		float CheckInterval = 1.f;

public:
	//Public Server-Side Function to Launch a Match (ONLY USABLE POST LOBBY CREATION)
	UFUNCTION(BlueprintCallable, Category = "Match")
		bool CreateMatch(FString MapName, int NumPlayers, bool bListen);

	UFUNCTION(BlueprintPure, Category = "Match")
		bool GetStarted() { return bStarted; }

	bool RequestMatchPosition(FMatchRequest inRequest, APlayerController* RequestingPC);

	TArray<FMatchRequest> GetMatchRequests() { return CurrRequests; }
};
