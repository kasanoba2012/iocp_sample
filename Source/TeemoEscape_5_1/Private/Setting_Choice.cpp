// Fill out your copyright notice in the Description page of Project Settings.


#include "Setting_Choice.h"
#include <Kismet/GameplayStatics.h>
#include "Blueprint/WidgetLayoutLibrary.h"
#include "TeemoGameInstance.h"
#include "App_Server.h"
#include "Client_To_Server.h"

void USetting_Choice::NativeConstruct()
{
	Super::NativeConstruct();

	teemo_game_instance = Cast<UTeemoGameInstance>(GetGameInstance());
}

// 서버 오픈
void USetting_Choice::ServerStart()
{
	UE_LOG(LogTemp, Warning, TEXT("Iocp_Odbc_Start\n"), );

	//teemo_game_instance = Cast<UTeemoGameInstance>(GetGameInstance());
	
	teemo_game_instance->server_client_check = true;

	teemo_game_instance->is_run_process_thread_ = true;

	teemo_game_instance->iocp_net_server_->ServerStart();
}

// 클라 오픈
void USetting_Choice::ClientStart()
{
	UE_LOG(LogTemp, Warning, TEXT("Client_Start\n"), );
	teemo_game_instance->server_client_check = false;
	teemo_game_instance->timer_destroy_sw = true;

	teemo_game_instance->Server_Connect();
}