// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TemplateBuilding.h"
#include "TimerManager.h"
#include "Components/StaticMeshComponent.h"
#include "BaseResource.h"
#include "Net/UnrealNetwork.h"
#include "BaseBuilding.generated.h"

UCLASS()
class RTS_FPS_API ABaseBuilding : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseBuilding();

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	UStaticMesh* GetFinalMesh() { return FinalMesh; }

	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere, Category = "Player")
		AActor* OwningPlayerPawn;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Components")
		UStaticMeshComponent* Mesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Components")
		UStaticMesh* FinalMesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Components")
		TArray<UStaticMesh*> ConstructionMeshes;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Components")
		float BuildSpeed = 1.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Components")
		float BuildIncrement = 1.f;

	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere, Category = "Stats")
		int Team = 0;

	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere, Category = "Stats")
		bool bFinishedConstruction = false;

private:
	UPROPERTY(ReplicatedUsing = ChangeMesh)
		UStaticMesh* CurrentMesh;

	UFUNCTION()
		void ChangeMesh();

	FTimerHandle ConstructionHandle;

	void ConstructionIterator();

	//Should stay within 0->1
	float BuildProgress = 0.f;

	int BuildStage = 0;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Selected();

	virtual void Deselected();

	UFUNCTION(BlueprintPure, Category = "Stats")
		int GetTeam() { return Team; }

	void SetTeam(int inTeam) { Team = inTeam; }

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Stats")
		float BuildingCost_Metal = 0.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Stats")
		float BuildingCost_Energy = 0.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Stats")
		float BuildingZOffset = 0.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Template")
		TSubclassOf<ATemplateBuilding> TemplateClass;

	UFUNCTION(BlueprintPure, Category = "Construction")
		bool IsConstructed() { return bFinishedConstruction; }

	void BeginConstruction();
};
