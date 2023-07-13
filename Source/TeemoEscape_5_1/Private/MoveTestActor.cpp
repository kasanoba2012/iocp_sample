// Fill out your copyright notice in the Description page of Project Settings.


#include "MoveTestActor.h"

// Sets default values
AMoveTestActor::AMoveTestActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    CubeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CubeMesh"));
    RootComponent = CubeMesh;
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshAsset(TEXT("/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube"));
    if (CubeMeshAsset.Succeeded())
    {
        CubeMesh->SetStaticMesh(CubeMeshAsset.Object);
    }

    StartLocation = GetActorLocation();
    EndLocation = StartLocation + FVector(500.0f, 0.0f, 0.0f);
    Direction = FVector(1.0f, 0.0f, 0.0f);
    Distance = 500.0f;
    Speed = 100.0f;
    turn_sw = true;
}

// Called when the game starts or when spawned
void AMoveTestActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMoveTestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    // 왔다 갔다 하는 부분
    FVector CurrentLocation = GetActorLocation();
    FVector NewLocation = CurrentLocation + Speed * Direction * DeltaTime;
    SetActorLocation(NewLocation);

    float DistanceMoved = (NewLocation - StartLocation).Size();

    UE_LOG(LogTemp, Warning, TEXT("CurrentLocation : %s"), *CurrentLocation.ToString());
    if (DistanceMoved >= Distance)
    {
        //UE_LOG(LogTemp, Warning, TEXT("Log Message : %f NewLocation : %s"), DistanceMoved, *NewLocation.ToString());
        //UE_LOG(LogTemp, Warning, TEXT("Direction : %s"), *Direction.ToString());
        Direction *= -1.0f;
    }
}
