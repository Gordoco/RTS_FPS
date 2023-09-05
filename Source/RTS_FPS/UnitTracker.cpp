// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitTracker.h"
#include <limits.h>
#include "Kismet/GameplayStatics.h"
#include "Runtime/Core/Public/Misc/AssertionMacros.h"

bool UUnitTracker::CheckForValidity(int index) {
	bool b = true;
	for (FTeamData Team : Teams) {
		ABaseUnit* U = nullptr;
		if (index >= 0 && index < Team.Units.Num()) {
			U = Team.Units[index];
		}
		if (!(U != nullptr
			&& U->IsValidLowLevel()
			&& !U->IsDead())) {
			b = false;
		}
	}
	return b;
}

TArray<ABaseUnit*> UUnitTracker::GetUnitsInRange(int Team, float inRange, FVector Location) {
	TArray<ABaseUnit*> returnArray;
	TArray<int> invalidsArray;
	if (bActive) {
		check(Team < Teams.Num() && Team > -1);
		if (Team < Teams.Num() && Team > -1) {
			float lowest = (float)INT_MAX;
			for (int i = 0; i < UUnitTracker::Teams.Num(); i++) {
				if (i != Team) {
					for (ABaseUnit* Unit : Teams[i].Units) {
						if (Unit != nullptr && Unit->IsValidLowLevel()) {
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
				}
			}
		}
	}
	return returnArray;
}

ABaseUnit* UUnitTracker::GetClosestUnit(int Team, FVector Location) {
	ABaseUnit* returnPointer = nullptr;
	if (bActive) {
		check(Team < UUnitTracker::Teams.Num() && Team > -1);
		if (Team < UUnitTracker::Teams.Num() && Team > -1) {
			float lowest = (float)INT_MAX;
			for (int i = 0; i < UUnitTracker::Teams.Num(); i++) {
				if (i != Team) {
					for (ABaseUnit* Unit : UUnitTracker::Teams[i].Units) {
						float dist = FVector::Dist(Unit->GetActorLocation(), Location);
						if (dist < lowest) {
							returnPointer = Unit;
						}
					}
				}
			}
		}
	}
	return returnPointer;
}

void UUnitTracker::RegisterUnit(ABaseUnit* Unit, int Team) {
	if (bActive) {
		//GEngine->AddOnScreenDebugMessage(-1, 25.f, FColor::Blue, "REGISTERED UNIT ON TEAM: " + FString::SanitizeFloat(Team));
		//Hacky Initialization Attempt (FIX LATER)
		if (UUnitTracker::Teams.Num() == 0) {
			UUnitTracker::Teams.Add(FTeamData());
		}
		check(Team <= UUnitTracker::Teams.Num() && Unit != nullptr);
		if (Team <= UUnitTracker::Teams.Num() && Unit != nullptr) {
			if (Team == UUnitTracker::Teams.Num()) {
				UUnitTracker::Teams.Add(FTeamData());
			}
			UUnitTracker::Teams[Team].Units.Add(Unit);
		}
	}
}

void UUnitTracker::DeregisterUnit(ABaseUnit* Unit, int Team) {
	if (bActive) {
		check(Team < UUnitTracker::Teams.Num() && Unit != nullptr);
		if (Team < UUnitTracker::Teams.Num() && Unit != nullptr) {
			int index = UUnitTracker::Teams[Team].Units.Find(Unit);
			check(index != -1);
			if (index != -1) {
				UUnitTracker::Teams[Team].Units.RemoveAt(index);
			}
		}
	}
}