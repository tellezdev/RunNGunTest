// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Runtime/UMG/Public/UMG.h"
#include "BufferInputWidget.generated.h"

/**
 *
 */
UCLASS()
class RUNNGUNTEST_API UBufferInputWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UBufferInputWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

	UFUNCTION()
		void DrawBuffer(TArray<int32> BufferedInput);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (BindWidget))
		class UCanvasPanel* MainCanvas;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (BindWidget))
		TArray<UTexture2D*> ArrowsTextures;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (BindWidget))
		class UHorizontalBox* ArrowsHorizontalBox;

	UPROPERTY()
		UHorizontalBoxSlot* ArrowsHorizontalBoxSlot;
};
