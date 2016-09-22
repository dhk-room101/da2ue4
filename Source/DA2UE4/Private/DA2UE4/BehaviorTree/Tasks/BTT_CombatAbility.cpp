// Fill out your copyright notice in the Description page of Project Settings.

#include "DA2UE4.h"
#include "BTT_CombatAbility.h"

#include "DA2UE4CreatureController.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h" 

EBTNodeResult::Type UBTT_CombatAbility::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ADA2UE4CreatureController* MyController = Cast<ADA2UE4CreatureController>(OwnerComp.GetAIOwner());
	if (MyController == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	/*if (NewWaypoint)
	{
	OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Object>(BlackboardKey.GetSelectedKeyID(), NewWaypoint);
	return EBTNodeResult::Succeeded;
	}*/

	return EBTNodeResult::InProgress;
}



