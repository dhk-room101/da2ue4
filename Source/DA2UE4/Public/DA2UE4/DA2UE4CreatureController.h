// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "DA2UE4CreatureController.generated.h"

class ADA2UE4TargetPoint;

/**
 * 
 */
UCLASS()
class DA2UE4_API ADA2UE4CreatureController : public AAIController
{
	GENERATED_BODY()

	ADA2UE4CreatureController(const class FObjectInitializer& ObjectInitializer);

	/* Called whenever the controller possesses a character bot */
	virtual void Possess(class APawn* InPawn) override;
	virtual void UnPossess() override;

	void SpawnCreature(APawn* InPawn);

public:
	void TriggerEventPerceptionAppear(APawn* NewPawn, int32 bPerceived, int32 bHostile, int32 bAbility, int32 bHostilityChanged);
};
