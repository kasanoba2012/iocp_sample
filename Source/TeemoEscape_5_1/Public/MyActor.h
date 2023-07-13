// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyActor.generated.h"

UCLASS()
class TEEMOESCAPE_5_1_API AMyActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyActor();

	// Teemo 타입의 블루프린트 할당 받을 변수
	UPROPERTY(EditAnywhere, Category = SpawnSettings)
	TSubclassOf<class ATeemo> enemyFactory;

	class AEnemy* EnemyActor;
	class ATeemo* AteemoActor;

	FVector NewLocation;
	FRotator NewRotation;

	bool in_game_sw = true;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
