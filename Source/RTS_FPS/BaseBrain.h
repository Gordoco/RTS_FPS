// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BaseUnitController.h"
#include "GameFramework/Character.h"
#include "BaseBrain.generated.h"

/**
 * Actor component meant to store AI interface with AIQueue ADT
 */
UCLASS(Blueprintable, BlueprintType)
class RTS_FPS_API UBaseBrain : public UObject
{
	GENERATED_BODY()

public:
	/**
	* #### SetOwner()
	* Sets the owning Character for the brain
	*/
	void SetOwner(ACharacter* inOwner);

	/**
	* FinishedCycle()
	* 
	*/
	virtual void FinishedCycle();

	UPROPERTY(BlueprintReadOnly, Category = "Team")
		int Team = 0;

protected:
	ABaseUnitController* AIController;

	ACharacter* Owner;

	UFUNCTION(BlueprintPure, Category = "Player")
		ACharacter* GetOwner() { return Owner; }

	UFUNCTION(BlueprintImplementableEvent, Category = "Init")
		void BP_StartBrain();

	virtual void StartBrain();
};
