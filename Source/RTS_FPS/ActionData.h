// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ActionData.generated.h"

/**
 *
 */
UCLASS()
class RTS_FPS_API UActionData : public UObject
{
	GENERATED_BODY()

protected:
	FString internalName = TEXT("DEFAULT");

public:
	FString GetName() { return internalName; }
};
