// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "STypes.h"
#include "GameFramework/Actor.h"
#include "DA2UE4Party.generated.h"

UCLASS()
class DA2UE4_API ADA2UE4Party : public AActor
{
	GENERATED_BODY()
	
public:	
	ADA2UE4Party();

	//henchmen
	UPROPERTY()
	FName Henchman0;
	UPROPERTY()
	FName Henchman1;
	UPROPERTY()
	FName Henchman2;
	UPROPERTY()
	FName Henchman3;

	TArray<FPlot> Plots;
};
