// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSPawn.h"
#include "Engine.h"
#include "Runtime/Core/Public/Misc/AssertionMacros.h"

// Sets default values
ARTSPawn::ARTSPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
}

void ARTSPawn::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ARTSPawn, PlayerLocation);
	DOREPLIFETIME(ARTSPawn, CurrentTemplateClass);
}

// Called when the game starts or when spawned
void ARTSPawn::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsLocallyControlled()) {
		CreateHUD();
		APlayerController* PC = GetPC();
		check(PC != nullptr);
		if (PC != nullptr) {
			PC->SetShowMouseCursor(true);
			FInputModeGameAndUI Params;
			PC->SetInputMode(Params.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways));
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
		CurrentTemplateClass = Server_CurrentBuilding->TemplateClass;
	}
}

void ARTSPawn::OnRep_CurrentTemplateClass() {
	CreateTemplateBuilding();
	check(CurrentTemplate != nullptr && CurrentTemplateClass != nullptr);
	if (CurrentTemplate != nullptr && CurrentTemplateClass != nullptr) {
		CurrentTemplate->SetFollowMouse(GetPC());
	}
}

void ARTSPawn::CreateTemplateBuilding_Implementation() {
	check(GetWorld() != nullptr);
	if (GetWorld() != nullptr) {
		check(CurrentTemplate == nullptr);
		if (CurrentTemplate == nullptr) {
			CurrentTemplate = GetWorld()->SpawnActor<ATemplateBuilding>(CurrentTemplateClass, FTransform());
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

	if (IsLocallyControlled()) {
		CalculateMovement();
		if (!MovementDirection.IsZero()) {
			const FVector newLocation = GetActorLocation() + (MovementDirection * DeltaTime * MovementSpeed);
			SetMyLocation(newLocation);
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

bool ARTSPawn::SetMyLocation_Validate(FVector Location) {
	return true;
}

void ARTSPawn::SetMyLocation_Implementation(FVector Location) {
	SetActorLocation(Location);
	PlayerLocation = Location;
}

void ARTSPawn::CalculateMovement() {
	FVector2D Screen = GetScreenSize();
	FVector2D Mouse = GetMousePosition();

	float val1 = 0;
	float val2 = 0;

	const float ScreenPercentage = 0.05;
	const float Margin = Screen.X * ScreenPercentage;

	if (Mouse.X >= Screen.X - Margin){
		val1 = Mouse.X/Screen.X;
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

	#ifdef UE_BUILD_DEBUG
		DebugCallsMovement(Screen, Mouse, val1, val2, Margin);
	#endif

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

}

