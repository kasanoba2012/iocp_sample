// Fill out your copyright notice in the Description page of Project Settings.


#include "ChatUserWidget.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"

UChatUserWidget::UChatUserWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UChatUserWidget::NativeConstruct()
{
	Super::NativeConstruct();
	// TODO: 다른 필수 위젯을 바인딩하고 초기 상태를 설정합니다.

    ChatScrollBox = Cast<UScrollBox>(GetWidgetFromName(TEXT("ChatScrollBox")));
    ChatTextBlock = Cast<UTextBlock>(GetWidgetFromName(TEXT("ChatTextBlock")));
}

FReply UChatUserWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
    if (InKeyEvent.GetKey() == EKeys::Enter)
    {
        // Enter 키 누름, 채팅 입력 처리
        FString ChatMessage = ChatTextBlock->GetText().ToString();
        AddChatMessage(FText::FromString(ChatMessage));
        ChatTextBlock->SetText(FText::GetEmpty());
        return FReply::Handled();
    }

    return FReply::Unhandled();
}

void UChatUserWidget::AddChatMessage(const FText& Message)
{
    if (ChatScrollBox)
    {
        UTextBlock* NewMessageTextBlock = NewObject<UTextBlock>(this);
        NewMessageTextBlock->SetText(Message);
        ChatScrollBox->AddChild(NewMessageTextBlock);
        ChatScrollBox->ScrollToEnd();
    }
}
