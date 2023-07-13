// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MoveTestActor.generated.h"

UCLASS()
class TEEMOESCAPE_5_1_API AMoveTestActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMoveTestActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* CubeMesh;

	FVector StartLocation;
	FVector EndLocation;
	FVector Direction;
	float Distance;
	float Speed;
	bool turn_sw;
};
