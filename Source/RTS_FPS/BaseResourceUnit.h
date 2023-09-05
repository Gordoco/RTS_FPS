// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseUnit.h"
#include "BaseResourceDropOff.h"
#include "BaseResource.h"
#include "TimerManager.h"
#include "BaseResourceUnit.generated.h"

/**
 * 
 */
UCLASS()
class RTS_FPS_API ABaseResourceUnit : public ABaseUnit
{
	GENERATED_BODY()

public:
	ABaseResourceUnit();

	virtual void FinishMovement(const FPathFollowingResult& Result) override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintPure, Category = "Resources")
		float GetResources() { return InternalResourceStorage; }

protected:
	virtual void InitCheckForCombat() override;

	virtual void CheckForCombatIterator() override;

	virtual void AddAttackAction(ABaseUnit* Enemy, int prio) override;

	virtual void RunAction() override;

	virtual void Die() override;

	void GatherActionHandler();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Resources", Replicated)
		float MaxResources = 100.f;

	UFUNCTION(BlueprintImplementableEvent, Category = "Resources")
		void BP_UpdateResourceCounter();

private:

	ABaseResourceDropOff* GetClosestDropOffPoint();

	ABaseResourceDropOff* CurrDropOff;

	UPROPERTY(Replicated)
		float InternalResourceStorage = 0.f;

	UPROPERTY(Replicated)
		TEnumAsByte<EResourceType> TypeCarried = ERT_None;

	FTimerHandle GatherHandle;

	UFUNCTION()
		void GatherIterator();

	bool bMovingToGather = false;

	bool bMovingToDropOff = false;

	FRotator GatherRotation;

	ABaseResource* CurrResource;

public:
	UFUNCTION(BlueprintCallable, Category = "Actions")
		void AddGatherAction(ABaseResource* Resource, int prio);

};
