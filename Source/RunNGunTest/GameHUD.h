// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Blueprint/UserWidget.h"
#include "HUD/ComboWidget.h"
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
		void ComboCounter(int32 ComboCount);

	UFUNCTION()
		void DrawBuffer(TArray<int32> BufferedInput);

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
		TSubclassOf<UUserWidget> ComboWidgetClass = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
		TSubclassOf<UUserWidget> BufferInputWidgetClass = nullptr;

private:
	UComboWidget* ComboWidget;

	UBufferInputWidget* BufferInputWidget;

};
