# **__Commanders and Commandos__** **__Documentation__**

## **Source Code Documentation**

### ActionData Class
-------



 Abstract Data Class for FAction

   Contains all relevent data for specified action

 


 #### internalName

 A searchable tag always in ALL CAPS for use within the AIQueue


	FString internalName = TEXT("DEFAULT"); Searchable Tag, Always ALL CAPS



 #### GetName()

 Accessor method for retrieving internalName


	FString GetName() { return internalName; }



 #### SetName()

 Mutator method for use within the hierarchy to set subclass internalNames


	void SetName(FString inName);

### AIQueue Class
-------



 #### FAction

 ---

 Struct for storing information regarding a single action an NPC can take

 Used in the heap ADT of AIQueue




 #### ActionType

 ALL CAPS identifier for each type of action NPCs can take

 Should be converted into an ENUM


	UPROPERTY(BlueprintReadWrite, Category = "Actions")

	FString ActionType;



 #### Priority

 Integer priority level for the action to allow sorting within the heap ADT of AIQueue


	UPROPERTY(BlueprintReadWrite, Category = "Actions")

	int Priority;



 #### ActionData

 Pointer to an instance of the UObject ActionData which holds all necessary action information


	UPROPERTY(BlueprintReadWrite, Category = "Actions")

	UActionData ActionData;



 #### FAction(UActionData Data, int newPrio)

 Initializer constructor for creating a new instance with values


	FAction(UActionData Data, int newPrio)

	ActionType = Data->GetName();

	Priority = newPrio;

	ActionData = Data;



 #### FAction()

 Default constructor implementation


	FAction()

	ActionType = "";

	Priority = 0;

	ActionData = nullptr;



  #### AIQueue

  ---

  Abstract ADT using a heap implementation to preserve a loosly sorted list of actions

  The highest priority action is always at the top of the heap but no assumptions can be made regarding the sorting of the rest of the data

 


 #### AIQueue(Default)

 Default constructor implementation




 #### Invalidate()

 Empties data structure and forgets owning character


	void Invalidate();



 #### IsEmpty()

 Boolean method to check if any actions are present in the data structure


	bool IsEmpty();



 #### Peek()

 Method which returns a copy of the top action

 non-destructive


	FAction Peek();



 #### DeleteMax()

 Method which returns a copy of the top action and removes it from the ADT


	FAction DeleteMax();



 #### Insert()

 Inserts an action into the ADT and triggers a check on the owning character to ensure highest priority action is executed


	void Insert(FAction Action);



 #### InsertNoCheck()

 Inserts an action into the ADT without triggering the check on the owning character to update their action


	void InsertNoCheck(FAction Action);



 #### Empty()

 Empties the ADT of all data but doesn't clear the owner


	void Empty();



 #### GetClosestAttackAction()

 Gets the closest ATTACK action to the given location with the specified priority value


	FAction GetClosestAttackAction(FVector Location, int prio);



 #### DebugListActions()

 Debug method to visualize the current state of the characters action ADT


	FString DebugListActions();



 #### SetOwner()

 Initialization method to assign an owner to the constructed AIQueue


	void SetOwner(AActor in) { Owner = in; }



 #### PrintNumActions()

 DebugMethod for visualizing the current size of the ADT


	UFUNCTION(BlueprintCallable, Category = "Debug")

	void PrintNumActions();



 #### Queue

 Internal array for ADT implementation


	UPROPERTY()

	TArray<FAction> Queue;



 #### Owner

 Owning actor which contains an interface for using the actions in the AIQueue


	AActor Owner;

### AttackActionData Class
-------



  Inherited ActionData class for attack actions and their additional required information

 


 #### UAttackActionData()

 Default constructor implementation




 #### SetEnemy()

 Mutator method for storing attack target pointer inside the object


	void SetEnemy(ABaseUnit inEnemy) { Enemy = inEnemy; }



 #### GetEnemy()

 Accessor method for internal Enemy pointer


	ABaseUnit GetEnemy() { return Enemy; }



 #### Enemy

 Internal BaseUnit pointer


	UPROPERTY()

	ABaseUnit Enemy;

### BaseBrain Class
-------



  Actor component meant to store AI interface with AIQueue ADT

 


 #### SetOwner()

 Sets the owning Character for the brain


	void SetOwner(ACharacter inOwner);



 FinishedCycle()

 


	virtual void FinishedCycle();

	UPROPERTY(BlueprintReadOnly, Category = "Team")

	int Team = 0;

	ABaseUnitController AIController;

	ACharacter Owner;

	UFUNCTION(BlueprintPure, Category = "Player")

	ACharacter GetOwner() { return Owner; }

	UFUNCTION(BlueprintImplementableEvent, Category = "Init")

	void BPStartBrain();

	virtual void StartBrain();

