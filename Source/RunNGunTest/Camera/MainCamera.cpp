// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCamera.h"
#include "Math/UnrealMathUtility.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

AMainCamera::AMainCamera()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	this->Tags.Add("Camera");

	Dummy = CreateDefaultSubobject<USphereComponent>(TEXT("Dummy"));
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(Dummy);
	CameraBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("CameraBounds"));
	CameraBounds->SetupAttachment(Dummy);
}

void AMainCamera::BeginPlay()
{
	Super::BeginPlay();

	GameHUD = Cast<AGameHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());

	Dummy->SetWorldRotation(FRotator(0.f, 270.f, 0.f));
	Dummy->SetWorldLocation(FVector(0.f, 450.f, 1050.f));

	CameraBounds->SetRelativeLocation(FVector(324.f, 100.f, 101.f));
	CameraBounds->SetWorldRotation(FRotator(0.f, 0.f, 0.f));
	CameraBounds->SetWorldScale3D(FVector(1.f, 1.f, 1.f));
	CameraBounds->SetBoxExtent(FVector(1921.265137f, 10.f, 213.259628f));

	CameraComponent->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
	CameraComponent->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
	CameraComponent->SetRelativeScale3D(FVector(1.f, 1.f, 1.f));
	CameraComponent->SetConstraintAspectRatio(true);
	CameraComponent->SetFieldOfView(90.f);
	CameraComponent->SetProjectionMode(ECameraProjectionMode::Orthographic);
	CameraComponent->SetOrthoWidth(941.530701f);

	// Debug only
	/*CameraBounds->SetHiddenInGame(false);
	CameraComponent->bCameraMeshHiddenInGame = 0;
	CameraComponent->SetHiddenInGame(false, true);*/

	Player = Cast<ACharacterCommon>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
}

void AMainCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (Player)
	{
		FVector PlayerLocation = Player->GetActorLocation();
		FVector CameraComponentLocation = CameraComponent->GetComponentLocation();
		FVector CameraBoundsLocation = CameraBounds->GetComponentLocation();

		FVector BoundsOrigin = CameraBounds->Bounds.Origin;
		FVector BoundsExtent = CameraBounds->Bounds.BoxExtent;
		FVector BoundsMax = FVector(CameraBounds->Bounds.Origin) + FVector(CameraBounds->Bounds.BoxExtent);
		FVector BoundsMin = FVector(CameraBounds->Bounds.Origin) - FVector(CameraBounds->Bounds.BoxExtent);

		float ClampX = FMath::Clamp(PlayerLocation.X, BoundsMin.X, BoundsMax.X);
		float ClampZ = FMath::Clamp(PlayerLocation.Z, BoundsMin.Z, BoundsMax.Z);


		FVector TargetToVInterp = FVector(ClampX, CameraComponentLocation.Y, ClampZ);
		FVector NewCameraLocation = FMath::VInterpTo(CameraComponentLocation, TargetToVInterp, DeltaTime, 5.f);

		/* Debug Stuff */
		/*FString TxtPlayerLocation = FString::Printf(TEXT("PlayerLocation: %s"), *PlayerLocation.ToString());
		FString TxtCameraComponentLocation = FString::Printf(TEXT("CameraComponentLocation: %s"), *CameraComponentLocation.ToString());
		FString TxtCameraBoundsLocation = FString::Printf(TEXT("CameraBoundsLocation: %s"), *CameraBoundsLocation.ToString());
		FString TxtBoundsOrigin = FString::Printf(TEXT("BoundsOrigin: %s"), *BoundsOrigin.ToString());
		FString TxtBoundsExtent = FString::Printf(TEXT("BoundsExtent: %s"), *BoundsExtent.ToString());
		FString TxtBoundsMax = FString::Printf(TEXT("BoundsMax: %s"), *BoundsMax.ToString());
		FString TxtBoundsMin = FString::Printf(TEXT("BoundsMin: %s"), *BoundsMin.ToString());
		FString TxtClampX = FString::Printf(TEXT("ClampX: %f"), ClampX);
		FString TxtClampZ = FString::Printf(TEXT("ClampZ: %f"), ClampZ);
		FString TxtTargetToVInterp = FString::Printf(TEXT("TargetToVInterp: %s"), *TargetToVInterp.ToString());
		FString TxtNewCameraLocation = FString::Printf(TEXT("NewCameraLocation: %s"), *NewCameraLocation.ToString());

		TArray<FString> DebugInfo;
		DebugInfo.Add(TxtPlayerLocation);
		DebugInfo.Add(TxtCameraComponentLocation);
		DebugInfo.Add(TxtCameraBoundsLocation);
		DebugInfo.Add(TxtBoundsOrigin);
		DebugInfo.Add(TxtBoundsExtent);
		DebugInfo.Add(TxtBoundsMax);
		DebugInfo.Add(TxtBoundsMin);
		DebugInfo.Add(TxtClampX);
		DebugInfo.Add(TxtClampZ);
		DebugInfo.Add(TxtTargetToVInterp);
		DebugInfo.Add(TxtNewCameraLocation);

		GameHUD->InsertDebugData(DebugInfo);*/
		/* Debug Stuff */

		CameraComponent->SetWorldLocation(NewCameraLocation);
	}
}

void AMainCamera::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->SetViewTargetWithBlend(Player);
}
