// Copyright Epic Games, Inc. All Rights Reserved.


#include "RTS_FPSGameModeBase.h"
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