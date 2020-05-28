// Fill out your copyright notice in the Description page of Project Settings.


#include "ComboWidget.h"
#include "Blueprint/UserWidget.h"

UComboWidget::UComboWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ComboTextBlock = NewObject<UTextBlock>();
	ExtraTextBlock = NewObject<UTextBlock>();
	HitsTextBlock = NewObject<UTextBlock>();
}

void UComboWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UComboWidget::ComboCounter(int32 ComboCount)
{
	/*if (ComboTextBlock->Visibility == ESlateVisibility::Hidden)
	{
		ComboTextBlock->SetVisibility(ESlateVisibility::Visible);
	}*/
	ComboTextBlock->SetText(FText::AsNumber(ComboCount));
	HitsTextBlock->SetText(FText::FromString("HITS!!"));
	FString Text;
	switch (ComboCount)
	{
	case 3:
		Text = "Not bad!";
		break;
	case 4:
		Text = "Cool!";
		break;
	case 5:
		Text = "Awesome!!";
		break;
	case 6:
		Text = "Amazing!";
		break;
	case 7:
		Text = "Smoking!!";
		break;
	default:
		Text = "Chillin'!!";
	}
	ExtraTextBlock->SetText(FText::FromString(Text));

	// Play the only animation
	UWidgetBlueprintGeneratedClass* WidgetClass = GetWidgetTreeOwningClass();
	if (WidgetClass->Animations[0] != NULL)
	{
		PlayAnimation(WidgetClass->Animations[0], 0.f);
	}
}