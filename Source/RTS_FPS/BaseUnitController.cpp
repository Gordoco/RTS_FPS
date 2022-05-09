// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseUnitController.h"
#include "MovementActionData.h"
#include "BaseUnit.h"

ABaseUnitController::ABaseUnitController() {
	bReplicates = false;
}

void ABaseUnitController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) {
	check(Cast<ABaseUnit>(GetPawn()));
	if (Cast<ABaseUnit>(GetPawn())) {
		check(RequestID.IsValid());
		Cast<ABaseUnit>(GetPawn())->FinishMovement(Result);
	}

}