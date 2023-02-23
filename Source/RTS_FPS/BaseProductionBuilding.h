// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseBuilding.h"
#include "BaseProductionWidget.h"
#include "PaperSpriteComponent.h"
#include "BaseProductionBuilding.generated.h"

//Forward Declarations
class ABaseUnit;
class UBaseProductionWidget;

USTRUCT() struct FUnitProduction
{
	GENERATED_BODY()

public:

	int TrainingTime;
	ABaseUnit* TrainedUnit;

	FUnitProduction(ABaseUnit* inUnit, float inTime) {
		TrainingTime = inTime;
		TrainedUnit = inUnit;
	}

	FUnitProduction() {
		TrainingTime = 0.f;
		TrainedUnit = nullptr;
	}

};

/**
 * 
 */
UCLASS()
class RTS_FPS_API ABaseProductionBuilding : public ABaseBuilding
{
	GENERATED_BODY()

	ABaseProductionBuilding();

private:
	UPROPERTY(ReplicatedUsing = "SetRallyPointPosition")
		FVector RallyPoint;

	UFUNCTION()
		void SetRallyPointPosition();

	UPROPERTY(Replicated)
		TArray<FUnitProduction> ProductionQueue;

	FUnitProduction Pop();

	FUnitProduction Peek();

	void Push(ABaseUnit* Unit, int time);

	UPROPERTY()
		UBaseProductionWidget* TrainingWidgetRef = nullptr;
	
public:
	virtual void Selected() override;

	virtual void Deselected() override;

	void Server_SetRallyPoint(FVector Location);

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	UFUNCTION(Server, Unreliable, WithValidation, BlueprintCallable, Category = "Training")
		void RecruitUnit(TSubclassOf<ABaseUnit> UnitType);

	UFUNCTION(Server, Unreliable, WithValidation, BlueprintCallable, Category = "Training")
		void CancelTraining();

	UFUNCTION(BlueprintPure, Category = "Training")
		float GetTrainingProgress();

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintPure, Category = "Rally")
		FVector GetRallyPoint() { return RallyPoint; }

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rally")
		UPaperSpriteComponent* RallySprite;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Training")
		UPaperSpriteComponent* SpawnLocationSprite;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Training")
		TSubclassOf<UBaseProductionWidget> WidgetClass;

	FTimerHandle TrainingHandle;
	bool bTraining = false;

	UPROPERTY(Replicated)
		int TrainingCount = 0;

	UFUNCTION()
		void UpdateTrainingProgress();

	void StartTraining();

};
