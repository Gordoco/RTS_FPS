// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BaseUnitController.h"
#include "GameFramework/Character.h"
#include "BaseBrain.generated.h"

/**
 *
 */
UCLASS(Blueprintable, BlueprintType)
class RTS_FPS_API UBaseBrain : public UObject
{
	GENERATED_BODY()

public:
	void SetOwner(ACharacter* inOwner);

	virtual void FinishedCycle();

protected:
	ABaseUnitController* AIController;

	ACharacter* Owner;

	virtual void StartBrain();
};
