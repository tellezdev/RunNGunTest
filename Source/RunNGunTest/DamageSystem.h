// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterBase.h"
#include "EnemyBase.h"

/**
 *
 */
class RUNNGUNTEST_API DamageSystem
{
public:
	DamageSystem();
	~DamageSystem();

	UPROPERTY()
		ACharacterBase* Player;
	UPROPERTY()
		AEnemyBase* Enemy;

	UFUNCTION()
		void HitCharacter(AActor* Emitter, AActor* Receiver, float Value);
};
