// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseResourceDropOff.h"
#include "RTSPawn.h"

void ABaseResourceDropOff::BeginPlay() {
	Super::BeginPlay();

}

void ABaseResourceDropOff::DropOffResources(EResourceType Type, float Resources) {
	check(HasAuthority());
	ARTSPawn* PlayerPawn = Cast<ARTSPawn>(OwningPlayerPawn);
	if (PlayerPawn != nullptr && HasAuthority()) {
		PlayerPawn->AddResources(Type, Resources);
	}
}