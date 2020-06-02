// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/SphereComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "../Characters/CharacterCommon.h"
#include "../HUD/GameHUD.h"
#include "MainCamera.generated.h"

/**
 *
 */
UCLASS()
class RUNNGUNTEST_API AMainCamera : public AActor
{
	GENERATED_BODY()

public:
	AMainCamera();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	// HUD
	UPROPERTY()
		AGameHUD* GameHUD;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Components")
		USphereComponent* Dummy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Components")
		UCameraComponent* CameraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Components")
		UBoxComponent* CameraBounds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Components")
		ACharacterCommon* Player = nullptr;

	UFUNCTION()
		void BeginOverlap(UPrimitiveComponent* OverlappedComponent,
						  AActor* OtherActor,
						  UPrimitiveComponent* OtherComp,
						  int32 OtherBodyIndex,
						  bool bFromSweep,
						  const FHitResult& SweepResult);

};
