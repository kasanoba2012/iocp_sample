// Fill out your copyright notice in the Description page of Project Settings.


#include "Remote_Teemo_Manager.h"
#include "Remotable_Teemo.h"
#include "TeemoGameInstance.h"

// Sets default values
ARemote_Teemo_Manager::ARemote_Teemo_Manager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ARemote_Teemo_Manager::BeginPlay()
{
	Super::BeginPlay();

	teemo_game_instance = Cast<UTeemoGameInstance>(GetGameInstance());

	NewLocation = FVector(-1000, 300, 88000000); // Set the desired new location for the actor
	NewRotation = FRotator(0);

	if (teemo_game_instance->server_client_check == false)
	{
		// client dummy actor
		UE_LOG(LogTemp, Warning, TEXT("npc_create_0\n"));
		NpcInit(teemo_game_instance->client_index_);
		teemo_game_instance->Fun_move_info_remote_.BindUFunction(this, FName("RemoteTeemoMoveChange"));
		teemo_game_instance->Fun_move_info_remote_init_.BindUFunction(this, FName("RemoteTeemoInit"));

	}
	else {
		// server dummy actor
		UE_LOG(LogTemp, Warning, TEXT("npc_create_1\n"));
	}		
}

// Called every frame
void ARemote_Teemo_Manager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARemote_Teemo_Manager::CreateRemoteTeemo()
{

}

void ARemote_Teemo_Manager::NpcInit(uint16 client_index)
{
	for (int i = 0; i <= 4; i++)
	{
		AteemoActor[i] = GetWorld()->SpawnActor<ARemotable_Teemo>(remote_teemo_factory, NewLocation, NewRotation);
		AteemoActor[i]->remote_teemo_client_index = i;
		NewLocation += FVector(200, 0, 0);
	}
}

void ARemote_Teemo_Manager::RemoteTeemoMoveChange(FSetMoveInfo move_data)
{
	UE_LOG(LogTemp, Warning, TEXT("remote_teemo_move_change client id : %d, length : %d vector : %s game index : %d"),
		move_data.client_id, move_data.packet_length, *move_data.fvector_.ToString(), teemo_game_instance->client_index_);

	if (move_data.client_id != teemo_game_instance->client_index_)
	{
		UE_LOG(LogTemp, Warning, TEXT("move_to_remote %d"), move_data.client_id);
		// 움직이는 클라이언트의 리모트 티모만 조정
		AteemoActor[move_data.client_id]->SetActorLocationAndRotation(move_data.fvector_, move_data.frotator_);	
	}
}

void ARemote_Teemo_Manager::RemoteTeemoInit(FSetMoveInfo move_data)
{
	UE_LOG(LogTemp, Warning, TEXT("init start4 client_id : %d / vector : %s / rotator : %s client_index %d"), 
	move_data.client_id, *move_data.fvector_.ToString(), *move_data.frotator_.ToString(), teemo_game_instance->client_index_);
	if (move_data.client_id != teemo_game_instance->client_index_)
	{
		UE_LOG(LogTemp, Warning, TEXT("init start5 client_id : %d / vector : %s / rotator : %s"), move_data.client_id, *move_data.fvector_.ToString(), *move_data.frotator_.ToString());
		// 움직이는 클라이언트의 리모트 티모만 조정
		AteemoActor[move_data.client_id]->SetActorLocationAndRotation(move_data.fvector_, move_data.frotator_);
	}
}

