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
	HitArea->InitSphereRadius(75.f);
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
	EnemyStopAttackTime = -1;
	TimeBetweenAttacks = 1.f;
	AttackDamage = 10.f;

	GetCapsuleComponent()->SetCapsuleHalfHeight(27.131327);
	GetCapsuleComponent()->SetCapsuleRadius(18.100616);
	CurrentFlipbook->SetWorldLocation(FVector(0, 0, -41));

	//AEnemyBase::OnActorBeginOverlap.AddDynamic(this, &AEnemyBase::Overlapped);
}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	// Sets where the sprite has to face
	float yaw = 0.0f;
	if (!bIsMovingRight)
	{
		yaw = 180.0f;
	}
	FRotator* rotation = new FRotator(0.0f, yaw, 1.0f);
	GetController()->SetControlRotation(*rotation);
}

// Called every frame
void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Player = Cast<ACharacterCommon>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	if (VisibilityArea->IsOverlappingActor(Player))
	{
		// Depending on distance from player, attack, follow him or wait
		float DistanceBetweenActors = GetActorLocation().Distance(GetActorLocation(), Player->GetActorLocation());
		if (DistanceBetweenActors > 75.f)
		{
			FVector toPlayer = Player->GetActorLocation() - GetActorLocation();
			toPlayer.Normalize();
			AddMovementInput(toPlayer, 1.f);
			FRotator toPlayerRotation = toPlayer.Rotation();
			toPlayerRotation.Pitch = 0;
			RootComponent->SetWorldRotation(toPlayerRotation);
			// Saving hit player position to set the right side to collide
			HitBoxOrientation = toPlayerRotation.Yaw == 0.f ? 30.f : -30.f;
			ControlCharacterAnimations(1.f);
		}
		else
		{
			ControlCharacterAnimations(0.f);
		}

		if (HitArea->IsOverlappingActor(Player))
		{
			Attack();
		}
		else
		{
			ResetAttack();
		}
	}
	else
	{
		ControlCharacterAnimations(0.f);
	}
}

void AEnemyBase::Attack()
{
	if (EnemyStopAttackTime + TimeBetweenAttacks < GetCurrentTime())
	{
		bIsAttacking = true;

		// Hit box will grow to detect collision
		FVector CurrentLocation = FVector(GetActorLocation().X + HitBoxOrientation, GetActorLocation().Y, GetActorLocation().Z);

		// Tracing collision
		FHitResult* HitResult = new FHitResult();

		if (UKismetSystemLibrary::BoxTraceSingle(GetWorld(), GetActorLocation(), CurrentLocation, FVector(20.0, 20.0, 20.0), GetActorRotation(), ETraceTypeQuery::TraceTypeQuery2, false, ActorsToIgnore, EDrawDebugTrace::ForDuration, *HitResult, true))
		{

			DrawDebugLine(GetWorld(), GetActorLocation(), CurrentLocation, FColor::Blue, true);
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Hit: %s"), *HitResult->Actor->GetName()));
			HitPlayer();
		}

		EnemyStartAttackTime = GetCurrentTime();

		if (GetCharacterMovement()->IsMovingOnGround())
		{
			bCanMove = false;
		}
		// Animation has to finish, with a little window to input next command
		if (GetCurrentTime() > AnimationAttackTimeStop && GetCharacterMovement()->IsMovingOnGround())
		{
			bCanMove = true;
		}
		AnimationAttackTimeStart = GetCurrentTime();
	}
}

void AEnemyBase::SetAttackAnimation()
{
	if (GetCharacterMovement()->IsMovingOnGround())
	{
		CurrentFlipbook->SetFlipbook(AttackingAnimation);
	}
	else
	{
		CurrentFlipbook->SetFlipbook(JumpingAttackAnimation);
	}
	AnimationAttackTimeStop = AnimationAttackTimeStart + CurrentFlipbook->GetFlipbookLength();
	EnemyStopAttackTime = AnimationAttackTimeStop;
	if (GetCurrentTime() > AnimationAttackTimeStop)
	{
		ResetAttack();
	}
}

void AEnemyBase::ResetAttack()
{
	bIsAttacking = false;
	EAnimationState = AnimationState::Idle;
	bCanMove = true;
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
	case AnimationState::Attacking:
		SetAttackAnimation();
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
		if (bIsAttacking)
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
				EAnimationState = AnimationState::Idle;

			}
		}
	}
	UpdateAnimations();
}

float AEnemyBase::GetCurrentTime()
{
	if (GetWorld())
	{
		return GetWorld()->GetRealTimeSeconds();
	}
	return 0.f;
}

void AEnemyBase::HitPlayer()
{
	Player->SetDamage(AttackDamage);
}

void AEnemyBase::SetDamage(float Value)
{
	Life -= Value;
	if (Life == 0)
	{
		Destroy();
	}
}

void AEnemyBase::HealLife(float Value)
{
	Life += Value;
}

// Called to bind functionality to input
void AEnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

