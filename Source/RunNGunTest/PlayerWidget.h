// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Runtime/UMG/Public/UMG.h"
#include "PlayerWidget.generated.h"

/**
 *
 */
UCLASS()
class RUNNGUNTEST_API UPlayerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPlayerWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

	UFUNCTION()
		void MessageFromLeftSide(FString Text);


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (BindWidget))
		class UCanvasPanel* MainCanvas;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (BindWidget))
		class UTextBlock* TXTBox;

};
