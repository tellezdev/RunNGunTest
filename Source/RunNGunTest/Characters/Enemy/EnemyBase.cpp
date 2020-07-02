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

	Player = Cast<ACharacterCommon>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
}

// Called every frame
void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ActionState != EActionState::ActionDamaged && ActionState != EActionState::ActionAttacking && CanMove() && VisibilityArea->IsOverlappingActor(Player) && nLastActionTime + TimeBetweenAttacks < GetCurrentTime())
	{
		SetCanMove(true);
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
	if (CanMove())
	{
		SetActionState(EActionState::ActionAttacking);

		if (bIsAttackFinished && AnimationActionCompleteTimeStop < GetCurrentTime())
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
			}
			HandleAttack();
		}
	}
}

void AEnemyBase::UpdateAnimations()
{
	Super::UpdateAnimations();
}

void AEnemyBase::ControlCharacterAnimations(float characterMovementSpeed)
{
	Super::ControlCharacterAnimations(characterMovementSpeed);
}

void AEnemyBase::SetDamage(float Value)
{
	Super::SetDamage(Value);

	if (Life <= 0)
	{
		Destroy();
	}
}

FVector AEnemyBase::GetPlayerPosition()
{
	FVector PlayerPosition = Player->GetActorLocation() - GetActorLocation();
	PlayerPosition.Normalize();
	return PlayerPosition;
}