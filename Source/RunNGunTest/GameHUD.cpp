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

	if (PlayerWidgetClass)
	{
		PlayerWidget = CreateWidget<UPlayerWidget>(GetWorld(), PlayerWidgetClass);
		if (PlayerWidget)
		{
			PlayerWidget->AddToViewport();
			PlayerWidget->SetPositionInViewport(FVector2D(500, 500));
		}
	}

	if (BufferInputWidgetClass)
	{
		BufferInputWidget = CreateWidget<UBufferInputWidget>(GetWorld(), BufferInputWidgetClass);
		if (BufferInputWidget)
		{
			BufferInputWidget->AddToViewport();
			BufferInputWidget->SetPositionInViewport(FVector2D(500, 500));
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

void AGameHUD::MessageFromLeftSide(FString Text)
{
	if (PlayerWidget)
	{
		PlayerWidget->MessageFromLeftSide(Text);
	}
}

void AGameHUD::DrawBuffer(TArray<int32> BufferedInput)
{
	if (BufferInputWidget)
	{
		BufferInputWidget->DrawBuffer(BufferedInput);
	}
}

