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
}

// Called when the game starts or when spawned
void AGenericProjectile::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AGenericProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGenericProjectile::BeginOverlap(UPrimitiveComponent* OverlappedComponent,
									  AActor* OtherActor,
									  UPrimitiveComponent* OtherComp,
									  int32 OtherBodyIndex,
									  bool bFromSweep,
									  const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag("Projectileable"))
	{

		FString debug0 = FString::Printf(TEXT("Actor: %s"), *OtherActor->GetName());
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Orange, debug0);
		/*FString debug0 = FString::Printf(TEXT("Ball overlapped!"));
		GEngine->AddOnScreenDebugMessage(0, 3.0f, FColor::Red, debug0);*/
		this->Destroy();
	}
}
