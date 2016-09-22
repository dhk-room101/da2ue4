#pragma once

//------------------------------------------------------------------------------
//  core_h.nss
//------------------------------------------------------------------------------
//  Include file containing definitions for low level functions independent of
//  specific systems.
//
//  This file may never include any other files with the exception of log_h
//  or constant files functions in this file usually do not carry a prefix
//
//  Please talk to Georg before using any of these functions or making any
//  changes.
//------------------------------------------------------------------------------
//  2006/11/27 - Owner: Georg Zoeller
//------------------------------------------------------------------------------

#include "ldf.h"

class ADA2UE4Creature;

//const int32   DA_LEVEL_CAP = 25;  // Dragon Age level cap. Note: This is one of several values that control this (including max_val on properties.xls!)

// -----------------------------------------------------------------------------
// Basic combat system configuration
// -----------------------------------------------------------------------------
const float COMBAT_CRITICAL_DAMAGE_MODIFIER = 1.5f; // critical hits increase damage by up to this factor.
const float COMBAT_DEFAULT_UNARMED_DAMAGE = 3.0f;   // basic unarmed damage
const float COMBAT_ARMOR_RANDOM_ELEMENT = 0.3f;     // How much of armor value is randomized when reducing incoming damage (default: 30%)
const float UNARMED_ATTRIBUTE_BONUS_FACTOR = 1.25;


// Weapon Timings
const float BASE_TIMING_DUAL_WEAPONS = 1.5;
const float BASE_TIMING_WEAPON_SHIELD = 2.0;
const float BASE_TIMING_TWO_HANDED = 2.5;

//const float REGENERATION_STAMINA_COMBAT_DEFAULT = 1.0f;       // was .5
//const float REGENERATION_STAMINA_COMBAT_NULL = -1.0f;
//const float REGENERATION_STAMINA_COMBAT_DEGENERATION = -2.0f;

//const float REGENERATION_STAMINA_EXPLORE_DEFAULT = 17.5f;     // was 10
//const float REGENERATION_STAMINA_EXPLORE_NULL = -17.5f;
//const float REGENERATION_STAMINA_EXPLORE_DEGENERATION = -20.0f;

//const float REGENERATION_HEALTH_COMBAT_DEFAULT = 0.0f;
//const float REGENERATION_HEALTH_EXPLORE_DEFAULT = 10.0f;


//Hand definitions
const int32 HAND_MAIN = 0;
const int32 HAND_OFFHAND = 1;
const int32 HAND_BOTH = 3;


// -----------------------------------------------------------------------------




//const resource INVALID_RESOURCE = R"";

// Generic compare Results for Compare* functions
//const int32 COMPARE_RESULT_HIGHER = 1;
//const int32 COMPARE_RESULT_EQUAL = -1;
//const int32 COMPARE_RESULT_LOWER = 0;




// Placeable State Controller types
//const FString PLC_STATE_CNT_BRIDGE = "StateCnt_Bridge";
//const FString PLC_STATE_CNT_AREA_TRANSITION = "StateCnt_AreaTransition";
//const FString PLC_STATE_CNT_FURNITURE = "StateCnt_Furniture";
//const FString PLC_STATE_CNT_INFORMATIONAL = "StateCnt_Informational";
//const FString PLC_STATE_CNT_AOE = "StateCnt_AOE";
//const FString PLC_STATE_CNT_FLIPCOVER = "StateCnt_FlipCover";
//const FString PLC_STATE_CNT_TRAP_TRIGGER = "StateCnt_Trap_Trigger";
//const FString PLC_STATE_CNT_NON_SELECTABLE_TRAP = "StateCnt_NonSelectable_Trap";
//const FString PLC_STATE_CNT_SELECTABLE_TRAP = "StateCnt_Selectable_Trap";
//const FString PLC_STATE_CNT_PUZZLE = "StateCnt_Puzzle";
//const FString PLC_STATE_CNT_CAGE = "StateCnt_Cage";
//const FString PLC_STATE_CNT_BODYBAG = "StateCnt_Bodybag";
//const FString PLC_STATE_CNT_CONTAINER_STATIC = "StateCnt_Container_Static";
//const FString PLC_STATE_CNT_CONTAINER = "StateCnt_Container";
//const FString PLC_STATE_CNT_TRIGGER = "StateCnt_Trigger";
//const FString PLC_STATE_CNT_DOOR = "StateCnt_Door";