### BaseBuilding Class
-------

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	UStaticMesh GetFinalMesh() { return FinalMesh; }

	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere, Category = "Player")

	AActor OwningPlayerPawn;

	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Components")

	UStaticMeshComponent Mesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Components")

	UStaticMesh FinalMesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Components")

	TArray<UStaticMesh> ConstructionMeshes;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Components")

	float BuildSpeed = 1.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Components")

	float BuildIncrement = 1.f;

	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere, Category = "Stats")

	int Team = 0;

	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere, Category = "Stats")

	bool bFinishedConstruction = false;

	UPROPERTY(ReplicatedUsing = ChangeMesh)

	UStaticMesh CurrentMesh;

	UFUNCTION()

	void ChangeMesh();

	FTimerHandle ConstructionHandle;

	void ConstructionIterator();

	float BuildProgress = 0.f;

	int BuildStage = 0;

	virtual void Tick(float DeltaTime) override;

	virtual void Selected();

	virtual void Deselected();

	UFUNCTION(BlueprintPure, Category = "Stats")

	int GetTeam() { return Team; }

	void SetTeam(int inTeam) { Team = inTeam; }

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Stats")

	float BuildingCostMetal = 0.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Stats")

	float BuildingCostEnergy = 0.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Stats")

	float BuildingZOffset = 0.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Template")

	TSubclassOf<ATemplateBuilding> TemplateClass;

	UFUNCTION(BlueprintPure, Category = "Construction")

	bool IsConstructed() { return bFinishedConstruction; }

	void BeginConstruction();

### BaseProductionBuilding Class
-------

	class ABaseUnit;

	int TrainingTime;

	ABaseUnit TrainedUnit;

	TrainingTime = inTime;

	TrainedUnit = inUnit;

	TrainingTime = 0.f;

	TrainedUnit = nullptr;



  

 
	UPROPERTY(ReplicatedUsing = "SetRallyPointPosition")

	FVector RallyPoint;

	UFUNCTION()

	void SetRallyPointPosition();

	UPROPERTY(Replicated)

	TArray<FUnitProduction> ProductionQueue;

	FUnitProduction Pop();

	FUnitProduction Peek();

	void Push(ABaseUnit Unit, int time);

	UPROPERTY()

	UBaseProductionWidget TrainingWidgetRef = nullptr;

	virtual void Selected() override;

	virtual void Deselected() override;

	void ServerSetRallyPoint(FVector Location);

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "Training")

	void RecruitUnit(TSubclassOf<ABaseUnit> UnitType);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Training")

	void ServerRecruitUnit(TSubclassOf<ABaseUnit> UnitType);

	UFUNCTION(Server, Unreliable, WithValidation, BlueprintCallable, Category = "Training")

	void CancelTraining();

	UFUNCTION(BlueprintPure, Category = "Training")

	float GetTrainingProgress();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintPure, Category = "Rally")

	FVector GetRallyPoint() { return RallyPoint; }

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rally")

	UPaperSpriteComponent RallySprite;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Training")

	UPaperSpriteComponent SpawnLocationSprite;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Training")

	TSubclassOf<UBaseProductionWidget> WidgetClass;

	FTimerHandle TrainingHandle;

	UPROPERTY(Replicated)

	bool bTraining = false;

	UPROPERTY(Replicated)

	int TrainingCount = 0;

	UPROPERTY(Replicated)

	int CurrTrainingTime = 0;

	UFUNCTION()

	void UpdateTrainingProgress();

	void StartTraining();

### BaseProductionWidget Class
-------

	class ABaseProductionBuilding;



  

 
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Building")

	ABaseProductionBuilding OwningBuilding;

	UFUNCTION(BlueprintPure, Category = "Training")

	FUnitInfo GetUnitInfo(TSubclassOf<ABaseUnit> UnitType);

### BaseResource Class
-------

	FTransform InstanceTransform;

	int InstanceIndex;

	UENUM()

	FTransform GetGatherLocation(FVector UnitLocation);

	float GetGatherAmount() { return GatherAmount; }

	EResourceType GetResourceType() { return Type; }

	TArray<FInstance> Instances;

	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Resources")

	float GatherAmount = 10.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Meshes")

	int NumMeshes = 10;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Meshes")

	float MeshRadialDistance = 100.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Meshes")

	float SpawningRadius = 1000.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Meshes")

	UInstancedStaticMeshComponent MasterMesh;

	UFUNCTION(CallInEditor, Category = "Generation")

	void GenerateMeshes();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")

	TEnumAsByte<EResourceType> Type;

	virtual void Tick(float DeltaTime) override;

