// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LobbyManager.generated.h"

USTRUCT(BlueprintType)
struct FGameMap
{
	GENERATED_BODY()

	FGameMap() {}

	FGameMap(FString inName, int inPlayers) {
		MapName = inName;
		MaxPlayers = inPlayers;
	}

	FString MapName = "";
	int MaxPlayers = -1;
};

UCLASS()
class RTS_FPS_API ALobbyManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALobbyManager();

	FGameMap* GetMapInfo(FString inName, int index);
private:
	//Map List
	TArray<FGameMap> Maps;

	void PopulateMapStruct();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
