// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSPawn.h"
#include "Engine.h"
#include "BaseProductionBuilding.h"
#include "FPSCharacter.h"
#include "Runtime/Core/Public/Misc/AssertionMacros.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ARTSPawn::ARTSPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetCharacterMovement()->DefaultLandMovementMode = EMovementMode::MOVE_Flying;
	GetCharacterMovement()->DefaultWaterMovementMode = EMovementMode::MOVE_Flying;
	GetCharacterMovement()->BrakingDecelerationFlying = MovementSpeed * 1.5;
	GetCharacterMovement()->MaxFlySpeed = MovementSpeed;
	
}

void ARTSPawn::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ARTSPawn, PlayerLocation);
	DOREPLIFETIME(ARTSPawn, CurrentTemplateClass);
	DOREPLIFETIME(ARTSPawn, BuildingMesh);
	DOREPLIFETIME(ARTSPawn, SelectedUnits);
	DOREPLIFETIME(ARTSPawn, SelectedBuildings);
	DOREPLIFETIME(ARTSPawn, bShiftPressed);
	DOREPLIFETIME(ARTSPawn, Team);
}

// Called when the game starts or when spawned
void ARTSPawn::BeginPlay()
{
	Super::BeginPlay();
	
	Init();
}

void ARTSPawn::Init() {
	if (IsLocallyControlled()) {
		CreateHUD();
		APlayerController* PC = GetPC();
		check(PC != nullptr);
		if (PC != nullptr) {
			PC->SetShowMouseCursor(true);
		}
	}
}

bool ARTSPawn::AttemptToBuild(TSubclassOf<ABaseBuilding> BuildingClass) {
	/*
	To Do: Add valdation code for building cost, already placing a structure, valid building subclass, etc.
	*/
	if (IsLocallyControlled()) {
		Server_CreateBuilding(BuildingClass);
		if (HasAuthority()) {
			OnRep_CurrentTemplateClass();
		}
		return true;
	}
	return false;
}

bool ARTSPawn::Server_CreateBuilding_Validate(TSubclassOf<ABaseBuilding> BuildingClass) {
	return BuildingClass != nullptr;
}

void ARTSPawn::Server_CreateBuilding_Implementation(TSubclassOf<ABaseBuilding> BuildingClass) {
	check(HasAuthority());
	check(Server_CurrentBuilding == nullptr);
	if (Server_CurrentBuilding == nullptr) {
		Server_CurrentBuilding = GetWorld()->SpawnActorDeferred<ABaseBuilding>(BuildingClass, FTransform());
		if (Server_CurrentBuilding != nullptr) {
			Server_CurrentBuilding->SetActorHiddenInGame(true);
			Server_CurrentBuilding->SetTeam(Team);
			BuildingMesh = Server_CurrentBuilding->GetFinalMesh();
			//GEngine->AddOnScreenDebugMessage(72, 5.f, FColor::Green, "SERVER: Pre-Spawned Building");
			CurrentTemplateClass = Server_CurrentBuilding->TemplateClass;
		}
	}
}

void ARTSPawn::OnRep_CurrentTemplateClass() {
	if (IsLocallyControlled()) {
		if (CurrentTemplateClass != nullptr) {
			CreateTemplateBuilding();
			if (CurrentTemplate != nullptr) {
				CurrentTemplate->SetFollowMouse(GetPC());
			}
		}
	}
}

void ARTSPawn::CreateTemplateBuilding_Implementation() {
	check(GetWorld() != nullptr);
	if (GetWorld() != nullptr) {
		check(CurrentTemplate == nullptr && BuildingMesh != nullptr);
		if (CurrentTemplate == nullptr && BuildingMesh != nullptr) {
			CurrentTemplate = GetWorld()->SpawnActor<ATemplateBuilding>(CurrentTemplateClass, FTransform());
			CurrentTemplate->SetMesh(BuildingMesh);
		}
	}
}

