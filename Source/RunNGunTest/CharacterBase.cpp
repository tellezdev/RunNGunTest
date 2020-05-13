// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterBase.h"
#include "PaperFlipbookComponent.h"
#include "GenericProjectile.h"
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

	AttackKeyPressedTimeStart = -1;
	SpecialKeyPressedTimeStart = -1;
}

// Called when the game starts or when spawned
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("RNGCharacter loaded"));
		if (CharacterArrowComponent)
		{
			FString debug0 = FString::Printf(TEXT("Arrow loaded"));
			GEngine->AddOnScreenDebugMessage(10, 3.0f, FColor::Red, debug0);
		}
	}
}

// Called every frame
void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Sets where the sprite has to face
	float yaw = 0.0f;
	if (!bIsMovingRight)
	{
		yaw = 180.0f;
	}
	FRotator* rotation = new FRotator(0.0f, yaw, 1.0f);
	GetController()->SetControlRotation(*rotation);

	// If special button is pressed, stamina grows up
	if (SpecialKeyPressedTimeStart != -1 && SpecialKeyPressedTimeStart <= GetCurrentTime())
	{
		ControlStamina();
	}
}

// Called to bind functionality to input
void ACharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveRight", this, &ACharacterBase::MoveRight);
	PlayerInputComponent->BindAction("DownDirection", IE_Pressed, this, &ACharacterBase::DownDirectionStart);
	PlayerInputComponent->BindAction("DownDirection", IE_Released, this, &ACharacterBase::DownDirectionStop);
	PlayerInputComponent->BindAction("UpDirection", IE_Pressed, this, &ACharacterBase::UpDirection);
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
	UpdateAnimations();
}

// Basic moving
void ACharacterBase::MoveRight(float Value)
{
	if (bCanMove)
	{
		if (!bIsDirectionPressed && Value != 0.f)
		{
			if (Value > 0.f)
			{
				InsertInputBuffer(KeyInput::Right);
			}
			else
			{
				InsertInputBuffer(KeyInput::Left);
			}
		}

		if (Value > 0.0f)
		{
			bIsMovingRight = true;
			bIsDirectionPressed = true;
		}
		else if (Value < 0.0f)
		{
			bIsMovingRight = false;
			bIsDirectionPressed = true;
		}
		else
		{
			bIsDirectionPressed = false;
		}

		// Direction will be determined by the tick, so here always will be 1 on X
		FVector* Direction = new FVector(1.0f, 0.0f, 0.0f);
		AddMovementInput(*Direction, Value);
	}
	ControlCharacterAnimations(Value);
}

void ACharacterBase::DownDirectionStart()
{
	InsertInputBuffer(KeyInput::Down);

	bIsDucking = true;
}

void ACharacterBase::DownDirectionStop()
{
	bIsDucking = false;
}

void ACharacterBase::UpDirection()
{
	InsertInputBuffer(KeyInput::Up);

}

// Triggers
void ACharacterBase::JumpStart()
{
	bCanMove = true;
	bPressedJump = true;
	ResetAttack();
	EAnimationState = AnimationState::Jumping;
}

void ACharacterBase::JumpStop()
{
	bPressedJump = false;

}

void ACharacterBase::AttackStart()
{
	AttackKeyPressedTimeStart = GetCurrentTime();
	bIsAttacking = true;
	InsertInputBuffer(KeyInput::Attack);
	if (GetCharacterMovement()->IsMovingOnGround())
	{
		bCanMove = false;
	}
	FVector CurrentLocation = FVector(GetActorLocation().X + 30.f, GetActorLocation().Y, GetActorLocation().Z);


	// Animation has to finish, with a little window to input next command
	if (GetCurrentTime() > AnimationAttackTimeStop - 0.2f)
	{
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), "Enemy", Enemies);
		if (GetCharacterMovement()->IsMovingOnGround())
		{
			// If attack is pressed continuously, it will be a combo
			if (AnimationAttackTimeStop < GetCurrentTime() || nAttackNumber >= AttackingComboAnimation.Num() - 1)
			{
				ResetAttack();
				bCanMove = true;
			}
			else
			{
				++nAttackNumber;
			}
		}
		if (Enemies.Num())
		{
			/*if (HitBoxArea->IsOverlappingActor(Enemies[0]))
			{*/
			ACharacterCommon* Enemy = Cast<ACharacterCommon>(Enemies[0]);
			Enemy->SetDamage(50.f);
			//}
		}
		AnimationAttackTimeStart = GetCurrentTime();
	}
}

void ACharacterBase::AttackStop()
{
	AttackKeyPressedTimeStop = GetCurrentTime(); // Not used for now
}

void ACharacterBase::SpecialStart()
{
	SpecialKeyPressedTimeStart = GetCurrentTime() + 1.f;
	InsertInputBuffer(KeyInput::Special);
	if (GetCharacterMovement()->IsMovingOnGround())
	{
		bCanMove = false;
	}
}

void ACharacterBase::SpecialStop()
{
	StopHandleStaminaCharge();
	SpecialKeyPressedTimeStop = GetCurrentTime(); // Not used for now
	ReadInputBuffer();
}

// Handling actions
void ACharacterBase::HandleAttack()
{
	if (GetCharacterMovement()->IsMovingOnGround())
	{

		CurrentFlipbook->SetFlipbook(AttackingComboAnimation[nAttackNumber]);
	}
	else
	{
		CurrentFlipbook->SetFlipbook(JumpingAttackAnimation);
	}

	AnimationAttackTimeStop = AnimationAttackTimeStart + CurrentFlipbook->GetFlipbookLength();
	if (GetCurrentTime() > AnimationAttackTimeStop)
	{
		bIsAttacking = false;
		EAnimationState = AnimationState::Idle;
		bCanMove = true;
	}
}

