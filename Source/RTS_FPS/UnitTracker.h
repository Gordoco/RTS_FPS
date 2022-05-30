// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BaseUnit.h"
#include "UnitTracker.generated.h"

USTRUCT(BlueprintType) struct FTeamData
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Unit")
		TArray<ABaseUnit*> Units;
};

/**
 * 
 */
UCLASS()
class RTS_FPS_API UUnitTracker : public UObject
{
	GENERATED_BODY()

private:
	inline static TArray<FTeamData> Teams;

public:
	static void Empty() { Teams.Empty(); }

	static void RegisterUnit(ABaseUnit* Unit, int Team);

	static void DeregisterUnit(ABaseUnit* Unit, int Team);

	static TArray<FTeamData> GetAllTeamData() { return UUnitTracker::Teams; }

	static FTeamData GetTeamData(int Team) { return UUnitTracker::Teams[Team]; }

	//Should be sorted from closest to furthest, Currently loosely sorted with closest first but unclear order after that
	static TArray<ABaseUnit*> GetUnitsInRange(int Team, float inRange, FVector Location);

	static ABaseUnit* GetClosestUnit(int Team, FVector Location);
};
