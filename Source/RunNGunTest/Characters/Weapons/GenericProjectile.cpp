// Fill out your copyright notice in the Description page of Project Settings.


#include "GenericProjectile.h"
#include "Engine/Engine.h"

// Sets default values
AGenericProjectile::AGenericProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	this->Tags.Add("Projectile");

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));

	ProjectileCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("ProjectileCapsule"));

	ProjectilePaperFlipbook = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("ProjectilePaperFlipbook"));
	ProjectilePaperFlipbook->SetupAttachment(ProjectileCapsule);

	ProjectileCapsule->OnComponentBeginOverlap.AddDynamic(this, &AGenericProjectile::BeginOverlap);

	RootComponent = ProjectileCapsule;
}

// Called when the game starts or when spawned
void AGenericProjectile::BeginPlay()
{
	Super::BeginPlay();

	ProjectilePaperFlipbook->SetFlipbook(ProjectileLoopPaperFlipbook);
}

// Called every frame
void AGenericProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bHasHit && AnimationTimeEnd < GetCurrentTime())
	{
		this->Destroy();
	}
}

void AGenericProjectile::BeginOverlap(UPrimitiveComponent* OverlappedComponent,
									  AActor* OtherActor,
									  UPrimitiveComponent* OtherComp,
									  int32 OtherBodyIndex,
									  bool bFromSweep,
									  const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag("Scenario") || OtherActor->ActorHasTag("Enemy"))
	{
		FString debug0 = FString::Printf(TEXT("Actor: %s"), *OtherActor->GetName());
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Orange, debug0);
		if (OtherActor->ActorHasTag("Enemy"))
		{
			Enemy = Cast<AEnemyBase>(OtherActor);
			if (Enemy->GetCapsuleComponent()->IsOverlappingComponent(OverlappedComponent))
			{
				Enemy->SetDamage(Damage);
				DestroyProjectile();
			}
		}
		else
		{
			DestroyProjectile();
		}
	}
}

void AGenericProjectile::DestroyProjectile()
{
	bHasHit = true;
	ProjectileMovement->StopMovementImmediately();
	ProjectilePaperFlipbook->SetFlipbook(ProjectileEndPaperFlipbook);
	AnimationTimeEnd = GetCurrentTime() + ProjectilePaperFlipbook->GetFlipbookLength();
	ProjectilePaperFlipbook->SetLooping(false);
}

float AGenericProjectile::GetCurrentTime()
{
	if (GetWorld())
	{
		return GetWorld()->GetRealTimeSeconds();
	}
	return 0.f;
}