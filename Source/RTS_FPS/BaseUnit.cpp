// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseUnit.h"
#include "BaseUnitController.h"
#include "MovementActionData.h"
#include "AttackActionData.h"
#include "UnitTracker.h"
#include "FPSCharacter.h"
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

void ABaseUnit::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABaseUnit, Health);
	DOREPLIFETIME(ABaseUnit, MaxHealth);
	DOREPLIFETIME(ABaseUnit, AttackRange);
	DOREPLIFETIME(ABaseUnit, VisionRange);
	DOREPLIFETIME(ABaseUnit, StopRange);
	DOREPLIFETIME(ABaseUnit, Damage);
	DOREPLIFETIME(ABaseUnit, AttackSpeed);
}

//Called from all network roles
void ABaseUnit::Selected() {
	
}

void ABaseUnit::AddAction(FAction Action) {
	check(Action.Action_Type != "");
	check(HasAuthority());
	if (Action.Action_Type != "" && HasAuthority()) {
		ActionQue->Insert(Action);
	}
}

void ABaseUnit::SearchForEnemies(int prio) {
	check(HasAuthority());
	ABaseUnit* Enemy = UUnitTracker::GetClosestUnit(Team, GetActorLocation());
	check(Enemy != nullptr);
	if (Enemy != nullptr) {
		float distance = FVector::Dist(GetActorLocation(), Enemy->GetActorLocation());
		if (distance <= VisionRange) {
			Enemy->SearchForEnemies(prio);
			AddAttackAction(Enemy, prio);
		}
	}
}

void ABaseUnit::RecieveAction() {
	check(HasAuthority());
	if (HasAuthority()) {
		if (!ActionQue->IsEmpty()) {
			/*
			Retrieve next action from queue (Based on priority)
			*/
			if (!DoneCurrentAction()) {

				if (CurrentAction.Priority < ActionQue->Peek().Priority) {
					/*
					Check and re-shuffle current action with highest priority
					*/
					FAction temp = ActionQue->DeleteMax();
					ActionQue->Insert_NoCheck(CurrentAction);
					CurrentAction = temp;
					RunAction();
				}
			}
			else {
				CurrentAction = ActionQue->DeleteMax();
				RunAction();
			}
		}
		else {
			Brain->FinishedCycle();
		}
	}
}

void ABaseUnit::AddMovementAction(FVector Location, int prio) {
	check(HasAuthority());
	if (HasAuthority()) {
		UMovementActionData* Data = NewObject<UMovementActionData>(this);
		check(Data->IsValidLowLevel());
		if (Data->IsValidLowLevel()) {
			Data->SetLocation(Location);
			AddAction(FAction(Data, prio));
		}
	}
}

void ABaseUnit::AddAttackAction(ABaseUnit* Enemy, int prio) {
	check(HasAuthority());
	if (HasAuthority()) {
		UAttackActionData* Data = NewObject<UAttackActionData>(this);
		check(Data->IsValidLowLevel());
		if (Data->IsValidLowLevel()) {
			Data->SetEnemy(Enemy);
			AddAction(FAction(Data, prio));
		}
	}
}

void ABaseUnit::InitCheckForCombat() {
	check(HasAuthority());
	if (HasAuthority()) {
		GetWorld()->GetTimerManager().SetTimer(CheckForCombatHandle, this, &ABaseUnit::CheckForCombatIterator, CheckForCombatFactor, true);
	}
}

void ABaseUnit::CheckForCombatIterator() {
	check(HasAuthority());
	if (HasAuthority()) {
		ABaseUnit* PotentialEnemy = UUnitTracker::GetClosestUnit(Team, GetActorLocation());
		float Dist = FVector::Dist(PotentialEnemy->GetActorLocation(), GetActorLocation());
		if (Dist <= VisionRange) {
			AddAttackAction(PotentialEnemy, UNIT_RESPONSE_PRIORITY);
			if (Dist <= PotentialEnemy->VisionRange) {
				PotentialEnemy->AddAttackAction(this, UNIT_RESPONSE_PRIORITY);
			}
		}
	}
}

//FIND BETTER SCALABLE SOLUTION
void ABaseUnit::RunAction() {
	check(HasAuthority());
	if (HasAuthority()) {
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "RAN ACTION");
		if (CurrentAction.ActionData != nullptr) {
			if (CurrentAction.Action_Type == "MOVEMENT") {
				InitCheckForCombat();
				UMovementActionData* Data = Cast<UMovementActionData>(CurrentAction.ActionData);
				if (Data != nullptr) {
					Cast<AAIController>(GetController())->MoveToLocation(Data->GetLocation());
				}
				else {
					Debug_ActionCastError();
				}
			}
			else if (CurrentAction.Action_Type == "ATTACK") {
				AttackActionHandler();
			}
			else {
				Debug_UnknownCommand(CurrentAction.Action_Type);
			}
		}
	}
}

