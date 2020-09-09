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
	CollisionSystem = CreateDefaultSubobject<UCollisionSystem>("CollisionSystem");

	// Character flipbook
	CurrentFlipbook = CreateDefaultSubobject<UPaperFlipbookComponent>("CurrentFlipbook");
	CurrentFlipbook->SetupAttachment(GetCapsuleComponent());
	CurrentFlipbook->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	// Collision flipbooks
	DamageSprite = CreateDefaultSubobject<UPaperSpriteComponent>("DamageSprite");
	DamageSprite->SetGenerateOverlapEvents(true);
	DamageSprite->SetupAttachment(CurrentFlipbook);

	HitSprite = CreateDefaultSubobject<UPaperSpriteComponent>("HitSprite");
	HitSprite->SetGenerateOverlapEvents(true);
	HitSprite->SetupAttachment(CurrentFlipbook);
	HitSprite->OnComponentBeginOverlap.AddDynamic(this, &ACharacterCommon::OnOverlapBegin);

	WorldSpaceSprite = CreateDefaultSubobject<UPaperSpriteComponent>("WorldSpaceSprite");
	WorldSpaceSprite->SetupAttachment(CurrentFlipbook);

	// Effects flipbooks
	Effect1Flipbook = CreateDefaultSubobject<UPaperFlipbookComponent>("Effect1Flipbook");
	Effect1Flipbook->SetupAttachment(CurrentFlipbook);

	Effect2Flipbook = CreateDefaultSubobject<UPaperFlipbookComponent>("Effect2Flipbook");
	Effect2Flipbook->SetupAttachment(CurrentFlipbook);

	Effect3Flipbook = CreateDefaultSubobject<UPaperFlipbookComponent>("Effect3Flipbook");
	Effect3Flipbook->SetupAttachment(CurrentFlipbook);

	/* Initial settings */
	GetCapsuleComponent()->SetCollisionObjectType(ECC_Pawn);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);

	DamageSprite->SetCollisionResponseToAllChannels(ECR_Ignore);
	DamageSprite->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	DamageSprite->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	HitSprite->SetCollisionResponseToAllChannels(ECR_Ignore);
	HitSprite->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	HitSprite->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	WorldSpaceSprite->SetCollisionResponseToAllChannels(ECR_Ignore);
	WorldSpaceSprite->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	WorldSpaceSprite->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
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

	DamageSprite->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	DamageSprite->ComponentTags.Add("DamageSprite");

	HitSprite->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	HitSprite->ComponentTags.Add("HitSprite");

	WorldSpaceSprite->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	WorldSpaceSprite->SetGenerateOverlapEvents(false);
	WorldSpaceSprite->ComponentTags.Add("WorldSpaceSprite");

	ActorsToIgnore.Append(GetActorsWithOtherTag("Scenario"));

	Effect1Flipbook->OnFinishedPlaying.AddDynamic(this, &ACharacterCommon::OnEffects1FlipbookFinishPlaying);
	Effect2Flipbook->OnFinishedPlaying.AddDynamic(this, &ACharacterCommon::OnEffects2FlipbookFinishPlaying);
	Effect3Flipbook->OnFinishedPlaying.AddDynamic(this, &ACharacterCommon::OnEffects3FlipbookFinishPlaying);
}

