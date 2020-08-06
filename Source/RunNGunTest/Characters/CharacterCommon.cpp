// Fill out your copyright notice in the Description page of Project Settings.

/*
		// Show debug string in HUD
		if (GameHUD)
		{
		TArray<FString> DebugString;
		DebugString.Add(FString::Printf(TEXT("test: %s"), somethingToShow));
		GameHUD->InsertDebugData(DebugString);
		}

		// Instant message on screen
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, TEXT("Start"));
*/

#include "CharacterCommon.h"

// Sets default values
ACharacterCommon::ACharacterCommon()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CurrentFlipbook = CreateDefaultSubobject<UPaperFlipbookComponent>("CurrentFlipbook");
	CurrentFlipbook->SetupAttachment(GetCapsuleComponent());
	CurrentFlipbook->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	Effect1Flipbook = CreateDefaultSubobject<UPaperFlipbookComponent>("Effect1Flipbook");
	Effect1Flipbook->SetupAttachment(CurrentFlipbook);

	Effect2Flipbook = CreateDefaultSubobject<UPaperFlipbookComponent>("Effect2Flipbook");
	Effect2Flipbook->SetupAttachment(CurrentFlipbook);

	Effect3Flipbook = CreateDefaultSubobject<UPaperFlipbookComponent>("Effect3Flipbook");
	Effect3Flipbook->SetupAttachment(CurrentFlipbook);
}

// Called when the game starts or when spawned
void ACharacterCommon::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	GameHUD = Cast<AGameHUD>(PlayerController->GetHUD());

	UUID = rand() % 1000 + 1;
	SetActionAnimationIsFinished(true);

	ResetActionAnimationFlags();
	SetCanMove(true);


	ActorsToIgnore.Append(GetActorsWithOtherTag("Scenario"));
}

void ACharacterCommon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ControlCharacterRotation();

	if (!GetActionAnimationIsFinished())
	{
		ControlAnimation();
		SetLastActionTime(GetCurrentTime());
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
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, FString::Printf(TEXT("Se acaba getting up")));
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

bool ACharacterCommon::GetCanMove()
{
	return bCanMove;
}

void ACharacterCommon::SetLastActionTime(float Time)
{
	nLastActionTime = Time;
}

float ACharacterCommon::GetLastActionTime()
{
	return nLastActionTime;
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
	if (GetActionAnimationIsFinished())
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
		AnimationActionCurrentFrame = 0;
		ControlAnimation();
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
				ControlAnimation();
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
			ControlAnimation();
		}
	}
}

void ACharacterCommon::HandleProjectile(UObject* Projectile, FVector ProjectilePosition)
{
	UBlueprint* GeneratedBP = Cast<UBlueprint>(Projectile);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	GetWorld()->SpawnActor<AActor>(GeneratedBP->GeneratedClass, ProjectilePosition, GetActorRotation(), SpawnParams);

	//GEngine->AddOnScreenDebugMessage(1, 3.f, FColor::Red, CharacterArrowComponent->GetComponentLocation().ToString());
}

void ACharacterCommon::PrepareProjectile(FActionAnimationStruct CurrentAnimation)
{
	if (CurrentAnimation.IsProjectile)
	{
		FTimerDelegate TimerDelegate;
		FTimerHandle TimerHandle;

		//Binding the function with specific variables
		FVector ProjectilePosition = GetActorLocation() + FaceElement(CurrentAnimation.ProjectileStartPosition);
		TimerDelegate.BindUFunction(this, FName("HandleProjectile"), CurrentAnimation.GenericProjectile, ProjectilePosition);
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
	SetLastActionTime(0.f);
	bIsFirstAttack = true;
	CurrentAttackHasHitObjective = false;
	ResetActionAnimationFlags();
}

void ACharacterCommon::ResetAttack()
{
	bIsAttackFinished = true;
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
	nCurrentAnimationTotalFrames = -1;
	SetCanMove(true);
}

void ACharacterCommon::ResetDamage()
{
	nCurrentAction = 0;
	nCurrentAnimationTotalFrames = -1;
	SetCanMove(true);
	ActionFinalLocation = FVector::ZeroVector;
}

void ACharacterCommon::DoAttack()
{
	SetActionState(EActionState::ActionAttacking);
	if (bIsAttackFinished && GetActionAnimationIsFinished())
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
		ControlAnimation();
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
		if (GetCharacterMovement()->IsMovingOnGround())
		{
			SetAnimationState(EAnimationState::AnimChargingUp);
		}
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
	AnimationActionCurrentFrame = 0;
	AnimationActionCompleteFramesNumber = 0;
	AnimationActionLastFrame = 0;
	RemoveEffectsAnimation();
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
	ControlAnimation();

	Life -= Value;
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, FString::Printf(TEXT("Life: %f"), Life));
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

void ACharacterCommon::ControlAnimation()
{
	// First time
	if (AnimationActionCurrentFrame == 0 || AnimationActionCurrentFrame == AnimationActionLastFrame)
	{
		PrepareAnimation();
	}
	++AnimationActionCurrentFrame;
}

void ACharacterCommon::PrepareAnimation()
{
	if (Actions.Num() > 0 && Actions.Num() >= nCurrentAction + 1)
	{
		FActionStruct Action = Actions[nCurrentAction];
		if (ActionsAnimationsFlags.Num() > 0)
		{
			TArray<FActionAnimationFlagsStruct>& AnimationsFlags = ActionsAnimationsFlags[nCurrentAction].Animations;

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
				SetActionAnimationIsFinished(false);
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
				GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, FString::Printf(TEXT("Total Frames: %i"), AnimationActionLastFrame));
				AnimationActionCurrentFrame = 0;
				AnimationActionLastFrame = 0;
				SetActionAnimationIsFinished(true);
			}

			ApplyCurrentAnimation(Action, AnimationsFlags);
		}
	}
}

