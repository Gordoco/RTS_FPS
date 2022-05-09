// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionData.h"
#include "MovementActionData.generated.h"

/**
 *
 */
UCLASS()
class RTS_FPS_API UMovementActionData : public UActionData
{
	GENERATED_BODY()

public:
	UMovementActionData();

	void SetLocation(FVector inLocation) { LocationToMove = inLocation; }

	FVector GetLocation() { return LocationToMove; }

protected:
	FVector LocationToMove = FVector(0.f, 0.f, 0.f);
};
