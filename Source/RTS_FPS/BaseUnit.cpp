// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseUnit.h"
#include "BaseUnitController.h"
#include "MovementActionData.h"
#include "Runtime/Core/Public/Misc/AssertionMacros.h"
#include "Engine.h"

// Sets default values
ABaseUnit::ABaseUnit()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	AIControllerClass = ABaseUnitController::StaticClass();

	bReplicates = true;
	SetReplicateMovement(true);
}

void ABaseUnit::AddAction(FAction Action) {
	check(Action.Action_Type != "");
	check(HasAuthority());
	if (Action.Action_Type != "") {
		ActionQue->Insert(Action);
	}
}

void ABaseUnit::RecieveAction() {
	check(HasAuthority());
	if (!ActionQue->IsEmpty()) {
		if (CurrentAction.Action_Type != "") {
			if (CurrentAction.Priority < ActionQue->Peek().Priority) {
				FAction temp = ActionQue->DeleteMax();
				ActionQue->Insert(CurrentAction);
				CurrentAction = temp;
				//GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Blue, "RunActionCalled: ActionType != \"\"");
				RunAction();
			}
		}
		else {
			CurrentAction = ActionQue->DeleteMax();
			//GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Blue, "RunActionCalled: ActionType == \"\"");
			RunAction();
		}
	}
	else {
		Brain->FinishedCycle();
		if (!ActionQue->IsEmpty() && DoneCurrentAction()) {
			RecieveAction();
		}
	}
}

void ABaseUnit::AddMovementAction(FVector Location, int prio) {
	if (HasAuthority()) {
		UMovementActionData* Data = NewObject<UMovementActionData>(this);
		//Data->AddToRoot();
		check(Data->IsValidLowLevel());
		if (Data->IsValidLowLevel()) {
			Data->SetLocation(Location);
			AddAction(FAction(Data, prio));
		}
	}
}

//FIND BETTER SCALABLE SOLUTION
void ABaseUnit::RunAction() {
	check(HasAuthority());
	if (CurrentAction.Action_Type == "MOVEMENT") {
		if (CurrentAction.ActionData != nullptr) {
			UMovementActionData* Data = Cast<UMovementActionData>(CurrentAction.ActionData);
			if (Data != nullptr) {
				Cast<AAIController>(GetController())->MoveToLocation(Data->GetLocation());
			}
			else {
				Debug_ActionCastError();
			}
		}
	}
	else {
		Debug_UnknownCommand(CurrentAction.Action_Type);
	}
}

void ABaseUnit::FinishAction() {
	check(HasAuthority());
	CurrentAction = FAction();
	RecieveAction();
}

void ABaseUnit::FinishMovement(const FPathFollowingResult& Result) {
	if (HasAuthority()) {
		if (CurrentAction.ActionData->IsValidLowLevel()) {
			CurrentAction.ActionData->ConditionalBeginDestroy();
		}
		FinishAction();
		/*if (Result.Code == EPathFollowingResult::Success) {

		}*/
	}
}

bool ABaseUnit::DoneCurrentAction() {
	check(HasAuthority());
	if (CurrentAction.Action_Type == "") {
		return true;
	}
	return false;
}

// Called when the game starts or when spawned
void ABaseUnit::BeginPlay()
{
	Super::BeginPlay();
	SpawnDefaultController();
	/*if (HasAuthority()) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Server: I AM A UNIT");
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Client: I AM A UNIT");
	}*/

	//Server Side Setup
	if (HasAuthority()) {
		ActionQue = NewObject<UAIQueue>();
		Brain = NewObject<UBaseBrain>(this, BrainClass);
		Brain->SetOwner(this);
		RecieveAction();
	}
}

// Called every frame
void ABaseUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Server Side Action Handling
	/*
	TO DO: Move this away from Tick based handling (Analyze performance)
	*/
}

// Called to bind functionality to input
void ABaseUnit::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

/********************************
	DEBUG MESSAGES
********************************/
void ABaseUnit::Debug_UnknownCommand(FString CommandTag) {
	GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Red, CommandTag);
}

void ABaseUnit::Debug_ActionCastError() {
	GEngine->AddOnScreenDebugMessage(2, 5.f, FColor::Red, "NO DATA");
}
