// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine.h"
#include "CharacterCommon.h"
#include "GameFramework/Character.h"
#include "PaperCharacter.h"
#include "PaperSpriteComponent.h"
#include "PaperFlipbookComponent.h"
#include "System/InputBuffer.h"
#include "CharacterBase.generated.h"


UENUM(BlueprintType)
enum KeyInput
{
	DownLeft = 1,
	Down = 2,
	DownRight = 3,
	Left = 4,
	Right = 6,
	UpLeft = 7,
	Up = 8,
	UpRight = 9,
	Attack = 0,
	Special = 5
};

//USTRUCT(BlueprintType)
//struct FSpecialMoveStruct
//{
//	GENERATED_BODY()
//
//public:
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FString Name;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		TArray<int32> Directions;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		int32 StaminaCost;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		bool IsProjectile;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		bool CanBeDoneInGround;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		bool CanBeDoneInAir;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		float DamageValue;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FVector ImpulseToCharacter = FVector(0.f, 0.f, 0.f);
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		float InterpolationSpeed = 5.f;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		UPaperFlipbook* SpecialMoveAnimation;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		UPaperFlipbook* NoStaminaAnimation;
//};

UCLASS()
class RUNNGUNTEST_API ACharacterBase : public ACharacterCommon
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FTimerHandle AttackDelayTimerHandle;

public:
	// Sets default values for this character's properties
	ACharacterBase();

	AInputBuffer InputBuffer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Data")
		bool bShowBuffer = true;

	// Character Movement
	UPROPERTY()
		bool bIsSpecialMove;
	UPROPERTY()
		bool bIsDirectionPressed;
	UPROPERTY()
		int32 ComboCount = 0;
	UPROPERTY()
		FVector SpecialMoveFinalLocation;


	// Key Pressing Times
	UPROPERTY()
		float AttackKeyPressedTimeStart;
	UPROPERTY()
		float AttackKeyPressedTimeStop;
	UPROPERTY()
		float SpecialKeyPressedTimeStart;
	UPROPERTY()
		float SpecialKeyPressedTimeStop;

	// Timers
	UPROPERTY()
		FTimerHandle TimerLagProjectile;

	UPROPERTY()
		FTimerHandle GlobalTimerHandle;


	// Flip books
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		UPaperFlipbook* ChargingUpAnimation;

	// Movement related
	UFUNCTION()
		void LeftDirectionStart();

	UFUNCTION()
		void LeftDirectionStop();

	UFUNCTION()
		void RightDirectionStart();

	UFUNCTION()
		void RightDirectionStop();

	UFUNCTION()
		void DownDirectionStart();

	UFUNCTION()
		void DownDirectionStop();

	UFUNCTION()
		void UpDirectionStart();

	UFUNCTION()
		void UpDirectionStop();

	UFUNCTION()
		void HandleDirections();

	UFUNCTION()
		void MoveCharacter(float MovementSpeed, bool IsFacingRight);

	// Actions related
	UFUNCTION()
		void JumpStart();

	UFUNCTION()
		void JumpStop();

	// Attacking related
	UFUNCTION()
		void AttackStart();

	UFUNCTION()
		void AttackStop();

	UFUNCTION()
		void SpecialStart();

	UFUNCTION()
		void SpecialStop();

	void HandleAttack();

	void ResetAttack();

	void SetAttackAnimationFlags();

	void ResetAttackAnimationFlags();

	void ApplyHitCollide(TArray<FComboAttackStruct> Combo);

	void NotifyComboToHUD();

	UFUNCTION()
		void ShowBufferOnScreen();

	void HandleSpecialMoves();

	void DoSpecialMove(FSpecialMoveStruct SpecialMove);

	void SetSpecialMoveAnimationFlags();

	void ResetSpecialMoveAnimationFlags();

	UFUNCTION()
		virtual void HandleProjectile(UObject* Projectile);

	UFUNCTION()
		void HandleStaminaCharge();

	UFUNCTION()
		void StopHandleStaminaCharge();

	UFUNCTION()
		void ConsumeStamina(float Value);

	UFUNCTION()
		void ControlStamina();

	// Buffer input related
	UFUNCTION()
		void HandleBuffer(KeyInput Direction);

	// Animations
	void UpdateAnimations();
	void ControlCharacterAnimations(float characterMovementSpeed);

	// -- Health related
	void SetDamage(float Value);
	void HealLife(float Value);
	void HealStamina(float Value);
	void DrainLife();
	void DrainStamina();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
