// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "TemplateBuilding.generated.h"

UCLASS()
class RTS_FPS_API ATemplateBuilding : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATemplateBuilding();

	void SetMesh(UStaticMesh* inMesh) { Mesh->SetStaticMesh(inMesh); }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Components")
		UStaticMeshComponent* Mesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Components")
		UMaterial* PlacementMat;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Components")
		UMaterial* ErrorMat;

	UFUNCTION()
		void BeginOverlap(UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult);

	UFUNCTION()
		void OnEndOverlap(UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex);

private:
	bool bFollowMouse = false;

	FVector CalculateMouseFollowLocation();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	APlayerController* PC;

	void SetFollowMouse(APlayerController* inController);

	bool bReadyToPlace = true;

};
