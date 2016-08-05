// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "DA2UE4Singleton.h"
#include "DA2UE4Library.generated.h"

/**
 * 
 */
UCLASS()
class DA2UE4_API UDA2UE4Library : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UDA2UE4Library(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintPure, Category = Singleton)
	static UDA2UE4Singleton* GetDA2UE4Data(bool& IsValid);
};
