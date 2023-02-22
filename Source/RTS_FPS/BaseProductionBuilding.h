// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseBuilding.h"
#include "PaperSpriteComponent.h"
#include "BaseProductionBuilding.generated.h"

class ABaseUnit;

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

	TArray<ABaseUnit*> ProductionQueue;

	ABaseUnit* Pop();

	void Push(ABaseUnit* Unit);
	
public:
	virtual void Selected() override;

	virtual void Deselected() override;

	void Server_SetRallyPoint(FVector Location);

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "Training")
		void RecruitUnit(TSubclassOf<ABaseUnit> UnitType);

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintPure, Category = "Rally")
		FVector GetRallyPoint() { return RallyPoint; }

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rally")
		UPaperSpriteComponent* RallySprite;


};
