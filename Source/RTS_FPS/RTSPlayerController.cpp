// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSPlayerController.h"
#include "RTSPawn.h"
#include "GameFramework/PlayerState.h"
#include "BaseBuilding.h"
#include "Kismet/GameplayStatics.h"
#include "Runtime/Core/Public/Misc/AssertionMacros.h"

ARTSPlayerController::ARTSPlayerController() {}

void ARTSPlayerController::BeginPlay() {
	Super::BeginPlay();
	FSlateApplication::Get().OnApplicationActivationStateChanged()
		.AddUObject(this, &ARTSPlayerController::OnWindowFocusChanged);
}

void ARTSPlayerController::InitLobby() {
	RetrieveMatchData();
}

bool ARTSPlayerController::RetrieveMatchData_Validate() {
	return true;
}

void ARTSPlayerController::RetrieveMatchData_Implementation() {
	ARTS_FPSGameModeBase* GM = Cast<ARTS_FPSGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GM != nullptr) {
		MatchRequests = GM->GetMatchRequests();
		if (HasAuthority()) {
			Client_FinishInitLobby();
		}
	}
}

void ARTSPlayerController::Client_FinishInitLobby() {
	for (FMatchRequest Request : MatchRequests) {
		CreatePlayerWidget(Request.MatchGameplayType, Request.Team, Request.PlayerName);
	}
}

void ARTSPlayerController::StartMatch() {
	if (Player != nullptr && Player->IsValidLowLevel()) {
		if (UGameplayStatics::GetCurrentLevelName(GetWorld()) != "LobbyMenu" && UGameplayStatics::GetCurrentLevelName(GetWorld()) != "AnimatedMainMenu") {
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "SPAWNING PAWNS");
			SpawnControlledPawn();
			CreatePlayerHUDs();
			InitializeBuildings();
		}
	}
	else {
		Destroy();
	}
}

void ARTSPlayerController::InitializeBuildings() {
	TArray<AActor*> Out;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseBuilding::StaticClass(), Out);
	for (AActor* Actor : Out) {
		ABaseBuilding* Building = Cast<ABaseBuilding>(Actor);
		if (Building->GetTeam() == Team) {
			Building->OwningPlayerPawn = this->GetPawn();
		}
	}
}

void ARTSPlayerController::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ARTSPlayerController, bReady);
	DOREPLIFETIME(ARTSPlayerController, Team);
	DOREPLIFETIME(ARTSPlayerController, MatchGameplayType);
	DOREPLIFETIME(ARTSPlayerController, MatchRequests);
}

void ARTSPlayerController::OnWindowFocusChanged(bool isFocused) {
	ARTSPawn* inPawn = Cast<ARTSPawn>(GetPawn());
	//check(inPawn != nullptr);
	if (inPawn != nullptr) {
		inPawn->ShouldTakeInput(isFocused);
	}
}

void ARTSPlayerController::InitPC(int inMatchGameplayType, int inTeam) {
	bLoaded = true;
	Team = inTeam;
	MatchGameplayType = inMatchGameplayType;
}

void ARTSPlayerController::SpawnControlledPawn() {
	check(HasAuthority());
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::SanitizeFloat(Team) + "   " + FString::SanitizeFloat(MatchGameplayType));
	if (GetPawn() != nullptr)
		GetPawn()->Destroy();
	if (MatchGameplayType == 0) {
		ARTSPawn* thisPlayer = GetWorld()->SpawnActorDeferred<ARTSPawn>(CommanderClass, FTransform());
		SetShowMouseCursor(true);
		thisPlayer->Team = Team;
		thisPlayer->FinishSpawning(FTransform());
		Possess(thisPlayer);
		MovePawnsToPlayerStarts(thisPlayer);
		thisPlayer->Init();
	}
	else if (MatchGameplayType == 1) {
		AFPSCharacter* thisPlayer = GetWorld()->SpawnActorDeferred<AFPSCharacter>(CommandoClass, FTransform());
		SetShowMouseCursor(false);
		thisPlayer->Team = Team;
		thisPlayer->FinishSpawning(FTransform());
		Possess(thisPlayer);
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
	if (GM->RequestMatchPosition(FMatchRequest(inTeam, inMatchGameplayType, PlayerState->GetPlayerName()), this)) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "SUCCESS");
		InitPC(inMatchGameplayType, inTeam);
		CreatePlayerWidget(inMatchGameplayType, inTeam, PlayerState->GetPlayerName());
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