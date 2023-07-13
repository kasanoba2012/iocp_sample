// Fill out your copyright notice in the Description page of Project Settings.


#include "TeemoManager.h"
#include "Enemy.h"
#include "Teemo.h"
#include "TimerManager.h"
#include "TeemoGameInstance.h"
#include "Client_To_Server.h"
#include "InGamePacket.h"
#include <GameFramework/ProjectileMovementComponent.h>
#include "App_Server.h"

// Sets default values
ATeemoManager::ATeemoManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ATeemoManager::BeginPlay()
{
	Super::BeginPlay();
	teemo_game_instance = Cast<UTeemoGameInstance>(GetGameInstance());

	NewLocation = FVector(-1000, 300, 88); // Set the desired new location for the actor
	NewRotation = FRotator(0);		

	UE_LOG(LogTemp, Warning, TEXT("teemo_manager_begin_setmoveinfo\n"));

	teemo_game_instance->Fun_move_info_.BindUFunction(this, FName("SetMoveInfo"));

	if (teemo_game_instance->server_client_check == false)
	{
		// client dummy actor
		UE_LOG(LogTemp, Warning, TEXT("npc_create_0\n"));
		NpcInit(teemo_game_instance->client_index_);
	}
	else {
		// server dummy actor
		UE_LOG(LogTemp, Warning, TEXT("npc_create_1\n"));
	}
	

}

// Called every frame
void ATeemoManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//UE_LOG(LogTemp, Warning, TEXT("npc_init client2 : %d\n"), teemo_game_instance->client_index_);
}

void ATeemoManager::NpcInit(uint16 client_index)
{
	UE_LOG(LogTemp, Warning, TEXT("npc_init client2 : %d\n"), teemo_game_instance->client_index_);

	for (int i = 0; i <= 4; i++)
	{
		/*if(i != client_index)
		{ */
			AteemoActor[i] = GetWorld()->SpawnActor<ATeemo>(teemoFactory, NewLocation, NewRotation);

			NewLocation += FVector(200, 0, 0); // Set the desired new location for the actor
		//}
		//else
		//{
		//	AteemoActor[i]->npc_index_ = client_index;
		//}
	}	
}

void ATeemoManager::SetMoveInfo(FSetMoveInfo move_data)
{
	UE_LOG(LogTemp, Warning, TEXT("move_info_4 id : %d, length : %d vector : %s"), 
	move_data.packet_id, move_data.packet_length ,*move_data.fvector_.ToString());

	bool test_sw = true;

	if (AteemoActor && test_sw && move_data.client_id == 4)
	{
		//AteemoActor[move_data.client_id]->SetActorLocationAndRotation(move_data.fvector_ + FVector(300, 0, 0), move_data.frotator_);
		AteemoActor[4]->SetActorLocationAndRotation(move_data.fvector_ + FVector(300, 0, 0), move_data.frotator_);
	}
}
