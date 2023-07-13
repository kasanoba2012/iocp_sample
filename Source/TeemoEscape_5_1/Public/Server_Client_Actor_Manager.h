// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Server_Client_Actor_Manager.generated.h"

UCLASS()
class TEEMOESCAPE_5_1_API AServer_Client_Actor_Manager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AServer_Client_Actor_Manager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	// Teemo 타입의 블루프린트 할당 받을 변수
	UPROPERTY(EditAnywhere, Category = SpawnSettings)
	TSubclassOf<class ATeemo> teemo_server_Factory;

	class ATeemo* AteemoActor;

public:
	UFUNCTION()
	void TestFuntion();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UTeemoGameInstance* teemo_game_instance;
};
