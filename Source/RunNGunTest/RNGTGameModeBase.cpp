// Fill out your copyright notice in the Description page of Project Settings.


#include "RNGTGameModeBase.h"
#include "Characters/CharacterPlayer.h"
#include "Blueprint/UserWidget.h"
#include "Engine/Engine.h"
#include "HUD/GameHUD.h"

void ARNGTGameModeBase::StartPlay()
{
	Super::StartPlay();

	Player = Cast<ACharacterCommon>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	Player->SetFacingDirectionRight(IsLevelDirectionRight());
	auto PlayerController = GetWorld()->GetFirstPlayerController();
	GameHUD = Cast<AGameHUD>(PlayerController->GetHUD());
	BindDataHUD();
}


void ARNGTGameModeBase::BindDataHUD()
{
	if (Player != nullptr && Player->Portrait != nullptr)
	{
		GameHUD->BindDataHUD(Player->Portrait);
		GameHUD->SetLife(Player->Life);
		GameHUD->SetStamina(Player->Stamina);
	}
}

void ARNGTGameModeBase::SetLevelDirectionRight(bool IsRight)
{
	bIsLevelDirectionRight = IsRight;
}

bool ARNGTGameModeBase::IsLevelDirectionRight()
{
	return bIsLevelDirectionRight;
}
