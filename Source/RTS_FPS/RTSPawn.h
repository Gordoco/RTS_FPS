// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Net/UnrealNetwork.h"
#include "BaseBuilding.h"
#include "BaseUnit.h"
#include "Components/StaticMeshComponent.h"
#include "TemplateBuilding.h"
#include "RTSPawn.generated.h"

UCLASS()
class RTS_FPS_API ARTSPawn : public APawn
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

public:	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Team")
		int Team = 0;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Input")
		float MovementSpeed = 2000.f;

	UFUNCTION(BlueprintCallable, Category = "Buildings")
		bool AttemptToBuild(TSubclassOf<ABaseBuilding> BuildingClass);

	void ShouldTakeInput(bool bInput) { bShouldMove = bInput; }

private:
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

	void DrawSelectionBox();

	TArray<ABaseUnit*> SelectedUnits;

	UFUNCTION(Server, Unreliable, WithValidation)
		void Server_FinalizeBuildingPlacement(FTransform Transform);

	void PlayerRightClick();

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
