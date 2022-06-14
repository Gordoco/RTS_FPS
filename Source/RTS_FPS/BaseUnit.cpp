// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseUnit.h"
#include "BaseUnitController.h"
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

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);

	bReplicates = true;
	SetReplicateMovement(true);

	SelectionSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("SELECTION"));
	SelectionSprite->SetupAttachment(RootComponent);
	SelectionSprite->SetCollisionResponseToAllChannels(ECR_Ignore);
	SelectionSprite->SetVisibility(false);
}

// Called when the game starts or when spawned
void ABaseUnit::BeginPlay()
{
	Super::BeginPlay();
	//Register Units and Players
	if (HasAuthority()) {
		UUnitTracker::RegisterUnit(this, Team);
	}
	if (Cast<AFPSCharacter>(this) == nullptr) {
		BeginPlay_Units();
	}
}

void ABaseUnit::BeginPlay_Units() {
	SpawnDefaultController();

	//Server Side Setup
	if (HasAuthority()) {
		Health = MaxHealth;
		ActionQue = NewObject<UAIQueue>();
		ActionQue->SetOwner(this);
		Brain = NewObject<UBaseBrain>(this, BrainClass);
		Brain->Team = Team;
		Brain->SetOwner(this);
		RecieveAction();
		CheckForCombatIterator();
	}
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
	check(HasAuthority());
	if (HasAuthority()) {
		if (ActionQue != nullptr) {
			CurrentAction = FAction();
			bAttacking = false;
			AAIController* AIController = Cast<AAIController>(GetController());
			if (AIController != nullptr) {
				AIController->StopMovement();
			}
			GetWorld()->GetTimerManager().ClearTimer(CheckForCombatHandle);
			GetWorld()->GetTimerManager().ClearTimer(AttackSpeedHandle);
			ActionQue->Empty();
		}
	}
}

