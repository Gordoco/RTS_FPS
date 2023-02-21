// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseResourceUnit.h"
#include "GatherActionData.h"
#include "UnitTracker.h"

ABaseResourceUnit::ABaseResourceUnit() {
	Damage = 0.f;
	AttackSpeed = 0.f;
	AttackRange = 500.f;
	VisionRange = 1000.f;
	StopRange = 2000.f;
}

void ABaseResourceUnit::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABaseResourceUnit, MaxResources);
	DOREPLIFETIME(ABaseResourceUnit, InternalResourceStorage);
	DOREPLIFETIME(ABaseResourceUnit, TypeCarried);
}

void ABaseResourceUnit::InitCheckForCombat() {
	check(HasAuthority());
	if (HasAuthority() && !IsDead()) {
		CheckForCombatIterator();
		GetWorld()->GetTimerManager().SetTimer(CheckForCombatHandle, this, &ABaseResourceUnit::CheckForCombatIterator, CheckForCombatFactor, true);
	}
}

void ABaseResourceUnit::CheckForCombatIterator() {
	check(HasAuthority());
	if (HasAuthority() && !IsDead()) {
		TArray<ABaseUnit*> PotentialEnemys = UUnitTracker::GetUnitsInRange(Team, VisionRange, GetActorLocation());
		for (int i = 0; i < PotentialEnemys.Num(); i++) {
			if (UUnitTracker::CheckForValidity(i)) {
				ABaseUnit* PotentialEnemy = PotentialEnemys[i];
				float Dist = FVector::Dist(PotentialEnemy->GetActorLocation(), GetActorLocation());
				if (Dist <= PotentialEnemy->GetVisionRange()) {
					if (!PotentialEnemy->EnemyList.Contains(this)) {
						PotentialEnemy->AddAttackAction(this, UNIT_RESPONSE_PRIORITY);
						PotentialEnemy->EnemyList.Add(this);
					}
				}
			}
		}
	}
}

void ABaseResourceUnit::AddAttackAction(ABaseUnit* Enemy, int prio) {
	AddMovementAction(Enemy->GetActorLocation(), prio);
}

void ABaseResourceUnit::AddGatherAction(ABaseResource* Resource, int prio) {
	check(HasAuthority());
	if (HasAuthority() && !IsDead()) {
		UGatherActionData* Data = NewObject<UGatherActionData>(this);
		if (Data != nullptr) {
			check(Data->IsValidLowLevel());
			if (Data->IsValidLowLevel()) {
				Data->SetResource(Resource);
				AddAction(FAction(Data, prio));
			}
		}
	}
}

void ABaseResourceUnit::RunAction() {
	Super::RunAction();
	check(HasAuthority());
	if (HasAuthority()) {
		if (CurrentAction.ActionData != nullptr) {
			if (CurrentAction.Action_Type == "GATHER") {
				GatherActionHandler();
			}
			else {
				GetWorld()->GetTimerManager().ClearTimer(GatherHandle);
			}
		}
	}
}

void ABaseResourceUnit::GatherActionHandler() {
	UGatherActionData* Data = Cast<UGatherActionData>(CurrentAction.ActionData);
	if (Data != nullptr) {
		AAIController* AIController = Cast<AAIController>(GetController());
		if (AIController != nullptr && AIController->IsValidLowLevel()) {
			FTransform Transform = Data->GetResource()->GetGatherLocation(GetActorLocation());
			AIController->MoveToLocation(Transform.GetTranslation(), 50.f, true, true, true, true, false);
			bMovingToGather = true;
			GatherRotation = Transform.Rotator();
			CurrResource = Data->GetResource();
			TypeCarried = Data->GetResource()->GetResourceType();
		}
		else {
			FinishAction();
		}
	}
	else {
		FinishAction();
	}
}

void ABaseResourceUnit::FinishMovement(const FPathFollowingResult& Result) {
	if (bMovingToGather && Result.IsSuccess()) {
		SetActorRotation(GatherRotation);
		GetWorld()->GetTimerManager().SetTimer(GatherHandle, this, &ABaseResourceUnit::GatherIterator, 1.f, true);
	}
	else if (bMovingToDropOff && Result.IsSuccess()) {
		GetClosestDropOffPoint()->DropOffResources(TypeCarried, InternalResourceStorage);
		InternalResourceStorage = 0.f;
		if (CurrResource != nullptr) {
			AddGatherAction(CurrResource, UNIT_RESPONSE_PRIORITY);
		}
	}
	bMovingToGather = false;
	bMovingToDropOff = false;
	Super::FinishMovement(Result);
}

ABaseResourceDropOff* ABaseResourceUnit::GetClosestDropOffPoint() {
	float MinDistance = (float)INT32_MAX;
	ABaseResourceDropOff* CurrDropOff = nullptr;

	for (ABaseResourceDropOff* DropOff : PotentialDropOffs) {
		float Dist = FVector::Dist(DropOff->GetActorLocation(), GetActorLocation());
		if (Dist < MinDistance) {
			CurrDropOff = DropOff;
			MinDistance = Dist;
		}
	}

	return CurrDropOff;
}

void ABaseResourceUnit::GatherIterator() {
	if (CurrResource != nullptr) {
		InternalResourceStorage = FMath::Clamp(InternalResourceStorage + CurrResource->GetGatherAmount(), 0.f, MaxResources);
		if (InternalResourceStorage == MaxResources) {
			GetWorld()->GetTimerManager().ClearTimer(GatherHandle);
			AddMovementAction(GetClosestDropOffPoint()->GetActorLocation(), UNIT_RESPONSE_PRIORITY, 50.f);
		}
	}
	else {
		GetWorld()->GetTimerManager().ClearTimer(GatherHandle);
	}
}