void ACharacterCommon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	HandleAutoActions();

	ControlCharacterRotation();

	if (!IsActionAnimationFinished())
	{
		ControlAnimation();
		SetLastActionTime(GetCurrentTime());
	}
	else
	{
		GetCharacterMovement()->GravityScale = 1.f;
		switch (ActionState)
		{
		case EActionState::ActionIdle:
			break;
		case EActionState::ActionDucking:
			break;
		case EActionState::ActionDuckingAttacking:
			SetActionState(EActionState::ActionDucking);
			ResetActionAnimationFlags();
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

/// <summary>
/// Used to know when was the last button pressed
/// </summary>
/// <param name="Time"></param>
void ACharacterCommon::SetLastActionTime(float Time)
{
	nLastActionTime = Time;
}
float ACharacterCommon::GetLastActionTime()
{
	return nLastActionTime;
}

/// <summary>
/// Faces the character to the right side
/// </summary>
void ACharacterCommon::ControlCharacterRotation()
{
	// Sets where the sprite has to face
	float yaw = 0.0f;
	if (!IsFacingRight())
	{
		yaw = 180.0f;
	}
	FRotator* rotation = new FRotator(0.0f, yaw, 1.0f);
	GetController()->SetControlRotation(*rotation);
}

void ACharacterCommon::BindDataHUD()
{
}

/// <summary>
/// Do the attack
/// </summary>
void ACharacterCommon::HandleAttack()
{
	if (IsActionAnimationFinished())
	{
		if (GetCharacterMovement()->IsMovingOnGround())
		{
			if (IsCrouched())
			{
				Actions = CrouchedAttack;
			}
			else
			{
				Actions = GroundCombo;
			}

		}
		else
		{
			Actions = AirCombo;
			SetCanMove(true);
		}
		SetActionAnimationFlags();

		nCurrentActionAnimation = 0;
		AnimationActionCurrentFrame = 0;
		ControlAnimation();
		bIsFirstAttack = false;
	}
}

/// <summary>
/// Do the special move
/// </summary>
void ACharacterCommon::HandleSpecialMoves()
{
	if (!IsExecutingSpecialMove())
	{
		SetIsExecutingSpecialMove(true);
		SetCanMove(false);
		// Checking if there are enough stamina
		SetHasStamina(SpecialMoves[nCurrentAction].StaminaCost <= Stamina);

		if ((SpecialMoves[nCurrentAction].IsGroundAttack && GetCharacterMovement()->IsMovingOnGround())
			|| (SpecialMoves[nCurrentAction].IsAirAttack && !GetCharacterMovement()->IsMovingOnGround()))
		{
			if (HasStamina())
			{
				ConsumeStamina(SpecialMoves[nCurrentAction].StaminaCost);
			}
			Actions = SpecialMoves;
			SetActionAnimationFlags();
			ControlAnimation();
		}
		else
		{
			//InputBuffer.ClearBuffer();
		}
	}
}

/// <summary>
/// Spawns a projectile
/// </summary>
/// <param name="Projectile"></param>
/// <param name="ProjectilePosition"></param>
void ACharacterCommon::HandleProjectile(UObject* Projectile, FVector ProjectilePosition)
{
	UBlueprint* GeneratedBP = Cast<UBlueprint>(Projectile);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	GetWorld()->SpawnActor<AActor>(GeneratedBP->GeneratedClass, ProjectilePosition, GetActorRotation(), SpawnParams);

	//GEngine->AddOnScreenDebugMessage(1, 3.f, FColor::Red, CharacterArrowComponent->GetComponentLocation().ToString());
}

/// <summary>
/// Sets up the projectile and generates the event to launch it
/// </summary>
/// <param name="CurrentAnimation"></param>
void ACharacterCommon::PrepareProjectile(FActionAnimationStruct CurrentAnimation)
{
	if (CurrentAnimation.IsProjectile)
	{
		FTimerDelegate TimerDelegate;
		FTimerHandle TimerHandle;

		//Binding the function with specific variables
		FVector ProjectilePosition = GetActorLocation() + FaceElement(CurrentAnimation.ProjectileStartPosition);
		TimerDelegate.BindUFunction(this, FName("HandleProjectile"), CurrentAnimation.GenericProjectile, ProjectilePosition);
		//Calling MyUsefulFunction after 0.1 seconds without looping
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 0.1f, false);
	}
}

void ACharacterCommon::HandleAutoActions()
{
	if (GetCharacterMovement()->IsMovingOnGround())
	{
		// Auto attack
		if (bIsAutoActionAttack)
		{
			DoAttack();
		}

		// Auto walk to right
		if (bIsAutoActionWalkForward)
		{
			SetFacingDirectionRight(true);
			FVector* Direction = new FVector(1.0f, 0.0f, 0.0f);
			SetRightPressed(true);
			SetAnimationState(EAnimationState::AnimWalking);
			AddMovementInput(*Direction, 1.0f);

			// Auto walk to left
			/*SetFacingDirectionRight(false);
			FVector* Direction = new FVector(-1.0f, 0.0f, 0.0f);
			SetLeftPressed(true);
			SetAnimationState(EAnimationState::AnimWalking);
			AddMovementInput(*Direction, 1.0f);*/
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
	SetLastActionTime(0.f);
	bIsFirstAttack = true;
	ResetActionAnimationFlags();
}

void ACharacterCommon::ResetAttack()
{
	bIsAttackFinished = true;
	SetCanMove(true);
}

void ACharacterCommon::ResetSpecialMove()
{
	nCurrentAction = 0;
	nCurrentActionAnimation = 0;
	nCurrentActionHitAnimation = 0;
	ActionsAnimationsFlags.Empty();
	SetIsExecutingSpecialMove(false);
	SetCanMove(true);
	ActionFinalLocation = FVector::ZeroVector;
}

void ACharacterCommon::ResetChargingUp()
{
	nCurrentAction = 0;
	nCurrentActionAnimation = 0;
	nCurrentActionHitAnimation = 0;
	ActionsAnimationsFlags.Empty();
	SetCanMove(true);
}

void ACharacterCommon::ResetDamage()
{
	nCurrentAction = 0;
	SetCanMove(true);
	ActionFinalLocation = FVector::ZeroVector;
}

void ACharacterCommon::OnEffects1FlipbookFinishPlaying()
{
	Effect1Flipbook->SetVisibility(false);
}
void ACharacterCommon::OnEffects2FlipbookFinishPlaying()
{
	Effect2Flipbook->SetVisibility(false);
}
void ACharacterCommon::OnEffects3FlipbookFinishPlaying()
{
	Effect2Flipbook->SetVisibility(false);
}

/// <summary>
/// Sets up the character to execute an attack
/// </summary>
void ACharacterCommon::DoAttack()
{
	SetActionState(EActionState::ActionAttacking);
	if (bIsAttackFinished && IsActionAnimationFinished())
	{
		bIsAttackFinished = false;
		if (GetCharacterMovement()->IsMovingOnGround())
		{
			if (IsCrouched())
			{
				ResetAttackCombo();
			}
			else
			{
				if (nCurrentAction < GroundCombo.Num() - 1)
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
			}
		}
		else
		{
			GetCharacterMovement()->Velocity.Set(0.f, 0.f, 0.f);
			GetCharacterMovement()->GravityScale = 0.f;
			if (nCurrentAction < AirCombo.Num() - 1)
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
		}
		HandleAttack();
	}
}

/// <summary>
/// Sets the right flipbook in current animation
/// </summary>
void ACharacterCommon::UpdateAnimations()
{
	switch (AnimationState)
	{
	case EAnimationState::AnimIdle:
		SetFlipbooks(IdleAnimation);
		break;
	case EAnimationState::AnimWalking:
		SetFlipbooks(WalkingAnimation);
		break;
	case EAnimationState::AnimJumping:
		SetFlipbooks(JumpingAnimation);
		break;
	case EAnimationState::AnimJumpingForward:
		SetFlipbooks(JumpingForwardAnimation);
		break;
	case EAnimationState::AnimDucking:
		SetFlipbooks(DuckingAnimation);
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
		SetFlipbooks(IdleAnimation);
		break;
	}
}

/// <summary>
/// Sets the character's animation state basing on the action state
/// </summary>
/// <param name="characterMovementSpeed"></param>
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
	case EActionState::ActionDuckingAttacking:
		SetAnimationState(EAnimationState::AnimDuckingAttacking);
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

/// <summary>
/// Sets the action animation flags, flags controls the complete character animation
/// (start, charging, hits and end animations) in order to do the right transition
/// </summary>
void ACharacterCommon::SetActionAnimationFlags()
{
	ActionsAnimationsFlags.Empty();
	if (Actions.Num() > 0)
	{
		for (int i = 0; i != Actions.Num(); ++i)
		{
			TArray<FActionCompleteAnimationStruct> CompleteAnimation = Actions[i].ActionAnimation;
			if (!HasStamina() && IsExecutingSpecialMove())
			{
				CompleteAnimation = Actions[i].NoStaminaAnimation;
			}

			FActionAnimationsFlagsStruct Action;
			for (int j = 0; j != CompleteAnimation.Num(); ++j)
			{
				FActionAnimationFlagsStruct Animations;
				Animations.bIsCompleted = false;
				Animations.bIsActionStart = false;
				Animations.bIsActionEnd = false;
				for (int k = 0; k != CompleteAnimation[j].AnimationHits.Num(); ++k)
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

/// <summary>
/// Set damage to the character and executes the damage animation
/// </summary>
/// <param name="Value"></param>
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
}

/// <summary>
/// Heals the character and notify the HUD
/// </summary>
/// <param name="Value"></param>
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

/// <summary>
/// Removes life to the character
/// </summary>
void ACharacterCommon::DrainLife()
{
	Life = 0.1f;
	GameHUD->SetLife(Life);
}

void ACharacterCommon::DrainStamina()
{
}

/// <summary>
/// Controls the animation and current frame
/// </summary>
void ACharacterCommon::ControlAnimation()
{
	// First time
	if (AnimationActionCurrentFrame == 0 || AnimationActionCurrentFrame == AnimationActionLastFrame)
	{
		PrepareAnimation();
	}
	++AnimationActionCurrentFrame;
}

/// <summary>
/// Basing on current action and current animation flag, 
/// sets the next animation to play
/// </summary>
void ACharacterCommon::PrepareAnimation()
{
	if (Actions.Num() > 0 && Actions.Num() >= nCurrentAction + 1)
	{
		FActionStruct Action = Actions[nCurrentAction];


		if (ActionsAnimationsFlags.Num() > 0)
		{
			TArray<FActionAnimationFlagsStruct>& AnimationsFlags = ActionsAnimationsFlags[nCurrentAction].Animations;

			// Initial verifications
			if (!Action.CanBeCharged || !IsChargingup())
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
				//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, TEXT("Start"));
			}
			else if (AnimationsFlags[nCurrentActionAnimation].bIsCompleted && !AnimationsFlags[nCurrentActionAnimation].bIsActionEnd)
			{
				CurrentAnimationState = ECurrentAnimationState::CurrentAnimationEnd;
				//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, TEXT("End"));
			}
			else if (!AnimationsFlags[nCurrentActionAnimation].bIsActionCharge && Action.CanBeCharged)
			{
				ControlChargingDamage(Action.ActionAnimation[nCurrentActionAnimation].AnimationCharge.DamageValue);
				GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Green, FString::Printf(TEXT("Total: %f"), GetTotalDamage()));
				CurrentAnimationState = ECurrentAnimationState::CurrentAnimationCharging;
				//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, TEXT("Charging"));
			}
			else if (AnimationsFlags[nCurrentActionAnimation].bIsActionHits.Num() > 0 &&
					 !AnimationsFlags[nCurrentActionAnimation].bIsActionHits[nCurrentActionHitAnimation])
			{
				SetChargingup(false);
				CurrentAnimationState = ECurrentAnimationState::CurrentAnimationHit;
				//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, TEXT("Hit"));
			}
			else
			{
				//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, FString::Printf(TEXT("Total Frames: %i"), AnimationActionLastFrame));
				AnimationActionCurrentFrame = 0;
				AnimationActionLastFrame = 0;
				SetActionAnimationIsFinished(true);
			}

			ApplyCurrentAnimation(Action, AnimationsFlags);
		}
	}
}

/// <summary>
/// Executes the current assigned animation and effects/projectile
/// </summary>
/// <param name="Action"></param>
/// <param name="AnimationsFlags"></param>
void ACharacterCommon::ApplyCurrentAnimation(FActionStruct Action, TArray<FActionAnimationFlagsStruct>& AnimationsFlags)
{
	FActionCompleteAnimationStruct& CompleteAnimation = Action.ActionAnimation[nCurrentActionAnimation];
	if (!HasStamina() && IsExecutingSpecialMove())
	{
		CompleteAnimation = Action.NoStaminaAnimation[nCurrentActionAnimation];
	}

	switch (CurrentAnimationState)
	{
	case ECurrentAnimationState::CurrentAnimationStart:
		SetAnimationBehaviour(CompleteAnimation.AnimationStart);
		ApplyEffectsAnimation(CompleteAnimation.AnimationStart);
		PrepareProjectile(CompleteAnimation.AnimationStart);
		AnimationsFlags[nCurrentActionAnimation].bIsActionStart = true;

		break;

	case ECurrentAnimationState::CurrentAnimationCharging:
		if (IsChargingup())
		{
			AnimationActionLastFrame += CompleteAnimation.AnimationCharge.Animation.Animation->GetNumFrames();
			ApplyEffectsAnimation(CompleteAnimation.AnimationCharge);
			SetAnimationBehaviour(CompleteAnimation.AnimationCharge);
			CurrentFlipbook->SetLooping(true);
		}
		else
		{
			AnimationsFlags[nCurrentActionAnimation].bIsActionCharge = true;
		}
		break;

	case ECurrentAnimationState::CurrentAnimationHit:
		ApplyEffectsAnimation(CompleteAnimation.AnimationHits[nCurrentActionHitAnimation]);
		SetAnimationBehaviour(CompleteAnimation.AnimationHits[nCurrentActionHitAnimation]);

		AnimationsFlags[nCurrentActionAnimation].bIsActionHits[nCurrentActionHitAnimation] = true;
		PrepareProjectile(CompleteAnimation.AnimationHits[nCurrentActionHitAnimation]);

		if (nCurrentActionHitAnimation < AnimationsFlags[nCurrentActionAnimation].bIsActionHits.Num() - 1)
		{
			++nCurrentActionHitAnimation;
		}
		else
		{
			AnimationsFlags[nCurrentActionAnimation].bIsCompleted = true;
		}
		break;

	case ECurrentAnimationState::CurrentAnimationEnd:
		ApplyEffectsAnimation(CompleteAnimation.AnimationEnd);
		SetAnimationBehaviour(CompleteAnimation.AnimationEnd);
		PrepareProjectile(CompleteAnimation.AnimationEnd);
		AnimationsFlags[nCurrentActionAnimation].bIsActionEnd = true;

		if (nCurrentActionAnimation < AnimationsFlags.Num() - 1)
		{
			nCurrentActionHitAnimation = 0;
			++nCurrentActionAnimation;
		}
		break;
	}
}

/// <summary>
/// Plays the effects assigned on the animation
/// </summary>
/// <param name="Action"></param>
void ACharacterCommon::ApplyEffectsAnimation(FActionAnimationStruct Action)
{
	if (Action.Animation.AnimationEffect1 != nullptr)
	{
		Effect1Flipbook->SetFlipbook(Action.Animation.AnimationEffect1);
		Effect1Flipbook->SetRelativeLocation(Action.Animation.Effect1Position);
		Effect1Flipbook->SetLooping(false);
		Effect1Flipbook->SetVisibility(true);
		Effect1Flipbook->PlayFromStart();
	}
	if (Action.Animation.AnimationEffect2 != nullptr)
	{
		Effect2Flipbook->SetFlipbook(Action.Animation.AnimationEffect2);
		Effect2Flipbook->SetRelativeLocation(Action.Animation.Effect2Position);
		Effect2Flipbook->SetLooping(false);
		Effect2Flipbook->SetVisibility(true);
		Effect2Flipbook->PlayFromStart();
	}
	if (Action.Animation.AnimationEffect3 != nullptr)
	{
		Effect3Flipbook->SetFlipbook(Action.Animation.AnimationEffect3);
		Effect3Flipbook->SetRelativeLocation(Action.Animation.Effect3Position);
		Effect3Flipbook->SetLooping(false);
		Effect3Flipbook->SetVisibility(true);
		Effect3Flipbook->PlayFromStart();
	}
}

/// <summary>
/// Sets the current animation flipbook, damage, hit and world space sprites to play
/// </summary>
/// <param name="AnimationStruct"></param>
void ACharacterCommon::SetFlipbooks(FAnimationStruct AnimationStruct)
{
	CurrentFlipbook->SetFlipbook(AnimationStruct.Animation);

	DamageSprite->SetSprite(AnimationStruct.SpriteDamage);
	HitSprite->SetSprite(AnimationStruct.SpriteHit);

	// Always will be the same worldspace
	if (AnimationStruct.SpriteWorldSpace != nullptr)
	{
		WorldSpaceSprite->SetSprite(AnimationStruct.SpriteWorldSpace);
	}
}

/// <summary>
/// Sets animation params
/// </summary>
/// <param name="ActionAnimationStruct"></param>
void ACharacterCommon::SetAnimationBehaviour(FActionAnimationStruct ActionAnimationStruct)
{
	SetCanMove(ActionAnimationStruct.CanMove);
	SetFlipbooks(ActionAnimationStruct.Animation);
	CurrentFlipbook->SetLooping(false);
	CurrentFlipbook->Play();

	AnimationActionLastFrame += CurrentFlipbook->GetFlipbookLengthInFrames();

	if (ActionAnimationStruct.ImpulseToOwner != FVector::ZeroVector)
	{
		GetCharacterMovement()->SetMovementMode(Actions[nCurrentAction].HitMode);
		GetCharacterMovement()->Velocity.Z = FMath::Max(0.f, ActionAnimationStruct.ImpulseToOwner.Z * 100.f);
		GetCharacterMovement()->Velocity.X = GetFacingX(ActionAnimationStruct.ImpulseToOwner.X);
	}
	CurrentAction = ActionAnimationStruct;
	//ApplyHitCollide(ActionAnimationStruct);
}

void ACharacterCommon::SetActionAnimationIsFinished(bool IsFinished)
{
	bActionAnimationIsFinished = IsFinished;
}
bool ACharacterCommon::IsActionAnimationFinished()
{
	return bActionAnimationIsFinished;
}

void ACharacterCommon::NotifyComboToHUD()
{
}

/// <summary>
/// Sets a new action state and finishes the old one
/// </summary>
/// <param name="State"></param>
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
	case EActionState::ActionDuckingAttacking:
		ResetAttack();
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

/// <summary>
/// Returns the current action state in string format
/// </summary>
/// <returns></returns>
FString ACharacterCommon::GetActionState()
{
	switch (ActionState)
	{
	case EActionState::ActionAttacking:
		return "Attacking";
	case EActionState::ActionChargingup:
		return "Charging";
	case EActionState::ActionDamaged:
		return "Damaged";
	case EActionState::ActionDucking:
		return "Crouching";
	case EActionState::ActionDuckingAttacking:
		return "CrouchingAttack";
	case EActionState::ActionGettingUp:
		return "Getting up";
	case EActionState::ActionIdle:
		return "Idle";
	case EActionState::ActionSpecialMove:
		return "SpecialMove";
	default:
		return "undefined";
	}
}

bool ACharacterCommon::IsFacingRight()
{
	return bIsMovingRight;
}
void ACharacterCommon::SetFacingDirectionRight(bool IsRight)
{
	bIsMovingRight = IsRight;
}

bool ACharacterCommon::IsLeft()
{
	return bIsLeft;
}
void ACharacterCommon::SetLeftPressed(bool State)
{
	bIsLeft = State;
}

bool ACharacterCommon::IsRight()
{
	return bIsRight;
}
void ACharacterCommon::SetRightPressed(bool State)
{
	bIsRight = State;
}

bool ACharacterCommon::IsUp()
{
	return bIsUp;
}
void ACharacterCommon::SetUpPressed(bool State)
{
	bIsUp = State;
}

bool ACharacterCommon::IsDown()
{
	return bIsDown;
}
void ACharacterCommon::SetDownPressed(bool State)
{
	bIsDown = State;
}

bool ACharacterCommon::IsChargingup()
{
	return bIsChargingup;
}
void ACharacterCommon::SetChargingup(bool State)
{
	bIsChargingup = State;
}

bool ACharacterCommon::IsFirstAttack()
{
	return bIsFirstAttack;
}

bool ACharacterCommon::IsDoingCombo()
{
	return bIsDoingCombo;
}

void ACharacterCommon::SetIsCombo(bool State)
{
	bIsDoingCombo = State;
}

bool ACharacterCommon::HasStamina()
{
	return bIsEnoughStamina;
}

void ACharacterCommon::SetHasStamina(bool State)
{
	bIsEnoughStamina = State;
}

bool ACharacterCommon::IsExecutingSpecialMove()
{
	return bIsExecutingSpecialMove;
}

void ACharacterCommon::SetIsExecutingSpecialMove(bool State)
{
	bIsExecutingSpecialMove = State;
}

void ACharacterCommon::AddToTotalDamage(float value)
{
	nTotalDamage += value;
}

void ACharacterCommon::SetTotalDamage(float value)
{
	nTotalDamage = value;
}

float ACharacterCommon::GetTotalDamage()
{
	return nTotalDamage;
}

bool ACharacterCommon::IsCrouched()
{
	return bIsCrouching;
}

void ACharacterCommon::SetIsCrouched(bool State)
{
	bIsCrouching = State;
}

void ACharacterCommon::SetAutoAttack(bool Value)
{
	bIsAutoActionAttack = Value;
}

void ACharacterCommon::SetAutoWalkForward(bool Value)
{
	bIsAutoActionWalkForward = Value;
}

void ACharacterCommon::ResetTotalDamage()
{
	nTotalDamage = 0.f;
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

void ACharacterCommon::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Filtering to get only hits to the other characters
	if (this->GetName() != Other->GetName() && OtherComp->GetName() == DamageSprite->GetName())
	{
		ACharacterCommon* ActorReceiver = Cast<ACharacterCommon>(Other);
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
		AddToTotalDamage(CurrentAction.DamageValue);
		ActorReceiver->SetDamage(GetTotalDamage());
		GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Red, FString::Printf(TEXT("Damage: %f"), GetTotalDamage()));
		ResetTotalDamage();

		++ComboCount;
	}
}

/// <summary>
/// Increases total damage in 1 until reaching limitDamage
/// </summary>
/// <param name="limitDamage"></param>
void ACharacterCommon::ControlChargingDamage(float limitDamage)
{
	if (GetTotalDamage() < limitDamage)
	{
		AddToTotalDamage(1);
	}
}
