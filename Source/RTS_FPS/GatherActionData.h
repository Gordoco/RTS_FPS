// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionData.h"
#include "BaseResource.h"
#include "GatherActionData.generated.h"

/**
 * 
 */
UCLASS()
class RTS_FPS_API UGatherActionData : public UActionData
{
	GENERATED_BODY()

public:
	UGatherActionData();

	void SetResource(ABaseResource* in) { Resource = in; }

	ABaseResource* GetResource() { return Resource; }

protected:
	UPROPERTY()
		ABaseResource* Resource;
};
