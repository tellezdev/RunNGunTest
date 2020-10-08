// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "System/Structs.h"
#include "GameFramework/Character.h"
#include "PaperSprite.h"
#include "PaperCharacter.h"
#include "PaperSpriteComponent.h"
#include "../System/CollisionSystem.h"
#include "../HUD/GameHUD.h"
#include "CharacterCommon.generated.h"

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

	UPROPERTY()
		UCollisionSystem* CollisionSystem;

	// Basic Character Data
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Data")
		uint8 UUID;
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
	UPROPERTY()
		bool CurrentAttackHasHitObjective = false;
	UPROPERTY()
		int8 nAttackNumber = 0;
	UPROPERTY()
		int8 nCurrentComboHit = 0;
	UPROPERTY()
		bool bIsAttackFinished = true;
	UPROPERTY()
		float nLastActionTime = 0.f;
	UPROPERTY()
		float nCurrentHitImpulseX = 0.f;
	UPROPERTY()
		float nCurrentHitImpulseZ = 0.f;
	UPROPERTY()
		int8 nCurrentAction = 0;
	UPROPERTY()
		int8 nCurrentActionHitAnimation = 0;
	UPROPERTY()
		int32 ComboCount = 0;
	UPROPERTY()
		float nTotalDamage = 0.f;

private:
	// Character Movement
	UPROPERTY()
		bool bIsMovingRight;
	UPROPERTY()
		bool bCanMove;
	UPROPERTY()
		bool bIsLeft;
	UPROPERTY()
		bool bIsRight;
	UPROPERTY()
		bool bIsUp;
	UPROPERTY()
		bool bIsDown;
	UPROPERTY()
		bool bIsChargingup = false;
	UPROPERTY()
		bool bActionAnimationIsFinished;
	UPROPERTY()
		bool bIsFirstAttack = true;
	UPROPERTY()
		bool bIsDoingCombo = false;
	UPROPERTY()
		bool bIsEnoughStamina = false;
	UPROPERTY()
		bool bIsExecutingSpecialMove;
	UPROPERTY()
		bool bIsCrouching;

	// Auto actions
	UPROPERTY()
		bool bIsAutoActionAttack = false;
	UPROPERTY()
		bool bIsAutoActionCrouch = false;
	UPROPERTY()
		bool bIsAutoActionWalkForward = false;


