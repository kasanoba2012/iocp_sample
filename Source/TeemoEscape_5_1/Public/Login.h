// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Login.generated.h"

/**
 * 
 */

UCLASS()
class TEEMOESCAPE_5_1_API ULogin : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Login)
	FString login_id;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Login)
	FString login_pw;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyCategory")
	bool login_to_ingame = false;

public:
	UFUNCTION(BlueprintCallable, Category = Login)
	FString SetLoginInfo(FString id, FString pw);

	UFUNCTION(BlueprintCallable, Category = Login)
	void ServerConnect();

	UFUNCTION(BlueprintCallable, Category = Login)
	void ClientToServerPacketTest();

	UFUNCTION(Category = Login)
	void InGameConnect(uint16 client_index);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UTeemoGameInstance* teemo_game_instance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UClient_To_Server* client_to_server;
};
