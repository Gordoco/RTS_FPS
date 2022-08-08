// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseResourceUnit.h"
#include "UnitTracker.h"

ABaseResourceUnit::ABaseResourceUnit() {
	Damage = 0.f;
	AttackSpeed = 0.f;
	AttackRange = 500.f;
	VisionRange = 1000.f;
	StopRange = 2000.f;
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
	if (HasAuthority()) {

	}
}
