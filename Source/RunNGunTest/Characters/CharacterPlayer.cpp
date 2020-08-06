// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterPlayer.h"
#include "PaperFlipbookComponent.h"
#include "Weapons/GenericProjectile.h"
#include "GameFramework/Character.h"
#include "Containers/Array.h"


// Sets default values
ACharacterPlayer::ACharacterPlayer()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	this->Tags.Add("Player");

	CharacterArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("CharacterArrowComponent"));
	//CharacterArrowComponent->SetupAttachment(GetArrowComponent());

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
void ACharacterPlayer::BeginPlay()
{
	Super::BeginPlay();
	ActorsToIgnore.Append(GetActorsWithOtherTag("Player"));
}

// Called every frame
void ACharacterPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// If special button is pressed, stamina grows up
	bCanCharge = bActionAnimationIsFinished || ActionState == EActionState::ActionChargingup;
	if (bCanCharge && SpecialKeyPressedTimeStart != -1 && SpecialKeyPressedTimeStart <= GetCurrentTime())
	{
		ControlStamina();
	}

	// User stops doing combo
	if (GetLastActionTime() > 0.f &&
		GetLastActionTime() + MaxComboTime < GetCurrentTime())
	{
		NotifyComboToHUD();
		ResetAttackCombo();
	}

	HandleDirections();
}

// Called to bind functionality to input
void ACharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//PlayerInputComponent->BindAxis("MoveRight", this, &ACharacterPlayer::MoveRight);
	PlayerInputComponent->BindAction("LeftDirection", IE_Pressed, this, &ACharacterPlayer::LeftDirectionStart);
	PlayerInputComponent->BindAction("LeftDirection", IE_Released, this, &ACharacterPlayer::LeftDirectionStop);
	PlayerInputComponent->BindAction("RightDirection", IE_Pressed, this, &ACharacterPlayer::RightDirectionStart);
	PlayerInputComponent->BindAction("RightDirection", IE_Released, this, &ACharacterPlayer::RightDirectionStop);
	PlayerInputComponent->BindAction("DownDirection", IE_Pressed, this, &ACharacterPlayer::DownDirectionStart);
	PlayerInputComponent->BindAction("DownDirection", IE_Released, this, &ACharacterPlayer::DownDirectionStop);
	PlayerInputComponent->BindAction("UpDirection", IE_Pressed, this, &ACharacterPlayer::UpDirectionStart);
	PlayerInputComponent->BindAction("UpDirection", IE_Released, this, &ACharacterPlayer::UpDirectionStop);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacterPlayer::JumpStart);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacterPlayer::JumpStop);
	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &ACharacterPlayer::AttackStart);
	PlayerInputComponent->BindAction("Attack", IE_Released, this, &ACharacterPlayer::AttackStop);
	PlayerInputComponent->BindAction("Special", IE_Pressed, this, &ACharacterPlayer::SpecialStart);
	PlayerInputComponent->BindAction("Special", IE_Released, this, &ACharacterPlayer::SpecialStop);
}

