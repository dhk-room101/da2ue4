#pragma once

#include "STypes.h"

class Engine;

// -----------------------------------------------------------------------------
// Point blank range (no penalty range for bows)
// -----------------------------------------------------------------------------
const float POINT_BLANK_RANGE = 8.0f;

const float ATTACK_LOOP_DURATION_INVALID = 999.0f;
const float ATTACK_HIT_BIAS = 4.0f; // General bias in the system towards hits instead of misses.
const int32 ATTACK_TYPE_MELEE = 1;
const int32 ATTACK_TYPE_RANGED = 2;

const float SPECIAL_BOSS_DEATHBLOW_THRESHOLD = 0.04;        // at this % of health, any meelee attack may trigger the deathblow of special bosses;

// Handles any combat related logic for when a creature disappears from combat. This function should be run
// at 2 cases both for a player AActor* and a creature AActor:
// 1. Disappear event
// 2. Appear event when the hostile creature turned non-hostile
void Combat_HandleCreatureDisappear(AActor* oCreature, AActor* oDisappearer);

/**
*  @brief Handles processing an Attack Command
*
*  @param oAttacker       The command owner, usually OBJEC_TSE
*  @param oTarget         The Target of the command
*  @param nCommandId      The command Id
*  @param nCommandSubType The command subtype
*
*  @returns COMBAT_RESULT_* constant
*
*  @author Georg Zoeller
**/
int32  Combat_HandleCommandAttack(AActor* oAttacker, AActor* oTarget, int32 nCommandSubType);

/**
* @brief Determine which hand to use for an attack
*
* Only applicable in dual weapon style. Returns which hand to use for the next attack
*
* @returns  0 - main hand, 1 - offhand
*
* @author Georg Zoeller
*
**/
int32 Combat_GetAttackHand(AActor* oCreature);

// -----------------------------------------------------------------------------
// Return the type of the current attack based on the weapon in the main hand
// used only in command_pending...
// -----------------------------------------------------------------------------
int32 Combat_GetAttackType(AActor* oAttacker, AActor* oWeapon);

/**
*  @brief Handles processing an Attack Command
*
*  @param oAttacker       The command owner, usually OBJECT_SELF
*  @param oTarget         The Target of the command
*
*  @returns CombatAttackResultStruct with damage and attack result populated
*
*  "Don't touch this if you want to live"
*
*  @author Georg Zoeller
**/
FCombatAttackResultStruct Combat_PerformAttack(AActor* oAttacker, AActor* oTarget, AActor* oWeapon, float fDamageOverride = 0.0f, int32 nAbility = 0);

/**
* @brief Determine whether or not an attack hits a target.
*
* Note that this function only calculates to hit and crits, death blows are
* determined in Combat_PerformAttack
*
* @param oAttacker  The attacker
* @param oTarget    The target that is being attacked
* @param nAbility   If != 0, it won't trigger backstabs and deathblows.
*
* @returns  COMBAT_RESULT_HIT, COMBAT_RESULT_CRITICALHIT or COMBAT_RESULT_MISS
*
* @author Georg Zoeller
*
**/
int32 Combat_GetAttackResult(AActor* oAttacker, AActor* oTarget, AActor* oWeapon, float fBonus = 0.0f, int32 nAbility = 0);

float Combat_GetFlankingBonus(AActor* oAttacker, AActor* oTarget);

// -----------------------------------------------------------------------------
// Check if backstab conditions are true
// -----------------------------------------------------------------------------
int32 Combat_CheckBackstab(AActor* oAttacker, AActor* oTarget, AActor* oWeapon, float fFlankingBonus);

/**
* @brief Check if a deathblow should occur.
*
* Examine a number of parameters to see whether or not we can play a deathblow
* without interrupting the flow of combat.
*
* @author Georg Zoeller
**/
int32 CheckForDeathblow(AActor* oAttacker, AActor* oTarget);

/**
* @brief Determine a valid deathblow for the current target
*
* Called when the script decides to do a deathblow, this function will return
* a DEATHBLOW_* constant representing the deathblow to use against the creature
*
* If the creature does not support deathblows (immunity, no animation) or can not
* be deathblowed (Immortal, Plot, Party Member, Flagged as invalid in the toolset)
* the function returns 0
*
* @param oTarget    The target for the Deathblow
*
* @returns  0 or DEATH_BLOW_*
*
* @author Georg Zoeller
*
**/
int32 Combat_GetValidDeathblow(AActor* oAttacker, AActor* oTarget);