// Fill out your copyright notice in the Description page of Project Settings.

#include "Chatting.h"

#include "Blueprint/WidgetTree.h"
#include "Components/TextBlock.h"
#include "Components/ScrollBox.h"
#include "TeemoController.h"
#include "Kismet/GameplayStatics.h"
//#include "Client_To_Server.h"
#include "TeemoGameInstance.h"
#include "Engine/World.h"
#include "TimerManager.h"

void UChatting::NativeConstruct()
{
	Super::NativeConstruct();

	UE_LOG(LogTemp, Warning, TEXT("[NativeConstruct]\n"));

	ChatHistory = Cast<UScrollBox>(GetWidgetFromName(TEXT("ChatHistory")));

	ChatMsgBlock = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());

	teemo_game_instance = Cast<UTeemoGameInstance>(GetGameInstance());

	if (teemo_game_instance != nullptr)
	{
		//teemo_game_instance->Fuc_boradcast_success.BindUFunction(this, FName("ChatHistory_Msg"));
		teemo_game_instance->Fuc_boradcast_two_success.BindUFunction(this, FName("ChatHistory_Msg"));
		//teemo_game_instance->Fuc_Dynamic_OneParam.AddDynamic(this, &UChatting::AddChatMessage);
	}

}

UChatting::UChatting(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	UE_LOG(LogTemp, Warning, TEXT("[UChatting]\n"));
}

void UChatting::ChatMsgSend(FString msg)
{
	SetMoveInfo login_send_packet;
	login_send_packet.packet_id = (int)CLIENT_PACKET_ID::CHAT_SEND_REQUEST;
	strcpy_s(login_send_packet.user_id, sizeof(login_send_packet.user_id), FStringToCharArray(*msg));
	//strcpy_s(login_send_packet.in_game_id, sizeof(login_send_packet.in_game_id), FStringToCharArray(*msg));

	login_send_packet.packet_length = sizeof(login_send_packet);

	UE_LOG(LogTemp, Warning, TEXT("[Login.cpp0] msg : %s my id : %s"),*CharArrayToFString(login_send_packet.user_id), *defind.CharArrayToFString(teemo_game_instance->signin_id));

	teemo_game_instance->Server_Packet_Send((char*)&login_send_packet, login_send_packet.packet_length);
}


// Fstring to char
char* UChatting::FStringToCharArray(FString fString)
{
	std::string stdString(TCHAR_TO_UTF8(*fString));
	char* charArray = new char[stdString.length() + 1];
	strcpy_s(charArray, sizeof(charArray),stdString.c_str());

	return charArray;
}

// char To FString
FString UChatting::CharArrayToFString(const char* CharArray)
{
	std::string StdString = CharArray;

	return FString(StdString.c_str());
}

//void UChatting::ChatHistory_Msg(FString msg)
//{
//	UE_LOG(LogTemp, Warning, TEXT("ChatHistory_Msg1 / %s"), *msg)
//
//	AddChatMessage(msg);
//}

void UChatting::ChatHistory_Msg(FString msg, FString id)
{

	UE_LOG(LogTemp, Warning, TEXT("Login.cpp6 : %s id : %s my id : %s"), *msg, *id, *defind.CharArrayToFString(teemo_game_instance->signin_id));
	// TODO teemo_game_instance->signin_id 여기에 뭐 들었나 확인하기 
	
	// Text 오브젝트를 생성하고, ScrollBox에 추가한다.	
	UTextBlock* NewTextBlock = NewObject<UTextBlock>(ChatHistory);
	// 티모 인스턴스에서 ID 받아오자
	//FString user_id = *teemo_game_instance->signin_id;
	FString user_id = "msg";
	FString NewMessage = " : ";

	//UE_LOG(LogTemp, Warning, TEXT("Login.cpp / %s"), *defind.CharArrayToFString(teemo_game_instance->signin_id));
	FText UpdatedText = FText::Format(FText::FromString("{0}{1}{2}"), FText::FromString(user_id), FText::FromString(NewMessage), FText::FromString(msg));
	NewTextBlock->SetText(UpdatedText);

	if (ChatHistory != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("ChatHistory_Msg3 / %d"), ChatHistory);
	}
	ChatHistory->AddChild(NewTextBlock);
	ChatHistory->ScrollToEnd(); // 가장 최근 채팅을 보기 위해, 스크롤을 가장 아래로 내린다.


}

void UChatting::AddChatMessage(FString Message)
{
	// Text 오브젝트를 생성하고, ScrollBox에 추가한다.	
	UTextBlock* NewTextBlock = NewObject<UTextBlock>(ChatHistory);
	// 티모 인스턴스에서 ID 받아오자
	//FString user_id = *teemo_game_instance->signin_id;
	FString user_id = "test";
	FString NewMessage = " : ";
	//NewTextBlock->SetText(FText::FromString(Message));

	//UTextBlock* NewTextBlock = NewObject<UTextBlock>(ChatHistory);
	//FText ExistingText = NewTextBlock->GetText();
	//FString NewMessage = "Additional text content";
	// defind
	//UE_LOG(LogTemp, Warning, TEXT("Login.cpp / %s"), *defind.CharArrayToFString(teemo_game_instance->signin_id));
	FText UpdatedText = FText::Format(FText::FromString("{0}{1}{2}"), FText::FromString(user_id), FText::FromString(NewMessage),FText::FromString(Message) );
	NewTextBlock->SetText(UpdatedText);

	if (ChatHistory != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("ChatHistory_Msg3 / %d"), ChatHistory);
	}
	ChatHistory->AddChild(NewTextBlock);
	ChatHistory->ScrollToEnd(); // 가장 최근 채팅을 보기 위해, 스크롤을 가장 아래로 내린다.
}