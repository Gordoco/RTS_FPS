// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionData.h"

void UActionData::SetName(FString inName) {
	internalName = inName.ToUpper();
}