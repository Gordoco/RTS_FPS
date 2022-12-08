// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ActionData.h"
#include "AIQueue.generated.h"


USTRUCT(BlueprintType) struct FAction
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, Category = "Actions")
		FString Action_Type;

	UPROPERTY(BlueprintReadWrite, Category = "Actions")
		int Priority;

	UPROPERTY(BlueprintReadWrite, Category = "Actions")
		UActionData* ActionData;

	FAction(UActionData* Data, int newPrio) {
		Action_Type = Data->GetName();
		Priority = newPrio;
		ActionData = Data;
	}

	FAction() {
		Action_Type = "";
		Priority = 0;
		ActionData = nullptr;
	}

};


/**
 *
 */
UCLASS()
class RTS_FPS_API UAIQueue : public UObject
{
	GENERATED_BODY()
public:
	UAIQueue();

	void Invalidate();

	bool IsEmpty();

	FAction Peek();

	FAction DeleteMax();

	void Insert(FAction Action);

	void Insert_NoCheck(FAction Action);

	void Empty();

	FAction GetClosestAttackAction(FVector Location, int prio);

	FString Debug_ListActions();

	void SetOwner(AActor* in) { Owner = in; }

	//DEBUG
	UFUNCTION(BlueprintCallable, Category = "Debug")
		void PrintNumActions();

protected:
	UPROPERTY()
		TArray<FAction> Queue;

private:
	AActor* Owner;
};
