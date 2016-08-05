#pragma once

#include "ai_constants_h.h"
#include "core_h.h"
#include "m2da_constants_h.h"

int32 AI_BehaviorCheck(AActor* OBJECT_SELF, FString sBehaviorType);

int32 AI_BehaviorCheck_AvoidAOE(AActor* OBJECT_SELF);

int32 AI_BehaviorCheck_AttackBack(AActor* OBJECT_SELF);

int32 AI_BehaviorCheck_PreferRange(AActor* OBJECT_SELF);

int32 AI_BehaviorCheck_ChaseEnemy(AActor* OBJECT_SELF);

int32 AI_BehaviorCheck_PreferMelee(AActor* OBJECT_SELF);

int32 AI_BehaviorCheck_AvoidNearbyEnemies(AActor* OBJECT_SELF);

int32 AI_BehaviorCheck_AvoidMelee(AActor* OBJECT_SELF);

int32 AI_BehaviorCheck_AttackOnCombatStart(AActor* OBJECT_SELF);

int32 AI_BehaviorCheck_DefaultAttack(AActor* OBJECT_SELF);