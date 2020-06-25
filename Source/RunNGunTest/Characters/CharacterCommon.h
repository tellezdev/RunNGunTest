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

// Action Animations Structs
USTRUCT()
struct FActionAnimationFlagsStruct
{
	GENERATED_BODY()

public:
	UPROPERTY()
		bool bIsActionStart = false;
	UPROPERTY()
		TArray<bool> bIsActionHits;
	UPROPERTY()
		bool bIsActionEnd = false;
	UPROPERTY()
		bool bIsCompleted = false;
};

USTRUCT()
struct FActionAnimationsFlagsStruct
{
	GENERATED_BODY()

public:
	UPROPERTY()
		TArray<FActionAnimationFlagsStruct> Animations;
};

USTRUCT(BlueprintType)
struct FActionAnimationStruct
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
		FVector HitBoxSize = FVector(0.f, 0.f, 0.f);
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
struct FActionCompleteAnimationStruct
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FActionAnimationStruct AnimationStart;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FActionAnimationStruct> AnimationHits;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FActionAnimationStruct AnimationEnd;
};

USTRUCT(BlueprintType)
struct FActionStruct
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
		float WindowFrameTimeToContinue = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FActionCompleteAnimationStruct> ActionAnimation;

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
	UPROPERTY()
		bool bIsAttackFinished = true;
	UPROPERTY()
		float nLastActionTime = 0.f;


	// Character Movement
	UPROPERTY()
		bool bIsMovingRight;
	UPROPERTY()
		bool bCanMove;
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
		bool bIsSpecialMove;
	UPROPERTY()
		bool bIsChargingup;
	UPROPERTY()
		bool bIsFirstAttack = true;
	UPROPERTY()
		FVector ActionFinalLocation;

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
		float AnimationActionTimeStart;
	UPROPERTY()
		float AnimationActionTimeStop;
	UPROPERTY()
		float AnimationActionCompleteTimeStop;
	UPROPERTY()
		bool bIsExecutingSpecialMove;
	UPROPERTY()
		int8 nCurrentAction = 0;
	UPROPERTY()
		int8 nCurrentActionHitAnimation = 0;

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
		TArray<AActor*> ActorsToIgnore;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations Moves")
		TArray<FActionStruct> AttackMoves;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations Moves")
		TArray<FActionStruct> JumpAttackMoves;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations Moves")
		TArray<FActionStruct> SpecialMoves;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations Moves")
		TArray<FActionStruct> NoStaminaMoves;

	UPROPERTY()
		TArray<FActionStruct> Actions;


	// Animation Flags
	UPROPERTY()
		TArray<FActionAnimationsFlagsStruct> ActionsAnimationsFlags;
	UPROPERTY()
		int32 nCurrentActionAnimation = 0;
	UPROPERTY()
		float nCurrentAnimationInterpolationSpeed = 0.f;

	UFUNCTION()
		virtual void SetCanMove(bool State);
	UFUNCTION()
		bool CanMove();


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
		virtual void ResetAttackCombo();

	UFUNCTION()
		virtual void ResetAttack();

	UFUNCTION()
		virtual void ResetSpecialMove();

	UFUNCTION()
		virtual FVector GetFacingVector(FVector OriginalVector);

	// Other stuff
	UFUNCTION()
		virtual void UpdateAnimations();

	UFUNCTION()
		virtual void ControlCharacterAnimations(float characterMovementSpeed);

	UFUNCTION()
		virtual void SetActionAnimationFlags();

	UFUNCTION()
		virtual void ResetActionAnimationFlags();

	UFUNCTION()
		virtual void SetAnimationState(AnimationState State);

	UFUNCTION()
		virtual void ConsumeStamina(float Value);

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
		virtual void ApplyHitCollide(FActionAnimationStruct CurrentAction);

	UFUNCTION()
		virtual void DoActionAnimation();

	UFUNCTION()
		virtual void SetAnimationBehaviour(FActionAnimationStruct AnimationStruct, bool bIsLoop);

	UFUNCTION()
		virtual void NotifyComboToHUD();
};