### BaseResourceDropOff Class
-------



 

 
	virtual void BeginPlay() override;

	void DropOffResources(EResourceType Type, float Resources);

	FVector GetDropOffLocation();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Location")

	UBoxComponent DropOffLocation;

### BaseResourceUnit Class
-------



  

 
	virtual void FinishMovement(const FPathFollowingResult& Result) override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintPure, Category = "Resources")

	float GetResources() { return InternalResourceStorage; }

	virtual void InitCheckForCombat() override;

	virtual void CheckForCombatIterator() override;

	virtual void AddAttackAction(ABaseUnit Enemy, int prio) override;

	virtual void RunAction() override;

	virtual void Die() override;

	void GatherActionHandler();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Resources", Replicated)

	float MaxResources = 100.f;

	UFUNCTION(BlueprintImplementableEvent, Category = "Resources")

	void BPUpdateResourceCounter();

	ABaseResourceDropOff GetClosestDropOffPoint();

	ABaseResourceDropOff CurrDropOff;

	UPROPERTY(Replicated)

	float InternalResourceStorage = 0.f;

	UPROPERTY(Replicated)

	TEnumAsByte<EResourceType> TypeCarried = ERTNone;

	FTimerHandle GatherHandle;

	UFUNCTION()

	void GatherIterator();

	bool bMovingToGather = false;

	bool bMovingToDropOff = false;

	FRotator GatherRotation;

	ABaseResource CurrResource;

	UFUNCTION(BlueprintCallable, Category = "Actions")

	void AddGatherAction(ABaseResource Resource, int prio);

### BaseUnit Class
-------

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Stats")

	int TrainingTime;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Stats")

	int TrainingCostEnergy;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Stats")

	int TrainingCostMetal;

	TrainingTime = 0;

	TrainingCostEnergy = 0;

	TrainingCostMetal = 0;

	TrainingTime = time;

	TrainingCostEnergy = energy;

	TrainingCostMetal = metal;

	void SearchForEnemies(int prio);

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "Damage")

	void DealDamage(float inDamage);

	static const int UNITRESPONSEPRIORITY = 10;

	static const int UNITORDEREDPRIORITY = 20;

	static const int UNITSMARTORDEREDPRIORITY = 8;

	const FVector NULLVECTOR = FVector(-9999, -9999, -9999);

	const float UNITSHUFFLEDISTANCE = 5.f;





int FailedMovementCount = 0;



UFUNCTION(BlueprintCallable, Category = "Stats")

void Kill() { DealDamage(MaxHealth); }



float GetVisionRange() { return VisionRange; }



Keeps an up-to-date list of all enemys with an active attack action

TArray<ABaseUnit> EnemyList;



bool RemoveEnemyFromList(ABaseUnit Enemy);



void EmptyQue();



bool TEST;



UFUNCTION(BlueprintImplementableEvent, Category = "SERVERDebug")

void BPSwappedActions();



UFUNCTION(BlueprintPure, Category = "Stats")

float GetHealth() { return Health; }



UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Team", Replicated, Meta = (ExposeOnSpawn = "true"))

int Team = 0;



UFUNCTION(BlueprintCallable, Category = "Actions")

void AddMovementAction(FVector Location, int prio, float inAcceptableRadius = 1.f);



bool VerifyMoveLocation(FVector Location, int prio, float inAcceptableRadius = 1.f);



void AddMovementActionHelper(UMovementActionData Data, FVector Location, int prio, float inAcceptableRadius);



UFUNCTION(BlueprintCallable, Category = "Actions")

virtual void AddAttackAction(ABaseUnit Enemy, int prio);



Should be called on Server

UFUNCTION(BlueprintCallable, Category = "Stats")

bool IsDead() { return Health <= 0; }



UFUNCTION(NetMulticast, Unreliable)

void DieVisuals();



UFUNCTION(BlueprintImplementableEvent, Category = "Visuals")

void BPDieVisuals();



FTimerHandle PostDeathCleanupHandle;



float PostDeathCleanupTime = 10.f;



FTimerHandle FailedMovementHandle;



FTimerHandle CheckIfInRangeHandle;



FTimerDelegate CheckIfInRangeDelegate;



UFUNCTION()

void CheckIfInRangeIterator(ABaseUnit Enemy);



float FailedMovementDelay = 0.5f;



UPROPERTY(Replicated, BlueprintReadOnly, Category = "Attacking")

bool bAttacking = false;



