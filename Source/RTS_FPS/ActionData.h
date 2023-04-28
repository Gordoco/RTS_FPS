// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ActionData.generated.h"

/*
 *	Abstract Data Class for FAction
 *  Contains all relevent data for specified action
 */
UCLASS()
class RTS_FPS_API UActionData : public UObject
{
	GENERATED_BODY()

private:
	FString internalName = TEXT("DEFAULT"); //Searchable Tag, Always _ALL CAPS_

public:
	FString GetName() { return internalName; }
	
protected:
	void SetName(FString inName);
};
