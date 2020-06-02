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

	if (bIsExecutingSpecialMove && SpecialMoveFinalLocation != FVector(0.f, 0.f, 0.f))
	{
		FVector ToVinterp = FMath::VInterpTo(GetActorLocation(), SpecialMoveFinalLocation, DeltaTime, SpecialMoves[nCurrentSpecialMove].InterpolationSpeed);
		SetActorLocation(ToVinterp);
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

	AnimationSpecialTimeStart = GetCurrentTime();
	if (!bIsExecutingSpecialMove)
	{
		bIsExecutingSpecialMove = true;
		// Checking if there are enough stamina
		int bIsEnoughStamina = SpecialMoves[nCurrentSpecialMove].StaminaCost <= Stamina;
		if (bIsEnoughStamina)
		{
			if ((SpecialMoves[nCurrentSpecialMove].CanBeDoneInGround && GetCharacterMovement()->IsMovingOnGround())
				|| (SpecialMoves[nCurrentSpecialMove].CanBeDoneInAir && !GetCharacterMovement()->IsMovingOnGround()))
			{
				ConsumeStamina(SpecialMoves[nCurrentSpecialMove].StaminaCost);
				CurrentFlipbook->SetFlipbook(SpecialMoves[nCurrentSpecialMove].SpecialMoveAnimation);
				// Do motion
				if (SpecialMoves[nCurrentSpecialMove].Name == "Hadouken")
				{
					SpecialHadouken();
				}
				else if (SpecialMoves[nCurrentSpecialMove].Name == "Shoryuken")
				{
					SpecialShoryuken();
				}
				else if (SpecialMoves[nCurrentSpecialMove].Name == "Tatsumaki")
				{
					SpecialTatsumaki();
				}
				else
				{

				}
				if (SpecialMoves[nCurrentSpecialMove].IsProjectile)
				{
					FTimerDelegate TimerDel;
					FTimerHandle TimerHandle;

					//Binding the function with specific variables
					TimerDel.BindUFunction(this, FName("HandleProjectile"));
					//Calling MyUsefulFunction after 5 seconds without looping
					GetWorldTimerManager().SetTimer(TimerHandle, TimerDel, 0.3f, false);
				}
			}
			else
			{
				ClearBuffer();
			}
		}
		else
		{
			CurrentFlipbook->SetFlipbook(SpecialMoves[nCurrentSpecialMove].NoStaminaAnimation);
		}
		AnimationSpecialTimeStop = AnimationSpecialTimeStart + CurrentFlipbook->GetFlipbookLength();
	}
	if (GetCurrentTime() > AnimationSpecialTimeStop)
	{
		bIsSpecialMove = false;
		bIsExecutingSpecialMove = false;
		bCanMove = true;
		SpecialMoveFinalLocation = FVector(0.f, 0.f, 0.f);
		GetCapsuleComponent()->SetSimulatePhysics(false);
	}
}

void ACharacterKen::SpecialHadouken()
{
}

void ACharacterKen::SpecialShoryuken()
{
	SpecialMoveFinalLocation = GetActorLocation() + SpecialMoves[nCurrentSpecialMove].ImpulseToCharacter;
	GetCapsuleComponent()->SetSimulatePhysics(true);
}

void ACharacterKen::SpecialTatsumaki()
{
	SpecialMoveFinalLocation = GetActorLocation() + SpecialMoves[nCurrentSpecialMove].ImpulseToCharacter;
	GetCapsuleComponent()->SetSimulatePhysics(true);
}