//NEEDS WORK REGARDING FPS V UNIT COMBAT (ie. STRAFING, COVER, ETC.)
void ABaseUnit::AttackActionHandler() {
	UAttackActionData* Data = Cast<UAttackActionData>(CurrentAction.ActionData);
	if (Data != nullptr) {
		ABaseUnit* Enemy = Data->GetEnemy();
		//Verify Enemy pointer validity (NOT Safe to Destroy() on death, Call Die() Instead), Check if Enemy should be dead, Check if within StopRange
		if (Enemy->IsValidLowLevel() && !Enemy->IsDead() && FVector::Dist(Enemy->GetActorLocation(), GetActorLocation()) <= StopRange) {
			//GEngine->AddOnScreenDebugMessage(12, 5.f, FColor::Green, "SHOULD ATTACK/MOVE AND ATTACK");
			if (CheckIfInRange(Enemy->GetActorLocation())) {
				//GEngine->AddOnScreenDebugMessage(13, 5.f, FColor::Green, "IN RANGE (ATTACKING)");
				Enemy->AddAttackAction(this, UNIT_RESPONSE_PRIORITY);
				MakeAttack(Enemy, Damage);
				GetWorld()->GetTimerManager().SetTimer(AttackSpeedHandle, this, &ABaseUnit::AttackActionHandler, AttackSpeed, false);
			}
			else {
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "OUT OF RANGE (MOVING TO: " + CalculateLocationInRange(Enemy->GetActorLocation()).ToString() + ")");
				AddMovementAction(CalculateLocationInRange(Enemy->GetActorLocation()), CurrentAction.Priority + 1);
			}
		}
		else {
			FinishAction();
		}
	}
	else {
		Debug_ActionCastError();
	}
}

bool ABaseUnit::CheckIfInRange(FVector Location) {
	return FVector::Dist(Location, GetActorLocation()) <= AttackRange;
}

//ADD LOS CHECKS
FVector ABaseUnit::CalculateLocationInRange(FVector EnemyLocation) {
	FVector Dir = EnemyLocation - GetActorLocation();
	Dir.Normalize();
	float Dist = FMath::Abs(FVector::Dist(EnemyLocation, GetActorLocation()));
	FVector ReturnVector = GetActorLocation() + (Dir * ((Dist - AttackRange) * 1.1));
	FHitResult Hit;
	GetWorld()->LineTraceSingleByChannel(Hit, FVector(ReturnVector.X, ReturnVector.Y, 10000), FVector(ReturnVector.X, ReturnVector.Y, -10000), ECC_Visibility);
	return FVector(ReturnVector.X, ReturnVector.Y, Hit.Location.Z);
}

//NEEDS CHANGING TO PROJECTILE/MELEE COMBAT AND TO MAKE INTERACTIVE WITH PLAYER
void ABaseUnit::MakeAttack(ABaseUnit* Enemy, float inDamage) {
	Enemy->DealDamage(inDamage);
}

void ABaseUnit::DealDamage(float inDamage) {
	check(HasAuthority());
	if (HasAuthority()) {
		Health = FMath::Clamp(Health - inDamage, 0, MaxHealth);
		if (Health == 0) {
			Die();
		}
	}
}

//Add OnRep_Health { Health == 0 -> Die }

void ABaseUnit::Die() {
	/*
	Add Animations and other various aesthetic aspects
	*/
	if (HasAuthority()) {
		GetWorld()->GetTimerManager().ClearTimer(CheckForCombatHandle);
		UUnitTracker::DeregisterUnit(this, Team);
	}
	Destroy();
}

void ABaseUnit::FinishAction() {
	check(HasAuthority());
	CurrentAction = FAction();
	RecieveAction();
}

void ABaseUnit::FinishMovement(const FPathFollowingResult& Result) {
	if (HasAuthority()) {
		FinishAction();
		GetWorld()->GetTimerManager().ClearTimer(CheckForCombatHandle);
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
	UUnitTracker::RegisterUnit(this, Team);
	if (!Cast<AFPSCharacter>(this)) {
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
			ActionQue->SetOwner(this);
			Brain = NewObject<UBaseBrain>(this, BrainClass);
			Brain->SetOwner(this);
			RecieveAction();
			CheckForCombatIterator();
		}
	}
}

void ABaseUnit::CheckActions() {
	RecieveAction();
}

// Called every frame
void ABaseUnit::Tick(float DeltaTime)
{Super::Tick(DeltaTime);}

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
