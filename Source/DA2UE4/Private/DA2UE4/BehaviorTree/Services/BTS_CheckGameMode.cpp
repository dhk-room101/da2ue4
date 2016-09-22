// Fill out your copyright notice in the Description page of Project Settings.

#include "DA2UE4.h"
#include "BTS_CheckGameMode.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"

#include "DA2UE4GameInstance.h"
#include "ldf.h"

UBTS_CheckGameMode::UBTS_CheckGameMode(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = "CheckGameMode";
}

void UBTS_CheckGameMode::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	//LogWarning("current value is " + FString::FromInt(static_cast<uint8>(GetGameMode())));

	if (GetModule())
	{
		if (OwnerComp.GetBlackboardComponent()->GetValue<UBlackboardKeyType_Enum>(BlackboardKey.GetSelectedKeyID()) != static_cast<uint8>(GetGameMode()))
		{
			OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Enum>(BlackboardKey.GetSelectedKeyID(), static_cast<uint8>(GetGameMode()));
		}
	}
}

