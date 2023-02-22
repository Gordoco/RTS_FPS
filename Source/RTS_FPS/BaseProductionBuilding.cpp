// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseProductionBuilding.h"
#include "Kismet/GameplayStatics.h"
#include "RTSPawn.h"
#include "BaseResource.h"
#include "BaseUnit.h"

ABaseProductionBuilding::ABaseProductionBuilding() {
	RallySprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("RALLYSPRITE"));
	RallySprite->SetupAttachment(RootComponent);
	RallySprite->SetVisibility(false);

	SpawnLocationSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("SPAWNINGSPRITE"));
	SpawnLocationSprite->SetupAttachment(RootComponent);
	SpawnLocationSprite->SetVisibility(false);
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
	DOREPLIFETIME(ABaseProductionBuilding, TrainingCount);
	DOREPLIFETIME(ABaseProductionBuilding, ProductionQueue);
}

void ABaseProductionBuilding::Selected() {
	Super::Selected();
	RallySprite->SetVisibility(true);
	if (TrainingWidgetRef == nullptr) {
		if (OwningPlayerPawn == nullptr) return;

		if (!Cast<APawn>(OwningPlayerPawn)) return;
		APawn* PlayerPawn = Cast<APawn>(OwningPlayerPawn);

		if (!Cast<APlayerController>(PlayerPawn->GetController())) return;
		APlayerController* PC = Cast<APlayerController>(PlayerPawn->GetController());
		TrainingWidgetRef = CreateWidget<UUserWidget>(PC, WidgetClass);
	}
	TrainingWidgetRef->AddToViewport(9999); //Render On Top
}

void ABaseProductionBuilding::Deselected() {
	Super::Deselected();
	RallySprite->SetVisibility(false);
	if (TrainingWidgetRef == nullptr) return;
	TrainingWidgetRef->RemoveFromParent();
}

void ABaseProductionBuilding::Server_SetRallyPoint(FVector Location) {
	RallyPoint = Location;
	if (HasAuthority()) {
		SetRallyPointPosition();
	}
}

FUnitProduction ABaseProductionBuilding::Pop() {
	if (ProductionQueue.Num() <= 0) return FUnitProduction();
	FUnitProduction Val = ProductionQueue[ProductionQueue.Num() - 1];
	ProductionQueue.RemoveAt(ProductionQueue.Num() - 1);
	return Val;
}

FUnitProduction ABaseProductionBuilding::Peek() {
	if (ProductionQueue.Num() <= 0) return FUnitProduction();
	return ProductionQueue[ProductionQueue.Num() - 1];
}

void ABaseProductionBuilding::Push(ABaseUnit* Unit, int time) {
	ProductionQueue.Add(FUnitProduction(Unit, time));
}

bool ABaseProductionBuilding::RecruitUnit_Validate(TSubclassOf<ABaseUnit> UnitType) {
	return true;
}

void ABaseProductionBuilding::RecruitUnit_Implementation(TSubclassOf<ABaseUnit> UnitType) {
	if (OwningPlayerPawn == nullptr) return;
	if (!Cast<ARTSPawn>(OwningPlayerPawn)) return;
	ARTSPawn* RTSPawn = Cast<ARTSPawn>(OwningPlayerPawn);

	ABaseUnit* NewUnit = GetWorld()->SpawnActorDeferred<ABaseUnit>(UnitType, FTransform::Identity);

	if (RTSPawn->GetEnergy() < NewUnit->GetTrainingCost_Energy() || RTSPawn->GetMetal() < NewUnit->GetTrainingCost_Metal()) return;
	RTSPawn->AddResources(ERT_Energy, -NewUnit->GetTrainingCost_Energy());
	RTSPawn->AddResources(ERT_Metal, -NewUnit->GetTrainingCost_Metal());

	Push(NewUnit, NewUnit->GetTrainingTime());
	StartTraining();
}

void ABaseProductionBuilding::UpdateTrainingProgress() {
	TrainingCount++;
	if (Peek().TrainingTime > TrainingCount) return;
	FUnitProduction Trained = Pop();
	UGameplayStatics::FinishSpawningActor(Trained.TrainedUnit, SpawnLocationSprite->GetComponentTransform());
	Trained.TrainedUnit->AddMovementAction(GetRallyPoint(), Trained.TrainedUnit->UNIT_RESPONSE_PRIORITY, 50.f);
	GetWorld()->GetTimerManager().ClearTimer(TrainingHandle);
	StartTraining();
}

void ABaseProductionBuilding::StartTraining() {
	if (bTraining || ProductionQueue.Num() <= 0) return; //Must not be training and must be something to train

	//Initialization
	bTraining = true;
	TrainingCount = 0;

	GetWorld()->GetTimerManager().SetTimer(TrainingHandle, this, &ABaseProductionBuilding::UpdateTrainingProgress, 1, true);
}

bool ABaseProductionBuilding::CancelTraining_Validate() {
	return true;
}

void ABaseProductionBuilding::CancelTraining_Implementation() {
	bTraining = false;
	GetWorld()->GetTimerManager().ClearTimer(TrainingHandle);
	if (OwningPlayerPawn != nullptr) {
		if (Cast<ARTSPawn>(OwningPlayerPawn)) {
			ARTSPawn* RTSPawn = Cast<ARTSPawn>(OwningPlayerPawn);

			RTSPawn->AddResources(ERT_Energy, Peek().TrainedUnit->GetTrainingCost_Energy());
			RTSPawn->AddResources(ERT_Metal, Peek().TrainedUnit->GetTrainingCost_Metal());
		}
	}
	Pop();
	StartTraining();
}

float ABaseProductionBuilding::GetTrainingProgress() {
	if (!bTraining) return -1.f;
	return (float)TrainingCount / (float)Peek().TrainingTime;
}
