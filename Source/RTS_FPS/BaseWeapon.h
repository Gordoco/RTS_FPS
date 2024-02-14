// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BaseWeapon.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RTS_FPS_API UBaseWeapon : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBaseWeapon();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Apperance")
		UStaticMesh* GunMesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Stats")
		float Damage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Stats")
		float FireRate;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Stats")
		float ClipSize;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Stats")
		float ReserveAmmo;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Stats")
		float VSpread;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Stats")
		float HSpread;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
		
};
