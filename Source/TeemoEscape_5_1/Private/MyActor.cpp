// Fill out your copyright notice in the Description page of Project Settings.


#include "MyActor.h"
#include "Enemy.h"
#include "Teemo.h"

// Sets default values
AMyActor::AMyActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMyActor::BeginPlay()
{
	Super::BeginPlay();
	
	UE_LOG(LogTemp, Warning, TEXT("AMyActor BeginPlay"));

	AteemoActor = GetWorld()->SpawnActor<ATeemo>(enemyFactory, FVector(-500, 300, -88), FRotator(0));
	EnemyActor = GetWorld()->SpawnActor<AEnemy>(AEnemy::StaticClass(), FVector(-1000, 300, 100), FRotator(0));
	//AEnemy* SpawnedActor = GetWorld()->SpawnActor<AEnemy>(FVector(-1000, 300, 300), FRotator(0));

	NewLocation = FVector(-1000, 300, 88); // Set the desired new location for the actor
	NewRotation = FRotator(0);

	if (EnemyActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Log SpawnCube"));
		FVector NewLocation2 = FVector(1000, 300, 300); // Set the desired new location for the actor
		FRotator NewRotation2 = FRotator(0);
		EnemyActor->SetActorLocationAndRotation(NewLocation2, NewRotation2);
	}

	if (AteemoActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Log SpawnCube"));
		FVector NewLocation2 = FVector(-500, 300, 300); // Set the desired new location for the actor
		FRotator NewRotation2 = FRotator(0);
		AteemoActor->SetActorLocationAndRotation(NewLocation2, NewRotation2);
	}
}

// Called every frame
void AMyActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UE_LOG(LogTemp, Warning, TEXT("AMyActor Tick"));
	NewLocation += FVector(1, 0, 0);

	if (AteemoActor)
	{
		AteemoActor->SetActorLocationAndRotation(NewLocation, NewRotation);
	}
}

