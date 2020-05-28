// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Blueprint/UserWidget.h"
#include "HUD/ComboWidget.h"
#include "HUD/DebugWidget.h"
#include "BufferInputWidget.h"
#include "PlayerWidget.h"
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

	UFUNCTION()
		void BindDataHUD(UTexture2D* Avatar);

	UFUNCTION()
		void SetLife(float Life);

	UFUNCTION()
		void SetStamina(float Stamina);

	UFUNCTION()
		void InsertDebugData(TArray<FString> DebugData);

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
		TSubclassOf<UUserWidget> ComboWidgetClass = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
		TSubclassOf<UUserWidget> BufferInputWidgetClass = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
		TSubclassOf<UUserWidget> PlayerWidgetClass = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
		TSubclassOf<UUserWidget> DebugWidgetClass = nullptr;

private:
	UComboWidget* ComboWidget;

	UBufferInputWidget* BufferInputWidget;

	UPlayerWidget* PlayerWidget;

	UDebugWidget* DebugWidget;

};
