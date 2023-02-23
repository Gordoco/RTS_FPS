// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BaseUnit.h"
#include "BaseProductionWidget.generated.h"

class ABaseProductionBuilding;

/**
 * 
 */
UCLASS()
class RTS_FPS_API UBaseProductionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Building")
		ABaseProductionBuilding* OwningBuilding;

protected:
	UFUNCTION(BlueprintPure, Category = "Training")
		FUnitInfo GetUnitInfo(TSubclassOf<ABaseUnit> UnitType);
};
