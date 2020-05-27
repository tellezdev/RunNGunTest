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
	auto PlayerController = GetWorld()->GetFirstPlayerController();
	GameHUD = Cast<AGameHUD>(PlayerController->GetHUD());
}

// Called every frame
void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Sets where the sprite has to face
	float yaw = 0.0f;
	// Saving hit player position to set the right side to collide
	HitBoxOrientation = 30.f;
	if (!bIsMovingRight)
	{
		yaw = 180.0f;
		HitBoxOrientation = -30.f;
	}
	FRotator* rotation = new FRotator(0.0f, yaw, 1.0f);
	GetController()->SetControlRotation(*rotation);

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

	// Player is being hit by something
	if (AnimationOtherTimeStop < GetCurrentTime())
	{
		bIsDamaged = false;
	}
	else
	{
		GetCapsuleComponent()->MoveComponent(FVector(bIsMovingRight ? -70.f * DeltaTime : 70.f * DeltaTime, 0.f, 0.f), GetActorRotation(), true);
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
			InsertInputBuffer(KeyInput::DownLeft);
			MovementSpeed = CrouchingSpeed;
			MoveCharacter(CrouchingSpeed * -1.f, false);
		}
		else if (bIsRight && bIsDown)
		{
			InsertInputBuffer(KeyInput::DownRight);
			MovementSpeed = CrouchingSpeed;
			MoveCharacter(CrouchingSpeed, true);
		}
		else if (bIsLeft && bIsUp)
		{
			InsertInputBuffer(KeyInput::UpLeft);
			MovementSpeed = WalkingSpeed;
			MoveCharacter(WalkingSpeed * -1.f, false);
		}
		else if (bIsRight && bIsUp)
		{
			InsertInputBuffer(KeyInput::UpRight);
			MovementSpeed = WalkingSpeed;
			MoveCharacter(WalkingSpeed, true);
		}
		else if (bIsDown)
		{
			InsertInputBuffer(KeyInput::Down);
			bIsDirectionPressed = true;
		}
		else if (bIsLeft)
		{
			InsertInputBuffer(KeyInput::Left);
			MovementSpeed = WalkingSpeed;
			MoveCharacter(WalkingSpeed * -1.f, false);
		}
		else if (bIsRight)
		{
			InsertInputBuffer(KeyInput::Right);
			MovementSpeed = WalkingSpeed;
			MoveCharacter(WalkingSpeed, true);
		}
		else if (bIsUp)
		{
			InsertInputBuffer(KeyInput::Up);
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
	bIsDirectionPressed = true;
	bIsMovingRight = IsFacingRight;
	FVector* Direction = new FVector(1.0f, 0.0f, 0.0f);
	AddMovementInput(*Direction, MovementSpeed);
}

//void ACharacterBase::MoveRight(float Value)
//{
//	if (bCanMove)
//	{
//		if (!bIsDirectionPressed && Value != 0.f)
//		{
//			if (Value > 0.f)
//			{
//				InsertInputBuffer(KeyInput::Right);
//			}
//			else
//			{
//				InsertInputBuffer(KeyInput::Left);
//			}
//		}
//
//		if (Value > 0.0f)
//		{
//			bIsMovingRight = true;
//			bIsDirectionPressed = true;
//		}
//		else if (Value < 0.0f)
//		{
//			bIsMovingRight = false;
//			bIsDirectionPressed = true;
//		}
//		else
//		{
//			bIsDirectionPressed = false;
//		}
//
//		// Direction will be determined by the tick, so here always will be 1 on X
//		FVector* Direction = new FVector(1.0f, 0.0f, 0.0f);
//		AddMovementInput(*Direction, Value);
//	}
//	ControlCharacterAnimations(Value);
//}

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
	InsertInputBuffer(KeyInput::Attack);

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
				ClearBuffer();
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
		ClearBuffer();
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
		GameHUD->SetStamina(Stamina);
		SpecialKeyPressedTimeStart = GetCurrentTime();
	}
}

void ACharacterBase::DoCombo(TArray<FComboAttackStruct> Combo)
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
		CurrentFlipbook->SetFlipbook(Combo[nAttackNumber].AttackAnimationStart);
		ComboAnimationFlags[nAttackNumber].bIsComboStart = true;
		AnimationFlipbookTimeStop = GetCurrentTime() + CurrentFlipbook->GetFlipbookLength();
		AnimationAttackCompleteTimeStop = GetCurrentTime() + CurrentFlipbook->GetFlipbookLength();
		bIsAnimationAttackComplete = false;
	}
	else if (bIsComboDone && !ComboAnimationFlags[nAttackNumber].bIsComboEnd)
	{
		CurrentFlipbook->SetFlipbook(Combo[nAttackNumber].AttackAnimationEnd);
		ComboAnimationFlags[nAttackNumber].bIsComboEnd = true;
		AnimationFlipbookTimeStop = GetCurrentTime() + CurrentFlipbook->GetFlipbookLength();
		AnimationAttackCompleteTimeStop = GetCurrentTime() + CurrentFlipbook->GetFlipbookLength();
		bIsInHitAttackFrames = false;
		bIsAnimationAttackComplete = true;
	}
	else
	{
		if (!ComboAnimationFlags[nAttackNumber].bIsComboHits[nCurrentComboHit])
		{
			bCurrentHitCollisionIsDone = false;
			CurrentFlipbook->SetFlipbook(Combo[nAttackNumber].AttackAnimationHits[nCurrentComboHit].AttackAnimationHit);
			ComboAnimationFlags[nAttackNumber].bIsComboHits[nCurrentComboHit] = true;
			AnimationFlipbookTimeStop = GetCurrentTime() + CurrentFlipbook->GetFlipbookLength();
			AnimationAttackCompleteTimeStop = GetCurrentTime() + CurrentFlipbook->GetFlipbookLength();
			bIsInHitAttackFrames = true;
		}
		if (nCurrentComboHit < ComboAnimationFlags[nAttackNumber].bIsComboHits.Num() - 1)
		{
			++nCurrentComboHit;
		}
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

// Play states
void ACharacterBase::SetDamage(float Value)
{
	bIsDamaged = true;
	EAnimationState = AnimationState::HitTop;
	AnimationOtherTimeStop = GetCurrentTime() + CurrentFlipbook->GetFlipbookLength();
	Life -= Value;
	GameHUD->SetLife(Life);
	if (Life <= 0.f)
	{
		UGameplayStatics::OpenLevel(this, "level_00");
	}
}

void ACharacterBase::HealLife(float Value)
{
	Life += Value;
	GameHUD->SetLife(Life);
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
	if (!bIsDirectionPressed || LastDirectionPressed != key)
	{
		if (BufferedInput.Num() > 9)
		{
			BufferedInput.RemoveSingle(BufferedInput[0]);
		}
		BufferedInput.Push(key);
		LastDirectionPressed = key;

		GameHUD->DrawBuffer(BufferedInput);
	}
}

TArray<int32> ACharacterBase::GetBufferedInput()
{
	return BufferedInput;
}

void ACharacterBase::ClearBuffer()
{
	BufferedInput.Empty();
	GameHUD->DrawBuffer(BufferedInput);
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
	GameHUD->SetStamina(Stamina);
}

void ACharacterBase::ApplyHitCollide(TArray<FComboAttackStruct> Combo)
{
	// Hit box position
	FVector CurrentLocation = FVector(GetActorLocation().X + HitBoxOrientation, GetActorLocation().Y, GetActorLocation().Z);

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