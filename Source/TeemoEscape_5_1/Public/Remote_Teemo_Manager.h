// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Remote_Teemo_Manager.generated.h"

UCLASS()
class TEEMOESCAPE_5_1_API ARemote_Teemo_Manager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARemote_Teemo_Manager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere, Category = SpawnSettings)
	TSubclassOf<class ARemotable_Teemo> remote_teemo_factory;

	class ARemotable_Teemo* AteemoActor[5];

	FVector NewLocation;
	FRotator NewRotation;

	// 티모 리모트 생성
	void CreateRemoteTeemo();

	void NpcInit(uint16 client_index);

	UFUNCTION()
	void RemoteTeemoMoveChange(FSetMoveInfo move_data);

	UFUNCTION()
	void RemoteTeemoInit(FSetMoveInfo move_data);
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UTeemoGameInstance* teemo_game_instance;
};
