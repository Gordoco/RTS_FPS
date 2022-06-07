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
	OldControllerList.Empty();
	for (auto It = GetWorld()->GetControllerIterator(); It; ++It)
	{
		OldControllerList.Add(It->Get());
	}
	Super::PostSeamlessTravel();
	CheckForPlayersLoaded();
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
	bool bStart = true;

	for (AController* Controller : OldControllerList) {
		if (Controller != nullptr) {
			if (Controller->IsValidLowLevel()) {
				APlayerController* PC = Cast<APlayerController>(Controller);
				if (PC == nullptr) {
					bStart = false;
				}
				else if (!PC->HasClientLoadedCurrentWorld()) {
					bStart = false;
				}
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, PC->GetDebugName(PC));
			}
		}
	}

	if (bStart) {
		StartGame();
	}
}

void ARTS_FPSGameModeBase::StartGame() {
	TArray<AActor*> PlayerControllerActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARTSPlayerController::StaticClass(), PlayerControllerActors);
	GetWorld()->GetTimerManager().ClearTimer(CheckForPlayersLoadedHandle);
	GetWorld()->GetTimerManager().ClearTimer(TimeoutHandle);
	bStarted = true;
	for (AActor* ActorPC : PlayerControllerActors) {
		ARTSPlayerController* PC = Cast<ARTSPlayerController>(ActorPC);
		if (PC != nullptr) {
			for (FPCData PCData : Data) {
				if (PCData.Owner == PC) {
					//GEngine->AddOnScreenDebugMessage(-1, 20.f, FColor::Red, GetDebugName(PC) + " should equal " + GetDebugName(PCData.Owner) + " || M: " + FString::SanitizeFloat(PCData.MatchData) + " T: " + FString::SanitizeFloat(PCData.Team));
					PC->InitPC(PCData.MatchData, PCData.Team);
					PC->StartMatch();
				}
			}
		}
	}
	Data.Empty();
}

void ARTS_FPSGameModeBase::TimeoutGame() {
	if (!bStarted) {
		URTS_FPSGameInstance* GI = Cast<URTS_FPSGameInstance>(GetWorld()->GetGameInstance());
		if (GI != nullptr) {
			GetWorld()->GetTimerManager().ClearTimer(CheckForPlayersLoadedHandle);
			GI->DestroySessionAndLeaveGame();
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "ERROR: Connection Timeout. Returning to Main Menu");
		}
	}
}

bool ARTS_FPSGameModeBase::RequestMatchPosition(FMatchRequest inRequest, APlayerController* RequestingPC) {
	int index = -1;
	for (int i = 0; i < CurrRequests.Num(); i++) {
		if (CurrRequests[i].PlayerName == inRequest.PlayerName) {
			index = i;
		}
	}
	if (index != -1) {
		if (CurrRequests[index].MatchGameplayType == 0) {
			if (CurrRequests[index].Team == 0) {
				Roles.bTeam1Commander = false;
			}
			else {
				Roles.bTeam2Commander = false;
			}
		}
		CurrRequests.RemoveAt(index);
	}
	if (inRequest.MatchGameplayType == 1) {
		CurrRequests.Add(inRequest);
		return true;
	}
	switch (inRequest.Team) {
		case 0:
			if (inRequest.MatchGameplayType == 0 && !Roles.bTeam1Commander) {
				Roles.bTeam1Commander = true;
				CurrRequests.Add(inRequest);
				return true;
			}
			break;
		case 1:
			if (inRequest.MatchGameplayType == 0 && !Roles.bTeam2Commander) {
				Roles.bTeam2Commander = true;
				CurrRequests.Add(inRequest);
				return true;
			}
			break;
	}
	return false;
}