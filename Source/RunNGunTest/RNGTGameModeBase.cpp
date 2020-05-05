// Fill out your copyright notice in the Description page of Project Settings.


#include "RNGTGameModeBase.h"

void ARNGTGameModeBase::StartPlay()
{
	Super::StartPlay();

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("RNGTGameModeBase Initiated"));
	}
}
