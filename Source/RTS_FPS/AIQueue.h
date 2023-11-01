// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ActionData.h"
#include "AIQueue.generated.h"

/**
* #### FAction
* ---
* Struct for storing information regarding a single action an NPC can take
* Used in the heap ADT of AIQueue
*/
USTRUCT(BlueprintType) struct FAction
{
	GENERATED_BODY()

public:

	/**
	* #### Action_Type
	* ALL CAPS identifier for each type of action NPCs can take
	* Should be converted into an ENUM
	*/
	UPROPERTY(BlueprintReadWrite, Category = "Actions")
		FString Action_Type;

	/**
	* #### Priority
	* Integer priority level for the action to allow sorting within the heap ADT of AIQueue
	*/
	UPROPERTY(BlueprintReadWrite, Category = "Actions")
		int Priority;

	/**
	* #### ActionData
	* Pointer to an instance of the UObject ActionData which holds all necessary action information
	*/
	UPROPERTY(BlueprintReadWrite, Category = "Actions")
		UActionData* ActionData;

	/**
	* #### FAction(UActionData* Data, int newPrio)
	* Initializer constructor for creating a new instance with values
	*/
	FAction(UActionData* Data, int newPrio)
	{
		Action_Type = Data->GetName();
		Priority = newPrio;
		ActionData = Data;
	}

	/**
	* #### FAction()
	* Default constructor implementation
	*/
	FAction()
	{
		Action_Type = "";
		Priority = 0;
		ActionData = nullptr;
	}

};


/**
 * #### AIQueue
 * ---
 * Abstract ADT using a heap implementation to preserve a loosly sorted list of actions
 * The highest priority action is always at the top of the heap but no assumptions can be made regarding the sorting of the rest of the data
 */
UCLASS()
class RTS_FPS_API UAIQueue : public UObject
{
	GENERATED_BODY()
public:
	/**
	* #### AIQueue(Default)
	* Default constructor implementation
	*/
	UAIQueue();

	/**
	* #### Invalidate()
	* Empties data structure and forgets owning character
	*/
	void Invalidate();

	/**
	* #### IsEmpty()
	* Boolean method to check if any actions are present in the data structure
	*/
	bool IsEmpty();

	/**
	* #### Peek()
	* Method which returns a copy of the top action
	* non-destructive
	*/
	FAction Peek();

	/**
	* #### DeleteMax()
	* Method which returns a copy of the top action and removes it from the ADT
	*/
	FAction DeleteMax();

	/**
	* #### Insert()
	* Inserts an action into the ADT and triggers a check on the owning character to ensure highest priority action is executed
	*/
	void Insert(FAction Action);

	/**
	* #### Insert_NoCheck()
	* Inserts an action into the ADT without triggering the check on the owning character to update their action
	*/
	void Insert_NoCheck(FAction Action);

	/**
	* #### Empty()
	* Empties the ADT of all data but doesn't clear the owner
	*/
	void Empty();

	/**
	* #### GetClosestAttackAction()
	* Gets the closest ATTACK action to the given location with the specified priority value
	*/
	FAction GetClosestAttackAction(FVector Location, int prio);

	/**
	* #### Debug_ListActions()
	* Debug method to visualize the current state of the characters action ADT
	*/
	FString Debug_ListActions();

	/**
	* #### SetOwner()
	* Initialization method to assign an owner to the constructed AIQueue
	*/
	void SetOwner(AActor* in) { Owner = in; }

	/**
	* #### PrintNumActions()
	* DebugMethod for visualizing the current size of the ADT
	*/
	UFUNCTION(BlueprintCallable, Category = "Debug")
		void PrintNumActions();

protected:
	/**
	* #### Queue
	* Internal array for ADT implementation
	*/
	UPROPERTY()
		TArray<FAction> Queue;

private:
	/**
	* #### Owner
	* Owning actor which contains an interface for using the actions in the AIQueue
	*/
	AActor* Owner;
};
