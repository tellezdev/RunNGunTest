// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Blueprint/UserWidget.h"
#include "PlayerWidget.h"
#include "BufferInputWidget.h"
#include "GameHUD.generated.h"

/**
 *
 */
UCLASS()
class RUNNGUNTEST_API AGameHUD : public AHUD
{
	GENERATED_BODY()
public:
	AGameHUD();

	virtual void DrawHUD() override;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION()
		void MessageFromLeftSide(FString Text);

	UFUNCTION()
		void DrawBuffer(TArray<int32> BufferedInput);

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
		TSubclassOf<UUserWidget> PlayerWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
		TSubclassOf<UUserWidget> BufferInputWidgetClass;

private:
	UPlayerWidget* PlayerWidget;

	UBufferInputWidget* BufferInputWidget;

};
