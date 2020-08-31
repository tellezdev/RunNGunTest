// Fill out your copyright notice in the Description page of Project Settings.


#include "InputBuffer.h"

AInputBuffer::AInputBuffer()
{
}

bool AInputBuffer::IsMatchingDirections(TArray<int32> Directions, bool IsFacingRight)
{
	bool isOK = false;
	bool DirectionsMatch = false;

	TArray<int> MatchingSpecialMoves;
	int8 BufferPosition;
	// Every special move
	if (Directions.Num() <= BufferedInput.Num())
	{
		BufferPosition = BufferedInput.Num() - 1;
		for (int j = Directions.Num() - 1; j >= 0; --j)
		{
			if (BufferedInput[BufferPosition] == CheckDirections(Directions[j], IsFacingRight))
			{
				DirectionsMatch = true;
			}
			else
			{
				DirectionsMatch = false;
				break;
			}
			--BufferPosition;
		}
	}
	return DirectionsMatch;
}

void AInputBuffer::InsertInputBuffer(int32 key, bool bIsDirectionPressed)
{
	if (!bIsDirectionPressed || LastDirectionPressed != key)
	{
		if (BufferedInput.Num() > 9)
		{
			BufferedInput.RemoveSingle(BufferedInput[0]);
		}
		BufferedInput.Push(key);
		LastDirectionPressed = key;
	}
}

TArray<int32> AInputBuffer::GetBufferedInput()
{
	return BufferedInput;
}

void AInputBuffer::ClearBuffer()
{
	BufferedInput.Empty();
}

int32 AInputBuffer::CheckDirections(int32 Direction, bool IsFacingRight)
{
	if (!IsFacingRight)
	{
		switch (Direction)
		{
		case 1:
			return 3;
		case 3:
			return 1;
		case 4:
			return 6;
		case 6:
			return 4;
		case 7:
			return 9;
		case 9:
			return 7;
		default:
			return Direction;
		}
	}
	return Direction;
}

AInputBuffer::~AInputBuffer()
{
}
