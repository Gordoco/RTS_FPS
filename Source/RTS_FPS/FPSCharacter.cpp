// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "RTSPlayerController.h"
#include "UnitTracker.h"
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

	ADSFPSCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ADS_CAM"));
	ADSFPSCamera->SetupAttachment(RootComponent);
	ADSFPSCamera->SetActive(false);

	ActiveCam = FPSCamera;
}

void AFPSCharacter::SpawnOrderMarker_Implementation(FVector Location, ABaseUnit* Enemy) {
	CreateOrderWidget(Location, Enemy);
}

void AFPSCharacter::Die() {
	Server_KillPlayer();
	BP_Die_Visuals();
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
		//IMPLEMENT FIRING SPREAD
		FVector HitLocation = SpreadHitTransform(Location + (ActiveCam->GetForwardVector() * AttackRange));
		GetWorld()->LineTraceSingleByChannel(Hit, Location, HitLocation, ECC_Camera);
	}
	return Hit;
}

FVector AFPSCharacter::SpreadHitTransform(FVector IdealHit) {
	float tx = FMath::Rand();
	float tz = FMath::Rand();

	float RightMost = IdealHit.X + SpreadVal;
	float LeftMost = IdealHit.X - SpreadVal;

	float TopMost = IdealHit.Z + SpreadVal;
	float BottomMost = IdealHit.Z - SpreadVal;

	return FVector((LeftMost*tx) + (RightMost*(1-tx)), IdealHit.Y, (BottomMost*tz) + (TopMost * (1-tz)));
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
	return ValidateHit(Enemy) != nullptr;
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

void AFPSCharacter::Init() {}

void AFPSCharacter::InitCheckForCombat() {
	check(HasAuthority());
	if (HasAuthority() && !IsDead()) {
		CheckForCombatIterator();
		GetWorld()->GetTimerManager().SetTimer(CheckForCombatHandle, this, &AFPSCharacter::CheckForCombatIterator, CheckForCombatFactor, true);
	}
}

void AFPSCharacter::CheckForCombatIterator() {
	check(HasAuthority());
	if (HasAuthority() && !IsDead()) {
		TArray<ABaseUnit*> PotentialEnemys = UUnitTracker::GetUnitsInRange(Team, VisionRange, GetActorLocation());
		for (int i = 0; i < PotentialEnemys.Num(); i++) {
			if (UUnitTracker::CheckForValidity(i)) {
				ABaseUnit* PotentialEnemy = PotentialEnemys[i];
				float Dist = FVector::Dist(PotentialEnemy->GetActorLocation(), GetActorLocation());
				if (Dist <= PotentialEnemy->GetVisionRange()) {
					if (!PotentialEnemy->EnemyList.Contains(this)) {
						PotentialEnemy->AddAttackAction(this, UNIT_RESPONSE_PRIORITY);
						PotentialEnemy->EnemyList.Add(this);
					}
				}
			}
		}
	}
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
	PlayerInputComponent->BindAction("SecondaryAction", IE_Pressed, this, &AFPSCharacter::ADS);
	PlayerInputComponent->BindAction("SecondaryAction", IE_Released, this, &AFPSCharacter::StopADS);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AFPSCharacter::StartSprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AFPSCharacter::StopSprint);

	PlayerInputComponent->BindAction("Slide", IE_Pressed, this, &AFPSCharacter::Slide);
}

void AFPSCharacter::ADS() {
	FPSCamera->SetActive(false);
	ADSFPSCamera->SetActive(true);
	ActiveCam = ADSFPSCamera;
}

void AFPSCharacter::StopADS() {
	ADSFPSCamera->SetActive(false);
	FPSCamera->SetActive(true);
	ActiveCam = FPSCamera;
}

void AFPSCharacter::StartSprint() {
	GetCharacterMovement()->MaxWalkSpeed = SprintMovementSpeed;
	UpdateSpeed(SprintMovementSpeed);
	bSprinting = true;
}

void AFPSCharacter::StopSprint() {
	GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
	UpdateSpeed(BaseMovementSpeed);
	bSprinting = false;
}

bool AFPSCharacter::UpdateSpeed_Validate(float newSpeed) {
	return newSpeed == BaseMovementSpeed || newSpeed == SprintMovementSpeed;
}

void AFPSCharacter::UpdateSpeed_Implementation(float newSpeed) {
	GetCharacterMovement()->MaxWalkSpeed = newSpeed;
}

void AFPSCharacter::Slide() {
	if (bSprinting) {
		//SLIDE
	}
}
