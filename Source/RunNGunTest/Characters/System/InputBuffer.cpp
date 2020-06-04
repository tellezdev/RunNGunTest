// Fill out your copyright notice in the Description page of Project Settings.


#include "InputBuffer.h"

AInputBuffer::AInputBuffer()
{
}

bool AInputBuffer::IsMatchingDirections(TArray<int32> Directions)
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
			if (BufferedInput[BufferPosition] == Directions[j])
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

AInputBuffer::~AInputBuffer()
{
}
