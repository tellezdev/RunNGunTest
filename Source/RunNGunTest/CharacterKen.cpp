// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterKen.h"

// Sets default values
ACharacterKen::ACharacterKen() 
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Setting default character movement's params
	GetCharacterMovement()->MaxWalkSpeed = 400;
	GetCharacterMovement()->MaxWalkSpeedCrouched = 50;
	GetCharacterMovement()->JumpZVelocity = 500;
	GetCharacterMovement()->AirControl = 0.8;

}

// Called when the game starts or when spawned
void ACharacterKen::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACharacterKen::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACharacterKen::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