void PostDeathCleanup() { Destroy(); }



 Called every frame

virtual void Tick(float DeltaTime) override;



void Selected();



void Deselected();



int GetTeam() { return Team; }



 Called to bind functionality to input

virtual void SetupPlayerInputComponent(class UInputComponent PlayerInputComponent) override;



virtual void AddAction(FAction Action);



virtual void FinishMovement(const FPathFollowingResult& Result);



bool OutOfActions() { return ActionQue->IsEmpty(); }



bool DoneCurrentAction();



void CheckActions();



UFUNCTION(BlueprintPure, Category = "Brain")

UBaseBrain GetBrain() { return Brain; }



int GetTrainingTime() { return TrainingInfo.TrainingTime; }



int GetTrainingCostEnergy() { return TrainingInfo.TrainingCostEnergy; }



int GetTrainingCostMetal() { return TrainingInfo.TrainingCostMetal; }



FUnitInfo GetTrainingInfo() { return TrainingInfo; }



void SetLightweightSpawn() { bLightweightSpawn = true; }

protected:

 Called when the game starts or when spawned

virtual void BeginPlay() override;



void BeginPlayUnits();



virtual void RunAction();



void AllocateActionWindow();



FTimerHandle ActionHandle;



const float ACTIONTIMEFRAME = .2f;



bool bFinishedAction = true;



UFUNCTION()

void CheckAction();



UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Selection")

UPaperSpriteComponent SelectionSprite;



virtual void InitCheckForCombat();



virtual void CheckForCombatIterator();



void CheckForCombatHelper(ABaseUnit PotentialEnemy);



FTimerHandle CheckForCombatHandle;



float CheckForCombatFactor = 0.1;



UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Brain")

TSubclassOf<UBaseBrain> BrainClass = UBaseBrain::StaticClass();



 Unit Stats (NEED REPLICATION, SERVERSIDE MANAGMENT, ETC.
	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere, Category = "Stats")

	float MaxHealth = 100.f;

	UPROPERTY(ReplicatedUsing = "OnRepCheckForDeath")

	float Health = MaxHealth;

	UFUNCTION()

	void OnRepCheckForDeath();

	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere, Category = "Stats")

	float Damage = 1.f;

	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere, Category = "Stats")

	float AttackSpeed = 1.f;

	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere, Category = "Stats")

	float AttackRange = 500.f;

	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere, Category = "Stats")

	float VisionRange = 1000.f;

	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere, Category = "Stats")

	float StopRange = 2000.f;

	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere, Category = "Stats")

	FUnitInfo TrainingInfo = FUnitInfo();

	UPROPERTY(BlueprintReadOnly, Category = "SERVERActions")

	FAction CurrentAction = FAction();

	void FinishAction();

	virtual void Die();

	static const int MAXMOVEMENTACTIONS = 10;

	bool bLightweightSpawn = false;

	FAction CachedAttackAction = FAction();

	UPROPERTY()

	UAIQueue ActionQue;

	UPROPERTY()

	UBaseBrain Brain;

	void RecieveAction();

	void CheckNewActionPrio();

	void PostMovementAction();

	void DebugUnknownCommand(FString CommandTag);

	void DebugActionCastError();

	UPROPERTY()

	int DebugCount = 0;

	FTimerHandle AttackSpeedHandle;

	FVector LastMoveLocation;

	UFUNCTION()

	void FinishRemoveEnemy(FAction Action);

	void MovementActionHandler();

	void AttackActionHandler();

	bool CheckLineOfSight(AAIController AIController, ABaseUnit Enemy);

	void AttackReset();

	bool bReadyToAttack = true;

	bool CheckIfInRange(FVector EnemyLocation);

	bool CheckFullCircle(float NewAngle, float OriginalAngle, float Interval);

	FVector CalculateLocationInRange(FVector EnemyLocation, ABaseUnit Enemy);

	FVector RecCalculateLocationInRange(FVector EnemyLocation, ABaseUnit Enemy, float Angle, float Interval, bool bLeft, float OriginalAngle);

	bool ValidateLocationInRange(ABaseUnit Enemy, FVector Final, UNavigationSystemV1 NavSys, const FNavAgentProperties& AgentProps, FNavLocation ProjectedLocation, AAIController AIController, FHitResult Hit);

	void MakeAttack(ABaseUnit Enemy, float inDamage);

	void DieInvalidateAndDestroyAI();

	void DieDisableCollision();

	void DieClearTimers();

	bool Dead = false;

### BaseUnitController Class
-------



 

 
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

