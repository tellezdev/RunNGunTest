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
		float EnemyStartAttackTime;
	UPROPERTY()
		float EnemyStopAttackTime;
	UPROPERTY()
		float AnimationAttackTimeStart;
	UPROPERTY()
		float AnimationAttackTimeStop;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Times")
		float TimeBetweenAttacks;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Collision Areas")
		USphereComponent* VisibilityArea;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Collision Areas")
		USphereComponent* HitArea;
	UPROPERTY()
		float HitBoxOrientation;


	// Movement related
	UFUNCTION(BlueprintCallable)
		void Attack();
	UFUNCTION()
		void SetAttackAnimation();

	void ResetAttack();

	void UpdateAnimations();

	void ControlCharacterAnimations(float characterMovementSpeed);

	float GetCurrentTime();

	UFUNCTION(BlueprintCallable)
		void HitPlayer();

	void SetDamage(float Value);
	void HealLife(float Value);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
