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
	FVector2D StartingGridPoint = FVector2D(GetActorLocation().X - SpawningRadius, GetActorLocation().Y + SpawningRadius);

	TArray<FVector2D> GridPoints;
	int NumPoints = (SpawningRadius*2) / MeshRadialDistance;
	for (int i = 0; i < NumPoints; i++) {
		for (int k = 0; k < NumPoints; k++) {
			FVector2D PotentialPoint = FVector2D(StartingGridPoint.X + (MeshRadialDistance * k), StartingGridPoint.Y + (MeshRadialDistance * i));
			if (FVector2D::Distance(PotentialPoint, FVector2D(GetActorLocation().X, GetActorLocation().Y)) <= SpawningRadius) {
				GridPoints.Add(PotentialPoint);
			}
		}
	}

	int limit;
	if (NumMeshes <= GridPoints.Num()) {
		limit = NumMeshes;
	}
	else {
		limit = GridPoints.Num();
	}
	for (int j = 0; j < limit; j++) {

		int index = FMath::RandRange(0, GridPoints.Num() - 1);

		float x = GridPoints[index].X;
		float y = GridPoints[index].Y;
		float z = GetActorLocation().Z;

		GridPoints.RemoveAt(index);

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