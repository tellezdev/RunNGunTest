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

// Combo Attacking Structs
USTRUCT()
struct FComboAnimationFlagsStruct
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
		UPaperFlipbook* AnimationHit;
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
		TArray<int32> Directions; //TODO

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool IsChargeable; // TODO

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool IsProjectile; // TODO

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UPaperFlipbook* AnimationStart;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FComboAttackHitsStruct> AnimationHits;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UPaperFlipbook* AnimationEnd;

};

// Special Moves Structs
USTRUCT()
struct FSpecialMoveAnimationFlagsStruct
{
	GENERATED_BODY()

public:
	UPROPERTY()
		bool bIsSpecialMoveStart = false;
	UPROPERTY()
		TArray<bool> bIsSpecialMoveHits;
	UPROPERTY()
		bool bIsSpecialMoveEnd = false;
	UPROPERTY()
		bool bIsCompleted = false;
};

USTRUCT()
struct FSpecialMoveAnimationsFlagsStruct
{
	GENERATED_BODY()

public:
	UPROPERTY()
		TArray<FSpecialMoveAnimationFlagsStruct> Animations;
};

USTRUCT(BlueprintType)
struct FSpecialMoveAnimationStruct
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UPaperFlipbook* Animation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float DamageValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector HitBoxPosition = FVector(0.f, 0.f, 0.f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector ImpulseToCharacter = FVector(0.f, 0.f, 0.f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float InterpolationSpeed = 5.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool IsProjectile;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UObject* GenericProjectile;
};

USTRUCT(BlueprintType)
struct FSpecialMoveCompleteAnimationStruct
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FSpecialMoveAnimationStruct AnimationStart;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FSpecialMoveAnimationStruct> AnimationHits;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FSpecialMoveAnimationStruct AnimationEnd;
};

USTRUCT(BlueprintType)
struct FSpecialMoveStruct
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<int32> Directions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 StaminaCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool CanBeDoneInGround;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool CanBeDoneInAir;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FSpecialMoveCompleteAnimationStruct> SpecialMoveAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UPaperFlipbook* NoStaminaAnimation;
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

	// Basic Character Data
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

	// Attacking Data
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bIsDamaged = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bIsInHitAttackFrames = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool CurrentAttackHasHitObjective = false;
	UPROPERTY()
		bool bCurrentHitCollisionIsDone = false;
	UPROPERTY()
		int8 nAttackNumber = 0;
	UPROPERTY()
		int8 nCurrentComboHit = 0;

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

	// Animation Times
	UPROPERTY()
		float AnimationAttackCompleteTimeStop;
	UPROPERTY()
		float AnimationFlipbookTimeStop;
	UPROPERTY()
		float AnimationOtherTimeStop;
	UPROPERTY()
		FVector CurrentTraceHit;

	// Special Moves
	UPROPERTY()
		float AnimationSpecialTimeStart;
	UPROPERTY()
		float AnimationSpecialTimeStop;
	UPROPERTY()
		float AnimationSpecialMoveCompleteTimeStop;
	UPROPERTY()
		bool bIsExecutingSpecialMove;
	UPROPERTY()
		int8 nCurrentSpecialMove = -1;
	UPROPERTY()
		int8 nCurrentSpecialMoveHitAnimation = 0;

	// HUD
	UPROPERTY()
		AGameHUD* GameHUD;

	UPROPERTY()
		TEnumAsByte<AnimationState> EAnimationState = AnimationState::Idle;

	// Inherited Components
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UArrowComponent* CharacterArrowComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UPaperFlipbookComponent* CurrentFlipbook;

	// Flip books
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		UPaperFlipbook* IdleAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		UPaperFlipbook* WalkingAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		UPaperFlipbook* JumpingAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		UPaperFlipbook* JumpingForwardAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		UPaperFlipbook* DuckingAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		UPaperFlipbook* HitTopAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations Attacking")
		TArray<FComboAttackStruct> AttackingComboAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations Attacking")
		TArray<FComboAttackStruct> AttackingJumpingAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations Attacking")
		TArray<FComboAttackStruct> AttackingCrouchingAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations Attacking")
		TArray<AActor*> ActorsToIgnore;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations Special Moves")
		TArray<FSpecialMoveStruct> SpecialMoves;


	// Animation Flags
	UPROPERTY()
		TArray<FComboAnimationFlagsStruct> ComboAnimationFlags;
	UPROPERTY()
		TArray<FSpecialMoveAnimationsFlagsStruct> SpecialMovesAnimationsFlags;
	UPROPERTY()
		int32 nCurrentSpecialMoveAnimation = 0;


	UFUNCTION()
		void ControlCharacterRotation();

	UFUNCTION()
		void SetDamagedState(float DeltaTime);

	UFUNCTION()
		virtual void BindDataHUD();

	UFUNCTION()
		virtual void HandleAttack();

	UFUNCTION()
		virtual void HandleSpecialMoves();

	UFUNCTION()
		virtual void ResetAttack();

	// Other stuff
	UFUNCTION()
		virtual void UpdateAnimations();

	UFUNCTION()
		virtual void ControlCharacterAnimations(float characterMovementSpeed);

	UFUNCTION()
		virtual void SetAttackAnimationFlags();

	UFUNCTION()
		virtual void ResetAttackAnimationFlags();

	UFUNCTION()
		virtual void SetSpecialMoveAnimationFlags();

	UFUNCTION()
		virtual void ResetSpecialMoveAnimationFlags();


	UFUNCTION()
		virtual float GetCurrentTime();

	UFUNCTION(BlueprintCallable)
		virtual void SetDamage(float Value);

	UFUNCTION(BlueprintCallable)
		virtual void HealLife(float Value);

	UFUNCTION(BlueprintCallable)
		virtual void HealStamina(float Value);

	UFUNCTION(BlueprintCallable)
		virtual void DrainLife();

	UFUNCTION(BlueprintCallable)
		virtual void DrainStamina();

	UFUNCTION()
		virtual void ApplyHitCollide(TArray<FComboAttackStruct> Combo);

	UFUNCTION()
		void DoCombo(TArray<FComboAttackStruct> Combo);

	UFUNCTION()
		virtual void DoSpecialMove(FSpecialMoveStruct SpecialMove);


	UFUNCTION()
		virtual void NotifyComboToHUD();
};
