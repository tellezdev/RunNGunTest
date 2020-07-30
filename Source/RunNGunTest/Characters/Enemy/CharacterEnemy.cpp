// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterEnemy.h"
#include "Kismet/KismetSystemLibrary.h"


// Sets default values
ACharacterEnemy::ACharacterEnemy()
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
void ACharacterEnemy::BeginPlay()
{
	Super::BeginPlay();

	//GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	Player = Cast<ACharacterCommon>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	ActorsToIgnore.Append(GetActorsWithOtherTag("Enemy"));
}

// Called every frame
void ACharacterEnemy::Tick(float DeltaTime)
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
			FacePlayer();
			AttackStart();
		}
	}
	else
	{
		ControlCharacterAnimations(0.f);
	}
}

void ACharacterEnemy::FacePlayer()
{
	float yaw = 0.0f;
	if (Player->GetActorLocation().X < GetActorLocation().X)
	{
		bIsMovingRight = false;
		yaw = 180.0f;
	}
	else
	{
		bIsMovingRight = true;
	}

	FRotator* rotation = new FRotator(0.0f, yaw, 1.0f);
	RootComponent->SetWorldRotation(*rotation);
}

void ACharacterEnemy::AttackStart()
{
	if (CanMove())
	{
		DoAttack();
	}
}

void ACharacterEnemy::UpdateAnimations()
{
	Super::UpdateAnimations();
}

void ACharacterEnemy::ControlCharacterAnimations(float characterMovementSpeed)
{
	Super::ControlCharacterAnimations(characterMovementSpeed);
}

void ACharacterEnemy::SetDamage(float Value)
{
	Super::SetDamage(Value);

	if (Life <= 0)
	{
		Destroy();
	}
}

FVector ACharacterEnemy::GetPlayerPosition()
{
	FVector PlayerPosition = Player->GetActorLocation() - GetActorLocation();
	PlayerPosition.Normalize();
	return PlayerPosition;
}