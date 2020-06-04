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
	AnimationFlipbookTimeStop = -1.f;
	AnimationAttackCompleteTimeStop = -1.f;
}

// Called when the game starts or when spawned
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();

	SetAnimationFlags();
	ResetAnimationFlags();
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	GameHUD = Cast<AGameHUD>(PlayerController->GetHUD());
}

// Called every frame
void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// If special button is pressed, stamina grows up
	if (SpecialKeyPressedTimeStart != -1 && SpecialKeyPressedTimeStart <= GetCurrentTime())
	{
		ControlStamina();
	}

	// Animation attack is on run
	if (AnimationFlipbookTimeStop > GetCurrentTime())
	{
		HandleAttack();
	}

	// User stops doing combo
	if (AnimationAttackCompleteTimeStop + 0.5f < GetCurrentTime())
	{
		FinishCombo();
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
	switch (EAnimationState)
	{
	case AnimationState::Idle:
		CurrentFlipbook->SetFlipbook(IdleAnimation);
		break;
	case AnimationState::Walking:
		CurrentFlipbook->SetFlipbook(WalkingAnimation);
		break;
	case AnimationState::Jumping:
		CurrentFlipbook->SetFlipbook(JumpingAnimation);
		break;
	case AnimationState::JumpingForward:
		CurrentFlipbook->SetFlipbook(JumpingForwardAnimation);
		break;
	case AnimationState::Ducking:
		CurrentFlipbook->SetFlipbook(DuckingAnimation);
		break;
	case AnimationState::SpecialMove:
		HandleSpecialMoves();
		break;
	case AnimationState::Attacking:
		HandleAttack();
		break;
	case AnimationState::ChargingUp:
		CurrentFlipbook->SetFlipbook(ChargingUpAnimation);
		HandleStaminaCharge();
		break;
	case AnimationState::HitTop:
		CurrentFlipbook->SetFlipbook(HitTopAnimation);
		break;
	default:
		CurrentFlipbook->SetFlipbook(IdleAnimation);
		break;
	}
}

void ACharacterBase::ControlCharacterAnimations(float characterMovementSpeed = 0.f)
{
	if (!GetCharacterMovement()->IsMovingOnGround())
	{
		if (bIsSpecialMove)
		{
			EAnimationState = AnimationState::SpecialMove;
		}
		else if (bIsAttacking)
		{
			EAnimationState = AnimationState::Attacking;
		}
		else
		{
			if (fabs(characterMovementSpeed) > 0.0f)
			{
				EAnimationState = AnimationState::JumpingForward;
			}
			else
			{
				EAnimationState = AnimationState::Jumping;
			}
		}
	}
	else
	{
		if (bIsDamaged)
		{
			EAnimationState = AnimationState::HitTop;
		}
		else
		{
			if (bIsChargingup)
			{
				EAnimationState = AnimationState::ChargingUp;
			}
			else if (bIsSpecialMove)
			{
				EAnimationState = AnimationState::SpecialMove;
			}
			else if (bIsAttacking)
			{
				EAnimationState = AnimationState::Attacking;
			}
			else
			{
				if (fabs(characterMovementSpeed) > 0.0f)
				{
					EAnimationState = AnimationState::Walking;
				}
				else
				{
					if (bIsDucking)
					{
						EAnimationState = AnimationState::Ducking;
					}
					else
					{
						EAnimationState = AnimationState::Idle;
					}
				}
			}
		}
	}
	UpdateAnimations();
}

// Basic moving
void ACharacterBase::HandleDirections()
{
	float MovementSpeed = 0.f;
	if (bCanMove)
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
	bIsDucking = true;
	bIsDown = true;
}

void ACharacterBase::DownDirectionStop()
{
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
	bCanMove = true;
	bPressedJump = true;
	bIsAttacking = false;
	ResetAttack();
	EAnimationState = AnimationState::Jumping;
}

void ACharacterBase::JumpStop()
{
	bPressedJump = false;
}

void ACharacterBase::AttackStart()
{
	bIsAttacking = true;
	HandleBuffer(KeyInput::Attack);

	// Animation has to finish, with a little window to input next command
	if (bIsAnimationAttackComplete && AnimationAttackCompleteTimeStop + 0.5f > GetCurrentTime())
	{
		if (GetCharacterMovement()->IsMovingOnGround())
		{
			bCanMove = false;
			// If attack is pressed continuously, it will be a combo 
			if (nAttackNumber >= AttackingComboAnimation.Num() - 1)
			{
				ResetAttack();
				bCanMove = true;
				InputBuffer.ClearBuffer();
				ShowBufferOnScreen();
			}
			else
			{
				++nAttackNumber;
				++ComboCount;
				CurrentAttackHasHitObjective = false;
			}
		}
	}
	else if (AnimationAttackCompleteTimeStop + 0.5f < GetCurrentTime())
	{
		ResetAttack();
		bCanMove = true;
		InputBuffer.ClearBuffer();
		ShowBufferOnScreen();
	}
}

void ACharacterBase::AttackStop()
{
	AttackKeyPressedTimeStop = GetCurrentTime(); // Not used for now
}

