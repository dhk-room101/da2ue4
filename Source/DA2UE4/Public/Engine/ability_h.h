#pragma once

// ----------------------------------------------------------------------------
// ability_h - Ability System Functions
// ----------------------------------------------------------------------------
/*
General Purpose functions dealing with the ability/talent/spell system,
including, but not limited to:

- Calculating and Subtracting Ability Costs
- Invoking and rerouting Spellscript events
- Wrappers for dealing with Upkeep Effects applied by Abilities

*/
// ----------------------------------------------------------------------------
// Owner: Georg Zoeller
// ----------------------------------------------------------------------------

//#include "effects_h.h"
#include "m2da_constants_h.h"
//#include "config_h.h"
#include "events_h.h"
#include "items_h.h"
//#include "sys_resistances_h.h"

// abi flags
const int32 ABILITY_FLAG_RANGED_WEAPON = 1;  /*0x01*/

// free:
const int32 ABILITY_FLAG_END_ON_OUT_OF_MANA = 64; /*0x40*/
const int32 ABILITY_FLAG_DISPELLABLE = 128;       /*0x80*/
const int32 ABILITY_FLAG_CURABLE = 256;           /*0x100*/

// flanking constants
const float ABILITY_FLANK_SIZE_BACK = 60.0f;
const float ABILITY_FLANK_SIZE_BACK2 = 45.0f;
const float ABILITY_FLANK_SIZE_SIDE = 60.0f;
const float ABILITY_FLANK_SIZE_LARGE_SIDE = 90.0f;
const float ABILITY_FLANK_SIZE_FRONT = 130.f;
const float ABILITY_FLANK_FACING_BACK_LEFT = -135.0f;
const float ABILITY_FLANK_FACING_BACK_RIGHT = 135.0f;
const float ABILITY_FLANK_FACING_BACK_LEFT2 = -160.0f;
const float ABILITY_FLANK_FACING_BACK_RIGHT2 = 160.0f;
const float ABILITY_FLANK_FACING_LEFT = -60.0f;
const float ABILITY_FLANK_FACING_RIGHT = 60.0f;
const float ABILITY_FLANK_FACING_LARGE_RIGHT = 45.0f;
const float ABILITY_FLANK_FACING_LARGE_LEFT = -45.0f;
const float ABILITY_FLANK_FACING_FRONT = 0.0f;

/**
* @brief Checks if a specific ability is active on an AActor
*
* The check includes modal abilities and any other abilities with
* a duration (all buffs and de-buffs)
*
* @param oActor the AActor* we are checking for the ability
* @param nAbilityID the ability we check if is active
* @returns TRUE_ if the ability is active, FALSE_ otherwise
*
* @author   Yaron Jakobs
*
**/
int32 Ability_IsAbilityActive(AActor* oCreature, int32 nAbilityID);

int32 Ability_CheckFlag(int32 nAbility, int32 nFlag);

/**
* @brief (core_h) Returns the ability type as a ABILITY_TYPE_* constant
*
* @param nAbility   ability id (row number in ABI_BASE)
*
* @returns ABILITY_TYPE_* constant
*
* @author   Georg Zoeller
**/
int32 Ability_GetAbilityType(int32 nAbility);

/**
* @brief Returns the TABLE_* constants for 2da lookups for a specific abiltiy
*
* Talents, Spells, Skills and Items use different 2das, even tho they have the
* same structure. This function returns the constant for use with the GetM2DA*
* functions
*
* @param nAbilityType ABILITY_TYPE_* constant
*
* @returns TABLE_* constant
*
* @author   Georg Zoeller
*
**/
int32 Ability_GetAbilityTable(int32 nAbilityType);

// -----------------------------------------------------------------------------
// Get a human readable FString for an ability
// -----------------------------------------------------------------------------
FString Ability_GetAbilityNameById(int32 nAbilityId);

/**
* @brief Performs an ability cost check to see if an ability can be used.
*
* Returns TRUE_ if the caster has enough mana or stamina (depending on ability type)
* to use (used in ability_core)
*
* @param oCaster      The caster using the ability
* @param nAbility     The ability being used
* @param nAbilityType The ability type of that ability
* @param oItem        The Item consumed to use the ability/spell (Optional)
*
* @returns  TRUE_ (enough), FALSE_ (not enough)
* @author   Georg Zoeller
*
**/
int32 Ability_CostCheck(AActor* oCaster, int32 nAbility, int32 nAbilityType, AActor* oItem = nullptr);

/**
* @brief Returns whether or not an ability is a modal ability
*
* @param oAttacker          The Attacking Creature
* @param oTarget            The attacked creature
*
* @author   Georg Zoeller

**/
int32 Ability_IsModalAbility(int32 nAbility);

/**
* @brief returns the cost (mana or stamina) of using an ability
*
* Note: nAbility = ABILITY_TYPE_INVALID will cause a lookup in the 2da
* This function is duplicated within the game executable. Any change made to this function will
* result in GUI glitches and other bugs. Sorry.
*
*
* @param oCaster        The creature to deactivate the ability on
* @param nAbility       The modal ability to deactivate
* @param nAbilityType   The ability type of the modal ability.
*
*
* @returns TRUE_ if the ability was terminated successfully
*
* @author   Georg Zoeller
*
**/
float Ability_GetAbilityCost(AActor* oCaster, int32 nAbility, int32 nAbilityType = ABILITY_TYPE_INVALID, int32 bUpkeep = FALSE_);

/**
* @brief Returns TRUE_ if the specified ability is a blood magic ability
*
* Used by the spell cost functions to determine whether or not to cast
* from health or mana/stamina
*
* @param nAbilityType ABILITY_TYPE_* constant
*
* @returns TRUE_ or False
*
* @author   Georg Zoeller
*
**/
int32 Ability_IsBloodMagic(AActor* oCaster);

/**
* @brief Determine whether or not all conditions for the current talent are met
*
* This is temporary, it will go into the engine at some point
* Check the use conditions for the ability, e.g. melee only
* Later this should be
* done mostly in the UI (e.g. don't even allow to use it)
* 0   None
* 1   Melee
* 2   Shield
* 4   Ranged
* 8   Behind Target
* 16  Mode active
*
*
* @author Georg
*/
int32 Ability_CheckUseConditions(AActor* oCaster, AActor* oTarget, int32 nAbility, AActor* oItem = nullptr);

/**
* @brief Performs an ability cost check to see if an ability can be used.
*
* Returns TRUE_ if the caster has enough mana or stamina (depending on ability type)
* to use (used in ability_core)
*
* @param oCaster      The caster using the ability
* @param nAbility     The ability being used
* @param nAbilityType The ability type of that ability
*
* @returns  TRUE_ (enough), FALSE_ (not enough)
* @author   Georg Zoeller
*
**/
int32 Ability_GetAbilityTargetType(int32 nAbility, int32 nAbilityType);