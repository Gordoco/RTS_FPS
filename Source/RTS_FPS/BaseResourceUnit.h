// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseUnit.h"
#include "BaseResource.h"
#include "BaseResourceUnit.generated.h"

/**
 * 
 */
UCLASS()
class RTS_FPS_API ABaseResourceUnit : public ABaseUnit
{
	GENERATED_BODY()

public:
	ABaseResourceUnit();

protected:
	virtual void InitCheckForCombat() override;

	virtual void CheckForCombatIterator() override;

	virtual void AddAttackAction(ABaseUnit* Enemy, int prio) override;

public:
	UFUNCTION(BlueprintCallable, Category = "Actions")
		void AddGatherAction(ABaseResource* Resource, int prio);

};
