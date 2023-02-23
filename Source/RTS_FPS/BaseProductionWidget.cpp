// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseProductionWidget.h"

FUnitInfo UBaseProductionWidget::GetUnitInfo(TSubclassOf<ABaseUnit> UnitType) {
	ABaseUnit* NewUnit = GetWorld()->SpawnActorDeferred<ABaseUnit>(UnitType, FTransform::Identity);
	return NewUnit->GetTrainingInfo();
}