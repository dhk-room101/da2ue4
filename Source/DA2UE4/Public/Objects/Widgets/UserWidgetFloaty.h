// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "UserWidgetFloaty.generated.h"

/**
 * 
 */
UCLASS()
class DA2UE4_API UUserWidgetFloaty : public UUserWidget
{
	GENERATED_BODY()

public:

	//text is get/set from parent class via SetText/GetText

	AActor* oCreature = nullptr;

	int32 nStyle = 0;

	int32 nColor = 0xFFFFFF;//white

	float fDuration = 0.5f;

	float fTimestamp;

	FVector vPosition = FVector(ForceInitToZero);
	
	
	
};