// Called every frame
void ARTSPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*if (HasAuthority()) {
		GEngine->AddOnScreenDebugMessage(88, 5.f, FColor::Orange, GetDebugName(CurrentBuilding));
	}
	else {
		GEngine->AddOnScreenDebugMessage(89, 5.f, FColor::Red, GetDebugName(CurrentBuilding));
	}*/

	if (IsLocallyControlled() && bShouldMove) {
		CalculateMovement();
		if (!MovementDirection.IsZero()) {
			//const FVector newLocation = GetActorLocation() + (MovementDirection * DeltaTime * MovementSpeed);
			AddMovementInput(MovementDirection, 1);
			//SetMyLocation(newLocation);
		}
	}
	if (SelectionBox != nullptr && SelectionBox->IsValidLowLevel()) {
		APlayerController* PC = GetPC();
		check(PC != nullptr);
		if (PC != nullptr) {
			//Generic Single Select (PURELY CLIENT SIDE)*****
			FHitResult Hit;
			PC->GetHitResultUnderCursor(ECC_Camera, false, Hit);
			FVector Center = (Hit.Location + StartLocation)/2;
			FVector Extent = (Hit.Location - SelectionBox->GetComponentLocation());
			SelectionBox->SetBoxExtent(FVector(FMath::Abs(Extent.X), FMath::Abs(Extent.Y), FMath::Abs(StartLocation.Z - Hit.Location.Z) + 50));
			SelectionBox->SetWorldTransform(FTransform(Center));
		}
	}
}

APlayerController* ARTSPawn::GetPC()
{
	check(GetController() != nullptr)
	if (GetController() != nullptr) {
		APlayerController* PC = Cast<APlayerController>(GetController());

		check(PC != nullptr);
		if (PC != nullptr) {
			return PC;
		}
	}
	return nullptr;
}

void ARTSPawn::OnRep_SetLocation() {
	SetActorLocation(PlayerLocation);
}

//DEPRICATED
bool ARTSPawn::SetMyLocation_Validate(FVector Location) {
	return true;
}

//DEPRICATED
void ARTSPawn::SetMyLocation_Implementation(FVector Location) {
	SetActorLocation(Location);
	PlayerLocation = Location;
}

void ARTSPawn::CalculateMovement() {
	FVector2D Screen = GetScreenSize();
	FVector2D Mouse;
	GEngine->GameViewport->GetMousePosition(Mouse);

	float val1 = 0;
	float val2 = 0;

	const float ScreenPercentage = 0.03;
	const float Margin = Screen.X * ScreenPercentage;

	#ifdef UE_BUILD_DEBUG
		//DebugCallsMovement(Screen, Mouse, val1, val2, Margin);
	#endif
	if (UKismetMathLibrary::NearlyEqual_FloatFloat(Mouse.X, 0, 0.1) && UKismetMathLibrary::NearlyEqual_FloatFloat(Mouse.Y, 0, 0.1)) {
		MovementDirection = CachedMovementDirection;
	}
	else {
		if (Mouse.X >= Screen.X - Margin) {
			val1 = Mouse.X / Screen.X;
		}
		else if (Mouse.X <= Margin) {
			val1 = (Mouse.X - Margin) / Margin;
		}

		if (Mouse.Y >= Screen.Y - Margin) {
			val2 = -1 * Mouse.Y / Screen.Y;
		}
		else if (Mouse.Y <= Margin) {
			val2 = -1 * (Mouse.Y - Margin) / Margin;
		}
		MovementDirection = (GetActorRightVector() * val1) + (GetActorForwardVector() * val2);
		CachedMovementDirection = MovementDirection;
	}

}

