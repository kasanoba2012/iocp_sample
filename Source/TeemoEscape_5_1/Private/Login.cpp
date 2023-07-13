// Fill out your copyright notice in the Description page of Project Settings.

#include "Login.h"
#include <Kismet/GameplayStatics.h>
#include "Blueprint/WidgetLayoutLibrary.h"
#include "TeemoGameInstance.h"
//#include "Client_To_Server.h"
#include "Client_Packet.h"
#include "InGamePacket.h"


void ULogin::NativeConstruct()
{
	Super::NativeConstruct();

	teemo_game_instance = Cast<UTeemoGameInstance>(GetGameInstance());

	if (teemo_game_instance != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ULogin NativeConstruct]\n"));
		teemo_game_instance->Fuc_in_game_connect.BindUFunction(this, FName("InGameConnect"));
	}
}

FString ULogin::SetLoginInfo(FString id, FString pw)
{
	Login_Send_Packet login_send_packet;
    login_send_packet.packet_id = (int)CLIENT_PACKET_ID::LOGIN_REQUEST;
	strcpy_s(login_send_packet.user_id, sizeof(login_send_packet.user_id),TCHAR_TO_ANSI(*id));
	strcpy_s(login_send_packet.user_pw, sizeof(login_send_packet.user_pw),TCHAR_TO_ANSI(*pw));
	login_send_packet.packet_length = sizeof(login_send_packet);

	login_id = id;
	login_pw = pw;

	UE_LOG(LogTemp, Warning, TEXT("[Login.cpp] pakcet_id : %d, pakcet_userid : %s\n"), login_send_packet.packet_id, login_send_packet.user_id);

	teemo_game_instance->Server_Packet_Send((char*)&login_send_packet, login_send_packet.packet_length);

	return login_id;	
}

void ULogin::ServerConnect()
{
	UE_LOG(LogTemp, Warning, TEXT("ServerConnect\n"), );

	teemo_game_instance->Server_Connect();
}

void ULogin::ClientToServerPacketTest()
{
	FVector test_vector = {0,0,0};
	FRotator test_rotator = {0,100,0};

	SetMoveInfo move_info_packet;
	move_info_packet.packet_id = (int)PACKET_ID::PACKET_TEST_SEND;
	move_info_packet.packet_length = sizeof(move_info_packet);
	move_info_packet.fvector_ = test_vector;
	move_info_packet.frotator_ = test_rotator;

	for (int i = 0; i < 5; i++)
	{
		UE_LOG(LogTemp, Warning, TEXT("PacketTest1_client_to_server pakcet_id : %d / fvector : %s / frotator_ : %s"),
			move_info_packet.packet_id, *move_info_packet.fvector_.ToString(), *move_info_packet.frotator_.ToString());
		teemo_game_instance->Server_Packet_Send((char*)&move_info_packet, move_info_packet.packet_length);
	}
}

void ULogin::InGameConnect(uint16 client_index)
{
	teemo_game_instance->InGameAccept();

	login_to_ingame = true;

	teemo_game_instance->client_index_ = client_index;

	UE_LOG(LogTemp, Warning, TEXT("InGameConnect [client_index] : %d\n"), teemo_game_instance->client_index_);	
}