// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "ComboWidget.generated.h"

/**
 *
 */
UCLASS()
class RUNNGUNTEST_API UComboWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UComboWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

	UFUNCTION()
		void ComboCounter(int32 ComboCount);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (BindWidget))
		class UTextBlock* ComboTextBlock;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (BindWidget))
		class UTextBlock* HitsTextBlock;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (BindWidget))
		class UTextBlock* ExtraTextBlock;
	UPROPERTY()
		TMap<FString, class UWidgetAnimation*> FindWidgetAnimation;
};
