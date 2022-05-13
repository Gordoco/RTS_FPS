// Fill out your copyright notice in the Description page of Project Settings.


#include "TemplateBuilding.h"
#include "Engine.h"
#include "Runtime/Core/Public/Misc/AssertionMacros.h"

// Sets default values
ATemplateBuilding::ATemplateBuilding()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SMesh"));
	RootComponent = Mesh;
	Mesh->SetCollisionResponseToAllChannels(ECR_Overlap);
	Mesh->OnComponentBeginOverlap.AddDynamic( this, &ATemplateBuilding::BeginOverlap );
	Mesh->OnComponentEndOverlap.AddDynamic( this, &ATemplateBuilding::OnEndOverlap );
}

// Called when the game starts or when spawned
void ATemplateBuilding::BeginPlay()
{
	Super::BeginPlay();
	check(PlacementMat != nullptr);
	check(ErrorMat != nullptr);
	Mesh->SetMaterial(0, PlacementMat);
}

void ATemplateBuilding::BeginOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult) 
{
	/*
		Add Checks for appropriate overlap events
	*/
	Mesh->SetMaterial(0, ErrorMat);
	bReadyToPlace = false;
}

void ATemplateBuilding::OnEndOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	/*
		Add Checks for appropriate overlap events
	*/
	Mesh->SetMaterial(0, PlacementMat);
	bReadyToPlace = true;
}

// Called every frame
void ATemplateBuilding::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bFollowMouse) {
		check(PC != nullptr);
		if (PC != nullptr) {
			SetActorLocation(CalculateMouseFollowLocation());
		}
	}
}

void ATemplateBuilding::SetFollowMouse(APlayerController* inController) {
	PC = inController;
	bFollowMouse = true;
}

FVector ATemplateBuilding::CalculateMouseFollowLocation() {
	FVector WorldLoc;
	FVector WorldDir;
	PC->DeprojectMousePositionToWorld(WorldLoc, WorldDir);

	FVector TraceEnd = WorldLoc + (WorldDir * 10000);
	FHitResult Hit;
	GetWorld()->LineTraceSingleByChannel(Hit, WorldLoc, TraceEnd, ECC_Visibility, FCollisionQueryParams());
	GEngine->AddOnScreenDebugMessage(10, 5.f, FColor::Purple, Hit.Location.ToString());
	return Hit.Location;
}