// used as artifical delimiter for GetNearest* functions in some spells.
//const int32 MAX_GETNEAREST_OBJECTS = 30;

// Rules stuff
//const int32 MIN_ATTRIBUTE_VALUE = 1;



const float RULES_ATTRIBUTE_MODIFIER = 10.0f;

const int32 CREATURE_RULES_FLAG_DYING = 0x00000002;
const int32 CREATURE_RULES_FLAG_DOT = 0x00000004;
const int32 CREATURE_RULES_FLAG_AI_OFF = 0x00000008;
const int32 CREATURE_RULES_FLAG_AI_NO_ABILITIES = 0x00000010;
const int32 CREATURE_RULES_FLAG_NO_COOLDOWN = 0x00000020;
const int32 CREATURE_RULES_FLAG_NO_RESISTANCE = 0x00000040;
const int32 CREATURE_RULES_FLAG_FORCE_COMBAT_0 = 0x00000080; // bit0 of combat result force
const int32 CREATURE_RULES_FLAG_FORCE_COMBAT_1 = 0x00000100; // bit1 of combat result force

const int32 APR_RULES_FLAG_CONSTRUCT = 0x00000002;
//const int32 AREA_FLAG_IS_FADE = 0x00000001;

const int32 WEAPON_WIELD_TWO_HANDED_MELEE = 3;

// Body bag stuff
//const FString BODY_BAG_TAG = "gen_ip_bodybag";

void _LogDamage(FString msg, AActor* oTarget = nullptr);

/** @brief (core_h) Returns true if a creature is one of the 4 controllable party members.
*   This is a wrapper around IsFollower.
*/
int32 IsPartyMember(AActor* oCreature);

/**
* @brief (core_h)Returns 1 if a creature is currently dying or
* has been dealt a deathblow
*
* @param oCreature The creature to check
*
* @returns  1 or 0
*
* @author Georg Zoeller
**/
int32 IsDying(AActor* oCreature);

/** ----------------------------------------------------------------------------
* @brief (core_h) Check if the creature is 'disabled' (negative effect)
*
* @param oCreature The creature to set the flag on
*
* @returns  1 or 0
*
* @author Georg Zoeller
*  ---------------------------------------------------------------------------**/
int32 IsDisabled(AActor* oCreature, int32 bGroundCheck = FALSE_);

/**
*   @brief Returns a creature's core class (mage, rogue, warrior)
*
*   @author georg
**/
int32 GetCreatureCoreClass(AActor* oCreature);

/**
*  @brief Gets an AActor's current health
*  @sa    SetCurrentHealth
*  @author Georg
**/
float GetCurrentHealth(AActor* oActor);

/**
*  @brief Set's an AActor's current health
*  @sa    GetCurrentHealth
*  @author Georg
**/
void SetCurrentHealth(AActor* oActor, float fNewValue);

/**
*  @brief Returns a creatures maximum health
*  @author Georg
**/
float GetMaxHealth(AActor* oActor);

/**
*  @brief Returns a creatures current mana/stamina
*  @sa    SetCurrentManaStamina
*  @author Georg
**/
float GetCurrentManaStamina(AActor* aActor);

/**
*  @brief Returns a creatures maximum health
*  @author Georg
**/
float GetMaxManaStamina(AActor* aActor);

/**
*  @brief returns true if creature x is using a melee weapon (or fists)
*  @param oCreature the creature to test
*  @param oItem an optional item to test (otherwise the main hand will be tested)
*  @author Georg
**/
int32 IsUsingMeleeWeapon(AActor* oCreature, AActor* oItem = nullptr);

/**
* @brief return the ITEM_TYPE_* of an item
*
* Returns the type an item belongs to (e.g. ITEM_TYPE_WEAPON_RANGED) from
* BITEM_base.xls, column "Type"
*
* @ param oItem - an Item
*
* @author Georg
**/
int32 GetItemType(AActor* oItem);

int32 IsShapeShifted(AActor* oCreature);

/**
*   @brief Returns whether the whole party has been wiped
*
*   [core_h] Returns true if all party members are dead.
*
*   @returns    True if all party members are dead.
*
*   @author georg
**/
int32 IsPartyDead();

AActor* GetRandomPartyMember();

/*
@brief Returns the greater value of f1 and f2

@param f1, f2 - floats

@returns the greater value of both
*/
float MaxF(float f1, float f2);

