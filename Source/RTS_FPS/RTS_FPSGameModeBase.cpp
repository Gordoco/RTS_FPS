// Copyright Epic Games, Inc. All Rights Reserved.


#include "RTS_FPSGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "RTS_FPSGameInstance.h"
#include "RTSPlayerController.h"
#include "Runtime/Core/Public/Misc/AssertionMacros.h"

bool ARTS_FPSGameModeBase::CreateMatch(FString MapName, int NumPlayers) {
	check(MapName != "" && NumPlayers > 0);
	if (MapName != "" && NumPlayers > 0) {
		check(GetWorld() != nullptr);
		if (GetWorld() != nullptr) {
			return GetWorld()->ServerTravel(MapName + "?listen", true, false);
		}
	}
	return false;
}

void ARTS_FPSGameModeBase::PostSeamlessTravel() {
	GetWorld()->GetTimerManager().SetTimer(CheckForPlayersLoadedHandle, this, &ARTS_FPSGameModeBase::CheckForPlayersLoaded, CheckInterval, true);
	GetWorld()->GetTimerManager().SetTimer(TimeoutHandle, this, &ARTS_FPSGameModeBase::TimeoutGame, TimeoutTime, false);
}

void ARTS_FPSGameModeBase::CheckForPlayersLoaded() {
	TArray<AActor*> PlayerControllerActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerController::StaticClass(), PlayerControllerActors);
	bool bStart = true;
	for (AActor* ActorPC : PlayerControllerActors) {
		ARTSPlayerController* PC = Cast<ARTSPlayerController>(ActorPC);
		if (PC != nullptr) {
			bStart = PC->HasClientLoadedCurrentWorld();
		}
	}
	GetWorld()->GetTimerManager().ClearTimer(CheckForPlayersLoadedHandle);
	GetWorld()->GetTimerManager().ClearTimer(TimeoutHandle);
	StartGame();
}

void ARTS_FPSGameModeBase::StartGame() {
	bStarted = true;
}

void ARTS_FPSGameModeBase::TimeoutGame() {
	if (!bStarted) {
		URTS_FPSGameInstance* GI = Cast<URTS_FPSGameInstance>(GetWorld()->GetGameInstance());
		if (GI != nullptr) {
			GetWorld()->GetTimerManager().ClearTimer(CheckForPlayersLoadedHandle);
			GI->DestroySessionAndLeaveGame();
		}
	}
}

bool ARTS_FPSGameModeBase::RequestMatchPosition(FMatchRequest inRequest, APlayerController* RequestingPC) {
	if (inRequest.MatchGameplayType == 1) {
		return true;
	}
	switch (inRequest.Team) {
		case 0:
			if (inRequest.MatchGameplayType == 0 && !bTeam1Commander) {
				bTeam1Commander = true;
				return true;
			}
			break;
		case 1:
			if (inRequest.MatchGameplayType == 0 && !bTeam2Commander) {
				bTeam1Commander = true;
				return true;
			}
			break;
	}
	return false;
}