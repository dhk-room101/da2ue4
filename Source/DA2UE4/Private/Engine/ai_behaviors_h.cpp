#include "DA2UE4.h"
#include "ai_behaviors_h.h"

int32 AI_BehaviorCheck(AActor* OBJECT_SELF, FString sBehaviorType)
{
	int32 nBehavior = GetAIBehavior(OBJECT_SELF);
	FString sLabel = GetM2DAString(TABLE_AI_BEHAVIORS, "Label", nBehavior);
	int32 nRet = GetM2DAInt(TABLE_AI_BEHAVIORS, sBehaviorType, nBehavior);
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "AI_BehaviorCheck", "*" + sLabel + "* [" + sBehaviorType + "]: " + IntToString(nRet));
#endif
	return nRet;
}

int32 AI_BehaviorCheck_AvoidAOE(AActor* OBJECT_SELF)
{
	return AI_BehaviorCheck(OBJECT_SELF, AI_BEHAVIOR_AVOID_AOE);
}

int32 AI_BehaviorCheck_AttackBack(AActor* OBJECT_SELF)
{
	return AI_BehaviorCheck(OBJECT_SELF, AI_BEHAVIOR_ATTACK_BACK);
}

int32 AI_BehaviorCheck_PreferRange(AActor* OBJECT_SELF)
{
	return AI_BehaviorCheck(OBJECT_SELF, AI_BEHAVIOR_PREFER_RANGE);
}

int32 AI_BehaviorCheck_ChaseEnemy(AActor* OBJECT_SELF)
{
	return AI_BehaviorCheck(OBJECT_SELF, AI_BEHAVIOR_CHASE_ENEMY);
}

int32 AI_BehaviorCheck_PreferMelee(AActor* OBJECT_SELF)
{
	return AI_BehaviorCheck(OBJECT_SELF, AI_BEHAVIOR_PREFER_MELEE);
}

int32 AI_BehaviorCheck_AvoidNearbyEnemies(AActor* OBJECT_SELF)
{
	return AI_BehaviorCheck(OBJECT_SELF, AI_BEHAVIOR_AVOID_NEARBY_ENEMIES);
}

int32 AI_BehaviorCheck_AvoidMelee(AActor* OBJECT_SELF)
{
	return AI_BehaviorCheck(OBJECT_SELF, AI_BEHAVIOR_AVOID_MELEE_ENEMIES);
}

int32 AI_BehaviorCheck_AttackOnCombatStart(AActor* OBJECT_SELF)
{
	return AI_BehaviorCheck(OBJECT_SELF, AI_BEHAVIOR_ATTACK_ON_COMBAT_START);
}

int32 AI_BehaviorCheck_DefaultAttack(AActor* OBJECT_SELF)
{
	return AI_BehaviorCheck(OBJECT_SELF, AI_BEHAVIOR_DEFAULT_ATTACK);
}