### BasicPatrolBrain Class
-------



 

 
	virtual void FinishedCycle() override;

	virtual void StartBrain() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Patrol")

	TArray<FVector> PatrolPoints;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Patrol")

	int PatrolPrio = 0;

	void AddPatrolActions();

### FPSCharacter Class
-------

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	void Init();

	UFUNCTION(Client, Unreliable)

	void SpawnOrderMarker(FVector Location, ABaseUnit Enemy = nullptr);

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")

	void CreateOrderWidget(FVector Location, ABaseUnit Enemy);

	UFUNCTION(BlueprintPure, Category = "Firing")

	FHitResult GetShotHit();

	FVector SpreadHitTransform(FVector IdealHit);

	UFUNCTION(BlueprintCallable, Category = "Firing")

	ABaseUnit ValidateHit(AActor HitActor);

	UFUNCTION(Server, WithValidation, Unreliable, BlueprintCallable, Category = "Firing")

	void ServerDamageEnemy(ABaseUnit Enemy);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Firing")

	UBoxComponent FiringLocation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Firing")

	float SpreadVal = 1.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Components")

	UCameraComponent FPSCamera;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Components")

	UCameraComponent ADSFPSCamera;

	UPROPERTY(BlueprintReadOnly, Category = "Components")

	UCameraComponent ActiveCam;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement")

	float BaseMovementSpeed = 600.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement")

	float SprintMovementSpeed = 1000.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement")

	float SlideDist = 10000.f;

	virtual void Die();

	virtual void InitCheckForCombat() override;

	virtual void CheckForCombatIterator() override;

	UFUNCTION(Server, WithValidation, Reliable)

	void ServerKillPlayer();

	float firstShotTimer = 0.5f;

	float firstShotTimerMax = 0.5f;

	UFUNCTION()

	void ADS();

	UFUNCTION()

	void StopADS();

	UFUNCTION()

	void StartSprint();

	UFUNCTION()

	void StopSprint();

	UFUNCTION()

	void Slide();

	UFUNCTION(Server, Reliable, WithValidation)

	void SlideServer();

	UFUNCTION(Client, Reliable, WithValidation)

	void SlideClient();

	FTimerHandle SlideHandle;

	FVector CurrSlideDir;

	float slideCount = 0.f;

	void SlideIterator();

	UFUNCTION(Server, Reliable, WithValidation)

	void UpdateSpeed(float newSpeed);

	bool bSprinting = false;

	bool bSliding = false;

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent PlayerInputComponent) override;

### GatherActionData Class
-------



  

 
	void SetResource(ABaseResource in) { Resource = in; }

	ABaseResource GetResource() { return Resource; }

	UPROPERTY()

	ABaseResource Resource;

### LobbyManager Class
-------

	USTRUCT(BlueprintType)

	MapName = inName;

	MaxPlayers = inPlayers;

	FString MapName = "";

	int MaxPlayers = -1;

	FGameMap GetMapInfo(FString inName, int index);

	TArray<FGameMap> Maps;

	void PopulateMapStruct();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

### MovementActionData Class
-------



 

 
	void SetLocation(FVector inLocation) { LocationToMove = inLocation; }

	FVector GetLocation() { return LocationToMove; }

	FAction OwningAttack;

	float AcceptableRadius = 1.f;

	FVector LocationToMove = FVector(0.f, 0.f, 0.f);

### RTSBrain Class
-------



  

 
	virtual void StartBrain() override;

	virtual void FinishedCycle() override;

