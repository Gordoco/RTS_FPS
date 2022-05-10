// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TemplateBuilding.h"
#include "BaseBuilding.generated.h"

UCLASS()
class RTS_FPS_API ABaseBuilding : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseBuilding();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Stats")
		float BuildingCost = 0.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Stats")
		TSubclassOf<ATemplateBuilding> TemplateClass;

};
