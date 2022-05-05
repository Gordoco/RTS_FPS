// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
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


#endif

public:
	// Sets default values for this pawn's properties
	ARTSPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