void ACharacterCommon::ApplyCurrentAnimation(FActionStruct Action, TArray<FActionAnimationFlagsStruct>& AnimationsFlags)
{
	FActionCompleteAnimationStruct& CompleteAction = Action.ActionAnimation[nCurrentActionAnimation];
	switch (CurrentAnimationState)
	{
	case ECurrentAnimationState::CurrentAnimationStart:
		SetAnimationBehaviour(CompleteAction.AnimationStart);
		ApplyEffectsAnimation(CompleteAction.AnimationStart);
		PrepareProjectile(CompleteAction.AnimationStart);
		AnimationsFlags[nCurrentActionAnimation].bIsActionStart = true;

		break;

	case ECurrentAnimationState::CurrentAnimationCharging:
		if (bIsChargingup)
		{
			AnimationActionLastFrame += Action.ActionAnimation[nCurrentActionAnimation].AnimationCharge.Animation->GetNumFrames();
			RemoveEffectsAnimation();
			ApplyEffectsAnimation(CompleteAction.AnimationCharge);

			SetAnimationBehaviour(CompleteAction.AnimationCharge);
			CurrentFlipbook->SetLooping(true);
		}
		else
		{
			AnimationsFlags[nCurrentActionAnimation].bIsActionCharge = true;
		}
		break;

	case ECurrentAnimationState::CurrentAnimationHit:
		RemoveEffectsAnimation();
		RemoveEffectsAnimation();
		ApplyEffectsAnimation(CompleteAction.AnimationHits[nCurrentActionHitAnimation]);
		SetAnimationBehaviour(CompleteAction.AnimationHits[nCurrentActionHitAnimation]);

		// If character is falling damaged, animation will remain until touches the ground.
		if (ActionState == EActionState::ActionDamaged && !GetCharacterMovement()->IsMovingOnGround())
		{
			//AnimationActionCompleteTimeStop += CurrentFlipbook->GetFlipbookLength();
		}
		else
		{
			AnimationsFlags[nCurrentActionAnimation].bIsActionHits[nCurrentActionHitAnimation] = true;
			PrepareProjectile(CompleteAction.AnimationHits[nCurrentActionHitAnimation]);

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
		RemoveEffectsAnimation();
		ApplyEffectsAnimation(CompleteAction.AnimationEnd);
		SetAnimationBehaviour(CompleteAction.AnimationEnd);

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

void ACharacterCommon::ApplyEffectsAnimation(FActionAnimationStruct Action)
{
	if (Action.AnimationEffects.AnimationEffect1 != nullptr)
	{
		Effect1Flipbook->SetFlipbook(Action.AnimationEffects.AnimationEffect1);
		Effect1Flipbook->SetRelativeLocation(Action.AnimationEffects.Effect1Position);
		Effect1Flipbook->SetVisibility(true);
	}
	if (Action.AnimationEffects.AnimationEffect2 != nullptr)
	{
		Effect2Flipbook->SetFlipbook(Action.AnimationEffects.AnimationEffect2);
		Effect2Flipbook->SetRelativeLocation(Action.AnimationEffects.Effect2Position);
		Effect2Flipbook->SetVisibility(true);
	}
	if (Action.AnimationEffects.AnimationEffect3 != nullptr)
	{
		Effect3Flipbook->SetFlipbook(Action.AnimationEffects.AnimationEffect3);
		Effect3Flipbook->SetRelativeLocation(Action.AnimationEffects.Effect3Position);
		Effect3Flipbook->SetVisibility(true);
	}
}

void ACharacterCommon::RemoveEffectsAnimation()
{
	Effect1Flipbook->SetVisibility(false);
	Effect2Flipbook->SetVisibility(false);
	Effect3Flipbook->SetVisibility(false);
}

void ACharacterCommon::SetAnimationBehaviour(FActionAnimationStruct AnimationStruct)
{
	SetCanMove(AnimationStruct.CanMove);
	CurrentFlipbook->SetFlipbook(AnimationStruct.Animation);
	CurrentFlipbook->SetLooping(false);
	CurrentFlipbook->Play();
	AnimationActionLastFrame += CurrentFlipbook->GetFlipbookLengthInFrames();

	nCurrentAnimationInterpolationSpeed = AnimationStruct.InterpolationSpeed;
	if (AnimationStruct.ImpulseToOwner != FVector::ZeroVector)
	{
		GetCharacterMovement()->SetMovementMode(Actions[nCurrentAction].HitMode);
		GetCharacterMovement()->Velocity.Z = FMath::Max(0.f, AnimationStruct.ImpulseToOwner.Z * 100.f);
		GetCharacterMovement()->Velocity.X = GetFacingX(AnimationStruct.ImpulseToOwner.X);
	}
	ApplyHitCollide(AnimationStruct);
}

void ACharacterCommon::SetActionAnimationIsFinished(bool IsFinished)
{
	bActionAnimationIsFinished = IsFinished;
}

bool ACharacterCommon::GetActionAnimationIsFinished()
{
	return bActionAnimationIsFinished;
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

