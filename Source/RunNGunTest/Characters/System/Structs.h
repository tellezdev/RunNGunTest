// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PaperFlipbookComponent.h"
#include "PaperFlipbook.h"
#include "Structs.generated.h"

UENUM(BlueprintType)
enum class EActionState : uint8
{
	ActionIdle,
	ActionAttacking,
	ActionSpecialMove,
	ActionChargingup,
	ActionDucking,
	ActionDamaged,
	ActionGettingUp
};
ENUM_CLASS_FLAGS(EActionState);

UENUM(BlueprintType)
enum class EAnimationState : uint8
{
	AnimIdle,
	AnimJumping,
	AnimJumpingForward,
	AnimWalking,
	AnimDucking,
	AnimAttacking,
	AnimSpecialMove,
	AnimChargingUp,
	AnimDamage,
	AnimDamageAir,
	AnimGettingUp
};
ENUM_CLASS_FLAGS(EAnimationState);

UENUM(BlueprintType)
enum class ECurrentAnimationState : uint8
{
	CurrentAnimationStart,
	CurrentAnimationCharging,
	CurrentAnimationHit,
	CurrentAnimationEnd
};
ENUM_CLASS_FLAGS(ECurrentAnimationState);

// Action Animations Structs
USTRUCT()
struct FActionAnimationFlagsStruct
{
	GENERATED_BODY()

public:
	UPROPERTY()
		bool bIsActionStart = false;
	UPROPERTY()
		bool bIsActionCharge = false;
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
struct FAnimationStruct
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UPaperFlipbook* Animation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UPaperFlipbook* AnimationDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UPaperFlipbook* AnimationHit;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UPaperFlipbook* AnimationWorldSpace;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UPaperFlipbook* AnimationEffect1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector Effect1Position = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UPaperFlipbook* AnimationEffect2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector Effect2Position = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UPaperFlipbook* AnimationEffect3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector Effect3Position = FVector::ZeroVector;
};

USTRUCT(BlueprintType)
struct FActionAnimationStruct
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FAnimationStruct Animation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float DamageValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector HitBoxPosition = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector HitBoxSize = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector ImpulseToOwner = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector ImpulseToReceiver = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float InterpolationSpeed = 5.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool IsProjectile;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UObject* GenericProjectile;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector ProjectileStartPosition = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool CanMove;

};

USTRUCT(BlueprintType)
struct FActionCompleteAnimationStruct
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FActionAnimationStruct AnimationStart;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FActionAnimationStruct AnimationCharge;
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
		bool CanBeCharged;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TEnumAsByte<EMovementMode> HitMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FActionCompleteAnimationStruct> ActionAnimation;
};

/**
 *
 */
UCLASS()
class RUNNGUNTEST_API UStructs : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

};
