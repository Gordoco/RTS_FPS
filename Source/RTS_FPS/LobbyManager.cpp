// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyManager.h"
#include "Runtime/Core/Public/Misc/AssertionMacros.h"

// Sets default values
ALobbyManager::ALobbyManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//Map Struct:
	PopulateMapStruct();
}

void ALobbyManager::PopulateMapStruct() {
	Maps.Add(FGameMap(TEXT("Map01"), 4));
	Maps.Add(FGameMap(TEXT("Map02"), 4));
	Maps.Add(FGameMap(TEXT("Map03"), 4));
}

FGameMap* ALobbyManager::GetMapInfo(FString inName = "", int index = -1) {
	check(inName != "" || index != -1);
	check(Maps.Num() > 0);
	if (inName != "") {
		for (int i = 0; i < Maps.Num(); i++) {
			if (Maps[i].MapName == inName) {
				return &Maps[i];
			}
		}
	}
	else if (index > -1) {
		return &Maps[index];
	}
	return nullptr;
}

// Called when the game starts or when spawned
void ALobbyManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALobbyManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

