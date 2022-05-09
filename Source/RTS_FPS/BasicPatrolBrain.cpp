// Fill out your copyright notice in the Description page of Project Settings.


#include "BasicPatrolBrain.h"
#include "BaseUnit.h"

void UBasicPatrolBrain::StartBrain() {
	Super::StartBrain();
	AddPatrolActions();
}

void UBasicPatrolBrain::AddPatrolActions() {
	check(PatrolPoints.Num() > 0);
	if (PatrolPoints.Num() > 0) {
		Cast<ABaseUnit>(Owner)->AddMovementAction(PatrolPoints[0], PatrolPrio);
		for (int i = PatrolPoints.Num() - 1; i > 0; i--) {
			Cast<ABaseUnit>(Owner)->AddMovementAction(PatrolPoints[i], PatrolPrio);
		}
	}
}

void UBasicPatrolBrain::FinishedCycle() {
	Super::FinishedCycle();
	AddPatrolActions();
}