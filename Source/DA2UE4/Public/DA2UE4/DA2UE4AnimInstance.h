// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Animation/AnimInstance.h"
#include "DA2UE4AnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class DA2UE4_API UDA2UE4AnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	FAnimNode_AssetPlayerBase* GetAssetFromState();
	
	
};
