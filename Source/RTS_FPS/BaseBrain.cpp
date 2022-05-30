// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseBrain.h"
#include "BaseUnit.h"
#include "Engine.h"

void UBaseBrain::SetOwner(ACharacter* inOwner) {
	Owner = inOwner;
	check(Cast<ABaseUnitController>(Owner->GetController()));
	if (Cast<ABaseUnitController>(Owner->GetController())) {
		AIController = Cast<ABaseUnitController>(Owner->GetController());
		StartBrain();
	}
}

void UBaseBrain::StartBrain() {
	BP_StartBrain();
}

void UBaseBrain::FinishedCycle() {}