#ifdef UE_BUILD_DEBUG
	/*
	UNIT TESTS********************************************
	*/
	void ARTSPawn::DebugCallsMovement(FVector2D Screen, FVector2D Mouse, float val1, float val2, float Margin) {
		GEngine->AddOnScreenDebugMessage(5, 5.f, FColor::Blue, "[ MouseX | ScreenX ] : [ " + FString::SanitizeFloat(Mouse.X) + " | " + FString::SanitizeFloat(Screen.X) + " ]");
		GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Green, "[ MouseY | ScreenY ] : [ " + FString::SanitizeFloat(Mouse.Y) + " | " + FString::SanitizeFloat(Screen.Y) + " ]");
		GEngine->AddOnScreenDebugMessage(2, 5.f, FColor::Blue, "Value X: " + FString::SanitizeFloat(val1));
		GEngine->AddOnScreenDebugMessage(3, 5.f, FColor::Green, "Value Y: " + FString::SanitizeFloat(val2));
		GEngine->AddOnScreenDebugMessage(4, 5.f, FColor::Blue, "Margin: " + FString::SanitizeFloat(Margin));
	}

#endif

FVector2D ARTSPawn::GetScreenSize() {
	APlayerController* PC = GetPC();
	check(PC != nullptr);
	if (PC != nullptr) {
		int32 X;
		int32 Y;
		PC->GetViewportSize(X, Y);
		return FVector2D(X, Y);
	}
	return FVector2D::ZeroVector;
}

FVector2D ARTSPawn::GetMousePosition() {
	APlayerController* PC = GetPC();
	check(PC != nullptr);
	if (PC != nullptr) {
		double X;
		double Y;
		PC->GetMousePosition(X, Y);
		return FVector2D(X, Y);
	}
	return FVector2D::ZeroVector;
}

// Called to bind functionality to input
void ARTSPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("PrimaryAction", IE_Pressed, this, &ARTSPawn::PlayerClick);
	PlayerInputComponent->BindAction("PrimaryAction", IE_Released, this, &ARTSPawn::ReleaseLeftClick);

	PlayerInputComponent->BindAction("Shift", IE_Pressed, this, &ARTSPawn::PShift);
	PlayerInputComponent->BindAction("Shift", IE_Released, this, &ARTSPawn::RShift);

	PlayerInputComponent->BindAction("SecondaryAction", IE_Pressed, this, &ARTSPawn::PlayerRightClick);
}

bool ARTSPawn::PShift_Validate() {
	return bShiftPressed == false;
}

void ARTSPawn::PShift_Implementation() {
	bShiftPressed = true;
}

bool ARTSPawn::RShift_Validate() {
	return bShiftPressed == true;
}

void ARTSPawn::RShift_Implementation() {
	bShiftPressed = false;
}

void ARTSPawn::PlayerClick() 
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "Clicked");
	if (CheckPlacingBuilding() && IsLocallyControlled()) {
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "Locally Controlled and Placing in Progress");
		Server_FinalizeBuildingPlacement(CurrentTemplate->GetTransform());
		CurrentTemplate->Destroy();
		CurrentTemplate = nullptr;
	}
	else {
		APlayerController* PC = GetPC();
		check(PC != nullptr);
		if (PC != nullptr) {
			//Generic Single Select (PURELY CLIENT SIDE)*****
			FHitResult Hit;
			PC->GetHitResultUnderCursor(ECC_Camera, false, Hit);
			ABaseUnit* HitUnit = Cast<ABaseUnit>(Hit.GetActor());
			ABaseBuilding* HitBuilding = Cast<ABaseBuilding>(Hit.GetActor());
			
			if (HitUnit != nullptr) {
				EvaluateHitUnit(HitUnit);
				for (int i = 0; i < SelectedUnits.Num(); i++) {
					if (SelectedUnits[i] != HitUnit) {
						SelectedUnits[i]->Deselected();
					}
				}
				for (int i = 0; i < SelectedBuildings.Num(); i++) {
					if (SelectedBuildings[i] != HitBuilding) {
						SelectedBuildings[i]->Deselected();
					}
				}
				DeselectAll();
			}
			else if (HitBuilding != nullptr){
				EvaluateHitBuilding(HitBuilding);
			}
			DrawSelectionBox(Hit);
		}
	}
}

void ARTSPawn::DrawSelectionBox(FHitResult Hit) 
{
	//Add in combination with unit selection
	SelectionBox = NewObject<UBoxComponent>(this, UBoxComponent::StaticClass(), FName(TEXT("SelectionBoxName")));
	SelectionBox->RegisterComponent();
	SelectionBox->SetVisibility(true);
	SelectionBox->bHiddenInGame = false;
	SelectionBox->SetWorldTransform(FTransform(Hit.Location));
	StartLocation = Hit.Location;
	//***********************************************
}

