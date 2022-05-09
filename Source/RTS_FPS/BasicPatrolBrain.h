// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseBrain.h"
#include "BasicPatrolBrain.generated.h"

/**
 *
 */
UCLASS()
class RTS_FPS_API UBasicPatrolBrain : public UBaseBrain
{
	GENERATED_BODY()

public:
	virtual void FinishedCycle() override;

protected:
	virtual void StartBrain() override;

	//Loosly Ordered
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Patrol")
		TArray<FVector> PatrolPoints;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Patrol")
		int PatrolPrio = 0;

private:
	void AddPatrolActions();
};
