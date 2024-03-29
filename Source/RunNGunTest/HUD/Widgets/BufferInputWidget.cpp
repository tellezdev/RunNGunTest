// Fill out your copyright notice in the Description page of Project Settings.


#include "BufferInputWidget.h"
#include "Blueprint/UserWidget.h"

UBufferInputWidget::UBufferInputWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UBufferInputWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UBufferInputWidget::DrawBuffer(TArray<int32> BufferedInput)
{
	ArrowsHorizontalBox->ClearChildren();
	for (int32 Direction : BufferedInput)
	{
		class UImage* ArrowImage = NewObject<UImage>(UImage::StaticClass());

		ArrowImage->SetBrushFromTexture(ArrowsTextures[Direction]);
		ArrowsHorizontalBox->AddChildToHorizontalBox(ArrowImage);
	}
}