void ARTSPawn::EvaluateHitUnit(ABaseUnit* HitUnit) {
	bool bDeselect = true;
	if (HitUnit != nullptr && !HitUnit->IsDead()) {
		if (HitUnit->GetTeam() == Team) {
			SelectUnit(HitUnit);
			HitUnit->Selected();
			bDeselect = false;
		}
	}
	if (bDeselect) {
		for (int i = 0; i < SelectedUnits.Num(); i++) {
			SelectedUnits[i]->Deselected();
		}
		for (int i = 0; i < SelectedBuildings.Num(); i++) {
			SelectedBuildings[i]->Deselected();
		}
		DeselectAll();
	}
}

void ARTSPawn::EvaluateHitBuilding(ABaseBuilding* HitBuilding) {
	for (int i = 0; i < SelectedUnits.Num(); i++) {
		SelectedUnits[i]->Deselected();
	}
	for (int i = 0; i < SelectedBuildings.Num(); i++) {
		SelectedBuildings[i]->Deselected();
	}
	DeselectAll();
	if (HitBuilding->GetTeam() == Team) {
		SelectBuilding(HitBuilding);
		HitBuilding->Selected();
	}
}

void ARTSPawn::ReleaseLeftClick() {
	if (SelectionBox->IsValidLowLevel()) {
		TArray<AActor*> Units;
		SelectionBox->GetOverlappingActors(Units, ABaseUnit::StaticClass());
		SelectionBox->ConditionalBeginDestroy();
		for (AActor* Actor : Units) {
			ABaseUnit* Unit = Cast<ABaseUnit>(Actor);
			if (Unit != nullptr && !Unit->IsDead()) {
				if (Unit->GetTeam() == Team) {
					SelectUnit(Unit);
					Unit->Selected();
				}
			}
		}
	}
}

bool ARTSPawn::DeselectAll_Validate() {
	return true;
}

void ARTSPawn::DeselectAll_Implementation() {
	SelectedUnits.Empty();
	SelectedBuildings.Empty();
}

bool ARTSPawn::SelectUnit_Validate(ABaseUnit* Unit) {
	return (Unit != nullptr) && (!Unit->IsDead()) && (Unit->GetTeam() == Team);
}

void ARTSPawn::SelectUnit_Implementation(ABaseUnit* Unit) {
	SelectedUnits.Add(Unit);
}

bool ARTSPawn::DeselectUnit_Validate(ABaseUnit* Unit) {
	return Unit != nullptr;
}

void ARTSPawn::DeselectUnit_Implementation(ABaseUnit* Unit) {
	SelectedUnits.Remove(Unit);
}

bool ARTSPawn::SelectBuilding_Validate(ABaseBuilding* Building) {
	return (Building != nullptr) && (Building->GetTeam() == Team);
}

void ARTSPawn::SelectBuilding_Implementation(ABaseBuilding* Building) {
	SelectedBuildings.Add(Building);
}

bool ARTSPawn::DeselectBuilding_Validate(ABaseBuilding* Building) {
	return (Building != nullptr);
}

void ARTSPawn::DeselectBuilding_Implementation(ABaseBuilding* Building) {
	SelectedBuildings.Remove(Building);
}

bool ARTSPawn::Server_FinalizeBuildingPlacement_Validate(FTransform Transform) 
{
	//Add code to validate Transform
	return (Server_CurrentBuilding != nullptr);
}

void ARTSPawn::Server_FinalizeBuildingPlacement_Implementation(FTransform Transform)
{
	UGameplayStatics::FinishSpawningActor(Server_CurrentBuilding, Transform);
	Server_CurrentBuilding->BeginConstruction();
	Server_CurrentBuilding->SetActorHiddenInGame(false);
	Server_CurrentBuilding = nullptr;
	CurrentTemplateClass = nullptr;
}

