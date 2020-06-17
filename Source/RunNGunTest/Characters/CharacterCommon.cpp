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

	ResetActionAnimationFlags();

	AnimationFlipbookTimeStop = -1.f;
	AnimationActionTimeStart = -1.f;
	AnimationActionTimeStop = -1.f;
	AnimationActionCompleteTimeStop = 0.f;
}

void ACharacterCommon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ControlCharacterRotation();

	SetDamagedState(DeltaTime);

	if (bIsExecutingAction && ActionFinalLocation != FVector(0.f, 0.f, 0.f))
	{
		FVector ToVinterp = FMath::VInterpConstantTo(GetActorLocation(), ActionFinalLocation, DeltaTime, nCurrentAnimationInterpolationSpeed * 100.f);
		SetActorLocation(ToVinterp);
	}

	if (AnimationActionCompleteTimeStop > GetCurrentTime())
	{
		DoActionAnimation();
		nLastActionTime = GetCurrentTime();
	}

	if (AnimationActionCompleteTimeStop < GetCurrentTime())
	{
		if (bIsSpecialMove)
		{
			ResetSpecialMove();
		}
		if (bIsAttacking)
		{
			ResetAttack();
		}
		EAnimationState = AnimationState::Idle;
		bCanMove = true;
		ResetActionAnimationFlags();
	}
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
	Actions = AttackMoves;

	if (AnimationActionCompleteTimeStop < GetCurrentTime())
	{
		if (GetCharacterMovement()->IsMovingOnGround())
		{
			bCanMove = false;

			/*if (ActionsAnimationsFlags.Num() == 0)
			{*/
			SetActionAnimationFlags();
			//}

			nCurrentActionAnimation = 0;
			CurrentAttackHasHitObjective = false;
			DoActionAnimation();
			bIsFirstAttack = false;
		}
		else
		{
			//DoCombo(AttackingJumpingAnimation);
			ResetAttackCombo();
			bCanMove = true;
			//InputBuffer.ClearBuffer();
		}
	}

	///*if (bIsInHitAttackFrames && !bCurrentHitCollisionIsDone)
	//{
	//	if (GetCharacterMovement()->IsMovingOnGround())
	//	{
	//		ApplyHitCollide(AttackingComboAnimation);
	//	}
	//	else
	//	{
	//		ApplyHitCollide(AttackingJumpingAnimation);
	//	}
	//}*/


}

void ACharacterCommon::HandleSpecialMoves()
{
	if (!bIsExecutingAction)
	{
		bIsExecutingAction = true;
		bCanMove = false;
		// Checking if there are enough stamina
		int bIsEnoughStamina = SpecialMoves[nCurrentAction].StaminaCost <= Stamina;
		if (bIsEnoughStamina)
		{
			if ((SpecialMoves[nCurrentAction].CanBeDoneInGround && GetCharacterMovement()->IsMovingOnGround())
				|| (SpecialMoves[nCurrentAction].CanBeDoneInAir && !GetCharacterMovement()->IsMovingOnGround()))
			{
				ConsumeStamina(SpecialMoves[nCurrentAction].StaminaCost);
				Actions = SpecialMoves;
				SetActionAnimationFlags();
				DoActionAnimation();
			}
			else
			{
				//InputBuffer.ClearBuffer();
			}
		}
		else
		{
			Actions = NoStaminaMoves;
			SetActionAnimationFlags();
			DoActionAnimation();
		}
	}
}

void ACharacterCommon::ConsumeStamina(float Value)
{
	Stamina = Stamina - Value;
	GameHUD->SetStamina(Stamina);
}

void ACharacterCommon::ResetAttackCombo()
{
	nCurrentAction = 0;
	bIsFirstAttack = true;
	ResetActionAnimationFlags();
}

void ACharacterCommon::ResetAttack()
{
	bIsAttacking = false;
	bIsAttackFinished = true;
	CurrentAttackHasHitObjective = false;
}

void ACharacterCommon::ResetSpecialMove()
{
	bIsSpecialMove = false;
	nCurrentAction = 0;
	nCurrentActionAnimation = 0;
	bIsExecutingAction = false;
	bCanMove = true;
	ActionFinalLocation = FVector(0.f, 0.f, 0.f);
	GetCapsuleComponent()->SetSimulatePhysics(false);
}

void ACharacterCommon::UpdateAnimations()
{

}

void ACharacterCommon::ControlCharacterAnimations(float characterMovementSpeed)
{

}