### RTSPawn Class
-------

	void DebugCallsMovement(FVector2D Screen, FVector2D Mouse, float val1, float val2, float Margin);

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	void Init();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")

	void CreateHUD();

	UFUNCTION(BlueprintPure, Category = "Units")

	bool HasBuilderSelected();

	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere, Category = "Team")

	int Team = 0;

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent PlayerInputComponent) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Input")

	float MovementSpeed = 3000.f;

	UFUNCTION(BlueprintCallable, Category = "Buildings")

	bool AttemptToBuild(TSubclassOf<ABaseBuilding> BuildingClass);

	void ShouldTakeInput(bool bInput) { bShouldMove = bInput; }

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Resources")

	void AddResources(EResourceType Type, float AddVal);

	UFUNCTION(BlueprintPure, Category = "Resources")

	float GetEnergy() { return Energy; }

	UFUNCTION(BlueprintPure, Category = "Resources")

	float GetMetal() { return Metal; }

	UPROPERTY TO AVOID GARBAGE COLLECTION. SHOULD NEVER BE REFERENCED OUTSIDE OF DRAWSELECTIONBOX()

	UPROPERTY()

	UBoxComponent SelectionBox;

	FVector StartLocation;

	FVector CachedMovementDirection = FVector();

	UPROPERTY(Replicated)

	float Energy = 0.f;

	UPROPERTY(Replicated)

	float Metal = 200.f;

	UPROPERTY(ReplicatedUsing = OnRepSetLocation)

	FVector PlayerLocation;

	UFUNCTION()

	void OnRepSetLocation();

	UFUNCTION()

	void OnRepCurrentTemplateClass();

	UFUNCTION(Server, WithValidation, Unreliable)

	void SetMyLocation(FVector Location);

	UFUNCTION(Server, WithValidation, Unreliable)

	void ServerCreateBuilding(TSubclassOf<ABaseBuilding> BuildingClass);

	UFUNCTION(Client, Unreliable)

	void CreateTemplateBuilding();

	void PlayerClick();

	bool bShouldMove = true;

	bool CheckPlacingBuilding() { return CurrentTemplate != nullptr && CurrentTemplate->bReadyToPlace; }

	void DrawSelectionBox(FHitResult Hit);

	void ReleaseLeftClick();

	UFUNCTION(Server, WithValidation, Unreliable)

	void PShift();

	UFUNCTION(Server, WithValidation, Unreliable)

	void RShift();

	UPROPERTY(Replicated)

	bool bShiftPressed = false;

	void EvaluateHitUnit(ABaseUnit HitUnit);

	void EvaluateHitBuilding(ABaseBuilding HitBuilding);

	UPROPERTY(Replicated)

	TArray<ABaseUnit> SelectedUnits;

	UPROPERTY(Replicated)

	TArray<ABaseBuilding> SelectedBuildings;

	UFUNCTION(Server, Unreliable, WithValidation)

	void ServerFinalizeBuildingPlacement(FTransform Transform);

	UFUNCTION(NetMulticast, Unreliable)

	void UpdateOwnership(ABaseBuilding BuildingRef);

	void PlayerRightClick();

	void DeselectAllUnits();

	UFUNCTION(BlueprintCallable, Category = "Units")

	bool SelectUnit(ABaseUnit Unit);

	UFUNCTION(Server, WithValidation, Reliable)

	void OrderUnits(FHitResult Hit);

	UFUNCTION(Server, WithValidation, Reliable)

	void OrderBuildings(FHitResult Hit);

	UFUNCTION(Server, Reliable, WithValidation)

	void ServerSelectUnit(ABaseUnit Unit);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Teleportation")

	void ServerTeleport(FVector Location);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Orders")

	void ServerOrderMovement(FVector LocationToMove);

	UFUNCTION(Server, Reliable, WithValidation)

	void DeselectUnit(ABaseUnit Unit);

	UFUNCTION(Server, Reliable, WithValidation)

	void SelectBuilding(ABaseBuilding Building);

	UFUNCTION(Server, Reliable, WithValidation)

	void DeselectBuilding(ABaseBuilding Building);

	UFUNCTION(Server, Reliable, WithValidation)

	void DeselectAll();

	void OrderMovement(FVector LocationToMove);

	void Swap(int One, int Two, TArray<ABaseUnit> Arr);

	void OrderAttack(ABaseUnit EnemyUnit);

	void OrderGather(ABaseResource Resource);

	APlayerController GetPC();

	FVector MovementDirection = FVector::ZeroVector;

	FVector2D GetScreenSize();

	FVector2D GetMousePosition();

	void CalculateMovement();

	ABaseBuilding ServerCurrentBuilding;

	UPROPERTY(ReplicatedUsing = OnRepCurrentTemplateClass)

	TSubclassOf<ATemplateBuilding> CurrentTemplateClass;

	UPROPERTY(Replicated)

	UStaticMesh BuildingMesh;

	UPROPERTY()

	ATemplateBuilding CurrentTemplate;

