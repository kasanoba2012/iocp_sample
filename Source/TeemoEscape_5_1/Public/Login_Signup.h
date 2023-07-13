// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Client_Packet.h"
#include "Blueprint/UserWidget.h"
#include "Login_Signup.generated.h"

/**
 * 
 */
UCLASS()
class TEEMOESCAPE_5_1_API ULogin_Signup : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Signup)
	FString signup_id;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Signup)
	FString signup_pw;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Signup)
	FString signup_check_pw;

public:
	UFUNCTION(BlueprintCallable, Category = Signup)
	bool Login_Signup(FString id, FString pw, FString pw2);

	void Signup_Packet_Send(FString id, FString pw, CLIENT_PACKET_ID packet_id);

public:
	class UTeemoGameInstance* teemo_game_instance;
};
