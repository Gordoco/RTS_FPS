// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSPlayerController.h"
#include "RTSPawn.h"
#include "Runtime/Core/Public/Misc/AssertionMacros.h"

void ARTSPlayerController::BeginPlay() {
	FSlateApplication::Get().OnApplicationActivationStateChanged()
		.AddUObject(this, &ARTSPlayerController::OnWindowFocusChanged);
}

void ARTSPlayerController::OnWindowFocusChanged(bool isFocused) {
	ARTSPawn* inPawn = Cast<ARTSPawn>(GetPawn());
	check(inPawn != nullptr);
	if (inPawn != nullptr) {
		inPawn->ShouldTakeInput(isFocused);
	}
}