// Fill out your copyright notice in the Description page of Project Settings.

#include "DA2UE4.h"
#include "DA2UE4Library.h"

UDA2UE4Library::UDA2UE4Library(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

//get singleton
UDA2UE4Singleton* UDA2UE4Library::GetDA2UE4Data(bool& IsValid)
{
	IsValid = false;
	UDA2UE4Singleton* DataInstance = Cast<UDA2UE4Singleton>(GEngine->GameSingleton);

	if (!DataInstance) return NULL;
	if (!DataInstance->IsValidLowLevel()) return NULL;

	IsValid = true;
	return DataInstance;
}