### RTSPlayerController Class
-------



  

 
	virtual void BeginPlay() override;

	UFUNCTION()

	void OnWindowFocusChanged(bool isFocused);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Classes")

	TSubclassOf<AFPSCharacter> CommandoClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Classes")

	TSubclassOf<ARTSPawn> CommanderClass;

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere, Category = "Init")

	int Team;

	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere, Category = "Init")

	int MatchGameplayType;

	UFUNCTION(Server, WithValidation, Reliable)

	void RetrieveMatchData();

	UFUNCTION()

	void ClientFinishInitLobby();

	UPROPERTY(ReplicatedUsing = "ClientFinishInitLobby")

	TArray<FMatchRequest> MatchRequests;

	void InitializeBuildings();

	UFUNCTION(BlueprintCallable, Category = "Lobby")

	void InitLobby();

	void InitPC(int inMatchGameplayType, int inTeam);

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Ready")

	bool bReady = false;

	bool bLoaded = false;

	void StartMatch();

	UFUNCTION(BlueprintPure, Category = "Init")

	int GetTeam() { return Team; }

	UFUNCTION(BlueprintPure, Category = "Init")

	int GetMatchGameplayType() { return MatchGameplayType; }

	UFUNCTION(BlueprintCallable, Category = "Replication")

	int ShouldStartGame();

	UFUNCTION(Server, WithValidation, Reliable, BlueprintCallable, Category = "Init")

	void JoinTeamAtPosition(int inTeam, int inMatchGameplayType);

	UFUNCTION(BlueprintCallable, Category = "Init")

	void SpawnControlledPawn();

	UFUNCTION(BlueprintImplementableEvent, Category = "Init")

	void CreatePlayerHUDs();

	UFUNCTION(BlueprintImplementableEvent, Category = "Spawning")

	void MovePawnsToPlayerStarts(APawn inPawn);

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")

	void CreatePlayerWidget(int inMatchGameplayType, int inTeam, const FString& PlayerName);

### RTS_FPS Class
-------

### RTS_FPSGameInstance Class
-------



  

 
	URTSFPSGameInstance(const FObjectInitializer& ObjectInitializer);

	virtual void Shutdown() override;

	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;

	FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;

 Handles to registered delegates for creatingstarting a session 

FDelegateHandle OnCreateSessionCompleteDelegateHandle;

FDelegateHandle OnStartSessionCompleteDelegateHandle;



TSharedPtr<class FOnlineSessionSettings> SessionSettings;



bool HostSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers);





Function fired when a session create request has completed



@param SessionName the name of the session this callback is for

@param bWasSuccessful true if the async action completed without error, false if there was an error


	virtual void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);



Function fired when a session start request has completed



@param SessionName the name of the session this callback is for

@param bWasSuccessful true if the async action completed without error, false if there was an error


	void OnStartOnlineGameComplete(FName SessionName, bool bWasSuccessful);



Find an online session



@param UserId user that initiated the request

@param bIsLAN are we searching LAN matches

@param bIsPresence are we searching presence sessions


	void FindSessions(TSharedPtr<const FUniqueNetId> UserId, bool bIsLAN, bool bIsPresence);

 Delegate for searching for sessions 

FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;



 Handle to registered delegate for searching a sessio
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;

	TSharedPtr<class FOnlineSessionSearch> SessionSearch;



Delegate fired when a session search query has completed



@param bWasSuccessful true if the async action completed without error, false if there was an error


	void OnFindSessionsComplete(bool bWasSuccessful);



Joins a session via a search result



@param SessionName name of session

@param SearchResult Session to join



@return bool true if successful, false otherwise


	bool RTSFPSJoinSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, const FOnlineSessionSearchResult& SearchResult);

 Delegate for joining a session 

FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;



 Handle to registered delegate for joining a sessio
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;



Delegate fired when a session join request has completed



@param SessionName the name of the session this callback is for

@param bWasSuccessful true if the async action completed without error, false if there was an error


	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

 Delegate for destroying a session 

FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;



 Handle to registered delegate for destroying a sessio
	FDelegateHandle OnDestroySessionCompleteDelegateHandle;



Delegate fired when a destroying an online session has completed



@param SessionName the name of the session this callback is for

@param bWasSuccessful true if the async action completed without error, false if there was an error


	virtual void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

	UFUNCTION(BlueprintCallable, Category = "EndOfGame")

	void ClearUnitTracker();

	UFUNCTION(BlueprintCallable, Category = "EndOfGame")

	void ActivateUnitTracker();

	UFUNCTION(BlueprintCallable, Category = "Network|Test")

	void StartOnlineGame(bool bLAN);

	UFUNCTION(BlueprintCallable, Category = "Network|Test")

	void FindOnlineGames(bool bLAN);

	UFUNCTION(BlueprintCallable, Category = "Network|Test")

	void JoinOnlineGame(int ID);

	UFUNCTION(BlueprintCallable, Category = "Network|Test")

	void DestroySessionAndLeaveGame();

	UFUNCTION(BlueprintImplementableEvent, Category = "TESTING")

	void JoinToServer();

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")

	void CreateSearchResultWidget(int ID, const FString& UserName);

