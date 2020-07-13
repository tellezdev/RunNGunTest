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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Data")
		float MaxComboTime = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Data")
		float DelayTimeUntilChargingUp = 0.5f;

	// Character Movement
	UPROPERTY()
		bool bIsDirectionPressed;

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

	void HandleSpecialMoves();

	void HandleProjectile(UObject* Projectile);

	UFUNCTION()
		void HandleDead();

	// HUD related
	void NotifyComboToHUD();

	UFUNCTION()
		void ShowBufferOnScreen();

	// Stamina related
	UFUNCTION()
		void HandleStaminaCharge();

	UFUNCTION()
		void StopHandleStaminaCharge();

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
	void HealStamina(float Value);
	void DrainLife();
	void ConsumeStamina(float Value);
	void DrainStamina();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
