// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BaseUnitController.generated.h"

/**
 *
 */
UCLASS()
class RTS_FPS_API ABaseUnitController : public AAIController
{
	GENERATED_BODY()
		ABaseUnitController();

protected:
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

};
