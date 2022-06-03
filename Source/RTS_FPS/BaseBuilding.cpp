// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseBuilding.h"

// Sets default values
ABaseBuilding::ABaseBuilding()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SMesh"));
	RootComponent = Mesh;
	CurrentMesh = Mesh->GetStaticMesh();

	bReplicates = true;
}

void ABaseBuilding::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABaseBuilding, CurrentMesh);
	DOREPLIFETIME(ABaseBuilding, bFinishedConstruction);
	DOREPLIFETIME(ABaseBuilding, Team);
}

void ABaseBuilding::Selected() {
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "Selected: " + GetDebugName(this));
}

void ABaseBuilding::Deselected() {
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "Deelected: " + GetDebugName(this));
}

void ABaseBuilding::ChangeMesh() {
	Mesh->SetStaticMesh(CurrentMesh);
}

//Treat as BeginPlay(), Called when server validates and finalizes building placement
void ABaseBuilding::BeginConstruction() 
{
	if (ConstructionMeshes.Num() > 0) {
		CurrentMesh = ConstructionMeshes[0];
		if (HasAuthority()) {
			ChangeMesh();
		}
		if (GetWorld() != nullptr) {
			GetWorld()->GetTimerManager().SetTimer(ConstructionHandle, this, &ABaseBuilding::ConstructionIterator, BuildSpeed, true);
		}
	}
}

//Based on build progress
void ABaseBuilding::ConstructionIterator() {
	check(BuildProgress >= 0 && BuildProgress <= 1);
	if (BuildProgress < 1 && BuildStage < ConstructionMeshes.Num() - 1) {
		BuildProgress = FMath::Clamp(BuildProgress + BuildIncrement, 0.f, 1.f);
		
		float Percent = BuildStage + 1 / ConstructionMeshes.Num();
		if (BuildProgress >= Percent) {
			BuildStage++;
			CurrentMesh = ConstructionMeshes[BuildStage];
			if (HasAuthority()) {
				ChangeMesh();
			}
		}
	}
	else {
		CurrentMesh = FinalMesh;
		if (HasAuthority()) {
			ChangeMesh();
			bFinishedConstruction = true;
		}
		GetWorld()->GetTimerManager().ClearTimer(ConstructionHandle);
	}
}

// Not relavent for buildings
void ABaseBuilding::BeginPlay()
{Super::BeginPlay();}

// Called every frame
void ABaseBuilding::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

