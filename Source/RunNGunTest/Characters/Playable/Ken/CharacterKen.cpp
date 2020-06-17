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
	GetCapsuleComponent()->SetCapsuleHalfHeight(27.131327);
	GetCapsuleComponent()->SetCapsuleRadius(18.100616);
	CurrentFlipbook->SetWorldLocation(FVector(0, 0, -31));
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

	if (bIsExecutingAction && ActionFinalLocation != FVector(0.f, 0.f, 0.f))
	{
		//FVector ToVinterp = FMath::VInterpConstantTo(GetActorLocation(), ActionFinalLocation, DeltaTime, Actions[nCurrentAction].InterpolationSpeed * 100.f);
		//SetActorLocation(ToVinterp);
	}

}

// Called to bind functionality to input
void ACharacterKen::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ACharacterKen::HandleSpecialMoves()
{
	Super::HandleSpecialMoves();
}

void ACharacterKen::SpecialHadouken()
{
	//DoActionAnimation(Actions[nCurrentAction]);
}

void ACharacterKen::SpecialShoryuken()
{
	//ActionFinalLocation = GetActorLocation() + Actions[nCurrentAction].ImpulseToCharacter;
	//GetCapsuleComponent()->SetSimulatePhysics(true);
}

void ACharacterKen::SpecialTatsumaki()
{
	//ActionFinalLocation = GetActorLocation() + Actions[nCurrentAction].ImpulseToCharacter;
	//GetCapsuleComponent()->SetSimulatePhysics(true);
}
