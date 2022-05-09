// Fill out your copyright notice in the Description page of Project Settings.


#include "AIQueue.h"
#include "Engine.h"

UAIQueue::UAIQueue() {}

bool UAIQueue::IsEmpty() {
	return Queue.Num() <= 0;
}

FAction UAIQueue::Peek() {
	if (!IsEmpty()) {
		return Queue[0];
	}
	return FAction();
}

void UAIQueue::Insert(FAction Action) {
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
		if (Parent.Priority < Action.Priority) {
			Queue[x] = Action;
			Queue[pos] = Parent;
			pos = x;
		}
		else {
			pos = 0;
		}
	}
}

FAction UAIQueue::DeleteMax() {
	FAction ReturnAction = Peek();
	if (!IsEmpty()) {
		Queue[0] = Queue[Queue.Num() - 1];
		Queue.RemoveAt(Queue.Num() - 1);
		int pos = 0;

		for (int i = 0; i < Queue.Num(); i++) {
			FAction temp = Queue[pos];
			int LeftChildPos = (pos * 2) + 1;
			int RightChildPos = (pos * 2) + 2;

			if (LeftChildPos < Queue.Num()) {
				if (Queue[pos].Priority < Queue[LeftChildPos].Priority) {
					Queue[pos] = Queue[LeftChildPos];
					Queue[LeftChildPos] = temp;
					pos = LeftChildPos;
				}
			}
			else if (RightChildPos < Queue.Num()) {
				if (Queue[pos].Priority < Queue[RightChildPos].Priority) {
					Queue[pos] = Queue[RightChildPos];
					Queue[RightChildPos] = temp;
					pos = RightChildPos;
				}
			}
		}
	}
	return ReturnAction;
}