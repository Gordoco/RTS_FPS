// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseResource.h"
#include "MathUtil.h"

// Sets default values
ABaseResource::ABaseResource()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MasterMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Master"));
	RootComponent = MasterMesh;
	GenerateMeshes();

}

void ABaseResource::GenerateMeshes() {
	MasterMesh->ClearInstances();
	for (int j = 0; j < NumMeshes; j++) {
		FVector2D RandomPointInRadius = FMath::RandPointInCircle(SpawningRadius);
		float x = RandomPointInRadius.X;
		float y = RandomPointInRadius.Y;
		float z = GetActorLocation().Z;

		MasterMesh->AddInstance(FTransform(FVector(x, y, z)));
	}
}

// Called when the game starts or when spawned
void ABaseResource::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABaseResource::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}