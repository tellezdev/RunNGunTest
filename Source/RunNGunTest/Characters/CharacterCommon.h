// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PaperSprite.h"
#include "PaperFlipbookComponent.h"
#include "PaperFlipbook.h"
#include "PaperCharacter.h"
#include "PaperSpriteComponent.h"
#include "../HUD/GameHUD.h"
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
	ChargingUp,
	HitTop
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector HitBoxPosition = FVector(0.f, 0.f, 0.f);
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

	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	// Sets default values for this character's properties
	ACharacterCommon();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Data")
		FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Data")
		UTexture2D* Portrait;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Data")
		float Life = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Data")
		float MaxLife = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Data")
		float Stamina = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Data")
		float MaxStamina = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Data")
		float StaminaVelocityChargingInSeconds = 0.05f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Data")
		float StaminaChargingUnit = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Data")
		float WalkingSpeed = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Data")
		float CrouchingSpeed = 0.5f;

	// Config params
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Params")
		bool bIsDamaged = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Params")
		bool bIsInHitAttackFrames = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Params")
		bool CurrentAttackHasHitObjective = false;
	UPROPERTY()
		bool bCurrentHitCollisionIsDone = false;

	// Character Movement
	UPROPERTY()
		bool bIsMovingRight;
	UPROPERTY()
		bool bCanMove = true;
	UPROPERTY()
		bool bIsDucking;
	UPROPERTY()
		bool bIsLeft;
	UPROPERTY()
		bool bIsRight;
	UPROPERTY()
		bool bIsUp;
	UPROPERTY()
		bool bIsDown;
	UPROPERTY()
		bool bIsAttacking;
	UPROPERTY()
		bool bIsChargingup;
	UPROPERTY()
		int8 nAttackNumber = 0;
	UPROPERTY()
		int8 nCurrentComboHit = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Data")
		float nRecoveryTime = 1.f;

	// Animation Times
	UPROPERTY()
		float AnimationAttackCompleteTimeStop;
	UPROPERTY()
		float AnimationFlipbookTimeStop;
	UPROPERTY()
		bool bIsAnimationAttackComplete = true;
	UPROPERTY()
		float AnimationOtherTimeStop;

	// HUD
	UPROPERTY()
		AGameHUD* GameHUD;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CollisionsHitInformation")
		TArray<AActor*> ActorsToIgnore;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations Movement")
		TEnumAsByte<AnimationState> EAnimationState = AnimationState::Idle;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character Components")
		UArrowComponent* CharacterArrowComponent;

	// Flip books
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Animations")
		UPaperFlipbookComponent* CurrentFlipbook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations Movement")
		UPaperFlipbook* IdleAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations Movement")
		UPaperFlipbook* WalkingAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations Movement")
		UPaperFlipbook* JumpingAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations Movement")
		UPaperFlipbook* JumpingForwardAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations Movement")
		UPaperFlipbook* DuckingAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations Attacking")
		TArray<FComboAttackStruct> AttackingComboAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations Attacking")
		TArray<FComboAttackStruct> AttackingJumpingAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations Attacking")
		TArray<FComboAttackStruct> AttackingCrouchingAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations Other")
		UPaperFlipbook* HitTopAnimation;

	UPROPERTY()
		TArray<FComboAnimationFlags> ComboAnimationFlags;

	UFUNCTION()
		void ControlCharacterRotation();

	UFUNCTION()
		void SetDamagedState(float DeltaTime);

	UFUNCTION()
		virtual void BindDataHUD();

	UFUNCTION()
		virtual void HandleAttack();

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

	UFUNCTION(BlueprintCallable)
		virtual void HealStamina(float Value);

	UFUNCTION()
		virtual void ApplyHitCollide(TArray<FComboAttackStruct> Combo);

	UFUNCTION()
		void DoCombo(TArray<FComboAttackStruct> Combo);

	UFUNCTION()
		virtual void FinishCombo();
};
