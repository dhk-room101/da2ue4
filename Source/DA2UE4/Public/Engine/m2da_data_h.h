#pragma once

#include "m2da_constants_h.h"

class Engine;

const FString SS_DATA_RESTRICT = "bRestricted";

// bitm_base.xls
const FString ITEM_DATA_BASE_ARMORVALUE = "ArmorValue";
const FString ITEM_DATA_BASE_DAMAGE = "BaseDamage";
const FString ITEM_DATA_STRENGTH_MODIFIER = "StrengthModifier";
const FString ITEM_DATA_BASE_AP = "armorpenetration";
const FString ITEM_DATA_BASE_SPEED = "dspeed";
const FString ITEM_DATA_EQUIPPABLE_SLOTS = "equippableslots";
const FString ITEM_DATA_ARMOR_TYPE = "armortype";
const FString ITEM_DATA_RANGE = "range";

// as_data.xls - Autoscaling Data
const FString AS_RANK_SCALE_FACTOR = "fscale";
const FString AS_RANK_HEALTH_SCALE_FACTOR = "fHealthScale";

// apr_base.xls
const FString APPEARANCE_DATA_CREATURE_TYPE = "CREATURE_TYPE";
const FString APPEARANCE_DATA_ONESHOT_KILL = "oneshotkills";
const FString APPEARANCE_DATA_CAN_DO_DEATHBLOWS = "candodeathblows";

/**
* @brief (2da_data) Returns the creature type associated with nAppearanceType
*
*   Returns the creature type classification associated with the given appearance
*   type. This is not the racial type of the creature set on the creature template.
*
* @param    nAppearanceType   The appearance type of the creature.
*
* @returns  Creature type CREATURE_TYPE_*
*
* @author   David Sims
**/
int32 GetCreatureTypeClassification(int32 nAppearanceType);

int32 GetIsSoundSetEntryTypeRestricted(int32 nSSEntry);

float GetItemRange(AActor* oItem);

/**
* @brief (2da_data)  Returns the ability type as a ABILITY_TYPE_* constant
*
* @param nAbility   ability id (row number in ABI_BASE)
*
* @returns ABILITY_TYPE_* constant
*
* @author   Georg Zoeller
**/
int32 GetAbilityType(int32 nAbility);

int32 CanPerformDeathblows(AActor* oCreature);