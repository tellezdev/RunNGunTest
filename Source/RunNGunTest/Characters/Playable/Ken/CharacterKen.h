// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../CharacterPlayer.h"
#include "GameFramework/Character.h"
#include "CharacterKen.generated.h"

UCLASS()
class RUNNGUNTEST_API ACharacterKen : public ACharacterPlayer
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACharacterKen();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
