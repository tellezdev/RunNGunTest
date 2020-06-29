// Fill out your copyright notice in the Description page of Project Settings.

/*
		// Show debug string in HUD
		TArray<FString> DebugString;
		DebugString.Add(FString::Printf(TEXT("test: %s"), somethingToShow));
		GameHUD->InsertDebugData(DebugString);
*/

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
	AnimationActionCurrentTimeStart = -1.f;
	AnimationActionCurrentTimeStop = -1.f;
	AnimationActionCompleteTimeStop = 0.f;
	SetCanMove(true);
}

void ACharacterCommon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ControlCharacterRotation();

	SetDamagedState(DeltaTime);

	if (bIsExecutingSpecialMove && ActionFinalLocation != FVector(0.f, 0.f, 0.f))
	{
		float XInterp = FMath::FInterpConstantTo(GetActorLocation().X, ActionFinalLocation.X, DeltaTime, nCurrentAnimationInterpolationSpeed * 100.f);
		float ZInterp = FMath::FInterpConstantTo(GetActorLocation().Z, ActionFinalLocation.Z, DeltaTime, nCurrentAnimationInterpolationSpeed * 100.f);
		SetActorLocation(FVector(XInterp, 0.f, ZInterp));
	}

	if (AnimationActionCompleteTimeStop > GetCurrentTime())
	{
		DoActionAnimation();
		nLastActionTime = GetCurrentTime();
	}
	else
	{
		if (bIsSpecialMove)
		{
			ResetSpecialMove();
		}
		if (bIsAttacking)
		{
			ResetAttack();
		}
		if (bIsChargingup)
		{
			ResetChargingUp();
		}

		CurrentFlipbook->SetLooping(true);
		CurrentFlipbook->Play();
		SetAnimationState(Idle);
		ResetActionAnimationFlags();
	}
}

void ACharacterCommon::SetCanMove(bool State)
{
	bCanMove = State;
}

bool ACharacterCommon::CanMove()
{
	return bCanMove;
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
	if (AnimationActionCompleteTimeStop < GetCurrentTime())
	{
		if (GetCharacterMovement()->IsMovingOnGround())
		{
			Actions = AttackMoves;
			SetCanMove(false);
		}
		else
		{
			Actions = JumpAttackMoves;
			ResetAttackCombo();
			SetCanMove(true);
			//InputBuffer.ClearBuffer();
		}
		SetActionAnimationFlags();

		nCurrentActionAnimation = 0;
		bCurrentHitCollisionIsDone = false;
		DoActionAnimation();
		bIsFirstAttack = false;
	}
}