// Basic moving
void ACharacterPlayer::HandleDirections()
{
	float MovementSpeed = 0.f;
	if (GetCanMove())
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

void ACharacterPlayer::MoveCharacter(float MovementSpeed, bool IsFacingRight)
{
	bIsMovingRight = IsFacingRight;
	FVector* Direction = new FVector(1.0f, 0.0f, 0.0f);
	AddMovementInput(*Direction, MovementSpeed);
}

void ACharacterPlayer::LeftDirectionStart()
{
	bIsLeft = true;
}

void ACharacterPlayer::LeftDirectionStop()
{
	bIsLeft = false;
	bIsDirectionPressed = false;
}

void ACharacterPlayer::RightDirectionStart()
{
	bIsRight = true;
}

void ACharacterPlayer::RightDirectionStop()
{
	bIsRight = false;
	bIsDirectionPressed = false;
}

void ACharacterPlayer::DownDirectionStart()
{
	bIsDown = true;
	if (GetCanMove())
	{
		SetActionState(EActionState::ActionDucking);
	}
}

void ACharacterPlayer::DownDirectionStop()
{
	if (GetCanMove())
	{
		SetActionState(EActionState::ActionIdle);
	}
	bIsDown = false;
	bIsDirectionPressed = false;
}

void ACharacterPlayer::UpDirectionStart()
{
	bIsUp = true;
}

void ACharacterPlayer::UpDirectionStop()
{
	bIsUp = false;
	bIsDirectionPressed = false;
}

// Triggers
void ACharacterPlayer::JumpStart()
{
	if (GetCanMove())
	{
		bPressedJump = true;
		SetActionState(EActionState::ActionIdle);
		ResetAttackCombo();
		SetAnimationState(EAnimationState::AnimJumping);
	}
}

void ACharacterPlayer::JumpStop()
{
	bPressedJump = false;
}

void ACharacterPlayer::AttackStart()
{
	if (GetCanMove())
	{
		HandleBuffer(KeyInput::Attack);
		AttackKeyPressedTimeStart = GetCurrentTime();
		bIsChargingup = true;
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
		else
		{
			DoAttack();
		}

	}
}

void ACharacterPlayer::AttackStop()
{
	AttackKeyPressedTimeStop = GetCurrentTime();
	bIsChargingup = false;
}

void ACharacterPlayer::SpecialStart()
{
	SpecialKeyPressedTimeStart = GetCurrentTime() + DelayTimeUntilChargingUp;
	bIsChargingup = true;
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

void ACharacterPlayer::SpecialStop()
{
	StopHandleStaminaCharge();
}

// Handling actions
void ACharacterPlayer::HandleAttack()
{
	Super::HandleAttack();
	ShowBufferOnScreen();
}

void ACharacterPlayer::HandleStaminaCharge()
{
	if (ActionState == EActionState::ActionChargingup && SpecialKeyPressedTimeStart + StaminaVelocityChargingInSeconds < GetCurrentTime())
	{
		Stamina += StaminaChargingUnit;
		GameHUD->SetStamina(Stamina);
		SpecialKeyPressedTimeStart = GetCurrentTime();
	}
}

void ACharacterPlayer::NotifyComboToHUD()
{
	if (ComboCount > 2)
	{
		GameHUD->ComboCounter(ComboCount);
	}
	ComboCount = 0;
}

void ACharacterPlayer::ShowBufferOnScreen()
{
	if (bShowBuffer)
	{
		GameHUD->DrawBuffer(InputBuffer.GetBufferedInput());
	}
}

// Play states
void ACharacterPlayer::SetDamage(float Value)
{
	Super::SetDamage(Value);

	GameHUD->SetLife(Life);
	HandleDead();
}

void ACharacterPlayer::HealStamina(float Value)
{
	if (Stamina < MaxStamina)
	{
		Stamina += Value;
		GameHUD->SetStamina(Stamina);
	}
}

void ACharacterPlayer::DrainLife()
{
	Super::DrainLife();

	HandleDead();
}

void ACharacterPlayer::DrainStamina()
{
	Super::DrainStamina();

	Stamina = 0.f;
	GameHUD->SetStamina(Stamina);
}

void ACharacterPlayer::HandleDead()
{
	if (Life <= 0.f)
	{
		UGameplayStatics::OpenLevel(this, "level_00");
	}
}

// Stamina
void ACharacterPlayer::ControlStamina()
{
	if (Stamina < MaxStamina)
	{
		if (ActionState != EActionState::ActionChargingup)
		{
			Actions = ChargingStaminaAnimation;
			SetActionAnimationFlags();
			SetCanMove(false);
			SetActionState(EActionState::ActionChargingup);
			ControlAnimation();
		}

		HandleStaminaCharge();
	}
	else
	{
		StopHandleStaminaCharge();
	}
}

// Buffer
void ACharacterPlayer::HandleBuffer(KeyInput Direction)
{
	InputBuffer.InsertInputBuffer(Direction, bIsDirectionPressed);
	ShowBufferOnScreen();
}

// Resetting states
void ACharacterPlayer::StopHandleStaminaCharge()
{
	SpecialKeyPressedTimeStart = -1;
	SpecialKeyPressedTimeStop = -1;
	bIsChargingup = false;
}
