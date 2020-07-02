// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterBase.h"
#include "PaperFlipbookComponent.h"
#include "Weapons/GenericProjectile.h"
#include "GameFramework/Character.h"
#include "Containers/Array.h"


// Sets default values
ACharacterBase::ACharacterBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	this->Tags.Add("Player");

	CharacterArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("CharacterArrowComponent"));
	CharacterArrowComponent->SetupAttachment(GetArrowComponent());
	CurrentFlipbook = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("CurrentFlipbook"));
	CurrentFlipbook->SetupAttachment(GetCapsuleComponent());

	// Setting default character movement's params
	GetCharacterMovement()->MaxAcceleration = 5000;
	GetCharacterMovement()->GroundFriction = 20;
	GetCharacterMovement()->MaxWalkSpeed = 400;
	GetCharacterMovement()->MaxWalkSpeedCrouched = 50;
	GetCharacterMovement()->JumpZVelocity = 500;
	GetCharacterMovement()->BrakingDecelerationFalling = 5000;
	GetCharacterMovement()->AirControl = 0.8;
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->SetPlaneConstraintAxisSetting(EPlaneConstraintAxisSetting::Y);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);


	SpecialKeyPressedTimeStart = -1.f;
}

// Called when the game starts or when spawned
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	GameHUD = Cast<AGameHUD>(PlayerController->GetHUD());

}

// Called every frame
void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// If special button is pressed, stamina grows up
	if (SpecialKeyPressedTimeStart != -1 && SpecialKeyPressedTimeStart <= GetCurrentTime()) // bActionAnimationIsFinished
	{
		ControlStamina();
	}

	// User stops doing combo
	if (nLastActionTime + MaxComboTime < GetCurrentTime())
	{
		NotifyComboToHUD();
	}

	if (GameHUD)
	{
		TArray<FString> DebugString;
		DebugString.Add(FString::Printf(TEXT("CurrentState: %i"), ActionState));
		GameHUD->InsertDebugData(DebugString);
	}
	HandleDirections();
}

// Called to bind functionality to input
void ACharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//PlayerInputComponent->BindAxis("MoveRight", this, &ACharacterBase::MoveRight);
	PlayerInputComponent->BindAction("LeftDirection", IE_Pressed, this, &ACharacterBase::LeftDirectionStart);
	PlayerInputComponent->BindAction("LeftDirection", IE_Released, this, &ACharacterBase::LeftDirectionStop);
	PlayerInputComponent->BindAction("RightDirection", IE_Pressed, this, &ACharacterBase::RightDirectionStart);
	PlayerInputComponent->BindAction("RightDirection", IE_Released, this, &ACharacterBase::RightDirectionStop);
	PlayerInputComponent->BindAction("DownDirection", IE_Pressed, this, &ACharacterBase::DownDirectionStart);
	PlayerInputComponent->BindAction("DownDirection", IE_Released, this, &ACharacterBase::DownDirectionStop);
	PlayerInputComponent->BindAction("UpDirection", IE_Pressed, this, &ACharacterBase::UpDirectionStart);
	PlayerInputComponent->BindAction("UpDirection", IE_Released, this, &ACharacterBase::UpDirectionStop);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacterBase::JumpStart);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacterBase::JumpStop);
	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &ACharacterBase::AttackStart);
	PlayerInputComponent->BindAction("Attack", IE_Released, this, &ACharacterBase::AttackStop);
	PlayerInputComponent->BindAction("Special", IE_Pressed, this, &ACharacterBase::SpecialStart);
	PlayerInputComponent->BindAction("Special", IE_Released, this, &ACharacterBase::SpecialStop);
}

// Animations
void ACharacterBase::UpdateAnimations()
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
		//HandleSpecialMoves();
		break;
	case EAnimationState::AnimAttacking:
		//HandleAttack();
		break;
	case EAnimationState::AnimChargingUp:
		//CurrentFlipbook->SetFlipbook(ChargingUpAnimation);
		//HandleStaminaCharge();
		break;
	case EAnimationState::AnimHitTop:
		CurrentFlipbook->SetFlipbook(HitTopAnimation);
		break;
	default:
		CurrentFlipbook->SetFlipbook(IdleAnimation);
		break;
	}
}

void ACharacterBase::ControlCharacterAnimations(float characterMovementSpeed = 0.f)
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
		SetAnimationState(EAnimationState::AnimHitTop);
		break;
	case EActionState::ActionDucking:
		SetAnimationState(EAnimationState::AnimDucking);
		break;
	case EActionState::ActionIdle:
		if (!GetCharacterMovement()->IsMovingOnGround())
		{
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
		SetAnimationState(EAnimationState::AnimIdle);
		break;
	}

	if (ActionState != EActionState::ActionSpecialMove)
	{
		CurrentFlipbook->SetLooping(true);
		CurrentFlipbook->Play();
	}
	UpdateAnimations();
}

