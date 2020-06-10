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

	ResetAttackAnimationFlags();
	ResetSpecialMoveAnimationFlags();

	AnimationFlipbookTimeStop = -1.f;
	AnimationSpecialTimeStart = -1.f;
	AnimationSpecialTimeStop = -1.f;
	AnimationAttackCompleteTimeStop = -1.f;
	AnimationSpecialMoveCompleteTimeStop = -1.f;
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
	// Only enter when animation is done
	if (AnimationFlipbookTimeStop <= GetCurrentTime())
	{
		if (GetCharacterMovement()->IsMovingOnGround())
		{
			DoCombo(AttackingComboAnimation);
		}
		else
		{
			DoCombo(AttackingJumpingAnimation);
		}
	}
}

void ACharacterCommon::HandleSpecialMoves()
{
}

void ACharacterCommon::ResetAttack()
{
	ResetAttackAnimationFlags();
}

void ACharacterCommon::UpdateAnimations()
{

}

void ACharacterCommon::ControlCharacterAnimations(float characterMovementSpeed)
{

}

void ACharacterCommon::SetAttackAnimationFlags()
{
	for (FComboAttackStruct combo : AttackingComboAnimation)
	{
		FComboAnimationFlagsStruct element;
		for (FComboAttackHitsStruct hit : combo.AnimationHits)
		{
			element.bIsComboHits.Add(false);
		}
		element.bIsComboStart = false;
		element.bIsComboEnd = false;
		ComboAnimationFlags.Add(element);
	}
}

void ACharacterCommon::ResetAttackAnimationFlags()
{
	ComboAnimationFlags.Empty();
	SetAttackAnimationFlags();
}

void ACharacterCommon::SetSpecialMoveAnimationFlags()
{

}

void ACharacterCommon::ResetSpecialMoveAnimationFlags()
{

}

float ACharacterCommon::GetCurrentTime()
{
	/*if (GetWorld())
	{*/
	return GetWorld()->GetRealTimeSeconds();
	/*}
	return 0.f;*/
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

void ACharacterCommon::DrainLife()
{
}

void ACharacterCommon::DrainStamina()
{
}

void ACharacterCommon::ApplyHitCollide(TArray<FComboAttackStruct> Combo)
{
	FVector HitBox = Combo[nAttackNumber].AnimationHits[nCurrentComboHit].HitBoxPosition;
	if (!bIsMovingRight)
	{
		HitBox = FVector(HitBox.X * -1, HitBox.Y, HitBox.Z);
	}
	// Hit box will grow to detect collision
	CurrentTraceHit = FVector(GetActorLocation().X + HitBox.X, GetActorLocation().Y + HitBox.Y, GetActorLocation().Z + HitBox.Z);
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
		CurrentFlipbook->SetFlipbook(Combo[nAttackNumber].AnimationStart);
		ComboAnimationFlags[nAttackNumber].bIsComboStart = true;
		AnimationFlipbookTimeStop = GetCurrentTime() + CurrentFlipbook->GetFlipbookLength();
		AnimationAttackCompleteTimeStop = GetCurrentTime() + CurrentFlipbook->GetFlipbookLength();
	}
	else if (bIsComboDone && !ComboAnimationFlags[nAttackNumber].bIsComboEnd)
	{
		CurrentFlipbook->SetFlipbook(Combo[nAttackNumber].AnimationEnd);
		ComboAnimationFlags[nAttackNumber].bIsComboEnd = true;
		AnimationFlipbookTimeStop = GetCurrentTime() + CurrentFlipbook->GetFlipbookLength();
		AnimationAttackCompleteTimeStop = GetCurrentTime() + CurrentFlipbook->GetFlipbookLength();
	}
	else
	{
		if (!ComboAnimationFlags[nAttackNumber].bIsComboHits[nCurrentComboHit])
		{
			CurrentFlipbook->SetFlipbook(Combo[nAttackNumber].AnimationHits[nCurrentComboHit].AnimationHit);
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

void ACharacterCommon::DoSpecialMove(FSpecialMoveStruct SpecialMove)
{

}

void ACharacterCommon::NotifyComboToHUD()
{
}

