// Fill out your copyright notice in the Description page of Project Settings.


#include "DebugWidget.h"
#include "Blueprint/UserWidget.h"


UDebugWidget::UDebugWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UDebugWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UDebugWidget::InsertDebugData(TArray<FString> DebugData)
{
	VerticalBoxObject->ClearChildren();
	for (FString Data : DebugData)
	{
		class UTextBlock* TextBlock = NewObject<UTextBlock>(UTextBlock::StaticClass());

		TextBlock->SetText(FText::FromString(Data));
		VerticalBoxObject->AddChildToVerticalBox(TextBlock);
	}
}