// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AIQueue.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"
#include "PaperSpriteComponent.h"
#include "MovementActionData.h"
#include "BaseBrain.h"
#include "NavigationSystem.h"
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
	/*
	PRIORITY CONSTANTS FOR AI QUEUE****************
	*/
	static const int UNIT_RESPONSE_PRIORITY = 10;

	static const int UNIT_ORDERED_PRIORITY = 20;

	static const int UNIT_SMART_ORDERED_PRIORITY = 8;

	const FVector NULL_VECTOR = FVector(-9999, -9999, -9999);

	const float UNIT_SHUFFLE_DISTANCE = 5.f;
	/*
	***********************************************
	*/

	int FailedMovementCount = 0;

	UFUNCTION(BlueprintCallable, Category = "Stats")
		void Kill() { DealDamage(MaxHealth); }

	float GetVisionRange() { return VisionRange; }

	//Keeps an up-to-date list of all enemys with an active attack action
	TArray<ABaseUnit*> EnemyList;

	bool RemoveEnemyFromList(ABaseUnit* Enemy);

	void EmptyQue();

	bool TEST;

	UFUNCTION(BlueprintPure, Category = "Stats")
		float GetHealth() { return Health; }

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Team", Replicated, Meta = (ExposeOnSpawn = "true"))
		int Team = 0;

	UFUNCTION(BlueprintCallable, Category = "Actions")
		void AddMovementAction(FVector Location, int prio, float inAcceptableRadius = 1.f);

	void AddMovementAction_Helper(UMovementActionData* Data, FVector Location, int prio, float inAcceptableRadius);

	UFUNCTION(BlueprintCallable, Category = "Actions")
		void AddAttackAction(ABaseUnit* Enemy, int prio);

	//Should be called on Server
	UFUNCTION(BlueprintCallable, Category = "Stats")
		bool IsDead() { return Health <= 0; }

	UFUNCTION(NetMulticast, Unreliable)
		void Die_Visuals();

	UFUNCTION(BlueprintImplementableEvent, Category = "Visuals")
		void BP_Die_Visuals();

	FTimerHandle PostDeathCleanupHandle;

	float PostDeathCleanupTime = 10.f;

	FTimerHandle FailedMovementHandle;

	FTimerHandle CheckIfInRangeHandle;

	FTimerDelegate CheckIfInRangeDelegate;

	UFUNCTION()
		void CheckIfInRange_Iterator(ABaseUnit* Enemy);

	float FailedMovementDelay = 0.5f;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Attacking")
		bool bAttacking = false;

	void PostDeathCleanup() { Destroy(); }

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Selected();

	void Deselected();

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


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void BeginPlay_Units();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Selection")
		UPaperSpriteComponent* SelectionSprite;

	virtual void InitCheckForCombat();

	virtual void CheckForCombatIterator();

	FTimerHandle CheckForCombatHandle;

	float CheckForCombatFactor = 0.1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Brain")
		TSubclassOf<UBaseBrain> BrainClass = UBaseBrain::StaticClass();
	/*
		Unit Stats (NEED REPLICATION, SERVERSIDE MANAGMENT, ETC.)
	*/
	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere, Category = "Stats")
		float MaxHealth = 100.f;

	UPROPERTY(ReplicatedUsing = "OnRep_CheckForDeath")
		float Health = MaxHealth;

	UFUNCTION()
		void OnRep_CheckForDeath();

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


private:
	static const int MAX_MOVEMENT_ACTIONS = 10;

	FAction CachedAttackAction = FAction();

	UPROPERTY()
		UAIQueue* ActionQue;

	UPROPERTY()
		UBaseBrain* Brain;

	UPROPERTY()
		FAction CurrentAction = FAction();

	void RecieveAction();

	void FinishAction();

	void PostMovementAction();

	void RunAction();

	void Debug_UnknownCommand(FString CommandTag);

	void Debug_ActionCastError();

	/*
		Attack Helper Functions
	*/
	UPROPERTY()
		int DebugCount = 0;

	FTimerHandle AttackSpeedHandle;

	FVector LastMoveLocation;

	UFUNCTION()
		void FinishRemoveEnemy(FAction Action);

	void MovementActionHandler();

	void AttackActionHandler();

	void AttackReset();

	bool bReadyToAttack = true;

	bool CheckIfInRange(FVector EnemyLocation);

	FVector CalculateLocationInRange(FVector EnemyLocation, ABaseUnit* Enemy);

	FVector Rec_CalculateLocationInRange(FVector EnemyLocation, ABaseUnit* Enemy, float Angle, float Interval, bool bLeft, float OriginalAngle);

	bool ValidateLocationInRange(ABaseUnit* Enemy, FVector Final, UNavigationSystemV1* NavSys, const FNavAgentProperties& AgentProps, FNavLocation* ProjectedLocation, AAIController* AIController, FHitResult Hit);

	void MakeAttack(ABaseUnit* Enemy, float inDamage);

	virtual void Die();

	void Die_InvalidateAndDestroyAI();

	void Die_DisableCollision();

	void Die_ClearTimers();

	bool Dead = false;
};
