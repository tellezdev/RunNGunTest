// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine.h"
#include "../CharacterCommon.h"
#include "GameFramework/Character.h"
#include "PaperSprite.h"
#include "PaperFlipbookComponent.h"
#include "PaperFlipbook.h"
#include "EnemyBase.generated.h"


UCLASS()
class RUNNGUNTEST_API AEnemyBase : public ACharacterCommon
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Sets default values for this character's properties
	AEnemyBase();

	UPROPERTY()
		ACharacterCommon* Player;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Data")
		float AttackDamage;

	// Animation Times
	UPROPERTY()
		float AnimationAttackTimeStart;
	UPROPERTY()
		float AnimationAttackTimeStop;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Times")
		float TimeBetweenAttacks = 2.f;
	UPROPERTY()
		float LastAttackTime;

	bool bIsAttacking = false;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Collision Areas")
		USphereComponent* VisibilityArea;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Collision Areas")
		USphereComponent* HitArea;


	// Movement related
	UFUNCTION(BlueprintCallable)
		void AttackStart();

	UFUNCTION()
		void FacePlayer();

	void HandleAttack();

	void ApplyHitCollide(TArray<FComboAttackStruct> Combo);

	UFUNCTION()
		void SetAttackAnimation();

	void ResetAttack();

	void UpdateAnimations();

	void ControlCharacterAnimations(float characterMovementSpeed);

	float GetCurrentTime();

	void SetDamage(float Value);
	void HealLife(float Value);

	void SetAnimationFlags();

	void ResetAnimationFlags();


	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
		FVector GetPlayerPosition();
};
