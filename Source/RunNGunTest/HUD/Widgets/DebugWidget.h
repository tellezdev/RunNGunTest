// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "DebugWidget.generated.h"

/**
 *
 */
UCLASS()
class RUNNGUNTEST_API UDebugWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UDebugWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (BindWidget))
		UVerticalBox* VerticalBoxObject;

	UFUNCTION()
		void InsertDebugData(TArray<FString> DebugData);
};