void ACharacterBase::HandleSpecialMoves()
{
	AnimationSpecialTimeStart = GetCurrentTime();
	if (!bIsExecutingSpecialMove)
	{
		bIsExecutingSpecialMove = true;
		// Checking if there are enough stamina
		int bIsEnoughStamina = SpecialMoves[nCurrentSpecialMove].StaminaCost <= Stamina;
		if (bIsEnoughStamina)
		{
			if ((SpecialMoves[nCurrentSpecialMove].CanBeDoneInGround && GetCharacterMovement()->IsMovingOnGround())
				|| (SpecialMoves[nCurrentSpecialMove].CanBeDoneInAir && !GetCharacterMovement()->IsMovingOnGround()))
			{
				ConsumeStamina(SpecialMoves[nCurrentSpecialMove].StaminaCost);
				CurrentFlipbook->SetFlipbook(SpecialMoves[nCurrentSpecialMove].SpecialMoveAnimation);
				if (SpecialMoves[nCurrentSpecialMove].IsProjectile)
				{
					FTimerDelegate TimerDel;
					FTimerHandle TimerHandle;

					//Binding the function with specific variables
					TimerDel.BindUFunction(this, FName("HandleProjectile"));
					//Calling MyUsefulFunction after 5 seconds without looping
					GetWorldTimerManager().SetTimer(TimerHandle, TimerDel, 0.3f, false);
				}
			}
			else
			{
				ClearBuffer();
			}
		}
		else
		{
			CurrentFlipbook->SetFlipbook(SpecialMoves[nCurrentSpecialMove].NoStaminaAnimation);
		}
		AnimationSpecialTimeStop = AnimationSpecialTimeStart + CurrentFlipbook->GetFlipbookLength();
	}
	if (GetCurrentTime() > AnimationSpecialTimeStop)
	{
		bIsSpecialMove = false;
		bIsExecutingSpecialMove = false;
		bCanMove = true;
	}
}

void ACharacterBase::HandleProjectile()
{
	UObject* SpawnActor = Cast<UObject>(StaticLoadObject(UObject::StaticClass(), NULL, TEXT("/Game/Game/Blueprints/BP_Hadouken.BP_Hadouken")));

	UBlueprint* GeneratedBP = Cast<UBlueprint>(SpawnActor);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	GetWorld()->SpawnActor<AActor>(GeneratedBP->GeneratedClass, GetActorLocation(), GetActorRotation(), SpawnParams);
	GEngine->AddOnScreenDebugMessage(1, 3.f, FColor::Red, CharacterArrowComponent->GetComponentLocation().ToString());
}

void ACharacterBase::HandleStaminaCharge()
{
	if (bIsChargingup && GetCurrentTime() > SpecialKeyPressedTimeStart + StaminaVelocityChargingInSeconds)
	{
		Stamina += StaminaChargingUnit;
		SpecialKeyPressedTimeStart = GetCurrentTime();
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
}

float ACharacterBase::GetCurrentTime()
{
	if (GetWorld())
	{
		return GetWorld()->GetRealTimeSeconds();
	}
	return 0.f;
}

// Play states
void ACharacterBase::SetDamage(float Value)
{
	Life -= Value;
	if (Life <= 0.f)
	{
		UGameplayStatics::OpenLevel(this, "level_00");
	}
}

void ACharacterBase::HealLife(float Value)
{
	Life += Value;
}

// Buffer
void ACharacterBase::ReadInputBuffer()
{
	bool isOK = false;

	TArray<int> MatchingSpecialMoves;
	int8 BufferPosition;
	// Every special move
	for (int i = 0; i < SpecialMoves.Num(); ++i)
	{
		if (SpecialMoves[i].Directions.Num() <= BufferedInput.Num())
		{
			BufferPosition = BufferedInput.Num() - 1;
			for (int j = SpecialMoves[i].Directions.Num() - 1; j >= 0; --j)
			{
				isOK = false;
				if (BufferedInput[BufferPosition] == SpecialMoves[i].Directions[j])
				{
					isOK = true;
				}
				else
				{
					break;
				}
				--BufferPosition;
			}
			if (isOK)
			{
				MatchingSpecialMoves.Push(i);
			}
		}
	}
	if (MatchingSpecialMoves.Num() > 0)
	{
		FString debugMatchingSpecialMoves = FString::Printf(TEXT("Matched: %d"), MatchingSpecialMoves.Num());
		GEngine->AddOnScreenDebugMessage(2, 1.0f, FColor::Blue, debugMatchingSpecialMoves);

		nCurrentSpecialMove = MatchingSpecialMoves[MatchingSpecialMoves.Num() - 1];
		bIsSpecialMove = true;
	}
	else
	{
		bCanMove = true;
	}
}

void ACharacterBase::InsertInputBuffer(KeyInput key)
{
	if (BufferedInput.Num() > 9)
	{
		BufferedInput.RemoveSingle(BufferedInput[0]);
	}
	BufferedInput.Push(key);
}

TArray<int32> ACharacterBase::GetBufferedInput()
{
	return BufferedInput;
}

void ACharacterBase::ClearBuffer()
{
	BufferedInput.Empty();
}

// Stamina
void ACharacterBase::ControlStamina()
{
	if (Stamina <= MaxStamina)
	{
		bIsChargingup = true;
	}
	else
	{
		StopHandleStaminaCharge();
	}
}

void ACharacterBase::ConsumeStamina(float Value)
{
	Stamina = Stamina - Value;
}
