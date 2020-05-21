// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerWidget.h"
#include "Blueprint/UserWidget.h"

UPlayerWidget::UPlayerWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UPlayerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	MainCanvas->AddChildToCanvas(TXTBox);	
}

void UPlayerWidget::MessageFromLeftSide(FString Text)
{
	if (TXTBox->Visibility == ESlateVisibility::Hidden)
	{
		TXTBox->SetVisibility(ESlateVisibility::Visible);
	}
	TXTBox->SetText(FText::FromString(Text));
}

