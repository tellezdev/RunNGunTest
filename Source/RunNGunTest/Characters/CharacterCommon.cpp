// Fill out your copyright notice in the Description page of Project Settings.

/*
		// Show debug string in HUD
		if (GameHUD)
		{
		TArray<FString> DebugString;
		DebugString.Add(FString::Printf(TEXT("test: %s"), somethingToShow));
		GameHUD->InsertDebugData(DebugString);
		}
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

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	GameHUD = Cast<AGameHUD>(PlayerController->GetHUD());

	UUID = rand() % 1000 + 1;
	AnimationFlipbookTimeStop = -1.f;
	AnimationActionCurrentTimeStart = -1.f;
	AnimationActionCurrentTimeStop = -1.f;
	AnimationActionCompleteTimeStop = 0.f;

	ResetActionAnimationFlags();
	SetCanMove(true);


	ActorsToIgnore.Append(GetActorsWithOtherTag("Scenario"));
}

void ACharacterCommon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	bActionAnimationIsFinished = AnimationActionCompleteTimeStop < GetCurrentTime();

	ControlCharacterRotation();

	if (!bActionAnimationIsFinished)
	{
		PrepareAnimation();
		nLastActionTime = GetCurrentTime();
	}
	else
	{
		switch (ActionState)
		{
		case EActionState::ActionIdle:
			break;
		case EActionState::ActionDucking:
			break;
		case EActionState::ActionDamaged:
			if (GetCharacterMovement()->IsFlying())
			{
				GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
			}
			if (GetCharacterMovement()->IsMovingOnGround())
			{
				SetActionState(EActionState::ActionGettingUp);
				ResetActionAnimationFlags();
				SetCanMove(false);
			}
			break;
		case EActionState::ActionGettingUp:
			SetActionState(EActionState::ActionIdle);
			ResetActionAnimationFlags();
			break;
		case EActionState::ActionChargingup:
			SetActionState(EActionState::ActionIdle);
			ResetActionAnimationFlags();
			break;
		default:
			SetActionState(EActionState::ActionIdle);
			ResetActionAnimationFlags();
			break;
		}
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

void ACharacterCommon::BindDataHUD()
{
}

void ACharacterCommon::HandleAttack()
{
	if (bActionAnimationIsFinished)
	{
		if (GetCharacterMovement()->IsMovingOnGround())
		{
			Actions = AttackMoves;
			//SetCanMove(false);
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
		PrepareAnimation();
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
				PrepareAnimation();
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
			PrepareAnimation();
		}
	}
}

void ACharacterCommon::HandleProjectile(UObject* Projectile)
{
	UBlueprint* GeneratedBP = Cast<UBlueprint>(Projectile);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	GetWorld()->SpawnActor<AActor>(GeneratedBP->GeneratedClass, GetActorLocation(), GetActorRotation(), SpawnParams);

	//GEngine->AddOnScreenDebugMessage(1, 3.f, FColor::Red, CharacterArrowComponent->GetComponentLocation().ToString());
}

void ACharacterCommon::PrepareProjectile(FActionAnimationStruct CurrentAnimation)
{
	if (CurrentAnimation.IsProjectile)
	{
		FTimerDelegate TimerDelegate;
		FTimerHandle TimerHandle;

		//Binding the function with specific variables
		TimerDelegate.BindUFunction(this, FName("HandleProjectile"), CurrentAnimation.GenericProjectile);
		//Calling MyUsefulFunction after 5 seconds without looping
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 0.1f, false);
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
	CurrentAttackHasHitObjective = false;
	ResetActionAnimationFlags();
}

void ACharacterCommon::ResetAttack()
{
	bIsAttackFinished = true;
	nCurrentFrame = -1;
	nCurrentAnimationTotalFrames = -1;
	CurrentAttackHasHitObjective = false;
	SetCanMove(true);
}

void ACharacterCommon::ResetSpecialMove()
{
	nCurrentAction = 0;
	nCurrentActionAnimation = 0;
	nCurrentActionHitAnimation = 0;
	ActionsAnimationsFlags.Empty();
	bIsExecutingSpecialMove = false;
	nCurrentFrame = -1;
	nCurrentAnimationTotalFrames = -1;
	CurrentAttackHasHitObjective = false;
	SetCanMove(true);
	ActionFinalLocation = FVector::ZeroVector;
}

void ACharacterCommon::ResetChargingUp()
{
	nCurrentAction = 0;
	nCurrentActionAnimation = 0;
	nCurrentActionHitAnimation = 0;
	ActionsAnimationsFlags.Empty();
	nCurrentFrame = -1;
	nCurrentAnimationTotalFrames = -1;
	SetCanMove(true);
}

void ACharacterCommon::ResetDamage()
{
	nCurrentAction = 0;
	nCurrentFrame = -1;
	nCurrentAnimationTotalFrames = -1;
	SetCanMove(true);
	ActionFinalLocation = FVector::ZeroVector;
}

void ACharacterCommon::DoAttack()
{
	SetActionState(EActionState::ActionAttacking);
	if (bIsAttackFinished && bActionAnimationIsFinished)
	{
		bIsAttackFinished = false;
		if (nCurrentAction < AttackMoves.Num() - 1)
		{
			if (!bIsFirstAttack)
			{
				++nCurrentAction;
			}
			else
			{
				nCurrentAction = 0;
			}
			nCurrentActionAnimation = 0;
		}
		else
		{
			ResetAttackCombo();
		}
		HandleAttack();
		/*++ComboCount;*/
	}
}

