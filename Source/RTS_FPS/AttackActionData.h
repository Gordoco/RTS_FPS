// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionData.h"
#include "BaseUnit.h"
#include "AttackActionData.generated.h"

/**
 * 
 */
UCLASS()
class RTS_FPS_API UAttackActionData : public UActionData
{
	GENERATED_BODY()

public:
	UAttackActionData();

	void SetEnemy(ABaseUnit* inEnemy) { Enemy = inEnemy; }

	ABaseUnit* GetEnemy() { return Enemy; }
protected:
	UPROPERTY()
		ABaseUnit* Enemy;

};
