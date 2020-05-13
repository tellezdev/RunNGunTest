// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
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
	ChargingUp
};

UCLASS()
class RUNNGUNTEST_API ACharacterCommon : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACharacterCommon();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitInformation")
		TArray<AActor*> ActorsToIgnore;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	UFUNCTION(BlueprintCallable)
		virtual void SetDamage(float Value);

	UFUNCTION(BlueprintCallable)
		virtual void HealLife(float Value);
};
