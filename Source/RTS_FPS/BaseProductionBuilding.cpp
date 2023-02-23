// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseProductionBuilding.h"
#include "Kismet/GameplayStatics.h"
#include "BaseProductionBuilding.h"
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
	DOREPLIFETIME(ABaseProductionBuilding, bTraining);
}

void ABaseProductionBuilding::Selected() {
	Super::Selected();
	RallySprite->SetVisibility(true);
	if (TrainingWidgetRef == nullptr) {
		if (OwningPlayerPawn == nullptr) return;

		if (Cast<APawn>(OwningPlayerPawn) == nullptr) return;
		APawn* PlayerPawn = Cast<APawn>(OwningPlayerPawn);

		if (Cast<APlayerController>(PlayerPawn->GetController()) == nullptr) return;
		APlayerController* PC = Cast<APlayerController>(PlayerPawn->GetController());
		TrainingWidgetRef = CreateWidget<UBaseProductionWidget>(PC, WidgetClass);
		TrainingWidgetRef->OwningBuilding = this;
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

bool ABaseProductionBuilding::Server_RecruitUnit_Validate(TSubclassOf<ABaseUnit> UnitType) {
	return true;
}

void ABaseProductionBuilding::Server_RecruitUnit_Implementation(TSubclassOf<ABaseUnit> UnitType) {
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Server_Recruit");
	if (OwningPlayerPawn == nullptr) return;
	if (Cast<ARTSPawn>(OwningPlayerPawn) == nullptr) return;
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Server_OwnerValid");
	ARTSPawn* RTSPawn = Cast<ARTSPawn>(OwningPlayerPawn);

	ABaseUnit* NewUnit = GetWorld()->SpawnActorDeferred<ABaseUnit>(UnitType, FTransform::Identity);
	if (RTSPawn->GetEnergy() < NewUnit->GetTrainingCost_Energy() || RTSPawn->GetMetal() < NewUnit->GetTrainingCost_Metal()) return;
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, GetDebugName(NewUnit));
	RTSPawn->AddResources(ERT_Energy, -NewUnit->GetTrainingCost_Energy());
	RTSPawn->AddResources(ERT_Metal, -NewUnit->GetTrainingCost_Metal());

	Push(NewUnit, NewUnit->GetTrainingTime());
	StartTraining();
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Server_Training");
}

void ABaseProductionBuilding::RecruitUnit(TSubclassOf<ABaseUnit> UnitType) {
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Client_Recruit");
	Server_RecruitUnit(UnitType);
}
void ABaseProductionBuilding::UpdateTrainingProgress() {
	TrainingCount++;
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Server_TrainingCount");
	if (Peek().TrainingTime > TrainingCount) return;
	FUnitProduction Trained = Pop();
	UGameplayStatics::FinishSpawningActor(Trained.TrainedUnit, SpawnLocationSprite->GetComponentTransform());
	Trained.TrainedUnit->AddMovementAction(GetRallyPoint(), Trained.TrainedUnit->UNIT_RESPONSE_PRIORITY, 50.f);
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, GetDebugName(Trained.TrainedUnit));
	GetWorld()->GetTimerManager().ClearTimer(TrainingHandle);
	bTraining = false;
	StartTraining();
}

void ABaseProductionBuilding::StartTraining() {
	if (bTraining || ProductionQueue.Num() <= 0) return; //Must not be training and must be something to train

	//Initialization
	bTraining = true;
	TrainingCount = 0;

	GetWorld()->GetTimerManager().SetTimer(TrainingHandle, this, &ABaseProductionBuilding::UpdateTrainingProgress, 1, true);
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Server_TrainingStarted");
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
	if (!bTraining) return 0.f;
	return (float)TrainingCount / (float)Peek().TrainingTime;
}
