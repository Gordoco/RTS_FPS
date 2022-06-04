// Copyright Epic Games, Inc. All Rights Reserved.


#include "RTS_FPSGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "RTS_FPSGameInstance.h"
#include "RTSPlayerController.h"
#include "Runtime/Core/Public/Misc/AssertionMacros.h"

bool ARTS_FPSGameModeBase::CreateMatch(FString MapName, int NumPlayers, bool bListen) {
	check(MapName != "" && NumPlayers > 0);
	if (MapName != "" && NumPlayers > 0) {
		check(GetWorld() != nullptr);
		if (GetWorld() != nullptr) {
			if (bListen) {
				return GetWorld()->ServerTravel(MapName + "?listen", true, false);
			}
			else {
				return GetWorld()->ServerTravel(MapName, true, false);
			}
		}
	}
	return false;
}

void ARTS_FPSGameModeBase::PostSeamlessTravel() {
	Super::PostSeamlessTravel();
	GetWorld()->GetTimerManager().SetTimer(CheckForPlayersLoadedHandle, this, &ARTS_FPSGameModeBase::CheckForPlayersLoaded, CheckInterval, true);
	GetWorld()->GetTimerManager().SetTimer(TimeoutHandle, this, &ARTS_FPSGameModeBase::TimeoutGame, TimeoutTime, false);
}

void ARTS_FPSGameModeBase::SwapPlayerControllers(APlayerController* OldPC, APlayerController* NewPC) 
{
	ARTSPlayerController* CAST_OldPC = Cast<ARTSPlayerController>(OldPC);
	ARTSPlayerController* CAST_NewPC = Cast<ARTSPlayerController>(OldPC);
	if (CAST_OldPC != nullptr && CAST_NewPC != nullptr) {
		Data.Add(FPCData(NewPC, CAST_OldPC->GetMatchGameplayType(), CAST_OldPC->GetTeam()));
	}
	Super::SwapPlayerControllers(OldPC, NewPC); //Must call super at the end as it destroys OldPC
}

void ARTS_FPSGameModeBase::CheckForPlayersLoaded() {
	TArray<AActor*> PlayerControllerActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARTSPlayerController::StaticClass(), PlayerControllerActors);
	bool bStart = true;
	for (AActor* ActorPC : PlayerControllerActors) {
		ARTSPlayerController* PC = Cast<ARTSPlayerController>(ActorPC);
		if (PC != nullptr) {
			for (FPCData PCData : Data) {
				if (PCData.Owner == PC) {
					//GEngine->AddOnScreenDebugMessage(-1, 20.f, FColor::Red, GetDebugName(PC) + " should equal " + GetDebugName(PCData.Owner) + " || M: " + FString::SanitizeFloat(PCData.MatchData) + " T: " + FString::SanitizeFloat(PCData.Team));
					PC->InitPC(PCData.MatchData, PCData.Team);
				}
			}
			bStart = PC->HasClientLoadedCurrentWorld();
		}
	}
	if (bStart) {
		GetWorld()->GetTimerManager().ClearTimer(CheckForPlayersLoadedHandle);
		GetWorld()->GetTimerManager().ClearTimer(TimeoutHandle);
		Data.Empty();
		StartGame();
	}
}

void ARTS_FPSGameModeBase::StartGame() {
	bStarted = true;
	TArray<AActor*> PlayerControllerActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARTSPlayerController::StaticClass(), PlayerControllerActors);
	for (AActor* ActorPC : PlayerControllerActors) {
		ARTSPlayerController* PC = Cast<ARTSPlayerController>(ActorPC);
		if (PC != nullptr) {
			PC->StartMatch();
		}
	}
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
	/*if (inRequest.MatchGameplayType == 1) {
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
	return false;*/
	return true;
}