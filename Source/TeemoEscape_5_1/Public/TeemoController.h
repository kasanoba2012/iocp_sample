// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Enemy.h"
#include "TeemoController.generated.h"

/**
 * 
 */
UCLASS()
class TEEMOESCAPE_5_1_API ATeemoController : public APlayerController
{
	GENERATED_BODY()

public:
    virtual void SetupInputComponent() override;
    void SpawnCharacter();

private:
    AEnemy* SpawnedCharacter;
};