void ARTSPawn::PlayerRightClick() 
{
	APlayerController* PC = GetPC();
	check(PC != nullptr);
	if (PC != nullptr) {
		//Generic Single Select (PURELY CLIENT SIDE)*****
		FHitResult Hit;
		PC->GetHitResultUnderCursor(ECC_Camera, false, Hit);
		if (SelectedUnits.Num() > 0) {
			OrderUnits(Hit);
		}
		else if (SelectedBuildings.Num() > 0) {
			OrderBuildings(Hit);
		}
	}
}

bool ARTSPawn::OrderUnits_Validate(FHitResult Hit) {
	//ADD CODE TO CHECK IF HIT IS VALID AND AVOID RPC IF POSSIBLE
	return true;
}

void ARTSPawn::OrderUnits_Implementation(FHitResult Hit) {
	check(HasAuthority());
	if (HasAuthority()) {
		ABaseUnit* PotentialEnemy = Cast<ABaseUnit>(Hit.GetActor());
		if (PotentialEnemy != nullptr) {
			if (PotentialEnemy->GetTeam() != Team) {
				OrderAttack(PotentialEnemy);
			}
		}
		else {
			if (Hit.Location != FVector()) {
				OrderMovement(Hit.Location);
			}
		}
	}
}

bool ARTSPawn::OrderBuildings_Validate(FHitResult Hit) {
	return true;
}

void ARTSPawn::OrderBuildings_Implementation(FHitResult Hit) {
	check(HasAuthority());
	if (HasAuthority()) {
		for (ABaseBuilding* Building : SelectedBuildings) {
			ABaseProductionBuilding* PBuilding = Cast<ABaseProductionBuilding>(Building);
			if (PBuilding != nullptr) {
				PBuilding->Server_SetRallyPoint(Hit.Location);
			}
		}
	}
}

//O(n^2) (EXPENSIVE AF)
void ARTSPawn::OrderMovement(FVector Location) {
	check(HasAuthority());
	if (HasAuthority()) {

		float LargestDist = 0;
		float MaxAcceptance = 34.f;
		SelectedUnits.Sort([Location](const ABaseUnit& A, const ABaseUnit& B) {
			return (A.GetActorLocation() - Location).Size() < (B.GetActorLocation() - Location).Size();
		});
		for (int i = 0; i < SelectedUnits.Num(); i++) {
			if (SelectedUnits[i]->IsValidLowLevel() && !SelectedUnits[i]->IsDead()) {
				AFPSCharacter* Player = Cast<AFPSCharacter>(SelectedUnits[i]);
				if (Player == nullptr) {
					SelectedUnits[i]->EmptyQue();
					if (bShiftPressed) {
						SelectedUnits[i]->AddMovementAction(Location, SelectedUnits[i]->UNIT_SMART_ORDERED_PRIORITY, i * MaxAcceptance);
					}
					else {
						SelectedUnits[i]->AddMovementAction(Location, SelectedUnits[i]->UNIT_ORDERED_PRIORITY, i * MaxAcceptance);
					}
				}
				else {
					Player->SpawnOrderMarker(Location + Player->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
				}
			}
		}
	}
}

void ARTSPawn::OrderAttack(ABaseUnit* EnemyUnit) {
	check(HasAuthority());
	if (HasAuthority()) {
		if (EnemyUnit != nullptr && EnemyUnit->IsValidLowLevel() && !EnemyUnit->IsDead()) {
			for (int i = 0; i < SelectedUnits.Num(); i++) {
				AFPSCharacter* Player = Cast<AFPSCharacter>(SelectedUnits[i]);
				if (Player == nullptr) {
					if (SelectedUnits[i]->IsValidLowLevel() && !SelectedUnits[i]->IsDead()) {
						SelectedUnits[i]->EmptyQue();
						SelectedUnits[i]->AddAttackAction(EnemyUnit, SelectedUnits[i]->UNIT_ORDERED_PRIORITY);
					}
				}
				else {
					Player->SpawnOrderMarker(FVector(), EnemyUnit);
				}
			}
		}
	}
}
