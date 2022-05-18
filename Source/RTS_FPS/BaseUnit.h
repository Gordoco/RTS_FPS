// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AIQueue.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"
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
	void SearchForEnemies(int prio);

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "Damage")
		void DealDamage(float inDamage);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
private:
	/*
	PRIORITY CONSTANTS FOR AI QUEUE****************
	*/
	static const int UNIT_RESPONSE_PRIORITY = 10;
	/*
	***********************************************
	*/

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

	/*
		Attack Helper Functions
	*/
	UPROPERTY()
	int DebugCount = 0;

	FTimerHandle AttackSpeedHandle;

	void AttackActionHandler();

	bool CheckIfInRange(FVector EnemyLocation);

	FVector CalculateLocationInRange(FVector EnemyLocation);

	void MakeAttack(ABaseUnit* Enemy, float inDamage);

	void Die();

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
		float Damage = 1.f;

	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere, Category = "Stats")
		float AttackSpeed = 1.f;

	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere, Category = "Stats")
		float AttackRange = 500.f;

	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere, Category = "Stats")
		float VisionRange = 1000.f;

	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere, Category = "Stats")
		float StopRange = 2000.f;

public:
	UFUNCTION(BlueprintCallable, Category = "Actions")
		void AddMovementAction(FVector Location, int prio);

	UFUNCTION(BlueprintCallable, Category = "Actions")
		void AddAttackAction(ABaseUnit* Enemy, int prio);

	//Should be called on Server
	UFUNCTION(BlueprintCallable, Category = "Stats")
		bool IsDead() { return Health <= 0; }

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
