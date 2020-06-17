// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBase.h"
#include "Kismet/KismetSystemLibrary.h"


// Sets default values
AEnemyBase::AEnemyBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	this->Tags.Add("Enemy");

	CurrentFlipbook = CreateDefaultSubobject<UPaperFlipbookComponent>("CurrentFlipbook");
	CurrentFlipbook->SetupAttachment(GetCapsuleComponent());
	CurrentFlipbook->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	VisibilityArea = CreateDefaultSubobject<USphereComponent>(TEXT("VisibilityArea"));
	VisibilityArea->InitSphereRadius(300.f);
	VisibilityArea->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	HitArea = CreateDefaultSubobject<USphereComponent>(TEXT("EnemyHitCollision"));
	HitArea->InitSphereRadius(45.f);
	HitArea->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	// Setting default character movement's params
	GetCharacterMovement()->MaxAcceleration = 5000;
	GetCharacterMovement()->GroundFriction = 20;
	GetCharacterMovement()->MaxWalkSpeed = 300;
	GetCharacterMovement()->MaxWalkSpeedCrouched = 110;
	GetCharacterMovement()->JumpZVelocity = 500;
	GetCharacterMovement()->BrakingDecelerationFalling = 5000;
	GetCharacterMovement()->AirControl = 0.8;
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->SetPlaneConstraintAxisSetting(EPlaneConstraintAxisSetting::Y);

	Life = 100.f;
	AttackDamage = 10.f;

	GetCapsuleComponent()->SetCapsuleHalfHeight(27.131327);
	GetCapsuleComponent()->SetCapsuleRadius(18.100616);
	CurrentFlipbook->SetWorldLocation(FVector(0, 0, -41));

	AnimationFlipbookTimeStop = -1.f;
	AnimationAttackCompleteTimeStop = -1.f;
}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	SetAttackAnimationFlags();
	ResetAttackAnimationFlags();

	Player = Cast<ACharacterCommon>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
}

// Called every frame
void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (LastAttackTime + TimeBetweenAttacks < GetCurrentTime())
	{
		bCanMove = true;
	}

	if (!bIsDamaged && !bIsAttacking && bCanMove && VisibilityArea->IsOverlappingActor(Player))
	{
		// Depending on distance from player, attack, follow him or wait
		float DistanceBetweenActors = GetActorLocation().Distance(GetActorLocation(), Player->GetActorLocation());


		if (DistanceBetweenActors > 60.f)
		{
			FacePlayer();
			AddMovementInput(GetPlayerPosition(), 1.f);
			ControlCharacterAnimations(1.f);
		}
		else
		{
			ControlCharacterAnimations(0.f);
		}

		if (HitArea->IsOverlappingActor(Player))
		{
			AttackStart();
		}
		else
		{
			ResetAttack();
		}


		if (AnimationActionTimeStop > GetCurrentTime())
		{
			HandleAttack();
		}

	}
	else
	{
		ControlCharacterAnimations(0.f);
	}
}

void AEnemyBase::FacePlayer()
{
	FRotator toPlayerRotation = GetPlayerPosition().Rotation();
	toPlayerRotation.Pitch = 0;
	if (toPlayerRotation.Yaw == 0.f)
	{
		bIsMovingRight = true;
	}
	else
	{
		bIsMovingRight = false;
	}
	RootComponent->SetWorldRotation(toPlayerRotation);
}

void AEnemyBase::AttackStart()
{
	bIsAttacking = true;

	// Animation has to finish, with a little window to input next command
	//if (AnimationAttackCompleteTimeStop + 1.f > GetCurrentTime())
	//{
	//	if (GetCharacterMovement()->IsMovingOnGround())
	//	{
	//		// If attack is pressed continuously, it will be a combo 
	//		if (nAttackNumber >= AttackingComboAnimation.Num() - 1)
	//		{
	//			ResetAttack();
	//		}
	//		else
	//		{
	//			++nAttackNumber;
	//			CurrentAttackHasHitObjective = false;
	//		}
	//	}
	//}
}

