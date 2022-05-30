// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSPlayerController.h"
#include "RTSPawn.h"
#include "Kismet/GameplayStatics.h"
#include "Runtime/Core/Public/Misc/AssertionMacros.h"

void ARTSPlayerController::BeginPlay() {
	Super::BeginPlay();
	FSlateApplication::Get().OnApplicationActivationStateChanged()
		.AddUObject(this, &ARTSPlayerController::OnWindowFocusChanged);
}

void ARTSPlayerController::StartMatch() {
	if (IsLocalPlayerController() && Player->IsValidLowLevel()) {
		if (UGameplayStatics::GetCurrentLevelName(GetWorld()) != "LobbyMenu" && UGameplayStatics::GetCurrentLevelName(GetWorld()) != "MainMenu") {
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "SPAWNING PAWNS");
			bLoaded = true;
			SpawnControlledPawn();
		}
	}
	else {
		Destroy();
	}
}

void ARTSPlayerController::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ARTSPlayerController, bReady);
	DOREPLIFETIME(ARTSPlayerController, Team);
	DOREPLIFETIME(ARTSPlayerController, MatchGameplayType);
}

void ARTSPlayerController::OnWindowFocusChanged(bool isFocused) {
	ARTSPawn* inPawn = Cast<ARTSPawn>(GetPawn());
	//check(inPawn != nullptr);
	if (inPawn != nullptr) {
		inPawn->ShouldTakeInput(isFocused);
	}
}

void ARTSPlayerController::InitPC(int inMatchGameplayType, int inTeam) {
	Team = inTeam;
	MatchGameplayType = inMatchGameplayType;
}

bool ARTSPlayerController::SpawnControlledPawn_Validate() {
	return (MatchGameplayType < 2) && (MatchGameplayType > -1) && Team != -1;
}

void ARTSPlayerController::SpawnControlledPawn_Implementation() {
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::SanitizeFloat(Team) + "   " + FString::SanitizeFloat(MatchGameplayType));
	if (GetPawn() != nullptr)
		GetPawn()->Destroy();
	if (MatchGameplayType == 0) {
		ARTSPawn* thisPlayer = GetWorld()->SpawnActor<ARTSPawn>(CommanderClass, FVector(), FRotator());
		SetShowMouseCursor(true);
		Possess(thisPlayer);
		thisPlayer->Team = Team;
		MovePawnsToPlayerStarts(thisPlayer);
		thisPlayer->Init();
	}
	else if (MatchGameplayType == 1) {
		AFPSCharacter* thisPlayer = GetWorld()->SpawnActor<AFPSCharacter>(CommandoClass, FVector(), FRotator());
		SetShowMouseCursor(false);
		Possess(thisPlayer);
		thisPlayer->Team = Team;
		MovePawnsToPlayerStarts(thisPlayer);
		thisPlayer->Init();
	}
}

bool ARTSPlayerController::JoinTeamAtPosition_Validate(int inTeam, int inMatchGameplayType) {
	return true;
}

void ARTSPlayerController::JoinTeamAtPosition_Implementation(int inTeam, int inMatchGameplayType) {
	ARTS_FPSGameModeBase* GM = Cast<ARTS_FPSGameModeBase>(GetWorld()->GetAuthGameMode());
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Attempting to join Team: " + FString::SanitizeFloat(inTeam) + " as Type: " + FString::SanitizeFloat(inMatchGameplayType));
	if (GM->RequestMatchPosition(FMatchRequest(inTeam, inMatchGameplayType), this)) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "SUCCESS");
		InitPC(inMatchGameplayType, inTeam);
		bReady = true;
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "FAILURE");
		bReady = false;
	}
}

int ARTSPlayerController::ShouldStartGame() {
	if (HasAuthority()) {
		int count = 0;
		TArray<AActor*> PCs;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARTSPlayerController::StaticClass(), PCs);
		for (AActor* Actor : PCs) {
			ARTSPlayerController* PC = Cast<ARTSPlayerController>(Actor);
			if (PC != nullptr) {
				if (!PC->bReady) {
					return -1;
				}
				count++;
			}
		}
		return count;
	}
	return -1;
}