void ACharacterCommon::HandleSpecialMoves()
{
	if (!bIsExecutingSpecialMove)
	{
		bIsExecutingSpecialMove = true;
		SetCanMove(false);
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
	bCurrentHitCollisionIsDone = false;
	nCurrentFrame = -1;
	nCurrentAnimationTotalFrames = -1;
	SetCanMove(true);
}

void ACharacterCommon::ResetSpecialMove()
{
	bIsSpecialMove = false;
	nCurrentAction = 0;
	nCurrentActionAnimation = 0;
	nCurrentActionHitAnimation = 0;
	ActionsAnimationsFlags.Empty();
	bIsExecutingSpecialMove = false;
	nCurrentFrame = -1;
	nCurrentAnimationTotalFrames = -1;
	SetCanMove(true);
	ActionFinalLocation = FVector(0.f, 0.f, 0.f);
	GetCapsuleComponent()->SetSimulatePhysics(false);
}

void ACharacterCommon::ResetChargingUp()
{
	bIsChargingup = false;
	nCurrentAction = 0;
	nCurrentActionAnimation = 0;
	nCurrentActionHitAnimation = 0;
	ActionsAnimationsFlags.Empty();
	nCurrentFrame = -1;
	nCurrentAnimationTotalFrames = -1;
	SetCanMove(true);
}

FVector ACharacterCommon::GetFacingVector(FVector OriginalVector)
{
	FVector FinalVector = OriginalVector;
	if (!bIsMovingRight)
	{
		FinalVector = FVector(OriginalVector.X * -1, OriginalVector.Y, OriginalVector.Z);
	}
	return FinalVector;
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
	AnimationActionCurrentTimeStop = 0.f;
	AnimationActionCompleteTimeStop = 0.f;
	SetActionAnimationFlags();
}

void ACharacterCommon::SetAnimationState(AnimationState State)
{
	EAnimationState = State;
}

float ACharacterCommon::GetCurrentTime()
{
	return GetWorld()->GetRealTimeSeconds();
}

void ACharacterCommon::SetDamage(float Value)
{
	bIsDamaged = true;
	SetAnimationState(HitTop);
	AnimationOtherTimeStop = GetCurrentTime() + CurrentFlipbook->GetFlipbookLength();
	if (bIsAttacking)
	{
		ResetAttack();
	}
	if (bIsSpecialMove)
	{
		ResetSpecialMove();
	}
	if (bIsChargingup)
	{
		ResetChargingUp();
	}
	Life -= Value;
}

void ACharacterCommon::HealLife(float Value)
{
	if (Life < MaxLife)
	{
		Life += Value;
		GameHUD->SetLife(Life);
	}
}

void ACharacterCommon::HealStamina(float Value)
{
}

void ACharacterCommon::DrainLife()
{
	Life = 0.1f;
	GameHUD->SetLife(Life);
}

void ACharacterCommon::DrainStamina()
{
}

void ACharacterCommon::ApplyHitCollide(FActionAnimationStruct CurrentAction)
{
	FVector Hitbox = GetFacingVector(CurrentAction.HitBoxPosition);
	// Hit box will grow to detect collision
	CurrentTraceHit = FVector(GetActorLocation().X + Hitbox.X, GetActorLocation().Y + Hitbox.Y, GetActorLocation().Z + Hitbox.Z);

	// Tracing collision
	FHitResult* HitResult = new FHitResult();
	if (!bCurrentHitCollisionIsDone && UKismetSystemLibrary::BoxTraceSingle(GetWorld(), CurrentTraceHit, CurrentTraceHit, CurrentAction.HitBoxSize, GetActorRotation(), ETraceTypeQuery::TraceTypeQuery2, false, ActorsToIgnore, EDrawDebugTrace::ForDuration, *HitResult, true, FLinearColor::Red, FLinearColor::Green, 0.5f))
	{
		if (HitResult->Actor->ActorHasTag("Damageable"))
		{
			ACharacterCommon* ActorReceiver = Cast<ACharacterCommon>(HitResult->Actor);
			ActorReceiver->SetDamage(CurrentAction.DamageValue);

			//bCurrentHitCollisionIsDone = true;
		}
	}
}

void ACharacterCommon::DoActionAnimation()
{
	if (Actions.Num() > 0 && Actions.Num() >= nCurrentAction + 1)
	{


		TArray<FString> DebugString; //DEBUG

		FActionStruct Action = Actions[nCurrentAction];
		if (ActionsAnimationsFlags.Num() > 0)
		{
			TArray<FActionAnimationFlagsStruct>& AnimationsFlags = ActionsAnimationsFlags[nCurrentAction].Animations;
			FActionCompleteAnimationStruct& CompleteAction = Action.ActionAnimation[nCurrentActionAnimation];

			//First time in this special move
			if (AnimationActionCompleteTimeStop == 0.f)
			{
				AnimationActionCurrentTimeStart = GetCurrentTime();
				for (int i = 0; i < Action.ActionAnimation.Num(); ++i)
				{
					// Start animation is mandatory
					AnimationActionCompleteTimeStop += Action.ActionAnimation[i].AnimationStart.Animation->GetNumFrames() / Action.ActionAnimation[i].AnimationStart.Animation->GetFramesPerSecond();
					if (Action.ActionAnimation[i].AnimationHits.Num() > 0)
					{
						/*for (int j = 0; j < Action.ActionAnimation[i].AnimationHits.Num(); ++j)
						{
							AnimationActionCompleteTimeStop += Action.ActionAnimation[i].AnimationHits[j].Animation->GetNumFrames() / Action.ActionAnimation[i].AnimationHits[j].Animation->GetFramesPerSecond();
						}*/
					}
					if (Action.ActionAnimation[i].AnimationEnd.Animation != nullptr)
					{
						AnimationActionCompleteTimeStop += Action.ActionAnimation[i].AnimationEnd.Animation->GetNumFrames() / Action.ActionAnimation[i].AnimationEnd.Animation->GetFramesPerSecond();
					}
				}
				AnimationActionCompleteTimeStop += AnimationActionCurrentTimeStart;
			}

			if (AnimationActionCurrentTimeStop < GetCurrentTime())
			{
				if (!Action.CanBeCharged)
				{
					AnimationsFlags[nCurrentActionAnimation].bIsActionCharge;
				}

				if (!AnimationsFlags[nCurrentActionAnimation].bIsActionStart)
				{
					SetAnimationBehaviour(CompleteAction.AnimationStart, false);
					AnimationsFlags[nCurrentActionAnimation].bIsActionStart = true;
				}
				else if (AnimationsFlags[nCurrentActionAnimation].bIsCompleted && !AnimationsFlags[nCurrentActionAnimation].bIsActionEnd)
				{
					SetAnimationBehaviour(CompleteAction.AnimationEnd, false);
					AnimationsFlags[nCurrentActionAnimation].bIsActionEnd = true;

					if (nCurrentActionAnimation < AnimationsFlags.Num() - 1)
					{
						nCurrentActionHitAnimation = 0;
						++nCurrentActionAnimation;
					}

				}
				else if (!AnimationsFlags[nCurrentActionAnimation].bIsActionCharge && Action.CanBeCharged)
				{
					if (bIsChargingup)
					{
						SetAnimationBehaviour(CompleteAction.AnimationCharge, true);
						AnimationActionCompleteTimeStop += CurrentFlipbook->GetFlipbookLength();
					}
					else
					{
						AnimationsFlags[nCurrentActionAnimation].bIsActionCharge = true;
					}

				}
				else if (!AnimationsFlags[nCurrentActionAnimation].bIsActionHits[nCurrentActionHitAnimation])
				{
					SetAnimationBehaviour(CompleteAction.AnimationHits[nCurrentActionHitAnimation], true);
					AnimationsFlags[nCurrentActionAnimation].bIsActionHits[nCurrentActionHitAnimation] = true;
					AnimationActionCompleteTimeStop += CurrentFlipbook->GetFlipbookLength();

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




			DebugString.Add(FString::Printf(TEXT("currentflipbook: %s"), *CurrentFlipbook->GetFlipbook()->GetName())); //DEBUG
			DebugString.Add(FString::Printf(TEXT("TimeStop: %f"), AnimationActionCurrentTimeStop)); //DEBUG
			DebugString.Add(FString::Printf(TEXT("CurrentTime: %f"), GetCurrentTime())); //DEBUG
			GameHUD->InsertDebugData(DebugString); //DEBUG
		}
	}
}

void ACharacterCommon::SetAnimationBehaviour(FActionAnimationStruct AnimationStruct, bool bIsLoop)
{
	CurrentFlipbook->SetFlipbook(AnimationStruct.Animation);
	CurrentFlipbook->SetLooping(bIsLoop);
	CurrentFlipbook->Play();
	nCurrentFrame = 0;
	AnimationActionCurrentTimeStop = GetCurrentTime() + CurrentFlipbook->GetFlipbookLength();
	nCurrentAnimationInterpolationSpeed = AnimationStruct.InterpolationSpeed;
	if (AnimationStruct.ImpulseToOwner != FVector(0.f, 0.f, 0.f))
	{
		ActionFinalLocation = GetActorLocation() + GetFacingVector(AnimationStruct.ImpulseToOwner);
		//GetCapsuleComponent()->SetSimulatePhysics(true);

	}
	ApplyHitCollide(AnimationStruct);
}

void ACharacterCommon::NotifyComboToHUD()
{
}