void ACharacterBase::SpecialStart()
{
	SpecialKeyPressedTimeStart = GetCurrentTime() + 1.f;
	HandleBuffer(KeyInput::Special);
	if (GetCharacterMovement()->IsMovingOnGround())
	{
		bCanMove = false;
	}

	TArray<int32> MatchingSpecialMoves;
	for (int i = 0; i < SpecialMoves.Num(); ++i)
	{
		if (InputBuffer.IsMatchingDirections(SpecialMoves[i].Directions))
		{
			MatchingSpecialMoves.Add(i);
		}
	}
	if (MatchingSpecialMoves.Num() > 0)
	{
		nCurrentSpecialMove = MatchingSpecialMoves[MatchingSpecialMoves.Num() - 1];
	}

	if (nCurrentSpecialMove >= 0)
	{
		bIsSpecialMove = true;
	}
	else
	{
		bCanMove = true;
	}
}

void ACharacterBase::SpecialStop()
{
	StopHandleStaminaCharge();
	SpecialKeyPressedTimeStop = GetCurrentTime(); // Not used for now
}

// Handling actions
void ACharacterBase::HandleAttack()
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


	if (bIsInHitAttackFrames && !bCurrentHitCollisionIsDone)
	{
		if (GetCharacterMovement()->IsMovingOnGround())
		{
			ApplyHitCollide(AttackingComboAnimation);
		}
		else
		{
			ApplyHitCollide(AttackingJumpingAnimation);
		}
	}

	if (AnimationAttackCompleteTimeStop < GetCurrentTime())
	{
		bIsAttacking = false;
		CurrentAttackHasHitObjective = false;
		EAnimationState = AnimationState::Idle;
		bCanMove = true;
		ResetAnimationFlags();
	}
}

void ACharacterBase::HandleSpecialMoves()
{

}

void ACharacterBase::HandleProjectile()
{

}

void ACharacterBase::HandleStaminaCharge()
{
	if (bIsChargingup && GetCurrentTime() > SpecialKeyPressedTimeStart + StaminaVelocityChargingInSeconds)
	{
		Stamina += StaminaChargingUnit;
		GameHUD->SetStamina(Stamina);
		SpecialKeyPressedTimeStart = GetCurrentTime();
	}
}

void ACharacterBase::FinishCombo()
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
	if (Life <= 0.f)
	{
		UGameplayStatics::OpenLevel(this, "level_00");
	}
}

void ACharacterBase::HealLife(float Value)
{
	if (Life < MaxLife)
	{
		Life += Value;
		GameHUD->SetLife(Life);
	}
}

void ACharacterBase::HealStamina(float Value)
{
	if (Stamina < MaxStamina)
	{
		Stamina += Value;
		GameHUD->SetStamina(Stamina);
	}
}

// Stamina
void ACharacterBase::ControlStamina()
{
	if (Stamina < MaxStamina)
	{
		bIsChargingup = true;
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
	Stamina = Stamina - Value;
	GameHUD->SetStamina(Stamina);
}

void ACharacterBase::ApplyHitCollide(TArray<FComboAttackStruct> Combo)
{
	FVector HitBox = Combo[nAttackNumber].AttackAnimationHits[nCurrentComboHit].HitBoxPosition;
	if (!bIsMovingRight)
	{
		HitBox = FVector(HitBox.X * -1, HitBox.Y, HitBox.Z);
	}
	// Hit box will grow to detect collision
	FVector CurrentLocation = FVector(GetActorLocation().X + HitBox.X, GetActorLocation().Y + HitBox.Y, GetActorLocation().Z + HitBox.Z);

	// Tracing collision
	FHitResult* HitResult = new FHitResult();
	if (!CurrentAttackHasHitObjective && UKismetSystemLibrary::BoxTraceSingle(GetWorld(), GetActorLocation(), CurrentLocation, FVector(20.0, 20.0, 20.0), GetActorRotation(), ETraceTypeQuery::TraceTypeQuery2, false, ActorsToIgnore, EDrawDebugTrace::ForDuration, *HitResult, true, FLinearColor::Red, FLinearColor::Green, 0.5f))
	{
		if (HitResult->Actor->ActorHasTag("Enemy"))
		{
			ACharacterCommon* EnemyCasted = Cast<ACharacterCommon>(HitResult->Actor);
			EnemyCasted->SetDamage(Combo[nAttackNumber].AttackAnimationHits[nCurrentComboHit].DamageValue);
			if (nCurrentComboHit >= Combo[nAttackNumber].AttackAnimationHits.Num())
			{
				CurrentAttackHasHitObjective = true;
			}
			bCurrentHitCollisionIsDone = true;
		}
	}
}

// Resetting states
void ACharacterBase::StopHandleStaminaCharge()
{
	bIsChargingup = false;
	bCanMove = true;
	SpecialKeyPressedTimeStart = -1;
	SpecialKeyPressedTimeStop = -1;
}

void ACharacterBase::ResetAttack()
{

	nAttackNumber = 0;
	nCurrentComboHit = 0;
	ResetAnimationFlags();
}

void ACharacterBase::SetAnimationFlags()
{
	for (FComboAttackStruct combo : AttackingComboAnimation)
	{
		FComboAnimationFlags element;
		for (FComboAttackHitsStruct hit : combo.AttackAnimationHits)
		{
			element.bIsComboHits.Add(false);
		}
		element.bIsComboStart = false;
		element.bIsComboEnd = false;
		ComboAnimationFlags.Add(element);
	}
}

void ACharacterBase::ResetAnimationFlags()
{
	ComboAnimationFlags.Empty();
	SetAnimationFlags();
}

float ACharacterBase::GetCurrentTime()
{
	if (GetWorld())
	{
		return GetWorld()->GetRealTimeSeconds();
	}
	return 0.f;
}