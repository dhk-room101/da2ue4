// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DA2UE4/Components/DA2UE4RulesComponent.h"
#include "DA2UE4PlayerComponent.generated.h"

/**
 * 
 */
UCLASS()
class DA2UE4_API UDA2UE4PlayerComponent : public UDA2UE4RulesComponent
{
	GENERATED_BODY()
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void EquipDefaultItem(AActor* OBJECT_SELF, int32 nAppType, int32 nTargetSlot, FString s2daColumnName);
	
};
