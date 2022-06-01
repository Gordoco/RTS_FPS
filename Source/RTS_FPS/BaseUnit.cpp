// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseUnit.h"
#include "BaseUnitController.h"
#include "MovementActionData.h"
#include "AttackActionData.h"
#include "UnitTracker.h"
#include "FPSCharacter.h"
#include "Kismet/KismetMathLibrary.h"
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

	SelectionSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("SELECTION"));
	SelectionSprite->SetupAttachment(RootComponent);
	SelectionSprite->SetCollisionResponseToAllChannels(ECR_Ignore);
	SelectionSprite->SetVisibility(false);
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
	DOREPLIFETIME(ABaseUnit, Team);
	DOREPLIFETIME(ABaseUnit, bAttacking);
}

//Called from all network roles
void ABaseUnit::Selected() {
	SelectionSprite->SetVisibility(true);
}

//Called from all network roles
void ABaseUnit::Deselected() {
	SelectionSprite->SetVisibility(false);
}

void ABaseUnit::EmptyQue() {
	if (ActionQue->IsValidLowLevel()) {
		GetWorld()->GetTimerManager().ClearTimer(CheckForCombatHandle);
		GetWorld()->GetTimerManager().ClearTimer(AttackSpeedHandle);
		ActionQue->Empty();
	}
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
		if (ActionQue != nullptr && Brain != nullptr) {
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
	if (HasAuthority() && !IsDead()) {
		TArray<ABaseUnit*> PotentialEnemys = UUnitTracker::GetUnitsInRange(Team, VisionRange, GetActorLocation());
		for (ABaseUnit* PotentialEnemy : PotentialEnemys) {
			if (!EnemyList.Contains(PotentialEnemy) && !PotentialEnemy->IsDead()) {
				float Dist = FVector::Dist(PotentialEnemy->GetActorLocation(), GetActorLocation());
				if (Dist <= VisionRange) {
					AddAttackAction(PotentialEnemy, UNIT_RESPONSE_PRIORITY);
					EnemyList.Add(PotentialEnemy);
					if (!PotentialEnemy->EnemyList.Contains(this)) {
						if (Dist <= PotentialEnemy->VisionRange) {
							PotentialEnemy->AddAttackAction(this, UNIT_RESPONSE_PRIORITY);
							PotentialEnemy->EnemyList.Add(this);
						}
					}
				}
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
				MovementActionHandler();
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

void ABaseUnit::MovementActionHandler() {
	InitCheckForCombat();
	UMovementActionData* Data = Cast<UMovementActionData>(CurrentAction.ActionData);
	if (Data != nullptr) {
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Attempted Movement");
		AAIController* AIController = Cast<AAIController>(GetController());
		if (AIController->IsValidLowLevel()) {
			AIController->MoveToLocation(Data->GetLocation());
		}
	}
	else {
		Debug_ActionCastError();
	}
}

//NEEDS WORK REGARDING FPS V UNIT COMBAT (ie. STRAFING, COVER, ETC.)
void ABaseUnit::AttackActionHandler() {
	bAttacking = true;
	UAttackActionData* Data = Cast<UAttackActionData>(CurrentAction.ActionData);
	if (Data != nullptr) {
		ABaseUnit* Enemy = Data->GetEnemy();
		//Verify Enemy pointer validity (NOT Safe to Destroy() on death, Call Die() Instead), Check if Enemy should be dead, Check if within StopRange
		if (Enemy->IsValidLowLevel() && !Enemy->IsDead() && FVector::Dist(Enemy->GetActorLocation(), GetActorLocation()) <= StopRange) {

			//GEngine->AddOnScreenDebugMessage(12, 5.f, FColor::Green, "SHOULD ATTACK/MOVE AND ATTACK");
			if (CheckIfInRange(Enemy->GetActorLocation())) {
				//GEngine->AddOnScreenDebugMessage(13, 5.f, FColor::Green, "IN RANGE (ATTACKING)");
				MakeAttack(Enemy, Damage);
			}
			else {
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "OUT OF RANGE (MOVING TO: " + CalculateLocationInRange(Enemy->GetActorLocation()).ToString() + ")");
				CheckForCombatIterator();
				AddMovementAction(CalculateLocationInRange(Enemy->GetActorLocation()), CurrentAction.Priority);
				RemoveEnemyFromList(Enemy);
				FinishAction();
			}
		}
		else {
			RemoveEnemyFromList(Enemy);
			FinishAction();
		}
	}
	else {
		Debug_ActionCastError();
	}
}

bool ABaseUnit::RemoveEnemyFromList(ABaseUnit* Enemy) {
	if (Enemy->IsValidLowLevel()) {
		if (EnemyList.Contains(Enemy)) {
			EnemyList.Remove(Enemy);
			return true;
		}
	}
	return false;
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
	SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Enemy->GetActorLocation()));
	Enemy->DealDamage(inDamage);
	GetWorld()->GetTimerManager().SetTimer(AttackSpeedHandle, this, &ABaseUnit::AttackActionHandler, AttackSpeed, false);
}

void ABaseUnit::DealDamage(float inDamage) {
	check(HasAuthority());
	if (HasAuthority() && !IsDead()) {
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
		UUnitTracker::DeregisterUnit(this, Team);
		SelectionSprite->SetVisibility(false);
		GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
		GetWorld()->GetTimerManager().ClearTimer(CheckForCombatHandle);
		GetWorld()->GetTimerManager().ClearTimer(AttackSpeedHandle);
		if (ActionQue->IsValidLowLevel()) {
			ActionQue->Invalidate();
			ActionQue->ConditionalBeginDestroy();
			ActionQue = nullptr;
		}
		if (Brain->IsValidLowLevel()) {
			Brain->ConditionalBeginDestroy();
			Brain = nullptr;
		}
		AAIController* AIController = Cast<AAIController>(GetController());
		if (AIController != nullptr) {
			AIController->StopMovement();
			AIController->Destroy();
		}
		Die_Visuals();
		GetWorld()->GetTimerManager().SetTimer(PostDeathCleanupHandle, this, &ABaseUnit::PostDeathCleanup, PostDeathCleanupTime, false);
	}
}

void ABaseUnit::Die_Visuals_Implementation() {
	BP_Die_Visuals();
}

void ABaseUnit::FinishAction() {
	check(HasAuthority());
	CurrentAction = FAction();
	bAttacking = false;
	RecieveAction();
}

void ABaseUnit::FinishMovement(const FPathFollowingResult& Result) {
	if (HasAuthority()) {
		GetWorld()->GetTimerManager().ClearTimer(CheckForCombatHandle);
		if (!Result.IsSuccess()) {
			//ATTEMPTING TO AVOID A STACK OVERFLOW WITH MASS UNITS. NEED A BETTER LONG TERM SOLUTION

		}
		FinishAction();
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::SanitizeFloat(Result.Code));
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
	//Register Units and Players
	if (HasAuthority()) {
		UUnitTracker::RegisterUnit(this, Team);
	}
	Super::BeginPlay();
	if (Cast<AFPSCharacter>(this) == nullptr) {
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
			Brain->Team = Team;
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
