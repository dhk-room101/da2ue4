#pragma once

class ADA2UE4Creature;

// -----------------------------------------------------------------------------
// sys_stealth
// -----------------------------------------------------------------------------
/*
Stealth System Implementation
*/
// -----------------------------------------------------------------------------
// owner: Georg Zoeller
// -----------------------------------------------------------------------------
#include "core_h.h"
#include "effect_upkeep_h.h"

// -----------------------------------------------------------------------------
// @brief  Forces a target to become drop out of stealth mode
// @author Georg
// -----------------------------------------------------------------------------
void DropStealth(AActor* oTarget);

int32 Stealth_CheckSuccess(AActor* oStealther, AActor* oPerceiver = nullptr);

int32 Stealth_GetStealthRank(AActor* oCreature);

int32 Stealth_GetPerceptionRank(AActor* oCreature);

// -----------------------------------------------------------------------------
// @brief  Returns if a target is stealthy
// @author georg
// -----------------------------------------------------------------------------
int32 IsStealthy(AActor* oTarget);