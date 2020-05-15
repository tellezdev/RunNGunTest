// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PaperSprite.h"
#include "PaperFlipbookComponent.h"
#include "PaperFlipbook.h"
#include "PaperCharacter.h"
#include "PaperSpriteComponent.h"
#include "CharacterCommon.generated.h"


UENUM(BlueprintType)
enum AnimationState
{
	Idle,
	Jumping,
	JumpingForward,
	Walking,
	Ducking,
	Attacking,
	SpecialMove,
	ChargingUp
};

USTRUCT()
struct FComboAnimationFlags
{
	GENERATED_BODY()

public:
	UPROPERTY()
		bool bIsComboStart = false;
	UPROPERTY()
		TArray<bool> bIsComboHits;
	UPROPERTY()
		bool bIsComboEnd = false;
};

USTRUCT(BlueprintType)
struct FComboAttackHitsStruct
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UPaperFlipbook* AttackAnimationHit;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float DamageValue;
};

USTRUCT(BlueprintType)
struct FComboAttackStruct
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<int32> Directions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool IsChargeable; // TODO

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool IsProjectile; // TODO

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool CanBeDoneInGround;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool CanBeDoneInAir;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float DamageValue;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UPaperFlipbook* AttackAnimationStart;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FComboAttackHitsStruct> AttackAnimationHits;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UPaperFlipbook* AttackAnimationEnd;

};

UCLASS()
class RUNNGUNTEST_API ACharacterCommon : public ACharacter
{
	GENERATED_BODY()


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Sets default values for this character's properties
	ACharacterCommon();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Data")
		FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Data")
		UPaperSprite* Portrait;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Data")
		float Life = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Data")
		float Stamina = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Data")
		float MaxStamina = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Data")
		float StaminaVelocityChargingInSeconds = 0.05f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Data")
		float StaminaChargingUnit = 0.1f;

	UPROPERTY()
		float HitBoxOrientation = 30.f;
	UPROPERTY()
		bool CurrentAttackHasHitObjective = false;

	// Character Movement
	UPROPERTY()
		bool bIsMovingRight;
	UPROPERTY()
		bool bCanMove = true;
	UPROPERTY()
		bool bIsDucking;
	UPROPERTY()
		bool bIsAttacking;
	UPROPERTY()
		bool bIsChargingup;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CollisionsHitInformation")
		TArray<AActor*> ActorsToIgnore;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Animations")
		TEnumAsByte<AnimationState> EAnimationState = AnimationState::Idle;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character Components")
		UArrowComponent* CharacterArrowComponent;

	// Flip books
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Animations")
		UPaperFlipbookComponent* CurrentFlipbook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Animations")
		UPaperFlipbook* IdleAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Animations")
		UPaperFlipbook* WalkingAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Animations")
		UPaperFlipbook* AttackingAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Animations")
		UPaperFlipbook* JumpingAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Animations")
		UPaperFlipbook* JumpingForwardAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Animations")
		UPaperFlipbook* JumpingAttackAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Animations")
		UPaperFlipbook* DuckingAnimation;
	// ---------------------

	UFUNCTION()
		virtual void ResetAttack();
	// Other stuff
	UFUNCTION()
		virtual void UpdateAnimations();

	UFUNCTION()
		virtual void ControlCharacterAnimations(float characterMovementSpeed);

	UFUNCTION()
		virtual void SetAnimationFlags();

	UFUNCTION()
		virtual void ResetAnimationFlags();

	UFUNCTION()
		virtual float GetCurrentTime();

	UFUNCTION(BlueprintCallable)
		virtual void SetDamage(float Value);

	UFUNCTION(BlueprintCallable)
		virtual void HealLife(float Value);

	UFUNCTION()
		virtual void ApplyHitCollide(TArray<FComboAttackStruct> Combo);

	UFUNCTION()
		virtual void DoCombo(TArray<FComboAttackStruct> Combo);
};