// Handling actions
void AEnemyBase::HandleAttack()
{
	/*Super::HandleAttack();

	bCanMove = false;

	if (AnimationAttackCompleteTimeStop < GetCurrentTime())
	{
		bIsAttacking = false;
		CurrentAttackHasHitObjective = false;
		EAnimationState = AnimationState::Idle;
		LastAttackTime = GetCurrentTime();
		ResetAttackAnimationFlags();
	}*/
}

//void AEnemyBase::ApplyHitCollide(TArray<FComboAttackStruct> Combo)
//{
//	//Super::ApplyHitCollide(Combo);
//
//	//// Tracing collision
//	//FHitResult* HitResult = new FHitResult();
//	//if (UKismetSystemLibrary::BoxTraceSingle(GetWorld(), GetActorLocation(), CurrentTraceHit, FVector(20.0, 20.0, 20.0), GetActorRotation(), ETraceTypeQuery::TraceTypeQuery2, false, ActorsToIgnore, EDrawDebugTrace::ForDuration, *HitResult, true))
//	//{
//	//	if (&HitResult->Actor)
//	//	{
//	//		Player->SetDamage(Combo[nAttackNumber].AnimationHits[nCurrentComboHit].DamageValue);
//	//		if (nCurrentComboHit >= Combo[nAttackNumber].AnimationHits.Num())
//	//		{
//	//			CurrentAttackHasHitObjective = true;
//	//		}
//	//	}
//	//}
//}

void AEnemyBase::SetAttackAnimation()
{
	HandleAttack();
}

void AEnemyBase::ResetAttack()
{
	bIsAttacking = false;
	EAnimationState = AnimationState::Idle;
	//bCanMove = true;
}

void AEnemyBase::UpdateAnimations()
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
	case AnimationState::Attacking:
		HandleAttack();
		break;
	case AnimationState::HitTop:
		CurrentFlipbook->SetFlipbook(HitTopAnimation);
		break;
	default:
		CurrentFlipbook->SetFlipbook(IdleAnimation);
		break;
	}
}

void AEnemyBase::ControlCharacterAnimations(float characterMovementSpeed)
{
	if (!GetCharacterMovement()->IsMovingOnGround())
	{
		if (bIsAttacking)
		{
			EAnimationState = AnimationState::Attacking;
		}
		else
		{
			if (fabs(characterMovementSpeed) > 0.f)
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
			if (bIsAttacking)
			{
				EAnimationState = AnimationState::Attacking;
			}
			else
			{
				if (fabs(characterMovementSpeed) > 0.f)
				{
					EAnimationState = AnimationState::Walking;
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

void AEnemyBase::SetDamage(float Value)
{
	Super::SetDamage(Value);

	Life -= Value;
	if (Life <= 0)
	{
		Destroy();
	}
}

void AEnemyBase::HealLife(float Value)
{
	Life += Value;
}

void AEnemyBase::SetAttackAnimationFlags()
{
	/*for (FComboAttackStruct combo : AttackingComboAnimation)
	{
		FComboAnimationFlagsStruct element;
		for (FComboAttackHitsStruct hit : combo.AnimationHits)
		{
			element.bIsComboHits.Add(false);
		}
		element.bIsComboStart = false;
		element.bIsComboEnd = false;
		ComboAnimationFlags.Add(element);
	}*/
}

void AEnemyBase::ResetAttackAnimationFlags()
{
	//ComboAnimationFlags.Empty();
	SetAttackAnimationFlags();
}

// Called to bind functionality to input
void AEnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

FVector AEnemyBase::GetPlayerPosition()
{
	FVector PlayerPosition = Player->GetActorLocation() - GetActorLocation();
	PlayerPosition.Normalize();
	return PlayerPosition;
}