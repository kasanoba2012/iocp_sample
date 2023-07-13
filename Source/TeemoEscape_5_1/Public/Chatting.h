// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Client_Packet.h"
#include "Blueprint/UserWidget.h"
#include "Chatting.generated.h"

/**
 * 
 */

UCLASS()
class TEEMOESCAPE_5_1_API UChatting : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

public:
	UChatting(const FObjectInitializer &ObjectInitializer);

public:
	UFUNCTION(BlueprintCallable, Category = Chatting)
	void ChatMsgSend(FString msg);
	char* FStringToCharArray(FString fstring);
	FString CharArrayToFString(const char* CharArray);

public: //ScorllBox
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UScrollBox* ChatHistory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UTextBlock* ChatMsgBlock;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chatting")
	FString MyString;

	Defind defind;

public:
	//UFUNCTION(BlueprintCallable, Category = Chatting)
	//void ChatHistory_Msg(FString msg);

	UFUNCTION(BlueprintCallable, Category = Chatting)
	void ChatHistory_Msg(FString msg , FString id);

	UFUNCTION(BlueprintCallable, Category = Chatting)
	void AddChatMessage(FString Message);




public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UTeemoGameInstance* teemo_game_instance;
};
