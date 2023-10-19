// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionData.h"
#include "BaseUnit.h"
#include "AttackActionData.generated.h"

/**
 * Inherited ActionData class for attack actions and their additional required information
 */
UCLASS()
class RTS_FPS_API UAttackActionData : public UActionData
{
	GENERATED_BODY()

public:
	/**
	* #### UAttackActionData()
	* Default constructor implementation
	*/
	UAttackActionData();

	/**
	* #### SetEnemy()
	* Mutator method for storing attack target pointer inside the object
	*/
	void SetEnemy(ABaseUnit* inEnemy) { Enemy = inEnemy; }

	/**
	* #### GetEnemy()
	* Accessor method for internal Enemy pointer
	*/
	ABaseUnit* GetEnemy() { return Enemy; }
protected:
	/**
	* #### Enemy
	* Internal BaseUnit pointer
	*/
	UPROPERTY()
		ABaseUnit* Enemy;

};