### RTS_FPSGameModeBase Class
-------

	class ARTSPlayerController;



  

 
	int Team;

	int MatchGameplayType;

	FString PlayerName;

	Team = -1;

	MatchGameplayType = -1;

	PlayerName = "";

	Team = inTeam;

	MatchGameplayType = inMatchGameplayType;

	PlayerName = inPlayerName;

	APlayerController Owner;

	int MatchData = 0;

	int Team = 0;

	Owner = inO;

	MatchData = inM;

	Team = inT;

	bool bTeam1Commander = false;

	bool bTeam2Commander = false;

	int Team1Num = 0;

	int Team2Num = 0;

	FRoles Roles = FRoles();

	virtual void PostSeamlessTravel() override;

	virtual void SwapPlayerControllers(APlayerController OldPC, APlayerController NewPC) override;

	FTimerHandle CheckForPlayersLoadedHandle;

	FTimerHandle TimeoutHandle;

	void CheckForPlayersLoaded();

	bool bStarted = false;

	void StartGame();

	void TimeoutGame();

	TArray<FPCData> Data;

	TArray<AController> OldControllerList;

	TArray<FMatchRequest> CurrRequests;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Multiplayer")

	float TimeoutTime = 15;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Multiplayer")

	float CheckInterval = 1.f;

	Public Server-Side Function to Launch a Match (ONLY USABLE POST LOBBY CREATION)

	UFUNCTION(BlueprintCallable, Category = "Match")

	bool CreateMatch(FString MapName, int NumPlayers, bool bListen);

	UFUNCTION(BlueprintPure, Category = "Match")

	bool GetStarted() { return bStarted; }

	bool RequestMatchPosition(FMatchRequest inRequest, APlayerController RequestingPC);

	TArray<FMatchRequest> GetMatchRequests() { return CurrRequests; }

### RTS_FPSObjectPool Class
-------



  

 
	void Initialize(int numObjects, TSubclassOf<AActor> objectClass);

	AActor Pool;

	int count;

### TemplateBuilding Class
-------

	void SetMesh(UStaticMesh inMesh) { Mesh->SetStaticMesh(inMesh); }

	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Components")

	UStaticMeshComponent Mesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Components")

	UMaterial PlacementMat;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Components")

	UMaterial ErrorMat;

	UFUNCTION()

	const FHitResult& SweepResult);

	UFUNCTION()

	int32 OtherBodyIndex);

	bool bFollowMouse = false;

	FVector CalculateMouseFollowLocation();

	virtual void Tick(float DeltaTime) override;

	APlayerController PC;

	void SetFollowMouse(APlayerController inController);

	bool bReadyToPlace = true;

### UnitTracker Class
-------



 #### FTeam Data

 Workaround for 2D arrays in Unreal Engine 5

 Stores an array of BaseUnit pointers which should be on the same team


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Unit")

	TArray<ABaseUnit> Units;



 #### Unit Tracker

 UObject derived static class responsible for keeping an accurate server-side list of BaseUnit instances

 for both internal and external use




 #### Teams

 2D array storing all units from all teams


	inline static TArray<FTeamData> Teams;



 #### bActive

 Tracker for when the game has loaded and the static UnitTracker should begin function


	inline static bool bActive = true;



 #### Empty()

 Clears all stored units and refreshes UnitTracker


	static void Empty() { Teams.Empty(); }



 #### RegisterUnit()

 Adds a unit to corresponding FTeamData for use in tracking


	static void RegisterUnit(ABaseUnit Unit, int Team);



 #### DeregisterUnit()

 Removes a unit from its corresponding FTeamData

 Usually used on unit death


	static void DeregisterUnit(ABaseUnit Unit, int Team);



 #### GetNumTeams()

 Accessor method to return number of initialized teams


	static int GetNumTeams() { return Teams.Num(); }



 #### CheckForValidity()

 Checks that the index passed belongs to a valid and initialized team with at least 1 member


	static bool CheckForValidity(int index);



 #### GetAllTeamData()

 Accessor method to retrieve the entire set of currently tracked units


	static TArray<FTeamData> GetAllTeamData() { return UUnitTracker::Teams; }



 #### GetTeamData()

 Accessor method to retrieve the set of currently tracked units belonging to a specified team


	static FTeamData GetTeamData(int Team) { return UUnitTracker::Teams[Team]; }



 #### GetUnitsInRange()

 Accessor method to retrieve all units within a specified distance of a position in world space

 TO DO: Make sorted from closest to furthest, currently will present the closest first with no ordering after that


	static TArray<ABaseUnit> GetUnitsInRange(int Team, float inRange, FVector Location);



 #### GetClosestUnit()

 Accessor method to retrieve the single closest unit to a location in world space


	static ABaseUnit GetClosestUnit(int Team, FVector Location);

