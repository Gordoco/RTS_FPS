// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BaseUnit.h"
#include "UnitTracker.generated.h"

/**
* #### FTeam Data
* Workaround for 2D arrays in Unreal Engine 5
* Stores an array of BaseUnit pointers which should be on the same team
*/
USTRUCT(BlueprintType) struct FTeamData
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Unit")
		TArray<ABaseUnit*> Units;
};


/**
* #### Unit Tracker
* UObject derived static class responsible for keeping an accurate server-side list of BaseUnit instances
* for both internal and external use
*/
UCLASS()
class RTS_FPS_API UUnitTracker : public UObject
{
	GENERATED_BODY()

private:
	/**
	* #### Teams
	* 2D array storing all units from all teams
	*/
	inline static TArray<FTeamData> Teams;

public:
	/**
	* #### bActive
	* Tracker for when the game has loaded and the static UnitTracker should begin function
	*/
	inline static bool bActive = true;

	/**
	* #### Empty()
	* Clears all stored units and refreshes UnitTracker
	*/
	static void Empty() { Teams.Empty(); }

	/**
	* #### RegisterUnit()
	* Adds a unit to corresponding FTeamData for use in tracking
	*/
	static void RegisterUnit(ABaseUnit* Unit, int Team);

	/**
	* #### DeregisterUnit()
	* Removes a unit from its corresponding FTeamData
	* Usually used on unit death
	*/
	static void DeregisterUnit(ABaseUnit* Unit, int Team);

	/**
	* #### GetNumTeams()
	* Accessor method to return number of initialized teams
	*/
	static int GetNumTeams() { return Teams.Num(); }

	/**
	* #### CheckForValidity()
	* Checks that the index passed belongs to a valid and initialized team with at least 1 member
	*/
	static bool CheckForValidity(int index);

	/**
	* #### GetAllTeamData()
	* Accessor method to retrieve the entire set of currently tracked units
	*/
	static TArray<FTeamData> GetAllTeamData() { return UUnitTracker::Teams; }

	/**
	* #### GetTeamData()
	* Accessor method to retrieve the set of currently tracked units belonging to a specified team
	*/
	static FTeamData GetTeamData(int Team) { return UUnitTracker::Teams[Team]; }

	/**
	* #### GetUnitsInRange()
	* Accessor method to retrieve all units within a specified distance of a position in world space
	* TO DO: Make sorted from closest to furthest, currently will present the closest first with no ordering after that
	*/
	static TArray<ABaseUnit*> GetUnitsInRange(int Team, float inRange, FVector Location);

	/**
	* #### GetClosestUnit()
	* Accessor method to retrieve the single closest unit to a location in world space
	*/
	static ABaseUnit* GetClosestUnit(int Team, FVector Location);
};
