// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TemplateBuilding.h"
#include "TimerManager.h"
#include "Components/StaticMeshComponent.h"
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

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Stats")
		float BuildingCost = 0.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Template")
		TSubclassOf<ATemplateBuilding> TemplateClass;

	void BeginConstruction();
};