// Basic moving
void ACharacterBase::HandleDirections()
{
	float MovementSpeed = 0.f;
	if (CanMove())
	{
		if (bIsLeft && bIsDown)
		{
			HandleBuffer(KeyInput::DownLeft);
			bIsDirectionPressed = true;
			MovementSpeed = CrouchingSpeed;
			MoveCharacter(CrouchingSpeed * -1.f, false);
		}
		else if (bIsRight && bIsDown)
		{
			HandleBuffer(KeyInput::DownRight);
			bIsDirectionPressed = true;
			MovementSpeed = CrouchingSpeed;
			MoveCharacter(CrouchingSpeed, true);
		}
		else if (bIsLeft && bIsUp)
		{
			HandleBuffer(KeyInput::UpLeft);
			bIsDirectionPressed = true;
			MovementSpeed = WalkingSpeed;
			MoveCharacter(WalkingSpeed * -1.f, false);
		}
		else if (bIsRight && bIsUp)
		{
			HandleBuffer(KeyInput::UpRight);
			bIsDirectionPressed = true;
			MovementSpeed = WalkingSpeed;
			MoveCharacter(WalkingSpeed, true);
		}
		else if (bIsDown && !bIsUp)
		{
			HandleBuffer(KeyInput::Down);
			bIsDirectionPressed = true;
		}
		else if (bIsLeft && !bIsRight)
		{
			HandleBuffer(KeyInput::Left);
			bIsDirectionPressed = true;
			MovementSpeed = WalkingSpeed;
			MoveCharacter(WalkingSpeed * -1.f, false);
		}
		else if (bIsRight && !bIsLeft)
		{
			HandleBuffer(KeyInput::Right);
			bIsDirectionPressed = true;
			MovementSpeed = WalkingSpeed;
			MoveCharacter(WalkingSpeed, true);
		}
		else if (bIsUp && !bIsDown)
		{
			HandleBuffer(KeyInput::Up);
			bIsDirectionPressed = true;
		}
		else
		{
			bIsDirectionPressed = false;
		}
	}
	ControlCharacterAnimations(MovementSpeed);
}

void ACharacterBase::MoveCharacter(float MovementSpeed, bool IsFacingRight)
{
	bIsMovingRight = IsFacingRight;
	FVector* Direction = new FVector(1.0f, 0.0f, 0.0f);
	AddMovementInput(*Direction, MovementSpeed);
}

void ACharacterBase::LeftDirectionStart()
{
	bIsLeft = true;
}

void ACharacterBase::LeftDirectionStop()
{
	bIsLeft = false;
	bIsDirectionPressed = false;
}

void ACharacterBase::RightDirectionStart()
{
	bIsRight = true;
}

void ACharacterBase::RightDirectionStop()
{
	bIsRight = false;
	bIsDirectionPressed = false;
}

void ACharacterBase::DownDirectionStart()
{
	bIsDown = true;
	if (CanMove())
	{
		SetActionState(EActionState::ActionDucking);
	}
}

void ACharacterBase::DownDirectionStop()
{
	if (CanMove())
	{
		SetActionState(EActionState::ActionIdle);
	}
	bIsDucking = false;
	bIsDown = false;
	bIsDirectionPressed = false;
}

void ACharacterBase::UpDirectionStart()
{
	bIsUp = true;
}

void ACharacterBase::UpDirectionStop()
{
	bIsUp = false;
	bIsDirectionPressed = false;
}

// Triggers
void ACharacterBase::JumpStart()
{
	if (CanMove())
	{
		//SetCanMove(true);
		bPressedJump = true;
		//bIsAttacking = false;
		SetActionState(EActionState::ActionIdle);
		ResetAttackCombo();
		SetAnimationState(EAnimationState::AnimJumping);
	}
}

void ACharacterBase::JumpStop()
{
	bPressedJump = false;
}

void ACharacterBase::AttackStart()
{
	if (CanMove())
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
				nCurrentActionAnimation = 0;
			}
			else
			{
				ResetAttackCombo();
				//InputBuffer.ClearBuffer();				
			}
			HandleAttack();
			++ComboCount;
		}
		HandleBuffer(KeyInput::Attack);
	}
}

void ACharacterBase::AttackStop()
{
	AttackKeyPressedTimeStop = GetCurrentTime(); // Not used for now
}

