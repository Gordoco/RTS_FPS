// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseBuilding.h"
#include "Components/BoxComponent.h"
#include "BaseResource.h"
#include "BaseResourceDropOff.generated.h"

/**
 *
 */
UCLASS()
class RTS_FPS_API ABaseResourceDropOff : public ABaseBuilding
{
	GENERATED_BODY()

public:

	ABaseResourceDropOff();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void DropOffResources(EResourceType Type, float Resources);

	FVector GetDropOffLocation();

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Location")
		UBoxComponent* DropOffLocation;

};
