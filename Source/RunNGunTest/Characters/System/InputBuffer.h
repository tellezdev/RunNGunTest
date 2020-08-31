// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../HUD/GameHUD.h"

/**
 *
 */
class RUNNGUNTEST_API AInputBuffer
{
public:
	AInputBuffer();
	~AInputBuffer();

	TArray<int32> BufferedInput;

	int32 LastDirectionPressed;

	void InsertInputBuffer(int32 key, bool bIsDirectionPressed);

	bool IsMatchingDirections(TArray<int32> Directions, bool IsFacingRight);

	TArray<int32> GetBufferedInput();

	void ClearBuffer();

	int32 CheckDirections(int32 Directions, bool IsFacingRight);
};
