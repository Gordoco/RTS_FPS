// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RTSPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class RTS_FPS_API ARTSPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
		void OnWindowFocusChanged(bool isFocused);
	
};
