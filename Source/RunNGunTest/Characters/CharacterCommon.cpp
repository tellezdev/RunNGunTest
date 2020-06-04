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

void ACharacterCommon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ControlCharacterRotation();

	SetDamagedState(DeltaTime);

}
void ACharacterCommon::ControlCharacterRotation()
{
	// Sets where the sprite has to face
	float yaw = 0.0f;
	if (!bIsMovingRight)
	{
		yaw = 180.0f;
	}
	FRotator* rotation = new FRotator(0.0f, yaw, 1.0f);
	GetController()->SetControlRotation(*rotation);
}

void ACharacterCommon::SetDamagedState(float DeltaTime)
{
	if (AnimationOtherTimeStop < GetCurrentTime())
	{
		bIsDamaged = false;
	}
	else
	{
		GetCapsuleComponent()->MoveComponent(FVector(bIsMovingRight ? -70.f * DeltaTime : 70.f * DeltaTime, 0.f, 0.f), GetActorRotation(), true);
	}
}

void ACharacterCommon::BindDataHUD()
{
}

void ACharacterCommon::HandleAttack()
{
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
	bIsDamaged = true;
	EAnimationState = AnimationState::HitTop;
	AnimationOtherTimeStop = GetCurrentTime() + CurrentFlipbook->GetFlipbookLength();
	Life -= Value;
}

void ACharacterCommon::HealLife(float Value)
{
}

void ACharacterCommon::HealStamina(float Value)
{
}

void ACharacterCommon::ApplyHitCollide(TArray<FComboAttackStruct> Combo)
{
}

void ACharacterCommon::DoCombo(TArray<FComboAttackStruct> Combo)
{
	bool bIsComboDone = true;
	for (bool ComboIsDone : ComboAnimationFlags[nAttackNumber].bIsComboHits)
	{
		if (!ComboIsDone)
		{
			bIsComboDone = false;
		}
	}

	if (!ComboAnimationFlags[nAttackNumber].bIsComboStart)
	{
		CurrentFlipbook->SetFlipbook(Combo[nAttackNumber].AttackAnimationStart);
		ComboAnimationFlags[nAttackNumber].bIsComboStart = true;
		AnimationFlipbookTimeStop = GetCurrentTime() + CurrentFlipbook->GetFlipbookLength();
		AnimationAttackCompleteTimeStop = GetCurrentTime() + CurrentFlipbook->GetFlipbookLength();
		bIsAnimationAttackComplete = false;
	}
	else if (bIsComboDone && !ComboAnimationFlags[nAttackNumber].bIsComboEnd)
	{
		CurrentFlipbook->SetFlipbook(Combo[nAttackNumber].AttackAnimationEnd);
		ComboAnimationFlags[nAttackNumber].bIsComboEnd = true;
		AnimationFlipbookTimeStop = GetCurrentTime() + CurrentFlipbook->GetFlipbookLength();
		AnimationAttackCompleteTimeStop = GetCurrentTime() + CurrentFlipbook->GetFlipbookLength();
		bIsAnimationAttackComplete = true;
	}
	else
	{
		if (!ComboAnimationFlags[nAttackNumber].bIsComboHits[nCurrentComboHit])
		{
			CurrentFlipbook->SetFlipbook(Combo[nAttackNumber].AttackAnimationHits[nCurrentComboHit].AttackAnimationHit);
			ComboAnimationFlags[nAttackNumber].bIsComboHits[nCurrentComboHit] = true;
			AnimationFlipbookTimeStop = GetCurrentTime() + CurrentFlipbook->GetFlipbookLength();
			AnimationAttackCompleteTimeStop = GetCurrentTime() + CurrentFlipbook->GetFlipbookLength();
			ApplyHitCollide(Combo);
		}
		if (nCurrentComboHit < ComboAnimationFlags[nAttackNumber].bIsComboHits.Num() - 1)
		{
			++nCurrentComboHit;
		}
	}
}

void ACharacterCommon::FinishCombo()
{
}

