// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FPSCharacter.h"
#include "RTSPawn.h"
#include "RTS_FPSGameModeBase.h"
#include "RTSPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class RTS_FPS_API ARTSPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
		void OnWindowFocusChanged(bool isFocused);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Classes")
		TSubclassOf<AFPSCharacter> CommandoClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Classes")
		TSubclassOf<ARTSPawn> CommanderClass;

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

protected:
	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere, Category = "Init")
		int Team;

	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere, Category = "Init")
		int MatchGameplayType;


private:
	void InitPC(int inMatchGameplayType, int inTeam);

public:
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Ready")
		bool bReady = false;

	bool bLoaded = false;

	UFUNCTION(BlueprintPure, Category = "Init")
	int GetTeam() { return Team; }

	UFUNCTION(BlueprintPure, Category = "Init")
	int GetMatchGameplayType() { return MatchGameplayType; }

	UFUNCTION(BlueprintCallable, Category = "Replication")
		int ShouldStartGame();

	UFUNCTION(Server, WithValidation, Reliable, BlueprintCallable, Category = "Init")
		void JoinTeamAtPosition(int inTeam, int inMatchGameplayType);

	UFUNCTION(Server, WithValidation, Reliable, BlueprintCallable, Category = "Init")
		void SpawnControlledPawn();

	UFUNCTION(BlueprintImplementableEvent, Category = "Spawning")
		void MovePawnsToPlayerStarts(APawn* inPawn);
};
