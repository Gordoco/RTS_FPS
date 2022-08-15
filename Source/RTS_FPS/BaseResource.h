// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "BaseResource.generated.h"

UCLASS()
class RTS_FPS_API ABaseResource : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseResource();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Meshes")
		int NumMeshes = 10;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Meshes")
		float MeshRadialDistance = 100.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Meshes")
		float SpawningRadius = 1000.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Meshes")
		UInstancedStaticMeshComponent* MasterMesh;

	UFUNCTION(CallInEditor, Category = "Generation")
		void GenerateMeshes();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
