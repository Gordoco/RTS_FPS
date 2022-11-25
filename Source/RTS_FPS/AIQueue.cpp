// Fill out your copyright notice in the Description page of Project Settings.


#include "AIQueue.h"
#include "Engine.h"
#include "BaseUnit.h"
#include "MovementActionData.h"
#include "AttackActionData.h"

UAIQueue::UAIQueue() {}

void UAIQueue::Invalidate() {
	Empty();
	Owner = nullptr;
}

void UAIQueue::PrintNumActions() {
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::SanitizeFloat(Queue.Num()));
}

//O(1)
bool UAIQueue::IsEmpty() {
	return Queue.Num() <= 0;
}

//O(1)
FAction UAIQueue::Peek() {
	if (!IsEmpty()) {
		return Queue[0];
	}
	return FAction();
}

//O(n)
void UAIQueue::Empty() {
	for (FAction Action : Queue) {
		if (Action.Action_Type == "ATTACK") {
			ABaseUnit* OwningUnit = Cast<ABaseUnit>(Owner);
			UAttackActionData* Data = Cast<UAttackActionData>(Action.ActionData);
			if (Data != nullptr && OwningUnit != nullptr) {
				OwningUnit->RemoveEnemyFromList(Data->GetEnemy());
			}
		}
	}
	Queue.Empty();
}

//O(log(n))
void UAIQueue::Insert_NoCheck(FAction Action) {
	check(Owner != nullptr);
	Queue.Emplace(Action);
	int pos = Queue.Num() - 1;
	while (pos != 0) {
		FAction Parent;
		int x;
		if (pos % 2 == 0) {
			//RIGHT CHILD
			x = (pos - 2) / 2;
		}
		else {
			//LEFT CHILD
			x = (pos - 1) / 2;
		}
		Parent = Queue[x];
		//ADDING '=' MAKES WORST CASE RUNTIME MORE COMMON
		if (Parent.Priority </*=*/ Action.Priority) {
			Queue[x] = Action;
			Queue[pos] = Parent;
			pos = x;
		}
		else {
			pos = 0;
		}
	}
}

//O(log(n))
void UAIQueue::Insert(FAction Action) {
	Insert_NoCheck(Action);
	ABaseUnit* Unit = Cast<ABaseUnit>(Owner);
	if (Unit != nullptr) {
		Unit->CheckActions();
	}
}

//O(n)
FAction UAIQueue::DeleteMax() {
	//PrintNumActions();
	FAction ReturnAction = Peek();
	if (!IsEmpty()) {
		Queue[0] = Queue[Queue.Num() - 1];
		Queue.RemoveAt(Queue.Num() - 1);
		int pos = 0;

		for (int i = 0; i < Queue.Num(); i++) {
		//while (pos < Queue.Num() - 1) {
			FAction temp = Queue[pos];
			int LeftChildPos = (pos * 2) + 1;
			int RightChildPos = (pos * 2) + 2;

			if (LeftChildPos < Queue.Num()) {
				if (Queue[pos].Priority < Queue[LeftChildPos].Priority) {
					//SWAP WITH LEFT CHILD
					Queue[pos] = Queue[LeftChildPos];
					Queue[LeftChildPos] = temp;
					//CHECK LEFT CHILD
					pos = LeftChildPos;
				}
			}
			else if (RightChildPos < Queue.Num()) {
				if (Queue[pos].Priority < Queue[RightChildPos].Priority) {
					//SWAP WITH RIGHT CHILD
					Queue[pos] = Queue[RightChildPos];
					Queue[RightChildPos] = temp;
					//CHECK RIGHT CHILD
					pos = RightChildPos;
				}
			}
		}
	}
	return ReturnAction;
}