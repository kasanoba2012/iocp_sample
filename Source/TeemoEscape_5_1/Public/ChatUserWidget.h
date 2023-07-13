// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChatUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class TEEMOESCAPE_5_1_API UChatUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    UChatUserWidget(const FObjectInitializer& ObjectInitializer);

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    class UScrollBox* ChatScrollBox;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    class UTextBlock* ChatTextBlock;

protected:
    virtual void NativeConstruct() override;
    virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

    void AddChatMessage(const FText& Message);
	
};
