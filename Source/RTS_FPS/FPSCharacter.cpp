// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "RTSPlayerController.h"
#include "GameFramework/GameModeBase.h"

// Sets default values
AFPSCharacter::AFPSCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	FiringLocation = CreateDefaultSubobject<UBoxComponent>(TEXT("Fire Collision"));
	FiringLocation->SetupAttachment(RootComponent);

	FPSCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAM"));
	FPSCamera->SetupAttachment(RootComponent);

}

void AFPSCharacter::Die() {
	Server_KillPlayer();
}

bool AFPSCharacter::Server_KillPlayer_Validate() {
	return true;
}

void AFPSCharacter::Server_KillPlayer_Implementation() {
	ARTSPlayerController* PC = Cast<ARTSPlayerController>(GetController());
	if (PC != nullptr) {
		PC->MovePawnsToPlayerStarts(this);
		Health = MaxHealth;
	}
}

FHitResult AFPSCharacter::GetShotHit() {
	FHitResult Hit;
	check(GetWorld() != nullptr);
	if (GetWorld() != nullptr) {
		FVector Location = FiringLocation->GetComponentLocation();
		GetWorld()->LineTraceSingleByChannel(Hit, Location, Location + (FPSCamera->GetForwardVector() * AttackRange), ECC_Pawn);
	}
	return Hit;
}

//Can add other validation code here
ABaseUnit* AFPSCharacter::ValidateHit(AActor* HitActor) {
	ABaseUnit* Unit = Cast<ABaseUnit>(HitActor);
	if (Unit != nullptr) {
		if (Unit->GetTeam() != Team) {
			return Unit;
		}
	}
	return nullptr;
}

bool AFPSCharacter::Server_DamageEnemy_Validate(ABaseUnit* Enemy) {
	return Enemy != nullptr;
}

void AFPSCharacter::Server_DamageEnemy_Implementation(ABaseUnit* Enemy) {
	Enemy->DealDamage(Damage);
}

void AFPSCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

// Called when the game starts or when spawned
void AFPSCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority()) {
		Health = MaxHealth;
		InitCheckForCombat();
	}
}

void AFPSCharacter::Init() {

}

// Called every frame
void AFPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