/*
@brief Returns the lesser value of f1 and f2

@param f1, f2 - floats

@returns the lesser value of both
*/
float MinF(float f1, float f2);

/*
@brief Returns the greater value of f1 and f2

@param n1, n2 - integers

@returns the greater value of both
*/
int32 Max(int32 n1, int32 n2);

/*
@brief Returns the greater value of n1 and n2

@param n1, n2 - integers

@returns the greater value of both
*/
int32 Min(int32 n1, int32 n2);

/**
*  @brief Small helper for dealing with random floats in some combat functions
*  @author Georg
**/
float RandFF(float fRange, float fStatic = 0.0f, int32 bDeterministic = FALSE_);

/**
* @brief (core_h)Sets a CREATURE_FLAG_* flag (boolean persistent variable) on a creature
*
* Flags are used by various game systems and should always be set through
* this function.
*
* @param oCreature The creature to set the flag on
* @param nFlag     CREATURE_FLAG_* to set.
* @param bSet      whether to set or to clear the flag.
*
* @returns  1 or 0
*
* @author Georg Zoeller
**/
void SetCreatureFlag(AActor* oCreature, int32 nFlag, int32 bSet = TRUE_);

/**
* @brief (core_h)Returns the state of a creature flag
*
* A creature flag (CREATURE_FLAG_*) is a persistent boolean variable
*
* @param oCreature The creature to check
*
* @returns  1 or 0 state of the flag.
*
* @author Georg Zoeller
*/
int32 GetCreatureFlag(AActor* oCreature, int32 nFlag);

/**
* @brief Return the selected AI Behavior (idx into aibehaviors.xls)
* @param oCreature
* @author   Georg Zoeller.
**/
int32 GetAIBehavior(AActor* oCreature);

/**
*  @brief returns true if creature x is using a ranged weapon
*  @param oCreature the creature to test
*  @param oItem an optional item to test (otherwise the main hand will be tested)
*  @param bExlcudeWand use to exclude wand (e.g. check specifically for box/xbow)
*  @author Georg
**/
int32 IsUsingRangedWeapon(AActor* oCreature, AActor* oItem = nullptr, int32 bExludeWand = FALSE_);

/**
*  @brief returns true if creature x is using a ranged weapon
*  @param oCreature the creature to test
*  @param oItem an optional item to test (otherwise the main hand will be tested)
*  @author Georg
**/
int32 IsUsingShield(AActor* oCreature, AActor* oItem = nullptr);

/** ----------------------------------------------------------------------------
* @brief (core_h) Wrapper for ApplyEffectOnObject.
*
* @param nDurationType can be EFFECT_DURATION_TYPE_PERMANENT EFFECT_DURATION_TYPE_INSTANTANEOUS or EFFECT_DURATION_TYPE_TEMPORARY.
* @param Effect the effect to be applied
* @param oTarget the target of the effect
* @param fDuration  this value needs to be set only when nDurationType is EFFECT_DURATION_TYPE_TEMPORARY
* @param oCreator effect creator
* @param nAbilityId The ability ID of the effect (Important for dispelling!!!)
*
* @author Georg Zoeller
*  ---------------------------------------------------------------------------**/
void ApplyEffectOnObject(int32 nDurationType, FEffect eEffect, AActor* oTarget, float fDuration = 0.0f, int32 nAbilityId = FALSE_);

// -----------------------------------------------------------------------------
// Return if a forced combat result was set on the creature
// -----------------------------------------------------------------------------
int32 GetForcedCombatResult(AActor* oCreature);

float GetCreatureAttackRating(AActor* oCreature);

//TODO Magic critical modifier?
/**
*  @brief Returns the creature critical hit modifier used for combat
*
*  There are 3 different critical hit modifiers on a creature: Melee, Magic and
*  Ranged.
*
*  @param oCreature the creature to retrieve the stat from
*  @param nCritModifier A CRITICAL_MODIFIER_* constant as follows:
*                       CRITICAL_MODIFIER_MELEE
*                       CRITICAL_MODIFIER_MAGIC
*                       CRITICAL_MODIFIER_RANGED
*
*  @author Georg
**/
float GetCreatureCriticalHitModifier(AActor* oCreature, int32 nCritModifier);

