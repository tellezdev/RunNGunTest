// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerWidget.h"
#include "Blueprint/UserWidget.h"

UPlayerWidget::UPlayerWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UPlayerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	LifeBar->SetPercent(100);
	StaminaBar->SetPercent(100);
}

void UPlayerWidget::BindAvatar(UTexture2D* Texture)
{
	Avatar->SetBrushFromTexture(Texture);
}

void UPlayerWidget::SetLife(float Life)
{
	LifeBar->SetPercent(Life * 0.01f);
}

void UPlayerWidget::SetStamina(float Stamina)
{
	StaminaBar->SetPercent(Stamina * 0.1f);
}
