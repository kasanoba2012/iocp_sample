// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Setting_Choice.generated.h"

/**
 * 
 */
UCLASS()
class TEEMOESCAPE_5_1_API USetting_Choice : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

public:
	UFUNCTION(BlueprintCallable, Category = Setting_Chioce)
	void ServerStart();

	UFUNCTION(BlueprintCallable, Category = Setting_Chioce)
	void ClientStart();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UTeemoGameInstance* teemo_game_instance;
	
};
