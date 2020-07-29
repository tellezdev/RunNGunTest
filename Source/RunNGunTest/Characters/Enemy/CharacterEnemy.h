// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine.h"
#include "../CharacterCommon.h"
#include "GameFramework/Character.h"
#include "PaperSprite.h"
#include "PaperFlipbookComponent.h"
#include "PaperFlipbook.h"
#include "CharacterEnemy.generated.h"


UCLASS()
class RUNNGUNTEST_API ACharacterEnemy : public ACharacterCommon
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Sets default values for this character's properties
	ACharacterEnemy();

	UPROPERTY()
		ACharacterCommon* Player;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Data")
		float AttackDamage;

	// Animation Times
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Times")
		float TimeBetweenAttacks = 2.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Collision Areas")
		USphereComponent* VisibilityArea;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Collision Areas")
		USphereComponent* HitArea;


	// Movement related
	void AttackStart();

	UFUNCTION()
		void FacePlayer();

	//void ApplyHitCollide(TArray<FComboAttackStruct> Combo);

	void UpdateAnimations();

	void ControlCharacterAnimations(float characterMovementSpeed);

	void SetDamage(float Value);


	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		FVector GetPlayerPosition();
};
