// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseProductionBuilding.h"

ABaseProductionBuilding::ABaseProductionBuilding() {
	RallySprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("RALLYSPRITE"));
	RallySprite->SetupAttachment(RootComponent);
	RallySprite->SetVisibility(false);
}

void ABaseProductionBuilding::BeginPlay() {
	Super::BeginPlay();
	if (HasAuthority()) {
		Server_SetRallyPoint(RallySprite->GetComponentLocation());
	}
}

void ABaseProductionBuilding::SetRallyPointPosition() {
	RallySprite->SetWorldTransform(FTransform(RallySprite->GetComponentRotation(), FVector(RallyPoint.X, RallyPoint.Y, RallyPoint.Z + 5), RallySprite->GetComponentScale()));
}

void ABaseProductionBuilding::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABaseProductionBuilding, RallyPoint);
}

void ABaseProductionBuilding::Selected() {
	Super::Selected();
	RallySprite->SetVisibility(true);
}

void ABaseProductionBuilding::Deselected() {
	Super::Deselected();
	RallySprite->SetVisibility(false);
}

void ABaseProductionBuilding::Server_SetRallyPoint(FVector Location) {
	RallyPoint = Location;
	if (HasAuthority()) {
		SetRallyPointPosition();
	}
}