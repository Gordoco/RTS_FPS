// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "RTS_FPSObjectPool.generated.h"

/**
 * 
 */
UCLASS()
class RTS_FPS_API URTS_FPSObjectPool : public UObject
{
	GENERATED_BODY()

public:
	URTS_FPSObjectPool();

	void Initialize(int numObjects, TSubclassOf<AActor> objectClass);


private:
	AActor** Pool;

	int count;

};
