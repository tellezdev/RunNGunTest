// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine.h"
#include "GameFramework/Character.h"
#include "PaperCharacter.h"
#include "PaperSpriteComponent.h"
#include "PaperFlipbookComponent.h"
#include "CharacterBase.generated.h"

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

UENUM(BlueprintType)
enum KeyInput
{
	NoMoving = 0,
	DownLeft = 1,
	Down = 2,
	DownRight = 3,
	Left = 4,
	Right = 6,
	UpLeft = 7,
	Up = 8,
	UpRight = 9,
	Attack = 10,
	Special = 11
};


USTRUCT(BlueprintType)
struct FSpecialMoveStruct
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<int32> Directions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 StaminaCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool IsProjectile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool CanBeDoneInGround;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool CanBeDoneInAir;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UPaperFlipbook* SpecialMoveAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UPaperFlipbook* NoStaminaAnimation;
};

UCLASS()
class RUNNGUNTEST_API ACharacterBase : public ACharacter
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FTimerHandle AttackDelayTimerHandle;


public:
	// Sets default values for this character's properties
	ACharacterBase();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Data")
		FString CharacterName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Data")
		UPaperSprite* CharacterPortrait;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Data")
		float PlayerLife = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Data")
		float PlayerStamina = 10.f;

	UPROPERTY()
		float MaxStamina = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character State")
		bool bIsMovingRight;
	UPROPERTY()
		bool bCanMove = true;
	UPROPERTY()
		bool bIsDucking;
	UPROPERTY()
		bool bIsAttacking;
	UPROPERTY()
		bool bIsSpecialMove;
	UPROPERTY()
		bool bIsChargingup;
	UPROPERTY()
		bool bIsExecutingSpecialMove;
	UPROPERTY()
		int8 nCurrentSpecialMove = 0;
	UPROPERTY()
		bool bIsDirectionPressed;
	UPROPERTY()
		int8 nAttackNumber = 0;
	UPROPERTY()
		float AnimationAttackTimeStart;
	UPROPERTY()
		float AnimationAttackTimeStop;
	UPROPERTY()
		float AnimationSpecialTimeStart;
	UPROPERTY()
		float AnimationSpecialTimeStop;
	UPROPERTY()
		float AttackKeyPressedTimeStart;
	UPROPERTY()
		float AttackKeyPressedTimeStop;
	UPROPERTY()
		float SpecialKeyPressedTimeStart;
	UPROPERTY()
		float SpecialKeyPressedTimeStop;
	UPROPERTY()
		FTimerHandle TimerLagProjectile;

	UPROPERTY()
		FTimerHandle GlobalTimerHandle;

	UPROPERTY()
		TArray<int32> BufferedInput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character State")
		TEnumAsByte<AnimationState> ECharacterAnimationState = AnimationState::Idle;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character State")
		UArrowComponent* CharacterArrowComponent;
	// Flip books
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Animations")
		UPaperFlipbookComponent* CharacterAnimationComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Animations")
		UPaperFlipbook* IdleAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Animations")
		UPaperFlipbook* WalkingAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Animations")
		UPaperFlipbook* JumpingAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Animations")
		UPaperFlipbook* JumpingForwardAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Animations")
		UPaperFlipbook* JumpingAttack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Animations")
		UPaperFlipbook* DuckingAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Animations")
		UPaperFlipbook* ChargingUpAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Animations")
		TArray<UPaperFlipbook*> AttackingComboAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Animations")
		TArray<FSpecialMoveStruct> SpecialMoves;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	// Movement related
	UFUNCTION()
		void MoveRight(float Value);

	UFUNCTION()
		void DownDirectionStart();

	UFUNCTION()
		void DownDirectionStop();

	UFUNCTION()
		void UpDirection();

	// Actions related
	UFUNCTION()
		void JumpStart();

	UFUNCTION()
		void JumpStop();

	UFUNCTION()
		void AttackStart();

	UFUNCTION()
		void AttackStop();

	UFUNCTION()
		void SpecialStart();

	UFUNCTION()
		void SpecialStop();

	UFUNCTION()
		void HandleAttack();

	UFUNCTION()
		void HandleSpecialMoves();

	UFUNCTION()
		void HandleProjectile();

	UFUNCTION()
		void HandleStaminaCharge();

	UFUNCTION()
		void StopHandleStaminaCharge();

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
		void HitPlayer(float Value);

	UFUNCTION()
		void ConsumeStamina(float Value);

	// Buffer input related
	UFUNCTION()
		void InsertInputBuffer(KeyInput key);

	UFUNCTION()
		void ReadInputBuffer();

	UFUNCTION(BlueprintCallable)
		TArray<int32> GetBufferedInput();

	UFUNCTION()
		void ClearBuffer();

	UFUNCTION()
		void ControlStamina();
};
