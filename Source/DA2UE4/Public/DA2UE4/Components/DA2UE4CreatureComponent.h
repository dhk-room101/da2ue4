// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DA2UE4/Components/DA2UE4RulesComponent.h"
#include "STypes.h"
#include "DA2UE4CreatureComponent.generated.h"

class ADA2UE4Creature;

/**
 * 
 */
UCLASS()
class DA2UE4_API UDA2UE4CreatureComponent : public UDA2UE4RulesComponent
{
	GENERATED_BODY()

	void EquipDefaultItem(AActor* OBJECT_SELF, int32 nAppType, int32 nTargetSlot, FString s2daColumnName);
	void SignalCommandPending(ADA2UE4Creature* OBJECT_SELF, FCommand command);

	void HandleCreatureEvent(ADA2UE4Creature* OBJECT_SELF);
	void HandleCreatureCommand(ADA2UE4Creature* OBJECT_SELF);
	void HandleCommandResult(ADA2UE4Creature* OBJECT_SELF);
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	
	
};