public:
	UPROPERTY()
		FVector ActionFinalLocation;
	// Animation System
	UPROPERTY()
		int32 AnimationActionCurrentFrame;
	UPROPERTY()
		int32 AnimationActionLastFrame;
	UPROPERTY()
		int32 AnimationActionCompleteFramesNumber;
	UPROPERTY()
		bool bAnimationActionCompleteHasEnded;
	UPROPERTY()
		TArray<FActionAnimationsFlagsStruct> ActionsAnimationsFlags;
	UPROPERTY()
		int32 nCurrentActionAnimation = 0;
	UPROPERTY()
		FActionAnimationStruct CurrentAction;

	// Collision System
	UPROPERTY()
		FVector CurrentTraceHit = FVector::ZeroVector;


	// HUD
	UPROPERTY()
		AGameHUD* GameHUD;

	// States
	UPROPERTY()
		EAnimationState AnimationState = EAnimationState::AnimIdle;
	UPROPERTY()
		ECurrentAnimationState CurrentAnimationState = ECurrentAnimationState::CurrentAnimationStart;
	UPROPERTY()
		EMovementState MovementState = EMovementState::MovementIdle;
	UPROPERTY()
		EActionState ActionState = EActionState::ActionIdle;

	// Inherited Components
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UArrowComponent* CharacterArrowComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UPaperFlipbookComponent* CurrentFlipbook;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UPaperSpriteComponent* DamageSprite;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UPaperSpriteComponent* HitSprite;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UPaperSpriteComponent* WorldSpaceSprite;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UPaperFlipbookComponent* Effect1Flipbook;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UPaperFlipbookComponent* Effect2Flipbook;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UPaperFlipbookComponent* Effect3Flipbook;

	// Flip books
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Animations")
		FAnimationStruct IdleAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Animations")
		FAnimationStruct WalkingAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Animations")
		FAnimationStruct JumpingAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Animations")
		FAnimationStruct JumpingForwardAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Animations")
		FAnimationStruct DuckingAnimation;

	// Animations Objects
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation System")
		TArray<AActor*> ActorsToIgnore;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation System")
		TArray<FActionStruct> GroundCombo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation System")
		TArray<FActionStruct> AirCombo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation System")
		TArray<FActionStruct> SpecialMoves;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation System")
		TArray<FActionStruct> ChargingStaminaAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation System")
		TArray<FActionStruct> DamageAnimations;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation System")
		TArray<FActionStruct> CrouchedAttack;

	UPROPERTY()
		TArray<FActionStruct> Actions;


	// Getters / Setters
	UFUNCTION()
		virtual void SetCanMove(bool State);
	UFUNCTION()
		virtual bool CanMove();

	UFUNCTION()
		virtual void SetLastActionTime(float Time);
	UFUNCTION()
		virtual float GetLastActionTime();

	UFUNCTION()
		virtual void SetAnimationState(EAnimationState State);

	UFUNCTION()
		virtual float GetCurrentTime();

	UFUNCTION()
		virtual void SetAnimationBehaviour(FActionAnimationStruct AnimationStruct);

	UFUNCTION()
		virtual void SetActionAnimationIsFinished(bool IsFinished);
	UFUNCTION()
		virtual bool IsActionAnimationFinished();

	UFUNCTION()
		virtual void SetMovementState(EMovementState State);
	UFUNCTION()
		virtual void SetActionState(EActionState State);
	UFUNCTION()
		virtual FString GetActionState();

	UFUNCTION()
		virtual bool IsFacingRight();
	UFUNCTION()
		virtual void SetFacingDirectionRight(bool IsRight);

	UFUNCTION()
		virtual bool IsLeft();
	UFUNCTION()
		virtual void SetLeftPressed(bool State);
	UFUNCTION()
		virtual bool IsRight();
	UFUNCTION()
		virtual void SetRightPressed(bool State);
	UFUNCTION()
		virtual bool IsUp();
	UFUNCTION()
		virtual void SetUpPressed(bool State);
	UFUNCTION()
		virtual bool IsDown();
	UFUNCTION()
		virtual void SetDownPressed(bool State);
	UFUNCTION()
		virtual bool IsChargingup();
	UFUNCTION()
		virtual void SetChargingup(bool State);
	UFUNCTION()
		virtual bool IsFirstAttack();

	UFUNCTION()
		virtual bool IsDoingCombo();
	UFUNCTION()
		virtual void SetIsCombo(bool State);

	UFUNCTION()
		virtual bool HasStamina();
	UFUNCTION()
		virtual void SetHasStamina(bool State);

	UFUNCTION()
		virtual bool IsExecutingSpecialMove();
	UFUNCTION()
		virtual void SetIsExecutingSpecialMove(bool State);

	UFUNCTION()
		virtual void AddToTotalDamage(float value);
	UFUNCTION()
		virtual void SetTotalDamage(float value);
	UFUNCTION()
		virtual float GetTotalDamage();

	UFUNCTION()
		virtual bool IsCrouched();
	UFUNCTION()
		virtual void SetIsCrouched(bool State);

	UFUNCTION()
		virtual void SetAutoAttack(bool Value);
	UFUNCTION()
		virtual void SetAutoWalkForward(bool Value);


	// Handling
	UFUNCTION()
		virtual void DoAttack();

	UFUNCTION()
		virtual void HandleAttack();

	UFUNCTION()
		virtual void HandleSpecialMoves();

	UFUNCTION()
		virtual void HandleProjectile(UObject* Projectile, FVector ProjectilePosition);

	UFUNCTION()
		virtual void PrepareProjectile(FActionAnimationStruct CurrentAnimation);

	UFUNCTION()
		virtual void HandleAutoActions();

	// Resetters
	UFUNCTION()
		virtual void ResetAttackCombo();

	UFUNCTION()
		virtual void ResetAttack();

	UFUNCTION()
		virtual void ResetSpecialMove();

	UFUNCTION()
		virtual void ResetChargingUp();

	UFUNCTION()
		virtual void ResetDamage();

	UFUNCTION()
		virtual void ResetTotalDamage();

	// Animation Events
	UFUNCTION()
		virtual void OnEffects1FlipbookFinishPlaying();
	UFUNCTION()
		virtual void OnEffects2FlipbookFinishPlaying();
	UFUNCTION()
		virtual void OnEffects3FlipbookFinishPlaying();

	// Animations Methods
	UFUNCTION()
		virtual void UpdateAnimations();

	UFUNCTION()
		virtual void ControlCharacterAnimations(float characterMovementSpeed);

	UFUNCTION()
		virtual void SetActionAnimationFlags();

	UFUNCTION()
		virtual void ResetActionAnimationFlags();

	UFUNCTION()
		virtual void PrepareAnimation();

	UFUNCTION()
		virtual void ControlAnimation();

	UFUNCTION()
		virtual void ApplyCurrentAnimation(FActionStruct Action, TArray<FActionAnimationFlagsStruct>& AnimationsFlags);

	UFUNCTION()
		virtual void ApplyEffectsAnimation(FActionAnimationStruct Action);

	UFUNCTION()
		virtual void SetFlipbooks(FAnimationStruct AnimationStruct);

	// Character System
	UFUNCTION()
		virtual void ConsumeStamina(float Value);

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

	// Utils
	UFUNCTION()
		void ControlCharacterRotation();

	UFUNCTION()
		virtual void BindDataHUD();

	UFUNCTION()
		virtual void NotifyComboToHUD();

	UFUNCTION()
		virtual float GetFacingX(float Impulse);

	UFUNCTION()
		virtual float GetFacingWhenHit(float Impulse, FVector OwnerPosition, FVector ReceiverPosition);

	UFUNCTION()
		virtual FVector FaceElement(FVector Vector);

	UFUNCTION()
		virtual TArray<AActor*> GetActorsWithOtherTag(FName Tag);

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void ControlChargingDamage(float limitDamage);
};
