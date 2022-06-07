// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseUnit.h"
#include "Components/BoxComponent.h"
#include "Camera/CameraComponent.h"
#include "FPSCharacter.generated.h"

UCLASS()
class RTS_FPS_API AFPSCharacter : public ABaseUnit
{
	GENERATED_BODY()

private:
#ifdef UE_BUILD_DEBUG
	/*
	UNIT TESTS********************************************
	*/


#endif

public:
	// Sets default values for this character's properties
	AFPSCharacter();

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	void Init();

	UFUNCTION(Client, Unreliable)
		void SpawnOrderMarker(FVector Location, ABaseUnit* Enemy = nullptr);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
		void CreateOrderWidget(FVector Location, ABaseUnit* Enemy);

	UFUNCTION(BlueprintPure, Category = "Firing")
		FHitResult GetShotHit();

	UFUNCTION(BlueprintCallable, Category = "Firing")
		ABaseUnit* ValidateHit(AActor* HitActor);

	UFUNCTION(Server, WithValidation, Unreliable, BlueprintCallable, Category = "Firing")
		void Server_DamageEnemy(ABaseUnit* Enemy);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Firing")
		UBoxComponent* FiringLocation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Components")
		UCameraComponent* FPSCamera;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Components")
		UCameraComponent* ADSFPSCamera;

	virtual void Die();

	virtual void InitCheckForCombat() override;

	virtual void CheckForCombatIterator() override;

	UFUNCTION(Server, WithValidation, Reliable)
		void Server_KillPlayer();

private:

	UCameraComponent* ActiveCam;

	UFUNCTION()
		void ADS();

	UFUNCTION()
		void StopADS();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
