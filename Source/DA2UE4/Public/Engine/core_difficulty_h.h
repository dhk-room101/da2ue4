#pragma once

// -----------------------------------------------------------------------------
// core_difficulty_h
// -----------------------------------------------------------------------------
/*
game difficulty scaling

as a core file, this CANNOT include any other files
*/
// -----------------------------------------------------------------------------
// owner: noel borstad
// -----------------------------------------------------------------------------

#include "m2da_constants_h.h"

class Engine;

/** @brief Returns the appropriate autoscale table to use based on current difficulty
*
*   Looks up which autoscale table to use based on the current difficulty setting
* @author Noel
**/
int32 Diff_GetAutoScaleTable();

//This function is duplicated within the game executable. Any change made to this function will 
//result in GUI glitches and other bugs. Sorry.
float Diff_GetAbilityUseMod(AActor* oCreature);

float Diff_GetRulesAttackBonus(AActor* oAttacker);

float Diff_GetRulesDefenseBonus(AActor* oCreature);

float Diff_GetRulesDamageBonus(AActor* oCreature);

float GetDamageScalingThreshold();