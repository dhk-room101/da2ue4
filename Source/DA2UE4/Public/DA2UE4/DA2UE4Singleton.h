// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DA2UE4Singleton.generated.h"

class ADA2UE4Module;

UCLASS(Blueprintable, BlueprintType)
class UDA2UE4Singleton : public UObject
{
	GENERATED_BODY()
public:
	UDA2UE4Singleton(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Singleton)
	UWorld* DA2UE4CurrentWorld;
	ADA2UE4Module* DA2UE4CurrentModule;
};
