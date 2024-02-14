// Fill out your copyright notice in the Description page of Project Settings.


#include "RTS_FPSObjectPool.h"

URTS_FPSObjectPool::URTS_FPSObjectPool() {

}

void URTS_FPSObjectPool::Initialize(int numObjects, TSubclassOf<AActor> objectClass) {
	Pool = new AActor*[numObjects];
	for (int i = 0; i < numObjects; i++) {
		Pool[i] = GetWorld()->SpawnActor<AActor>(objectClass, FTransform::Identity);
		//Pool[i]->SetActive(false);
	}
	count = numObjects;
}
