// Fill out your copyright notice in the Description page of Project Settings.

#include "DA2UE4.h"
#include "BTT_CombatTactic.h"

#include "STypes.h"
#include "ldf.h"

#include "DA2UE4CreatureController.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h" 

EBTNodeResult::Type UBTT_CombatTactic::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ADA2UE4CreatureController* AIController = Cast<ADA2UE4CreatureController>(OwnerComp.GetAIOwner());
	if (AIController == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	ECombatResult nTactic = AIController->CombatTactics(AIController->GetPawn());

	switch (nTactic)
	{
	case ECombatResult::CREATURE_TYPE_NON_COMBATANT:
	{
#ifdef DEBUG
		LogError("CombatTactic: CREATURE_TYPE_NON_COMBATANT");

#endif // DEBUG

		return EBTNodeResult::Failed;
	}
	case ECombatResult::NOT_COMBAT_OR_EXPLORE_GAMEMODE:
	{
#ifdef DEBUG
		LogError("CombatTactic: NOT_COMBAT_OR_EXPLORE_GAMEMODE");

#endif // DEBUG
		return EBTNodeResult::Failed;
	}
	case ECombatResult::CREATURE_COMMAND_IN_PROGRESS:
	{
#ifdef DEBUG
		LogError("CombatTactic: CREATURE_COMMAND_IN_PROGRESS");

#endif // DEBUG
		return EBTNodeResult::Failed;
	}
	case ECombatResult::CREATURE_RULES_FLAG_AI_OFF_ENABLED:
	{
#ifdef DEBUG
		LogError("CombatTactic: CREATURE_RULES_FLAG_AI_OFF_ENABLED");

#endif // DEBUG
		return EBTNodeResult::Failed;
	}
	case ECombatResult::AI_LIGHT_ACTIVE:
	{
#ifdef DEBUG
		LogWarning("CombatTactic: AI_LIGHT_ACTIVE");

#endif // DEBUG
		return EBTNodeResult::Succeeded;
	}
	case ECombatResult::AI_AOE_FLEE:
	{
#ifdef DEBUG
		LogWarning("CombatTactic: AI_AOE_FLEE");

#endif // DEBUG
		return EBTNodeResult::Succeeded;
	}
	case ECombatResult::AI_PARTIAL_ACTIVE:
	{
#ifdef DEBUG
		LogWarning("CombatTactic: AI_PARTIAL_ACTIVE");

#endif // DEBUG
		return EBTNodeResult::Succeeded;
	}
	case ECombatResult::AI_MODIFIER_IGNORE:
	{
#ifdef DEBUG
		LogWarning("CombatTactic: AI_MODIFIER_IGNORE");

#endif // DEBUG
		return EBTNodeResult::Succeeded;
	}
	case ECombatResult::AI_WEAPON_SET_RANGED:
	{
#ifdef DEBUG
		LogWarning("CombatTactic: AI_WEAPON_SET_RANGED");

#endif // DEBUG
		return EBTNodeResult::Succeeded;
	}
	case ECombatResult::AI_WEAPON_SET_MELEE:
	{
#ifdef DEBUG
		LogWarning("CombatTactic: AI_WEAPON_SET_MELEE");

#endif // DEBUG
		return EBTNodeResult::Succeeded;
	}
	case ECombatResult::AI_MOVE_AWAY_DISTANCE_MELEE:
	{
#ifdef DEBUG
		LogWarning("CombatTactic: AI_MOVE_AWAY_DISTANCE_MELEE");

#endif // DEBUG
		return EBTNodeResult::Succeeded;
	}
	case ECombatResult::FOLLOWER_NOT_COMBAT_STATE:
	{
#ifdef DEBUG
		LogError("CombatTactic: FOLLOWER_NOT_COMBAT_STATE");

#endif // DEBUG
		return EBTNodeResult::Failed;
	}
	case ECombatResult::TOO_MANY_TACTICS:
	{
#ifdef DEBUG
		LogError("CombatTactic: TOO_MANY_TACTICS");

#endif // DEBUG
		return EBTNodeResult::Failed;
	}
	case ECombatResult::LAST_TACTIC_FAILED:
	{
#ifdef DEBUG
		LogError("CombatTactic: LAST_TACTIC_FAILED");

#endif // DEBUG
		return EBTNodeResult::Failed;
	}
	case ECombatResult::OBJECT_SELF_INACTIVE:
	{
#ifdef DEBUG
		LogError("CombatTactic: OBJECT_SELF_INACTIVE");

#endif // DEBUG
		return EBTNodeResult::Failed;
	}
	case ECombatResult::NOT_IN_COMBAT:
	{
#ifdef DEBUG
		LogError("CombatTactic: NOT_IN_COMBAT");

#endif // DEBUG
		return EBTNodeResult::Failed;
	}
	case ECombatResult::TACTIC_EXECUTED:
	{
#ifdef DEBUG
		LogWarning("CombatTactic: TACTIC_EXECUTED");

#endif // DEBUG
		return EBTNodeResult::Succeeded;
	}
	case ECombatResult::DEFAULT_ACTION:
	{
#ifdef DEBUG
		LogWarning("CombatTactic: DEFAULT_ACTION");

#endif // DEBUG
		return EBTNodeResult::Succeeded;
	}
	case ECombatResult::UNKNOWN_COMBAT_RESULT:
	{
#ifdef DEBUG
		LogError("CombatTactic: UNKNOWN_COMBAT_RESULT");

#endif // DEBUG
		return EBTNodeResult::Failed;
	}
	default:
	{
#ifdef DEBUG
		LogError("CombatTactic: unknown tactic");

#endif // DEBUG
		return EBTNodeResult::Failed;
	}
	}

	return EBTNodeResult::InProgress;
}

