// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseResourceUnit.h"
#include "GatherActionData.h"
#include "Engine.h"
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
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "TEST: GATHER ACTION ADDED");
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
		GetWorld()->GetTimerManager().ClearTimer(GatherHandle);
		if (CurrentAction.ActionData != nullptr) {
			if (CurrentAction.Action_Type == "GATHER") {
				GatherActionHandler();
			}
		}
	}
}

void ABaseResourceUnit::GatherActionHandler() {
	UGatherActionData* Data = Cast<UGatherActionData>(CurrentAction.ActionData);
	if (Data != nullptr) {
		AAIController* AIController = Cast<AAIController>(GetController());
		if (AIController != nullptr && AIController->IsValidLowLevel()) {
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "TEST: GATHER ACTION RAN");
			FTransform Transform = Data->GetResource()->GetGatherLocation(GetActorLocation());
			AIController->MoveToLocation(Transform.GetTranslation(), 50.f, true, true, true, true, false);
			bMovingToGather = true;
			GatherRotation = Transform.Rotator();
			CurrResource = Data->GetResource();
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
	check(HasAuthority());
	if (!HasAuthority()) return;

	//if (CurrResource != nullptr) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, GetDebugName(CurrResource));
	//if (CurrDropOff != nullptr) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, CurrDropOff->GetDropOffLocation().ToString());

	if (bMovingToGather && Result.IsSuccess()) {
		SetActorRotation(GatherRotation);
		GetWorld()->GetTimerManager().SetTimer(GatherHandle, this, &ABaseResourceUnit::GatherIterator, 1.f, true);
	}
	else if (bMovingToDropOff && Result.IsSuccess() && CurrDropOff != nullptr) {
		CurrDropOff->DropOffResources(TypeCarried, InternalResourceStorage);
		InternalResourceStorage = 0.f;
		TypeCarried = ERT_None;
		BP_UpdateResourceCounter();
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
	TArray<AActor*> PotentialDropOffs;
	ABaseResourceDropOff* ClosestDropOff = nullptr;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseResourceDropOff::StaticClass(), PotentialDropOffs);
	for (AActor* DropOff : PotentialDropOffs) {
		if (DropOff == nullptr) continue;
		ABaseResourceDropOff* DropOff_Casted = Cast<ABaseResourceDropOff>(DropOff);
		if (DropOff_Casted->GetTeam() != Team) continue;
		float Dist = FVector::Dist(DropOff_Casted->GetActorLocation(), GetActorLocation());
		if (Dist < MinDistance) {
			ClosestDropOff = DropOff_Casted;
			MinDistance = Dist;
		}
	}
	return ClosestDropOff;
}

void ABaseResourceUnit::GatherIterator() {
	if (CurrResource != nullptr) {
		if (TypeCarried != CurrResource->GetResourceType()) InternalResourceStorage = 0;
		TypeCarried = CurrResource->GetResourceType();
		InternalResourceStorage = FMath::Clamp(InternalResourceStorage + CurrResource->GetGatherAmount(), 0.f, MaxResources);
		BP_UpdateResourceCounter();
		if (InternalResourceStorage == MaxResources) {
			GetWorld()->GetTimerManager().ClearTimer(GatherHandle);
			CurrDropOff = GetClosestDropOffPoint();
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Pre Dropoff Valid");
			if (CurrDropOff == nullptr) return;
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Post Dropoff Valid");
			bMovingToDropOff = true;
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Should Move To Drop Off");
			AddMovementAction(CurrDropOff->GetDropOffLocation(), UNIT_RESPONSE_PRIORITY, 50.f);
		}
	}
	else {
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Curr Resource Null");
		GetWorld()->GetTimerManager().ClearTimer(GatherHandle);
	}
}
