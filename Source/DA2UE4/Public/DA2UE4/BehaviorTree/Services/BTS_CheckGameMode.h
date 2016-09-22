// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTS_CheckGameMode.generated.h"

class Engine;

/**
 * 
 */
UCLASS()
class DA2UE4_API UBTS_CheckGameMode : public UBTService_BlackboardBase
{
	GENERATED_BODY()
	
	UBTS_CheckGameMode(const class FObjectInitializer& ObjectInitializer);

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;	
};
