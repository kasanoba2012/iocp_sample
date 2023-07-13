// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "InGamePacket.generated.h"

USTRUCT(BlueprintType)
struct FSetMoveInfo
{
	GENERATED_BODY()

	//UPROPERTY(BlueprintReadWrite)
	uint16 packet_length;

	//UPROPERTY(BlueprintReadWrite)
	uint16 packet_id;

	uint16 client_id;

	char user_id[33];

	char user_pw[33];

	//UPROPERTY(BlueprintReadWrite)
	FVector fvector_;

	//UPROPERTY(BlueprintReadWrite)
	FRotator frotator_;

	uint16 in_game_connect = 0;

	char in_game_id[33];
};

/**
 * 
 */
UCLASS()
class TEEMOESCAPE_5_1_API UInGamePacket : public UObject
{
	GENERATED_BODY()
	
};