/**
*  @brief Returns a creatures defense rating. This does not exclude shields
*  @sa    GetCreatureShieldRating
*  @author Georg
**/
float GetCreatureDefense(AActor* oCreature);

/*
* @brief Retrieves weapon damage for the staff
* @author georg
*/
float Combat_Damage_GetMageStaffDamage(AActor* oAttacker, AActor* oTarget, AActor* oWeapon, int32 bDeterministic = FALSE_);

float GetCreatureSpellPower(AActor* oCreature);

/*
* @brief Retrieves a weapons attack damage. This is randomized unless bForceMaxDamage is specified.
*
* @returns Weapon damage of a single attack.
*
* @author georg
*/
float DmgGetWeaponDamage(AActor* oWeapon, int32 bForceMaxDamage = FALSE_);

/*
* @brief Retrieves a weapons base damage from the defined item stat.
*
* @returns weapon base damage
*
* @author georg
*/
float DmgGetWeaponBaseDamage(AActor* oWeapon);

/*
* @brief Retrieves a weapons max damage from the defined item stat.
*
* @returns weapon max damage
*
* @author georg
*/
float DmgGetWeaponMaxDamage(AActor* oWeapon);

float GetWeaponAttributeBonusFactor(AActor* oWeapon);

/*
* @brief Calculate attack damage bonus based on a number of parameters
*        such as wield mode and talents
*
* @returns Damage bonus
*
* @author georg
*/
float Combat_Damage_GetAttributeBonus(AActor* oCreature, int32 nHand = HAND_MAIN, AActor* oWeapon = nullptr, int32 bDeterministic = FALSE_);

float GetAttributeModifier(AActor* oCreature, int32 nAttribute);

/*
* @brief Return true if a weapon is a two handed melee weapon (bitm_base lookup)
*
* @author georg
*
*/
int32 IsMeleeWeapon2Handed(AActor* oMeleeWeapon);

float _GetRelativeResourceLevel(AActor* oCreature, int32 nResource);

/**
* @brief (core_h)Returns creature appearance flags (from apr_base)
*
* @param oCreature The creature to check
*
* @returns  1 or 0 state of the flag.
*
* @author Georg Zoeller
*/
int32 GetCreatureAppearanceFlag(AActor* oCreature, int32 nFlag);

int32 GetLevel(AActor* oCreature);

/**
*   @brief returns the time for the ranged aim loop delay
*
*   The ranged aim loop is used on bows and crossbows and controls the rate of
*   fire for the weapon by telling the engine how long to loop the aim animation
*   before releasing the projectile. This function calculates the lengths of
*   the aimloop for a particular character and weapon, which is later passed back
*   to the engine via SetAimLoop function in the CommandPending event.
*
*   @param oShooter The creature shooting the weapon
*   @param oWeapon  The ranged weapon used
*
*   @returns Calculated time of the aimloop, minimal 0.0f.
*
*   @author georg
*
**/
float GetCreatureRangedDrawSpeed(AActor* oCreature, AActor* oWeapon = nullptr);

/*
* @brief Calculate the attack duration value to be passed to the engine based
*        on factors like wield style, effects and stats
*
* @returns Attack duration value or 0.0 for default (anim controlled)
*
* @author georg
*/
float CalculateAttackTiming(AActor* oAttacker, AActor* oWeapon);

int32 IsArmorMassive(AActor* oArmor);

int32 IsArmorHeavyOrMassive(AActor* oArmor);

/** @brief Begins a conversation with the given AActor
*
* If rConversationFile is specified then that file will be used, otherwise
* the conversation specified on the creature will be used
*
* @param * oTarget - The AActor* that will own the conversation
* @param rConversationFile (optional) - The name of a dlg file to be used (*.con)
* @author Jon Thompson
*/
void BeginConversation(AActor* oTarget, int32 rConversationFile = FALSE_);

/**
*   @brief returns if the creature can be deathblowed
*
*   [core_h] Sets the requested combat state on all party members
*
*   @param bCombatState True to set combat state, false to unset
*
*   @author georg
**/
void SetCombatStateParty(int32 bCombatState);

// -----------------------------------------------------------------------------
// @brief Safe Wrapper for DestroyObject
//      Reason: There were accidents where creatures would deplete themselves
//              as Ammo because people were not paying attention....
// @author Georg
// -----------------------------------------------------------------------------
void Safe_Destroy_Object(AActor* aActor, int32 nDelayMs = 0);