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
	float tx = FMath::FRand();
	float tz = FMath::FRand();

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::SanitizeFloat(tx) + " | " + FString::SanitizeFloat(tz));
	return (IdealHit
		+ (ActiveCam->GetRightVector() * ((-1 * SpreadVal * tx) + (SpreadVal * (1-tx))))
		+ (ActiveCam->GetUpVector() * ((-1 * SpreadVal * tz) + (SpreadVal * (1 - tz)))));
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
void AFPSCharacter::Tick(float DeltaTime) {
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
	if (HasAuthority()) {
		Slide_Server();
	}
	else {
		Slide_Client();
	}
}

bool AFPSCharacter::Slide_Server_Validate() {
	return true;
}

bool AFPSCharacter::Slide_Client_Validate() {
	return true;
}

void AFPSCharacter::Slide_Server_Implementation() {
	if (bSprinting && !bSliding) {
		//SLIDE
		GetWorld()->GetTimerManager().SetTimer(SlideHandle, this, &AFPSCharacter::SlideIterator, .02f, true);
		FVector Vel = GetVelocity();
		Vel.Normalize();
		CurrSlideDir = Vel;
		bSliding = true;
		Slide_Client();
	}
}

void AFPSCharacter::Slide_Client_Implementation() {
	if (bSprinting && !bSliding) {
		//SLIDE
		GetWorld()->GetTimerManager().SetTimer(SlideHandle, this, &AFPSCharacter::SlideIterator, .02f, true);
		FVector Vel = GetVelocity();
		Vel.Normalize();
		CurrSlideDir = Vel;
		bSliding = true;
		Slide_Server();
	}
}

void AFPSCharacter::SlideIterator() {
	SetActorLocation(GetActorLocation() + (CurrSlideDir * (SlideDist * 0.03)));
	slideCount += 0.02f;
	if (slideCount >= 1.f) {
		GetWorld()->GetTimerManager().ClearTimer(SlideHandle);
		bSliding = false;
		slideCount = 0;
	}
}
