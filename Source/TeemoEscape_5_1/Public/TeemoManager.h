// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TeemoManager.generated.h"

UCLASS()
class TEEMOESCAPE_5_1_API ATeemoManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATeemoManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	// Teemo 타입의 블루프린트 할당 받을 변수
	UPROPERTY(EditAnywhere, Category = SpawnSettings)
	TSubclassOf<class ATeemo> teemoFactory;

	class ATeemo* AteemoActor[5];

	FVector NewLocation;
	FRotator NewRotation;
public:
	UFUNCTION()
	void SetMoveInfo(FSetMoveInfo move_data);

	void NpcInit(uint16 client_index);
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UTeemoGameInstance* teemo_game_instance;
};