void ACharacterCommon::UpdateAnimations()
{
	switch (AnimationState)
	{
	case EAnimationState::AnimIdle:
		CurrentFlipbook->SetFlipbook(IdleAnimation);
		break;
	case EAnimationState::AnimWalking:
		CurrentFlipbook->SetFlipbook(WalkingAnimation);
		break;
	case EAnimationState::AnimJumping:
		CurrentFlipbook->SetFlipbook(JumpingAnimation);
		break;
	case EAnimationState::AnimJumpingForward:
		CurrentFlipbook->SetFlipbook(JumpingForwardAnimation);
		break;
	case EAnimationState::AnimDucking:
		CurrentFlipbook->SetFlipbook(DuckingAnimation);
		break;
	case EAnimationState::AnimSpecialMove:
		break;
	case EAnimationState::AnimAttacking:
		break;
	case EAnimationState::AnimChargingUp:
		break;
	case EAnimationState::AnimDamage:
		break;
	case EAnimationState::AnimDamageAir:
		break;
	case EAnimationState::AnimGettingUp:
		Actions = DamageAnimations;
		nCurrentAction = 2;
		PrepareAnimation();
		break;
	default:
		CurrentFlipbook->SetFlipbook(IdleAnimation);
		break;
	}
}

void ACharacterCommon::ControlCharacterAnimations(float characterMovementSpeed)
{
	switch (ActionState)
	{
	case EActionState::ActionAttacking:
		SetAnimationState(EAnimationState::AnimAttacking);
		break;
	case EActionState::ActionChargingup:
		SetAnimationState(EAnimationState::AnimChargingUp);
		break;
	case EActionState::ActionDamaged:
		if (!GetCharacterMovement()->IsMovingOnGround())
		{
			SetAnimationState(EAnimationState::AnimDamageAir);
		}
		else
		{
			SetAnimationState(EAnimationState::AnimDamage);
		}
		break;
	case EActionState::ActionGettingUp:
		SetAnimationState(EAnimationState::AnimGettingUp);
		break;
	case EActionState::ActionDucking:
		SetAnimationState(EAnimationState::AnimDucking);
		break;
	case EActionState::ActionIdle:
		if (!GetCharacterMovement()->IsMovingOnGround())
		{
			GetCharacterMovement()->SetMovementMode(MOVE_Falling);
			if (fabs(characterMovementSpeed) > 0.0f)
			{
				SetAnimationState(EAnimationState::AnimJumpingForward);
			}
			else
			{
				SetAnimationState(EAnimationState::AnimJumping);
			}
		}
		else
		{
			GetCharacterMovement()->SetMovementMode(MOVE_Walking);
			if (fabs(characterMovementSpeed) > 0.0f)
			{
				SetAnimationState(EAnimationState::AnimWalking);
			}
			else
			{
				SetAnimationState(EAnimationState::AnimIdle);
			}
		}
		break;
	case EActionState::ActionSpecialMove:
		SetAnimationState(EAnimationState::AnimSpecialMove);
		break;
	default:
		break;
	}

	if (ActionState != EActionState::ActionSpecialMove)
	{
		CurrentFlipbook->SetLooping(true);
		CurrentFlipbook->Play();
	}
	UpdateAnimations();
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

void ACharacterCommon::SetAnimationState(EAnimationState State)
{
	AnimationState = State;
}

float ACharacterCommon::GetCurrentTime()
{
	return GetWorld()->GetRealTimeSeconds();
}

void ACharacterCommon::SetDamage(float Value)
{
	SetActionState(EActionState::ActionDamaged);

	Actions = DamageAnimations;
	if (GetCharacterMovement()->IsMovingOnGround())
	{
		nCurrentAction = 0;
	}
	else
	{
		nCurrentAction = 1;
	}
	SetCanMove(false);
	SetActionAnimationFlags();
	PrepareAnimation();

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
	FVector Hitbox = FaceElement(CurrentAction.HitBoxPosition);
	// Hit box will grow to detect collision
	CurrentTraceHit = FVector(GetActorLocation().X + Hitbox.X, GetActorLocation().Y + Hitbox.Y, GetActorLocation().Z + Hitbox.Z);

	// Tracing collision
	TArray<FHitResult>* HitResults = new TArray<FHitResult>();
	if (UKismetSystemLibrary::BoxTraceMulti(GetWorld(), CurrentTraceHit, CurrentTraceHit, CurrentAction.HitBoxSize, GetActorRotation(), ETraceTypeQuery::TraceTypeQuery2, false, ActorsToIgnore, EDrawDebugTrace::ForDuration, *HitResults, true, FLinearColor::Red, FLinearColor::Green, 0.5f))
	{
		for (const FHitResult& Result : *HitResults)
		{
			if (Result.Actor != nullptr && Result.Actor->ActorHasTag("Damageable") && !CurrentAttackHasHitObjective)
			{
				ACharacterCommon* ActorReceiver = Cast<ACharacterCommon>(Result.Actor);
				if (CurrentAction.ImpulseToReceiver != FVector::ZeroVector)
				{
					EMovementMode ModeToReceiver;
					if (Actions[nCurrentAction].HitMode == EMovementMode::MOVE_Flying)
					{
						ModeToReceiver = MOVE_Falling;
					}
					else
					{
						ModeToReceiver = Actions[nCurrentAction].HitMode;
					}
					ActorReceiver->GetCharacterMovement()->SetMovementMode(ModeToReceiver);
					ActorReceiver->GetCharacterMovement()->Velocity.Z = FMath::Max(0.f, CurrentAction.ImpulseToReceiver.Z * 100.f);
					ActorReceiver->GetCharacterMovement()->Velocity.X = GetFacingWhenHit(CurrentAction.ImpulseToReceiver.X, GetActorLocation(), ActorReceiver->GetActorLocation());
				}
				ActorReceiver->SetDamage(CurrentAction.DamageValue);
				CurrentAttackHasHitObjective = true;
				++ComboCount;
			}
		}
		CurrentAttackHasHitObjective = false;
	}
}

void ACharacterCommon::PrepareAnimation()
{
	if (Actions.Num() > 0 && Actions.Num() >= nCurrentAction + 1)
	{
		FActionStruct Action = Actions[nCurrentAction];
		if (ActionsAnimationsFlags.Num() > 0)
		{
			TArray<FActionAnimationFlagsStruct>& AnimationsFlags = ActionsAnimationsFlags[nCurrentAction].Animations;

			// Takes the duration of start and end animations
			if (AnimationActionCompleteTimeStop == 0.f)
			{
				AnimationActionCurrentTimeStart = GetCurrentTime();
				for (int i = 0; i < Action.ActionAnimation.Num(); ++i)
				{
					// Start animation is mandatory
					AnimationActionCompleteTimeStop += Action.ActionAnimation[i].AnimationStart.Animation->GetNumFrames() / Action.ActionAnimation[i].AnimationStart.Animation->GetFramesPerSecond();
					if (Action.ActionAnimation[i].AnimationEnd.Animation != nullptr)
					{
						AnimationActionCompleteTimeStop += Action.ActionAnimation[i].AnimationEnd.Animation->GetNumFrames() / Action.ActionAnimation[i].AnimationEnd.Animation->GetFramesPerSecond();
					}
				}
				AnimationActionCompleteTimeStop += AnimationActionCurrentTimeStart;
			}

			if (AnimationActionCurrentTimeStop < GetCurrentTime())
			{
				// Initial verifications
				if (!Action.CanBeCharged || !bIsChargingup)
				{
					AnimationsFlags[nCurrentActionAnimation].bIsActionCharge = true;
				}

				if (AnimationsFlags[nCurrentActionAnimation].bIsActionHits.Num() == 0)
				{
					AnimationsFlags[nCurrentActionAnimation].bIsCompleted = true;
				}


				if (!AnimationsFlags[nCurrentActionAnimation].bIsActionStart)
				{
					CurrentAnimationState = ECurrentAnimationState::CurrentAnimationStart;
					GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, TEXT("Start"));
				}
				else if (AnimationsFlags[nCurrentActionAnimation].bIsCompleted && !AnimationsFlags[nCurrentActionAnimation].bIsActionEnd)
				{
					CurrentAnimationState = ECurrentAnimationState::CurrentAnimationEnd;
					GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, TEXT("End"));
				}
				else if (!AnimationsFlags[nCurrentActionAnimation].bIsActionCharge && Action.CanBeCharged)
				{
					CurrentAnimationState = ECurrentAnimationState::CurrentAnimationCharging;
					GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, TEXT("Charging"));
				}
				else if (AnimationsFlags[nCurrentActionAnimation].bIsActionHits.Num() > 0 &&
						 !AnimationsFlags[nCurrentActionAnimation].bIsActionHits[nCurrentActionHitAnimation])
				{
					CurrentAnimationState = ECurrentAnimationState::CurrentAnimationHit;
					GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, TEXT("Hit"));
				}
				else
				{
					AnimationActionCompleteTimeStop = 0.f;
				}

				ApplyCurrentAnimation(Action, AnimationsFlags);
			}
		}
	}
}

