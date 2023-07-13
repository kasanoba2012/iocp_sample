// Fill out your copyright notice in the Description page of Project Settings.


#include "Server_Client_Actor_Manager.h"
#include "Teemo.h"
#include "TeemoGameInstance.h"
#include "Client_To_Server.h"
#include "InGamePacket.h"
#include "App_Server.h"

// Sets default values
AServer_Client_Actor_Manager::AServer_Client_Actor_Manager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AServer_Client_Actor_Manager::BeginPlay()
{
	Super::BeginPlay();
	
	// 클라이언트 예외 처리 해야하네
	
	teemo_game_instance = Cast<UTeemoGameInstance>(GetGameInstance());

	if (teemo_game_instance->server_client_check)
	{
		UE_LOG(LogTemp, Warning, TEXT("AServer_Client_Actor_Manager BeginPlay\n"));

		teemo_game_instance->iocp_net_server_->P_packet_manager_->Fclient_connect_.BindUFunction(this, FName("TestFuntion"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AServer_Client_Actor_Manager BeginPlay client\n"));
	}
}

// Called every frame
void AServer_Client_Actor_Manager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AServer_Client_Actor_Manager::TestFuntion()
{
	UE_LOG(LogTemp, Warning, TEXT("TestFuntion\n"));

	//AteemoActor = GetWorld()->SpawnActor<ATeemo>(teemo_server_Factory, FVector(-1000, 300, 88), FRotator(0));
}

