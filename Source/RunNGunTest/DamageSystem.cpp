// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageSystem.h"

DamageSystem::DamageSystem()
{
}

DamageSystem::~DamageSystem()
{
}

void DamageSystem::HitCharacter(AActor* Emitter, AActor* Receiver, float Value)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("It's working!"));
	}
}
