// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "BaseResource.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BaseBuilding.h"
#include "BaseUnit.h"
#include "Components/StaticMeshComponent.h"
#include "TemplateBuilding.h"
#include "RTSPawn.generated.h"

UCLASS()
class RTS_FPS_API ARTSPawn : public ACharacter
{
	GENERATED_BODY()

private:
#ifdef UE_BUILD_DEBUG
	/*
	UNIT TESTS********************************************
	*/
	void DebugCallsMovement(FVector2D Screen, FVector2D Mouse, float val1, float val2, float Margin);

#endif

public:
	// Sets default values for this pawn's properties
	ARTSPawn();

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	void Init();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
		void CreateHUD();

	UFUNCTION(BlueprintPure, Category = "Units")
		bool HasBuilderSelected();

public:	
	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere, Category = "Team")
		int Team = 0;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Input")
		float MovementSpeed = 3000.f;

	UFUNCTION(BlueprintCallable, Category = "Buildings")
		bool AttemptToBuild(TSubclassOf<ABaseBuilding> BuildingClass);

	void ShouldTakeInput(bool bInput) { bShouldMove = bInput; }

	UFUNCTION(/*Server, Reliable, WithValidation, */BlueprintCallable, Category = "Resources")
		void AddResources(EResourceType Type, float AddVal);

	UFUNCTION(BlueprintPure, Category = "Resources")
		float GetEnergy() { return Energy; }

	UFUNCTION(BlueprintPure, Category = "Resources")
		float GetMetal() { return Metal; }

private:
	//UPROPERTY TO AVOID GARBAGE COLLECTION. SHOULD NEVER BE REFERENCED OUTSIDE OF DRAWSELECTIONBOX()
	UPROPERTY()
		UBoxComponent* SelectionBox;

	FVector StartLocation;

	FVector CachedMovementDirection = FVector();

	UPROPERTY(Replicated)
		float Energy = 0.f;

	UPROPERTY(Replicated)
		float Metal = 200.f;

	UPROPERTY(ReplicatedUsing = OnRep_SetLocation)
		FVector PlayerLocation;

	UFUNCTION()
		void OnRep_SetLocation();

	UFUNCTION()
		void OnRep_CurrentTemplateClass();

	UFUNCTION(Server, WithValidation, Unreliable)
		void SetMyLocation(FVector Location);

	UFUNCTION(Server, WithValidation, Unreliable)
		void Server_CreateBuilding(TSubclassOf<ABaseBuilding> BuildingClass);

	UFUNCTION(Client, Unreliable)
		void CreateTemplateBuilding();

	//PLAYER CLICK FUNCTIONS
	void PlayerClick();

	bool bShouldMove = true;

	//Currently Checking Building Placement Validity on the Client *BAD*
	bool CheckPlacingBuilding() { return CurrentTemplate != nullptr && CurrentTemplate->bReadyToPlace; }

	void DrawSelectionBox(FHitResult Hit);

	void ReleaseLeftClick();

	UFUNCTION(Server, WithValidation, Unreliable)
		void PShift();

	UFUNCTION(Server, WithValidation, Unreliable)
		void RShift();

	UPROPERTY(Replicated)
		bool bShiftPressed = false;

	void EvaluateHitUnit(ABaseUnit* HitUnit);

	void EvaluateHitBuilding(ABaseBuilding* HitBuilding);

	UPROPERTY(Replicated)
		TArray<ABaseUnit*> SelectedUnits;

	UPROPERTY(Replicated)
		TArray<ABaseBuilding*> SelectedBuildings;

	UFUNCTION(Server, Unreliable, WithValidation)
		void Server_FinalizeBuildingPlacement(FTransform Transform);

	UFUNCTION(NetMulticast, Unreliable)
		void UpdateOwnership(ABaseBuilding* BuildingRef);

	void PlayerRightClick();

	void DeselectAllUnits();

	UFUNCTION(BlueprintCallable, Category = "Units")
		bool SelectUnit(ABaseUnit* Unit);

	UFUNCTION(Server, WithValidation, Reliable)
		void OrderUnits(FHitResult Hit);

	UFUNCTION(Server, WithValidation, Reliable)
		void OrderBuildings(FHitResult Hit);

	UFUNCTION(Server, Reliable, WithValidation)
		void Server_SelectUnit(ABaseUnit* Unit);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Teleportation")
		void Server_Teleport(FVector Location);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Orders")
		void Server_OrderMovement(FVector LocationToMove);

	UFUNCTION(Server, Reliable, WithValidation)
		void DeselectUnit(ABaseUnit* Unit);

	UFUNCTION(Server, Reliable, WithValidation)
		void SelectBuilding(ABaseBuilding* Building);

	UFUNCTION(Server, Reliable, WithValidation)
		void DeselectBuilding(ABaseBuilding* Building);

	UFUNCTION(Server, Reliable, WithValidation)
		void DeselectAll();

	void OrderMovement(FVector LocationToMove);

	void Swap(int One, int Two, TArray<ABaseUnit*>* Arr);

	void OrderAttack(ABaseUnit* EnemyUnit);

	void OrderGather(ABaseResource* Resource);

	APlayerController* GetPC();

	FVector MovementDirection = FVector::ZeroVector;

	FVector2D GetScreenSize();

	FVector2D GetMousePosition();

	void CalculateMovement();

	//NULL ON CLIENT ALWAYS
	ABaseBuilding* Server_CurrentBuilding;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentTemplateClass)
		TSubclassOf<ATemplateBuilding> CurrentTemplateClass;

	UPROPERTY(Replicated)
		UStaticMesh* BuildingMesh;

	UPROPERTY()
		ATemplateBuilding* CurrentTemplate;
};