void ACharacterBase::SpecialStart()
{
	SpecialKeyPressedTimeStart = GetCurrentTime() + DelayTimeUntilChargingUp;
	bIsSpecialButtonPressed = true;
	HandleBuffer(KeyInput::Special);
	if (bActionAnimationIsFinished)
	{
		TArray<int32> MatchingActions;
		for (int i = 0; i < SpecialMoves.Num(); ++i)
		{
			if (InputBuffer.IsMatchingDirections(SpecialMoves[i].Directions))
			{
				MatchingActions.Add(i);
			}
		}
		if (MatchingActions.Num() > 0)
		{
			nCurrentAction = MatchingActions[MatchingActions.Num() - 1];
			SetActionState(EActionState::ActionSpecialMove);
			HandleSpecialMoves();
		}
	}
}

void ACharacterBase::SpecialStop()
{
	StopHandleStaminaCharge();

	bIsSpecialButtonPressed = false;
	SpecialKeyPressedTimeStop = GetCurrentTime(); // Not used for now
}

// Handling actions
void ACharacterBase::HandleAttack()
{
	Super::HandleAttack();
	ShowBufferOnScreen();
}

void ACharacterBase::HandleSpecialMoves()
{
	Super::HandleSpecialMoves();
}

void ACharacterBase::HandleProjectile(UObject* Projectile)
{
	UObject* SpawnActor = Projectile;

	UBlueprint* GeneratedBP = Cast<UBlueprint>(SpawnActor);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	GetWorld()->SpawnActor<AActor>(GeneratedBP->GeneratedClass, GetActorLocation(), GetActorRotation(), SpawnParams);

	//GEngine->AddOnScreenDebugMessage(1, 3.f, FColor::Red, CharacterArrowComponent->GetComponentLocation().ToString());
}

void ACharacterBase::HandleStaminaCharge()
{
	if (ActionState == EActionState::ActionChargingup && SpecialKeyPressedTimeStart + StaminaVelocityChargingInSeconds < GetCurrentTime())
	{
		Stamina += StaminaChargingUnit;
		GameHUD->SetStamina(Stamina);
		SpecialKeyPressedTimeStart = GetCurrentTime();
	}
}

void ACharacterBase::NotifyComboToHUD()
{
	if (ComboCount > 2)
	{
		GameHUD->ComboCounter(ComboCount);
	}
	ComboCount = 0;
}

void ACharacterBase::ShowBufferOnScreen()
{
	if (bShowBuffer)
	{
		GameHUD->DrawBuffer(InputBuffer.GetBufferedInput());
	}
}

// Play states
void ACharacterBase::SetDamage(float Value)
{
	Super::SetDamage(Value);

	GameHUD->SetLife(Life);
	HandleDead();
}

void ACharacterBase::HealStamina(float Value)
{
	if (Stamina < MaxStamina)
	{
		Stamina += Value;
		GameHUD->SetStamina(Stamina);
	}
}

void ACharacterBase::DrainLife()
{
	Super::DrainLife();

	HandleDead();
}

void ACharacterBase::DrainStamina()
{
	Super::DrainStamina();

	Stamina = 0.f;
	GameHUD->SetStamina(Stamina);
}

void ACharacterBase::HandleDead()
{
	if (Life <= 0.f)
	{
		UGameplayStatics::OpenLevel(this, "level_00");
	}
}

// Stamina
void ACharacterBase::ControlStamina()
{
	if (Stamina < MaxStamina)
	{
		if (ActionState != EActionState::ActionChargingup)
		{
			Actions = ChargingStaminaAnimation;
			SetActionAnimationFlags();
			SetCanMove(false);
			SetActionState(EActionState::ActionChargingup);
			DoActionAnimation();
		}
		HandleStaminaCharge();
	}
	else
	{
		StopHandleStaminaCharge();
	}
}

// Buffer
void ACharacterBase::HandleBuffer(KeyInput Direction)
{
	InputBuffer.InsertInputBuffer(Direction, bIsDirectionPressed);
	ShowBufferOnScreen();
}

void ACharacterBase::ConsumeStamina(float Value)
{
	Super::ConsumeStamina(Value);
}

// Resetting states
void ACharacterBase::StopHandleStaminaCharge()
{
	//bIsChargingup = false;
	//SetActionState(EActionState::ActionIdle);
	SpecialKeyPressedTimeStart = -1;
	SpecialKeyPressedTimeStop = -1;
	/*nCurrentActionAnimation = 0;
	nCurrentActionHitAnimation = 0;*/
}
