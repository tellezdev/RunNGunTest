// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterCommon.h"

// Sets default values
ACharacterCommon::ACharacterCommon()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ACharacterCommon::BeginPlay()
{
	Super::BeginPlay();

}

void ACharacterCommon::ResetAttack()
{
}

void ACharacterCommon::UpdateAnimations()
{

}

void ACharacterCommon::ControlCharacterAnimations(float characterMovementSpeed)
{

}

void ACharacterCommon::SetAnimationFlags()
{
}

void ACharacterCommon::ResetAnimationFlags()
{
}

float ACharacterCommon::GetCurrentTime()
{
	return 0.0f;
}

void ACharacterCommon::SetDamage(float Value)
{

}

void ACharacterCommon::HealLife(float Value)
{
}

void ACharacterCommon::ApplyHitCollide(TArray<FComboAttackStruct> Combo)
{
}

void ACharacterCommon::DoCombo(TArray<FComboAttackStruct> Combo)
{
}

