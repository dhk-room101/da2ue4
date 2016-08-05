#pragma once

#include "m2da_constants_h.h"
#include "sys_stealth_h.h"
#include "core_h.h"

const float SOUND_THRESH_MANA_STAMINA_LOW = 0.15;
const float SOUND_THRESH_HEALTH_LOW = 0.25;
const float SOUND_THRESH_HEALTH_NEAR_DEATH = 0.15;
const float SOUND_THRESH_DAMAGE_AMOUNT = 5.0f; // minimum damage for pain grunt, see damaged event in rules core

const int32 SOUND_SITUATION_COMMAND_COMPLETE = 1;
const int32 SOUND_SITUATION_ENEMY_SIGHTED = 2;
const int32 SOUND_SITUATION_GOT_DAMAGED = 3;
const int32 SOUND_SITUATION_ENEMY_KILLED = 4;
const int32 SOUND_SITUATION_COMBAT_CHARGE = 5;
const int32 SOUND_SITUATION_COMBAT_BATTLECRY = 6;
const int32 SOUND_SITUATION_ATTACK_IMPACT = 7;
const int32 SOUND_SITUATION_PARTY_MEMBER_SLAIN = 8;
const int32 SOUND_SITUATION_DYING = 9;
const int32 SOUND_SITUATION_END_OF_COMBAT = 10;
const int32 SOUND_SITUATION_SELECTED = 11;
const int32 SOUND_SITUATION_ORDER_RECEIVED = 12;
const int32 SOUND_SITUATION_SPELL_INTERRUPTED = 13;
const int32 SOUND_SITUATION_SKILL_FAILURE = 14;

void SSPlaySituationalSound(AActor* oCreature, int32 nSituation, AActor* oTarget = nullptr, int32 nCommandType = COMMAND_TYPE_INVALID);

int32 _GetSituationalCombatSound(AActor* oCreature, int32 nSituation, AActor* oTarget = nullptr, int32 nCommandType = COMMAND_TYPE_INVALID);

int32 _GetSituationalExploreSound(AActor* oCreature, int32 nSituation, AActor* oTarget, int32 nCommandType);

/**
* @brief Returns the state of a soundset flag
*
* A creature flag (SOUNDSET_FLAG_*) is a persistent boolean variable
*
* @param oCreature The creature to check
* @returns  TRUE_ or FALSE_ state of the flag.
*/
int32 SSGetSoundSetFlag(AActor* oCreature, int32 nSSEntry);

/**
* @brief Sets a SOUNDSET_FLAG_* flag (boolean persistent variable) on a creature
**
* @param oCreature The creature to set the flag on
* @param nFlag     SOUNDSET_FLAG_* to set.
* @param bSet      whether to set or to clear the flag.
*
* @returns  TRUE_ or FALSE_
**/
void SSSetSoundSetFlag(AActor* oCreature, int32 nSSEntry, int32 bSet = TRUE_);