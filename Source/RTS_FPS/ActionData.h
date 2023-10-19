// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ActionData.generated.h"

/**
 *	Abstract Data Class for FAction
 *  Contains all relevent data for specified action
 */
UCLASS()
class RTS_FPS_API UActionData : public UObject
{
	GENERATED_BODY()

private:
	/**
	* #### internalName
	* A searchable tag always in ALL CAPS for use within the AIQueue
	*/
	FString internalName = TEXT("DEFAULT"); //Searchable Tag, Always _ALL CAPS_

public:
	/**
	* #### GetName()
	* Accessor method for retrieving internalName
	*/
	FString GetName() { return internalName; }
	
protected:
	/**
	* #### SetName()
	* Mutator method for use within the hierarchy to set subclass internalNames
	*/
	void SetName(FString inName);
};