void ACharacterCommon::ApplyCurrentAnimation(FActionStruct Action, TArray<FActionAnimationFlagsStruct>& AnimationsFlags)
{
	FActionCompleteAnimationStruct& CompleteAction = Action.ActionAnimation[nCurrentActionAnimation];
	switch (CurrentAnimationState)
	{
	case ECurrentAnimationState::CurrentAnimationStart:
		SetAnimationBehaviour(CompleteAction.AnimationStart, false);
		PrepareProjectile(CompleteAction.AnimationStart);
		AnimationsFlags[nCurrentActionAnimation].bIsActionStart = true;

		break;

	case ECurrentAnimationState::CurrentAnimationCharging:
		if (bIsChargingup)
		{
			SetAnimationBehaviour(CompleteAction.AnimationCharge, true);
			AnimationActionCompleteTimeStop += CompleteAction.AnimationCharge.Animation->GetNumFrames() / CompleteAction.AnimationCharge.Animation->GetFramesPerSecond() + 1;
		}
		else
		{
			AnimationsFlags[nCurrentActionAnimation].bIsActionCharge = true;
		}
		break;

	case ECurrentAnimationState::CurrentAnimationHit:
		SetAnimationBehaviour(CompleteAction.AnimationHits[nCurrentActionHitAnimation], true);

		// If character is falling damaged, animation will remain until touches the ground.
		if (ActionState == EActionState::ActionDamaged && !GetCharacterMovement()->IsMovingOnGround())
		{
			AnimationActionCompleteTimeStop += CurrentFlipbook->GetFlipbookLength();
		}
		else
		{
			AnimationsFlags[nCurrentActionAnimation].bIsActionHits[nCurrentActionHitAnimation] = true;
			AnimationActionCompleteTimeStop += CurrentFlipbook->GetFlipbookLength();
			PrepareProjectile(CompleteAction.AnimationHits[nCurrentActionHitAnimation]);

			//if (CompleteAction.AnimationHits[nCurrentActionHitAnimation].IsProjectile)
			//{
			//	FTimerDelegate TimerDelegate;
			//	FTimerHandle TimerHandle;

			//	//Binding the function with specific variables
			//	TimerDelegate.BindUFunction(this, FName("HandleProjectile"), Action.ActionAnimation[nCurrentActionAnimation].AnimationHits[nCurrentActionHitAnimation].GenericProjectile);
			//	//Calling MyUsefulFunction after 5 seconds without looping
			//	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 0.1f, false);

			//}

			if (nCurrentActionHitAnimation < AnimationsFlags[nCurrentActionAnimation].bIsActionHits.Num() - 1)
			{
				++nCurrentActionHitAnimation;
			}
			else
			{
				AnimationsFlags[nCurrentActionAnimation].bIsCompleted = true;
			}
		}
		break;

	case ECurrentAnimationState::CurrentAnimationEnd:
		SetAnimationBehaviour(CompleteAction.AnimationEnd, false);
		PrepareProjectile(CompleteAction.AnimationEnd);
		AnimationsFlags[nCurrentActionAnimation].bIsActionEnd = true;

		if (nCurrentActionAnimation < AnimationsFlags.Num() - 1)
		{
			nCurrentActionHitAnimation = 0;
			++nCurrentActionAnimation;
		}
		break;
	}
}

