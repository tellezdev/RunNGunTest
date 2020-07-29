// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PaperFlipbookComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "../Enemy/CharacterEnemy.h"
#include "GenericProjectile.generated.h"

UCLASS()
class RUNNGUNTEST_API AGenericProjectile : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGenericProjectile();

	ACharacterEnemy* Enemy;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
		bool bHasHit = false;

	UPROPERTY()
		float AnimationTimeEnd;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UPaperFlipbookComponent* ProjectilePaperFlipbook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UPaperFlipbook* ProjectileLoopPaperFlipbook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UPaperFlipbook* ProjectileEndPaperFlipbook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UCapsuleComponent* ProjectileCapsule;

	UFUNCTION()
		void BeginOverlap(UPrimitiveComponent* OverlappedComponent,
						  AActor* OtherActor,
						  UPrimitiveComponent* OtherComp,
						  int32 OtherBodyIndex,
						  bool bFromSweep,
						  const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
		void DestroyProjectile();

	UFUNCTION()
		float GetCurrentTime();
};