void ABaseUnit::AddAction(FAction Action) {
	check(Action.Action_Type != "");
	check(HasAuthority());
	if (Action.Action_Type != "" && HasAuthority() && ActionQue != nullptr) {
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
	if (HasAuthority() && !IsDead()) {
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
						bAttacking = false;
						GetWorld()->GetTimerManager().ClearTimer(FailedMovementHandle);
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

void ABaseUnit::AddMovementAction(FVector Location, int prio, float inAcceptableRadius) {
	check(HasAuthority());
	if (HasAuthority() && !IsDead()) {
		UMovementActionData* Data = NewObject<UMovementActionData>(this);
		check(Data->IsValidLowLevel());
		if (Data->IsValidLowLevel()) {
			//VERIFY LOCATION
			UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
			const FNavAgentProperties& AgentProps = GetNavAgentPropertiesRef();
			FNavLocation ProjectedLocation;

			if (Location != LastMoveLocation
				&& (FAISystem::IsValidLocation(Location) == true 
				&& NavSys->ProjectPointToNavigation(Location, ProjectedLocation, INVALID_NAVEXTENT, &AgentProps))) {
				LastMoveLocation = Location;
				AddMovementAction_Helper(Data, Location, prio, inAcceptableRadius);
			}
			else {
				FNavLocation outLoc;
				NavSys->GetRandomReachablePointInRadius(GetActorLocation(), 50.f, outLoc);
				LastMoveLocation = FVector();
				AddMovementAction_Helper(Data, outLoc.Location, prio, inAcceptableRadius);
			}
		}
	}
}

void ABaseUnit::AddMovementAction_Helper(UMovementActionData* Data, FVector Location, int prio, float inAcceptableRadius) {
	Data->SetLocation(Location);
	Data->AcceptableRadius = inAcceptableRadius;
	AddAction(FAction(Data, prio));
}

void ABaseUnit::AddAttackAction(ABaseUnit* Enemy, int prio) {
	check(HasAuthority());
	if (HasAuthority() && !IsDead()) {
		UAttackActionData* Data = NewObject<UAttackActionData>(this);
		if (Data != nullptr) {
			check(Data->IsValidLowLevel());
			if (Data->IsValidLowLevel()) {
				Data->SetEnemy(Enemy);
				AddAction(FAction(Data, prio));
			}
		}
	}
}

void ABaseUnit::InitCheckForCombat() {
	check(HasAuthority());
	if (HasAuthority() && !IsDead()) {
		CheckForCombatIterator();
		GetWorld()->GetTimerManager().SetTimer(CheckForCombatHandle, this, &ABaseUnit::CheckForCombatIterator, CheckForCombatFactor, true);
	}
}

void ABaseUnit::CheckForCombatIterator() {
	check(HasAuthority());
	if (HasAuthority() && !IsDead()) {
		TArray<ABaseUnit*> PotentialEnemys = UUnitTracker::GetUnitsInRange(Team, VisionRange, GetActorLocation());
		for (int i = 0; i < PotentialEnemys.Num(); i++) {
			if (UUnitTracker::CheckForValidity(i)) {
				CheckForCombatHelper(PotentialEnemys[i]);
			}
			else {
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "UNIT NOT VALID");
			}
		}
	}
}

void ABaseUnit::CheckForCombatHelper(ABaseUnit* PotentialEnemy) {
	check(PotentialEnemy != nullptr && PotentialEnemy->IsValidLowLevel() && !PotentialEnemy->IsDead());
	if (PotentialEnemy != nullptr && PotentialEnemy->IsValidLowLevel() && !PotentialEnemy->IsDead()) {
		float Dist = FVector::Dist(PotentialEnemy->GetActorLocation(), GetActorLocation());
		if (Dist <= VisionRange) {
			if (!EnemyList.Contains(PotentialEnemy)) {
				AddAttackAction(PotentialEnemy, UNIT_RESPONSE_PRIORITY);
				EnemyList.Add(PotentialEnemy);
			}
		}
		if (Dist <= PotentialEnemy->VisionRange) {
			if (!PotentialEnemy->EnemyList.Contains(this)) {
				PotentialEnemy->AddAttackAction(this, UNIT_RESPONSE_PRIORITY);
				PotentialEnemy->EnemyList.Add(this);
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
				FinishAction();
			}
		}
		else {
			FinishAction();
		}
	}
}

void ABaseUnit::MovementActionHandler() {
	InitCheckForCombat();
	UMovementActionData* Data = Cast<UMovementActionData>(CurrentAction.ActionData);
	if (Data != nullptr) {
		AAIController* AIController = Cast<AAIController>(GetController());
		if (AIController != nullptr && AIController->IsValidLowLevel()) {
			AIController->MoveToLocation(Data->GetLocation(), Data->AcceptableRadius, true, true, true, true, false);
		}
		else {
			FinishAction();
		}
	}
	else {
		Debug_ActionCastError();
		FinishAction();
	}
}

//NEEDS WORK REGARDING FPS V UNIT COMBAT (ie. STRAFING, COVER, ETC.)
void ABaseUnit::AttackActionHandler() {
	bAttacking = true;
	UAttackActionData* Data = Cast<UAttackActionData>(CurrentAction.ActionData);
	AAIController* AIController = Cast<AAIController>(GetController());
	if (Data != nullptr && AIController != nullptr) {
		ABaseUnit* Enemy = Data->GetEnemy();

		//Verify Enemy pointer validity (NOT Safe to Destroy() on death, Call Die() Instead), Check if Enemy should be dead, Check if within StopRange
		if (Enemy != nullptr && Enemy->IsValidLowLevel() && !Enemy->IsDead() && FVector::Dist(Enemy->GetActorLocation(), GetActorLocation()) <= StopRange) {

			//Check for Range and LOS
			if (CheckIfInRange(Enemy->GetActorLocation()) && AIController->LineOfSightTo(Enemy, GetActorLocation())) {

				//Check for Attack Speed
				if (bReadyToAttack) {
					MakeAttack(Enemy, Damage);
					bReadyToAttack = false;
					GetWorld()->GetTimerManager().SetTimer(AttackSpeedHandle, this, &ABaseUnit::AttackReset, AttackSpeed, false);
				}
			}
			else {

				//Attempt to get a valid location to Move To
				FVector TargetLocation = CalculateLocationInRange(Enemy->GetActorLocation(), Enemy);

				if (TargetLocation != NULL_VECTOR) {

					//On Success Move In Range
					CachedAttackAction = CurrentAction;
					CheckIfInRangeDelegate.BindUFunction(this, "CheckIfInRange_Iterator", Enemy);
					GetWorld()->GetTimerManager().SetTimer(CheckIfInRangeHandle, CheckIfInRangeDelegate, 0.2, true);
					AddMovementAction(TargetLocation, CurrentAction.Priority + 1);
				}
				else {
					FTimerHandle FailedRangeHandle;
					FTimerDelegate FailedRangeDelegate;
					FailedRangeDelegate.BindUFunction(this, "FinishRemoveEnemy", CurrentAction);
					GetWorld()->GetTimerManager().SetTimer(FailedRangeHandle, FailedRangeDelegate, 1.f, false);
					FinishAction();
				}
			}
		}
		else {
			RemoveEnemyFromList(Enemy);
			FinishAction();
		}
	}
	else {
		Debug_ActionCastError();
		FinishAction();
	}
}

void ABaseUnit::FinishRemoveEnemy(FAction Action) {
	UAttackActionData* Data = Cast<UAttackActionData>(Action.ActionData);
	if (Data != nullptr) {
		ABaseUnit* Enemy = Data->GetEnemy();
		if (Enemy != nullptr && Enemy->IsValidLowLevel() && !Enemy->IsDead()) {
			RemoveEnemyFromList(Enemy);
		}
	}
}

void ABaseUnit::CheckIfInRange_Iterator(ABaseUnit* Enemy) {
	if (CheckIfInRange(Enemy->GetActorLocation()) || Enemy->IsDead()) {
		GetWorld()->GetTimerManager().ClearTimer(CheckIfInRangeHandle);
		AAIController* AIController = Cast<AAIController>(GetController());
		if (AIController != nullptr) {
			AIController->StopMovement();
			PostMovementAction();
		}
	}
}

void ABaseUnit::AttackReset() {
	bReadyToAttack = true;
	AttackActionHandler();
}

bool ABaseUnit::RemoveEnemyFromList(ABaseUnit* Enemy) {
	if (Enemy != nullptr && Enemy->IsValidLowLevel()) {
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

FVector ABaseUnit::CalculateLocationInRange(FVector EnemyLocation, ABaseUnit* Enemy) {
	//Calculate Direction
	FVector Dir = EnemyLocation - GetActorLocation();
	Dir.Normalize();

	//Get Straight line point in range
	float Dist = FMath::Abs(FVector::Dist(EnemyLocation, GetActorLocation()));
	FVector ReturnVector = GetActorLocation() + (Dir * ((Dist - AttackRange) * 1.1));

	//Line trace down to ground at straight line location
	FHitResult Hit;
	GetWorld()->LineTraceSingleByChannel(Hit, FVector(ReturnVector.X, ReturnVector.Y, 10000), FVector(ReturnVector.X, ReturnVector.Y, -10000), ECC_Camera);
	FVector Final = FVector(ReturnVector.X, ReturnVector.Y, Hit.Location.Z);

	//Ensure location is in range and when tracing to the ground it hit a valid surface
	if (FVector::Dist(Final, EnemyLocation) > AttackRange || !Hit.bBlockingHit) {
		return NULL_VECTOR;
	}

	//Return vector init as NULL
	FVector EndVector = NULL_VECTOR;

	//NAV SYSTEM VARs
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	const FNavAgentProperties& AgentProps = GetNavAgentPropertiesRef();
	FNavLocation ProjectedLocation;


	AAIController* AIController = Cast<AAIController>(GetController());
	if (AIController != nullptr) {

		//Return Straight Line Result if Valid
		if (ValidateLocationInRange(Enemy, Final, NavSys, AgentProps, &ProjectedLocation, AIController, Hit)) {
			EndVector = Final;
		}
		else {

			//Recursively find points left and right at an interval along the circumfrence of the "In Range Circle" and return closest valid result
			float Angle = UKismetMathLibrary::Atan2(Dir.Y, Dir.X) + UKismetMathLibrary::GetPI() / 2; //Calculate angle of straight line point (Atan2 for 360 degree return value)
			
			//Rotate around the circle in opposite directions
			FVector FirstLeftPoint = Rec_CalculateLocationInRange(EnemyLocation, Enemy, Angle, 0.5f, true, Angle);
			FVector FirstRightPoint = Rec_CalculateLocationInRange(EnemyLocation, Enemy, Angle, 0.5f, false, Angle);

			//Skip determining result if either side is a NULL value
			if (FirstLeftPoint == NULL_VECTOR && FirstRightPoint == NULL_VECTOR) {
				return NULL_VECTOR;
			}
			else {
				if (FirstLeftPoint == NULL_VECTOR) {
					EndVector = FirstRightPoint;
				}
				else if (FirstRightPoint == NULL_VECTOR) {
					EndVector = FirstLeftPoint;
				}
				else {

					//Choose closest valid location if both left and right are valid
					float Dist1 = FVector::Dist(GetActorLocation(), FirstLeftPoint);
					float Dist2 = FVector::Dist(GetActorLocation(), FirstRightPoint);
					if (Dist1 < Dist2) {
						EndVector = FirstLeftPoint;
					}
					else {
						EndVector = FirstRightPoint;
					}
				}
			}
		}
	}
	return EndVector;
}

FVector ABaseUnit::Rec_CalculateLocationInRange(FVector EnemyLocation, ABaseUnit* Enemy, float Angle, float Interval, bool bLeft, float OriginalAngle) {

	//Rotate around the range circle by an interval
	float NewAngle;
	if (bLeft) {
		NewAngle = Angle + Interval;
	}
	else {
		NewAngle = Angle - Interval;
	}

	//Set ReturnVector to next rotated value
	FVector ReturnVector = FVector(EnemyLocation.X + ((AttackRange * 0.9) * FMath::Sin(NewAngle)), EnemyLocation.Y + (AttackRange * FMath::Cos(NewAngle)), EnemyLocation.Z);

	//Trace Returned location to the ground
	FHitResult Hit;
	GetWorld()->LineTraceSingleByChannel(Hit, FVector(ReturnVector.X, ReturnVector.Y, 10000), FVector(ReturnVector.X, ReturnVector.Y, -10000), ECC_Camera);
	FVector Final = FVector(ReturnVector.X, ReturnVector.Y, Hit.Location.Z);

	//NAV SYSTEM VARs
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	const FNavAgentProperties& AgentProps = GetNavAgentPropertiesRef();
	FNavLocation ProjectedLocation;

	AAIController* AIController = Cast<AAIController>(GetController());
	if (AIController != nullptr) {

		//Return Current Result if Valid (Base Case)
		if (ValidateLocationInRange(Enemy, Final, NavSys, AgentProps, &ProjectedLocation, AIController, Hit)) {
			return Final;
		}
		else if (FMath::Abs(NewAngle - OriginalAngle) < Interval) { return NULL_VECTOR; } //Prevent infinite recursion by stopping when completing a full circle
		else {
			//Check Next Angle (Recursive Case)
			return Rec_CalculateLocationInRange(EnemyLocation, Enemy, NewAngle, Interval, bLeft, OriginalAngle);
		}
	}
	else { return NULL_VECTOR; } //If AI is invalidated terminate
}

bool ABaseUnit::ValidateLocationInRange(ABaseUnit* Enemy, FVector Final, UNavigationSystemV1* NavSys, const FNavAgentProperties& AgentProps, FNavLocation* ProjectedLocation, AAIController* AIController, FHitResult Hit) {
	if (FAISystem::IsValidLocation(Final) == true
		&& NavSys->ProjectPointToNavigation(Final, *ProjectedLocation, INVALID_NAVEXTENT, &AgentProps)
		&& !Cast<ABaseUnit>(Hit.GetActor())
		&& AIController->LineOfSightTo(Enemy, FVector(Final.X, Final.Y, Final.Z + GetCapsuleComponent()->GetScaledCapsuleHalfHeight()))
		&& !(FVector::Dist(Final, Enemy->GetActorLocation()) > AttackRange || !Hit.bBlockingHit)
		&& !(UKismetMathLibrary::NearlyEqual_FloatFloat(Final.X, 0, 0.5) && UKismetMathLibrary::NearlyEqual_FloatFloat(Final.Y, 0, 0.5) && UKismetMathLibrary::NearlyEqual_FloatFloat(Final.Z, 0, 0.5))) {
		return true;
	}
	return false;
}

//NEEDS CHANGING TO PROJECTILE/MELEE COMBAT AND TO MAKE INTERACTIVE WITH PLAYER
void ABaseUnit::MakeAttack(ABaseUnit* Enemy, float inDamage) {
	SetActorRotation(FRotator(0.f, FRotator(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Enemy->GetActorLocation())).Yaw, 0.f));
	Enemy->DealDamage(inDamage);
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

void ABaseUnit::OnRep_CheckForDeath() {
	if (Health == 0) {
		Die();
	}
}

//Add OnRep_Health { Health == 0 -> Die }
void ABaseUnit::Die() {
	
	//Remove Selection Sprite (All Network Roles)
	SelectionSprite->SetVisibility(false);

	//Server Only Cleanup
	if (HasAuthority() && !Dead) {

		//Set Variables and Deregister Unit from UnitTracker
		Dead = true;
		bAttacking = false;
		UUnitTracker::DeregisterUnit(this, Team);

		//Helper Functions for Death
		Die_DisableCollision();
		Die_ClearTimers();
		Die_InvalidateAndDestroyAI();
		Die_Visuals();

		//Final cleanup of actor including Destroy()
		GetWorld()->GetTimerManager().SetTimer(PostDeathCleanupHandle, this, &ABaseUnit::PostDeathCleanup, PostDeathCleanupTime, false);
	}
}

void ABaseUnit::Die_InvalidateAndDestroyAI() {

	AAIController* AIController = Cast<AAIController>(GetController());
	if (AIController != nullptr) {
		//Destroy AIController and stop any active movement actions
		AIController->StopMovement();
		AIController->Destroy();
	}
	if (ActionQue != nullptr) {
		//Destroy Action Data structure and Invalidate Members (Destruction not instant but disassociates data structure from unit)
		ActionQue->Invalidate();
		ActionQue->ConditionalBeginDestroy();
		ActionQue = nullptr;
	}
	if (Brain != nullptr) {
		//Destroys and invalidates the brain (Useful once AI Functionality moves to the BaseBrain Class)
		Brain->ConditionalBeginDestroy();
		Brain = nullptr;
	}
}

void ABaseUnit::Die_DisableCollision() {
	//Removes all collision that would affect other unit pathfinding and hit registration
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
}

void ABaseUnit::Die_ClearTimers() {
	//Stops and timers that could be running at time of death
	GetWorld()->GetTimerManager().ClearTimer(CheckIfInRangeHandle);
	GetWorld()->GetTimerManager().ClearTimer(FailedMovementHandle);
	GetWorld()->GetTimerManager().ClearTimer(CheckForCombatHandle);
	GetWorld()->GetTimerManager().ClearTimer(AttackSpeedHandle);
}

void ABaseUnit::Die_Visuals_Implementation() {
	//Clientside Collision functionality (To stop potential jitters on clientside
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	//Blueprint Die method for purely visual clientside things like Animation
	BP_Die_Visuals();
}

void ABaseUnit::FinishAction() {
	check(HasAuthority())
	bAttacking = false;
	GetWorld()->GetTimerManager().ClearTimer(FailedMovementHandle);
	CurrentAction = FAction();
	CachedAttackAction = FAction();
	RecieveAction();
}

void ABaseUnit::FinishMovement(const FPathFollowingResult& Result) {
	if (HasAuthority()) {
		GetWorld()->GetTimerManager().ClearTimer(CheckIfInRangeHandle);
		GetWorld()->GetTimerManager().ClearTimer(CheckForCombatHandle);

		//Check for a faild move
		if (!Result.IsSuccess()) {
			if (FailedMovementCount > MAX_MOVEMENT_ACTIONS) {
				//If failure happens too many times delay whole AI system
				GetWorld()->GetTimerManager().SetTimer(FailedMovementHandle, this, &ABaseUnit::FinishAction, FailedMovementDelay, false);
			}
			else {
				FailedMovementCount++;
				PostMovementAction();
			}
		}
		else {
			FailedMovementCount = 0;
			PostMovementAction();
		}
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::SanitizeFloat(Result.Code));
	}
}

void ABaseUnit::PostMovementAction() {
	//Check for Cached action if this is a Range Move
	if (CachedAttackAction.Action_Type == "ATTACK") {
		check(HasAuthority())
		bAttacking = false;
		GetWorld()->GetTimerManager().ClearTimer(FailedMovementHandle);
		CurrentAction = CachedAttackAction;
		CachedAttackAction = FAction();
		RunAction();
	}
	else {
		FinishAction();
	}
}

bool ABaseUnit::DoneCurrentAction() {
	check(HasAuthority());
	if (CurrentAction.Action_Type == "") {
		return true;
	}
	return false;
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
