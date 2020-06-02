// Fill out your copyright notice in the Description page of Project Settings.


#include "GameHUD.h"
#include "Engine/Engine.h"
#include "GameFramework/HUD.h"

AGameHUD::AGameHUD()
{

}

void AGameHUD::BeginPlay()
{
	Super::BeginPlay();


	if (ComboWidget != nullptr)
	{
		ComboWidget->RemoveFromViewport();
		ComboWidget = nullptr;
	}

	if (ComboWidgetClass)
	{
		ComboWidget = CreateWidget<UComboWidget>(GetWorld(), ComboWidgetClass);
		if (ComboWidget)
		{
			ComboWidget->AddToViewport();
		}
	}

	if (BufferInputWidget != nullptr)
	{
		BufferInputWidget->RemoveFromViewport();
		BufferInputWidget = nullptr;
	}
	if (BufferInputWidgetClass)
	{
		BufferInputWidget = CreateWidget<UBufferInputWidget>(GetWorld(), BufferInputWidgetClass);

		if (BufferInputWidget)
		{
			BufferInputWidget->AddToViewport();
		}
	}

	if (PlayerWidget != nullptr)
	{
		PlayerWidget->RemoveFromViewport();
		PlayerWidget = nullptr;
	}
	if (PlayerWidgetClass)
	{
		PlayerWidget = CreateWidget<UPlayerWidget>(GetWorld(), PlayerWidgetClass);

		if (PlayerWidget)
		{
			PlayerWidget->AddToViewport();
		}
	}

	if (DebugWidget != nullptr)
	{
		DebugWidget->RemoveFromViewport();
		DebugWidget = nullptr;
	}
	if (DebugWidgetClass)
	{
		DebugWidget = CreateWidget<UDebugWidget>(GetWorld(), DebugWidgetClass);

		if (DebugWidget)
		{
			DebugWidget->AddToViewport();
		}
	}
}

void AGameHUD::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AGameHUD::DrawHUD()
{
	Super::DrawHUD();
}

void AGameHUD::ComboCounter(int32 ComboCount)
{
	if (ComboWidget)
	{
		ComboWidget->ComboCounter(ComboCount);
	}
}

void AGameHUD::DrawBuffer(TArray<int32> BufferedInput)
{
	if (BufferInputWidget)
	{
		BufferInputWidget->DrawBuffer(BufferedInput);
	}
}

void AGameHUD::BindDataHUD(UTexture2D* Avatar)
{
	if (PlayerWidget)
	{
		PlayerWidget->BindAvatar(Avatar);
	}
}

void AGameHUD::SetLife(float Life)
{
	PlayerWidget->SetLife(Life);
}

void AGameHUD::SetStamina(float Stamina)
{
	PlayerWidget->SetStamina(Stamina);
}

void AGameHUD::InsertDebugData(TArray<FString> DebugData)
{
	DebugWidget->InsertDebugData(DebugData);
}
