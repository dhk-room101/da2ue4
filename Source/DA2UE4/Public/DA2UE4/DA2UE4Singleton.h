// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DA2UE4Singleton.generated.h"

class UDA2UE4GameInstance;

UCLASS(Blueprintable, BlueprintType)
class UDA2UE4Singleton : public UObject
{
	GENERATED_BODY()

	UDA2UE4Singleton(const FObjectInitializer& ObjectInitializer);


public:

	UPROPERTY()
	UWorld* DA2UE4CurrentWorld = nullptr;
	
	UPROPERTY()
	FStreamableManager AssetLoader;
};