void ACharacterCommon::SetActionAnimationFlags()
{
	ActionsAnimationsFlags.Empty();
	if (Actions.Num() > 0)
	{
		for (int i = 0; i != Actions.Num(); ++i)
		{
			FActionAnimationsFlagsStruct Action;
			for (int j = 0; j != Actions[i].ActionAnimation.Num(); ++j)
			{
				FActionAnimationFlagsStruct Animations;
				Animations.bIsCompleted = false;
				Animations.bIsActionStart = false;
				Animations.bIsActionEnd = false;
				for (int k = 0; k != Actions[i].ActionAnimation[j].AnimationHits.Num(); ++k)
				{
					Animations.bIsActionHits.Add(false);
				}
				Action.Animations.Add(Animations);
			}
			ActionsAnimationsFlags.Add(Action);
		}
	}
}

void ACharacterCommon::ResetActionAnimationFlags()
{
	nCurrentActionHitAnimation = 0;
	nCurrentActionAnimation = 0;
	AnimationActionTimeStop = 0.f;
	AnimationActionCompleteTimeStop = 0.f;
	SetActionAnimationFlags();
}

float ACharacterCommon::GetCurrentTime()
{
	return GetWorld()->GetRealTimeSeconds();
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

void ACharacterCommon::ApplyHitCollide(FActionAnimationStruct CurrentAction)
{
	FVector Hitbox = CurrentAction.HitBoxPosition;
	if (!bIsMovingRight)
	{
		Hitbox = FVector(Hitbox.X * -1, Hitbox.Y, Hitbox.Z);
	}
	// Hit box will grow to detect collision
	CurrentTraceHit = FVector(GetActorLocation().X + Hitbox.X, GetActorLocation().Y + Hitbox.Y, GetActorLocation().Z + Hitbox.Z);

	// Tracing collision
	FHitResult* HitResult = new FHitResult();
	if (!CurrentAttackHasHitObjective && UKismetSystemLibrary::BoxTraceSingle(GetWorld(), CurrentTraceHit, CurrentTraceHit, CurrentAction.HitBoxSize, GetActorRotation(), ETraceTypeQuery::TraceTypeQuery2, false, ActorsToIgnore, EDrawDebugTrace::ForDuration, *HitResult, true, FLinearColor::Red, FLinearColor::Green, 0.5f))
	{
		if (HitResult->Actor->ActorHasTag("Enemy"))
		{
			ACharacterCommon* EnemyCasted = Cast<ACharacterCommon>(HitResult->Actor);
			EnemyCasted->SetDamage(CurrentAction.DamageValue);
			/*if (nCurrentComboHit >= CurrentAction.Animation->AnimationHits.Num())
			{
				CurrentAttackHasHitObjective = true;
			}*/
			bCurrentHitCollisionIsDone = true;
		}
	}

}

void ACharacterCommon::DoActionAnimation()
{
	if (Actions.Num() > 0 && Actions.Num() >= nCurrentAction + 1)
	{
		FActionStruct Action = Actions[nCurrentAction];
		TArray<FActionAnimationFlagsStruct>& AnimationsFlags = ActionsAnimationsFlags[nCurrentAction].Animations;
		FActionCompleteAnimationStruct& CompleteAction = Action.ActionAnimation[nCurrentActionAnimation];

		// First time in this special move
		if (AnimationActionCompleteTimeStop == 0.f)
		{
			AnimationActionTimeStart = GetCurrentTime();
			for (int i = 0; i < Action.ActionAnimation.Num(); ++i)
			{
				// Start animation is mandatory
				AnimationActionCompleteTimeStop += Action.ActionAnimation[i].AnimationStart.Animation->GetNumFrames() / Action.ActionAnimation[i].AnimationStart.Animation->GetFramesPerSecond();
				if (Action.ActionAnimation[i].AnimationHits.Num() > 0)
				{
					for (int j = 0; j < Action.ActionAnimation[i].AnimationHits.Num(); ++j)
					{
						AnimationActionCompleteTimeStop += Action.ActionAnimation[i].AnimationHits[j].Animation->GetNumFrames() / Action.ActionAnimation[i].AnimationHits[j].Animation->GetFramesPerSecond();
					}
				}
				if (Action.ActionAnimation[i].AnimationEnd.Animation != nullptr)
				{
					AnimationActionCompleteTimeStop += Action.ActionAnimation[i].AnimationEnd.Animation->GetNumFrames() / Action.ActionAnimation[i].AnimationEnd.Animation->GetFramesPerSecond();
				}
			}
			AnimationActionCompleteTimeStop += AnimationActionTimeStart;
		}

		if (AnimationActionTimeStop < GetCurrentTime())
		{
			if (!AnimationsFlags[nCurrentActionAnimation].bIsActionStart)
			{
				CurrentFlipbook->SetFlipbook(CompleteAction.AnimationStart.Animation);
				CurrentFlipbook->SetLooping(false);
				CurrentFlipbook->Play();
				AnimationActionTimeStop = GetCurrentTime() + CurrentFlipbook->GetFlipbookLength();
				nCurrentAnimationInterpolationSpeed = CompleteAction.AnimationStart.InterpolationSpeed;
				if (CompleteAction.AnimationStart.ImpulseToCharacter != FVector(0.f, 0.f, 0.f))
				{
					ActionFinalLocation = GetActorLocation() + CompleteAction.AnimationStart.ImpulseToCharacter;
					GetCapsuleComponent()->SetSimulatePhysics(true);
				}
				ApplyHitCollide(CompleteAction.AnimationStart);
				AnimationsFlags[nCurrentActionAnimation].bIsActionStart = true;

			}
			else if (AnimationsFlags[nCurrentActionAnimation].bIsCompleted && !AnimationsFlags[nCurrentActionAnimation].bIsActionEnd)
			{
				CurrentFlipbook->SetFlipbook(CompleteAction.AnimationEnd.Animation);
				CurrentFlipbook->SetLooping(false);
				CurrentFlipbook->Play();
				AnimationActionTimeStop = GetCurrentTime() + CurrentFlipbook->GetFlipbookLength();
				nCurrentAnimationInterpolationSpeed = CompleteAction.AnimationEnd.InterpolationSpeed;
				if (CompleteAction.AnimationEnd.ImpulseToCharacter != FVector(0.f, 0.f, 0.f))
				{
					ActionFinalLocation = GetActorLocation() + CompleteAction.AnimationEnd.ImpulseToCharacter;
					GetCapsuleComponent()->SetSimulatePhysics(true);
				}
				ApplyHitCollide(CompleteAction.AnimationEnd);
				AnimationsFlags[nCurrentActionAnimation].bIsActionEnd = true;

				if (nCurrentActionAnimation < AnimationsFlags.Num() - 1)
				{
					nCurrentActionHitAnimation = 0;
					++nCurrentActionAnimation;
				}
			}
			else if (!AnimationsFlags[nCurrentActionAnimation].bIsActionHits[nCurrentActionHitAnimation])
			{
				CurrentFlipbook->SetFlipbook(CompleteAction.AnimationHits[nCurrentActionHitAnimation].Animation);
				CurrentFlipbook->SetLooping(false);
				CurrentFlipbook->Play();
				AnimationActionTimeStop = GetCurrentTime() + CurrentFlipbook->GetFlipbookLength();
				nCurrentAnimationInterpolationSpeed = CompleteAction.AnimationHits[nCurrentActionHitAnimation].InterpolationSpeed;
				if (CompleteAction.AnimationHits[nCurrentActionHitAnimation].ImpulseToCharacter != FVector(0.f, 0.f, 0.f))
				{
					ActionFinalLocation = GetActorLocation() + CompleteAction.AnimationHits[nCurrentActionHitAnimation].ImpulseToCharacter;
					GetCapsuleComponent()->SetSimulatePhysics(true);
				}
				ApplyHitCollide(CompleteAction.AnimationHits[nCurrentActionHitAnimation]);
				AnimationsFlags[nCurrentActionAnimation].bIsActionHits[nCurrentActionHitAnimation] = true;
				if (CompleteAction.AnimationHits[nCurrentActionHitAnimation].IsProjectile)
				{
					FTimerDelegate TimerDel;
					FTimerHandle TimerHandle;

					//Binding the function with specific variables
					TimerDel.BindUFunction(this, FName("HandleProjectile"), Action.ActionAnimation[nCurrentActionAnimation].AnimationHits[nCurrentActionHitAnimation].GenericProjectile);
					//Calling MyUsefulFunction after 5 seconds without looping
					GetWorldTimerManager().SetTimer(TimerHandle, TimerDel, 0.1f, false);
				}
				if (nCurrentActionHitAnimation < AnimationsFlags[nCurrentActionAnimation].bIsActionHits.Num() - 1)
				{
					++nCurrentActionHitAnimation;
				}
				else
				{
					AnimationsFlags[nCurrentActionAnimation].bIsCompleted = true;
				}
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::Printf(TEXT("Nothing here")));
			}
		}
	}
}

void ACharacterCommon::NotifyComboToHUD()
{
}

