// Copyright Epic Games, Inc. All Rights Reserved.


#include "RTS_FPSGameModeBase.h"
#include "Runtime/Core/Public/Misc/AssertionMacros.h"

bool ARTS_FPSGameModeBase::CreateMatch(FString MapName, int NumPlayers) {
	check(MapName != "" && NumPlayers > 1 && NumPlayers % 2 == 0);
	if (MapName != "" && NumPlayers > 1 && NumPlayers % 2 == 0) {
		check(GetWorld() != nullptr);
		if (GetWorld() != nullptr) {
			GetWorld()->ServerTravel(MapName);
			return true;
		}
	}
	return false;
}