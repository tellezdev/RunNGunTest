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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (BindWidget))
		class UImage* Avatar;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (BindWidget))
		class UProgressBar* LifeBar;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (BindWidget))
		class UProgressBar* StaminaBar;

	UFUNCTION()
		void BindAvatar(UTexture2D* Texture);
	UFUNCTION()
		void SetLife(float Life);
	UFUNCTION()
		void SetStamina(float Stamina);
};
