// Fill out your copyright notice in the Description page of Project Settings.


#include "TeemoController.h"

void ATeemoController::SetupInputComponent()
{
    Super::SetupInputComponent();

    InputComponent->BindAction("SpawnCharacter", IE_Pressed, this, &ATeemoController::SpawnCharacter);
}

void ATeemoController::SpawnCharacter()
{
    UE_LOG(LogTemp, Warning, TEXT("SpawnCharacter"));
}
