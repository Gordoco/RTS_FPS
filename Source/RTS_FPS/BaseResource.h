// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "BaseResource.generated.h"

USTRUCT(BlueprintType) struct FInstance
{
	GENERATED_BODY()

public:
	FTransform InstanceTransform;
	int InstanceIndex;

};

UENUM()
enum EResourceType
{
	ERT_Metal,
	ERT_Energy
};

UCLASS()
class RTS_FPS_API ABaseResource : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseResource();

	FTransform GetGatherLocation(FVector UnitLocation);

	float GetGatherAmount() { return GatherAmount; }

	EResourceType GetResourceType() { return Type; }

private:
	TArray<FInstance> Instances;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Resources")
		float GatherAmount = 10.f;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
		TEnumAsByte<EResourceType> Type;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
