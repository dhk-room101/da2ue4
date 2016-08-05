// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/TargetPoint.h"
#include "DA2UE4TargetPoint.generated.h"

/**
 * 
 */
UCLASS()
class DA2UE4_API ADA2UE4TargetPoint : public ATargetPoint
{
	GENERATED_BODY()

public:
	ADA2UE4TargetPoint(const class FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Location)
	int32 nIndex;
};