void ACharacterCommon::SetAnimationBehaviour(FActionAnimationStruct AnimationStruct, bool bIsLoop)
{
	SetCanMove(AnimationStruct.CanMove);
	CurrentFlipbook->SetFlipbook(AnimationStruct.Animation);
	CurrentFlipbook->SetLooping(bIsLoop);
	CurrentFlipbook->Play();
	nCurrentFrame = 0;
	AnimationActionCurrentTimeStop = GetCurrentTime() + CurrentFlipbook->GetFlipbookLength();
	nCurrentAnimationInterpolationSpeed = AnimationStruct.InterpolationSpeed;
	if (AnimationStruct.ImpulseToOwner != FVector::ZeroVector)
	{
		GetCharacterMovement()->SetMovementMode(Actions[nCurrentAction].HitMode);
		GetCharacterMovement()->Velocity.Z = FMath::Max(0.f, AnimationStruct.ImpulseToOwner.Z * 100.f);
		GetCharacterMovement()->Velocity.X = GetFacingX(AnimationStruct.ImpulseToOwner.X);
	}
	ApplyHitCollide(AnimationStruct);
}

void ACharacterCommon::NotifyComboToHUD()
{
}

void ACharacterCommon::SetActionState(EActionState State)
{
	switch (ActionState)
	{
	case EActionState::ActionAttacking:
		if (State != EActionState::ActionAttacking)
		{
			ResetAttack();
		}
		break;
	case EActionState::ActionChargingup:
		ResetChargingUp();
		break;
	case EActionState::ActionDamaged:
		if (State != ActionState)
		{
			ResetDamage();
		}
		break;
	case EActionState::ActionGettingUp:
		ResetDamage();
		break;
	case EActionState::ActionDucking:

		break;
	case EActionState::ActionIdle:
		break;
	case EActionState::ActionSpecialMove:
		ResetSpecialMove();
		break;
	default:
		break;
	}

	ActionState = State;
}

float ACharacterCommon::GetFacingX(float Impulse)
{
	if (!bIsMovingRight)
	{
		return FMath::Min(0.f, Impulse * -100.f);
	}
	return FMath::Max(0.f, Impulse * 100.f);
}

float ACharacterCommon::GetFacingWhenHit(float Impulse, FVector OwnerPosition, FVector ReceiverPosition)
{
	if (OwnerPosition.X > ReceiverPosition.X)
	{
		return FMath::Min(0.f, Impulse * -100.f);
	}
	return FMath::Max(0.f, Impulse * 100.f);
}

FVector ACharacterCommon::FaceElement(FVector Vector)
{
	FVector FinalVector = Vector;
	if (!bIsMovingRight)
	{
		FinalVector = FVector(Vector.X * -1, Vector.Y, Vector.Z);
	}
	return FinalVector;
}

TArray<AActor*> ACharacterCommon::GetActorsWithOtherTag(FName Tag)
{
	TArray<AActor*> Out;
	for (TActorIterator<AActor> ActorIterator(GetWorld()); ActorIterator; ++ActorIterator)
	{
		AActor* Actor = *ActorIterator;
		if (Actor->ActorHasTag(Tag))
		{
			Out.Add(Actor);
		}
	}
	return Out;
}

