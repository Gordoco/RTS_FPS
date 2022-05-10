// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AIQueue.h"
#include "BaseBrain.h"
#include "Navigation/PathFollowingComponent.h"
#include "BaseUnit.generated.h"

UCLASS()
class RTS_FPS_API ABaseUnit : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseUnit();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY()
		UAIQueue* ActionQue;

	UPROPERTY()
		UBaseBrain* Brain;

	UPROPERTY()
		FAction CurrentAction = FAction();

	void RecieveAction();

	void FinishAction();

	void RunAction();

	void Debug_UnknownCommand(FString CommandTag);

	void Debug_ActionCastError();

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Brain")
		TSubclassOf<UBaseBrain> BrainClass = UBaseBrain::StaticClass();


public:
	UFUNCTION(BlueprintCallable, Category = "Actions")
		void AddMovementAction(FVector Location, int prio);
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void AddAction(FAction Action);

	void FinishMovement(const FPathFollowingResult& Result);

	bool OutOfActions() { return ActionQue->IsEmpty(); }

	bool DoneCurrentAction();

	void CheckActions();

	UFUNCTION(BlueprintPure, Category = "Brain")
		UBaseBrain* GetBrain() { return Brain; }

};
