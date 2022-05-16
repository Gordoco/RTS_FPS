// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AIQueue.h"
#include "Net/UnrealNetwork.h"
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

	//EXPENSIVE, LIMIT CALLS. PUBLIC DUE TO RECURSIVE NATURE BETWEEN INSTANCES
	void SearchForEnemies();

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void BeginDestroy() override;

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

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Team")
		int Team = 0;

	/*
		Unit Stats (NEED REPLICATION, SERVERSIDE MANAGMENT, ETC.)
	*/
	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere, Category = "Stats")
		float MaxHealth = 100.f;

	UPROPERTY(Replicated)
		float Health = MaxHealth;

	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere, Category = "Stats")
		float AttackRange = 500.f;

	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere, Category = "Stats")
		float VisionRange = 1000.f;

public:
	UFUNCTION(BlueprintCallable, Category = "Actions")
		void AddMovementAction(FVector Location, int prio);

	UFUNCTION(BlueprintCallable, Category = "Actions")
		void AddAttackAction(ABaseUnit* Enemy, int prio);
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Selected();

	int GetTeam() { return Team; }

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
