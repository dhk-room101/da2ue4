// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Components/TextWidgetTypes.h"
#include "Components/TextBlock.h"
#include "FloatyText.generated.h"

/**
*
*/
UCLASS()
class DA2UE4_API UFloatyText : public UTextBlock
{
	GENERATED_BODY()

public:

	//text is get/set from parent class via SetText/GetText

	AActor* oCreature = nullptr;

	int32 nStyle = 0;

	int32 nColor = 0xFFFFFF;//white

	float fDuration = 1.f;

	float fTimestamp;

	FVector vPosition = FVector(0.f);
};
