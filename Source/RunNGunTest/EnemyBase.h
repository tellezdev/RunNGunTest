// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine.h"
#include "CharacterCommon.h"
#include "GameFramework/Character.h"
#include "PaperSprite.h"
#include "PaperFlipbookComponent.h"
#include "PaperFlipbook.h"
#include "EnemyBase.generated.h"


UCLASS()
class RUNNGUNTEST_API AEnemyBase : public ACharacterCommon
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyBase();

	ACharacterCommon* Player;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Data")
		FString EnemyName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Data")
		UPaperSprite* EnemyPortrait;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Data")
		float EnemyLife;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Data")
		float AttackDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy State")
		bool bIsMovingRight;
	UPROPERTY()
		bool bCanMove = true;
	UPROPERTY()
		bool bIsAttacking;
	UPROPERTY()
		float EnemyStartAttackTime;
	UPROPERTY()
		float EnemyStopAttackTime;
	UPROPERTY()
		float AnimationAttackTimeStart;
	UPROPERTY()
		float AnimationAttackTimeStop;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Data")
		float TimeBetweenAttacks;

	TArray<AActor*> ActorsToIgnore;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy State")
		TEnumAsByte<AnimationState> EEnemyAnimationState = AnimationState::Idle;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		USphereComponent* VisibilityArea;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		USphereComponent* HitArea;
	UPROPERTY()
		float HitPlayerPosition;

	// Flip books
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy Animations")
		UPaperFlipbookComponent* EnemyAnimationComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Animations")
		UPaperFlipbook* IdleAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Animations")
		UPaperFlipbook* WalkingAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Animations")
		UPaperFlipbook* AttackingAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Animations")
		UPaperFlipbook* JumpingAttackAnimation;


	// Movement related
	UFUNCTION(BlueprintCallable)
		void Attack();
	UFUNCTION()
		void SetAttackAnimation();
	UFUNCTION()
		void ResetAttack();

	// Other stuff
	UFUNCTION()
		void UpdateAnimations();

	UFUNCTION()
		void ControlCharacterAnimations(float characterMovementSpeed);

	UFUNCTION()
		float GetCurrentTime();

	UFUNCTION(BlueprintCallable)
		void HitPlayer();

	void SetDamage(float Value);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
