// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine.h"
#include "GameFramework/GameModeBase.h"
#include "UObject/ObjectMacros.h"
#include "Characters/CharacterCommon.h"
#include "HUD/GameHUD.h"
#include "RNGTGameModeBase.generated.h"

/**
 *
 */
UCLASS()
class RUNNGUNTEST_API ARNGTGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

		virtual void StartPlay() override;

public:
	void BindDataHUD();

private:

	UPROPERTY()
		ACharacterCommon* Player;

	// HUD
	UPROPERTY()
		AGameHUD* GameHUD;
};
