// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitTracker.h"
#include <limits.h>
#include "Kismet/GameplayStatics.h"
#include "Runtime/Core/Public/Misc/AssertionMacros.h"

TArray<ABaseUnit*> UUnitTracker::GetUnitsInRange(int Team, float inRange, FVector Location) {
	TArray<ABaseUnit*> returnArray;
	check(Team < Teams.Num() && Team > -1);
	if (Team < Teams.Num() && Team > -1) {
		float lowest = (float)INT_MAX;
		for (ABaseUnit* Unit : Teams[Team].Units) {
			float dist = FVector::Dist(Unit->GetActorLocation(), Location);
			if (dist < lowest) {
				returnArray.Push(Unit);
				lowest = dist;
			}
			else {
				returnArray.Add(Unit);
			}
		}
	}
	return returnArray;
}

ABaseUnit* UUnitTracker::GetClosestUnit(int Team, FVector Location) {
	ABaseUnit* returnPointer = nullptr;
	check(Team < UUnitTracker::Teams.Num() && Team > -1);
	if (Team < UUnitTracker::Teams.Num() && Team > -1) {
		float lowest = (float)INT_MAX;
		for (ABaseUnit* Unit : UUnitTracker::Teams[Team].Units) {
			float dist = FVector::Dist(Unit->GetActorLocation(), Location);
			if (dist < lowest) {
				returnPointer = Unit;
			}
		}
	}
	return returnPointer;
}

void UUnitTracker::RegisterUnit(ABaseUnit* Unit, int Team) {
	check(Team <= UUnitTracker::Teams.Num() && Unit != nullptr);
	if (Team <= UUnitTracker::Teams.Num() && Unit != nullptr) {
		if (Team == UUnitTracker::Teams.Num()) {
			UUnitTracker::Teams.Add(FTeamData());
		}
		UUnitTracker::Teams[Team].Units.Add(Unit);
	}
}