// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseBrain.h"
#include "RTSBrain.generated.h"

/**
 * 
 */
UCLASS()
class RTS_FPS_API URTSBrain : public UBaseBrain
{
	GENERATED_BODY()

private:
	virtual void StartBrain() override;

public:
	virtual void FinishedCycle() override;
	
};
