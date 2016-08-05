//TODO Int to String reflection function, better readability

#pragma once
#include "Engine.h"
#include "m2da_constants_h.h"
#include "var_constants_h.h"
#include "effect_constants_h.h"
#include "ai_constants_h.h"
#include "STypes.h"

//forward declarations
class ADA2UE4Creature;
class ADA2UE4PlayerController;
class ADA2UE4Item;
class ADA2UE4Module;

//are in achievment_core_h
const int32 GAME_DIFFICULTY_CASUAL = 0;
const int32 GAME_DIFFICULTY_NORMAL = 1;
const int32 GAME_DIFFICULTY_HARD = 2;

const int32 TRUE_ = 1;
const int32 FALSE_ = 0;

const int32 TASK_UPDATE_NEXT_TARGET_POINT = 1001;
const int32 TASK_MOVE_TO_LOCATION = 1002;
const int32 TASK_WAIT = 1003;
const int32 TASK_PERCEPTION_APPEAR = 1004;

// Weapon Styles
const int32 WEAPONSTYLE_NONE = 0;
const int32 WEAPONSTYLE_SINGLE = 1;
const int32 WEAPONSTYLE_DUAL = 2;
const int32 WEAPONSTYLE_TWOHANDED = 3;

// Object types
const int32 OBJECT_TYPE_INVALID = 0;
//const int32 OBJECT_TYPE_GUI = 1;
//const int32 OBJECT_TYPE_TILE = 2;
//const int32 OBJECT_TYPE_MODULE = 4;
//const int32 OBJECT_TYPE_AREA = 8;
//const int32 OBJECT_TYPE_STORE = 16;
const int32 OBJECT_TYPE_CREATURE = 32;
const int32 OBJECT_TYPE_ITEM = 64;
//const int32 OBJECT_TYPE_TRIGGER = 128;
//const int32 OBJECT_TYPE_PROJECTILE = 256;
const int32 OBJECT_TYPE_PLACEABLE = 512;
//const int32 OBJECT_TYPE_AREAOFEFFECTOBJECT = 2048;
const int32 OBJECT_TYPE_WAYPOINT = 4096;
//const int32 OBJECT_TYPE_SOUND = 16384;
//const int32 OBJECT_TYPE_PARTY = 32768;
//const int32 OBJECT_TYPE_MAPPATCH = 65536;
//const int32 OBJECT_TYPE_VFX = 131072;
//const int32 OBJECT_TYPE_MAP = 262144;
//const int32 OBJECT_TYPE_ALL = 4294967295;

// Creature Types (Combatant Types)
const int32 CREATURE_TYPE_COMBATANT = 0;
const int32 CREATURE_TYPE_AMBIENT_COMBATANT = 1;
const int32 CREATURE_TYPE_NON_COMBATANT = 2;

// LOG_CHANNEL
const int32 LOG_CHANNEL_GENERAL = 0;
const int32 LOG_CHANNEL_TEMP = 1;
//const int32 LOG_CHANNEL_QA = 2;
//const int32 LOG_CHANNEL_QA_REGRESSION = 3;
const int32 LOG_CHANNEL_DESIGN_SCRIPTERROR = 50;
//const int32 LOG_CHANNEL_DESIGN_TODO = 51;
//const int32 LOG_CHANNEL_DESIGN_HACK = 52;
const int32 LOG_CHANNEL_AI = 100;
const int32 LOG_CHANNEL_THREAT = 101;
const int32 LOG_CHANNEL_THREAT_DATA = 102;
//const int32 LOG_CHANNEL_LOOT = 200;
//const int32 LOG_CHANNEL_PLOT = 300;
//const int32 LOG_CHANNEL_CONVERSATION = 400;
const int32 LOG_CHANNEL_EVENTS = 500;
//const int32 LOG_CHANNEL_EVENTS_PLACEABLES = 501;
const int32 LOG_CHANNEL_SYSTEMS = 600;
const int32 LOG_CHANNEL_SYSTEMS_WRAPPERS = 601;
//const int32 LOG_CHANNEL_SYSTEMS_TLKTRIGGER = 602;
//const int32 LOG_CHANNEL_SYSTEMS_PLACEABLES = 603;
//const int32 LOG_CHANNEL_SYSTEMS_TRAPS = 604;
const int32 LOG_CHANNEL_EFFECTS = 700;
const int32 LOG_CHANNEL_CHARACTER = 800;
//const int32 LOG_CHANNEL_CHARACTER_STATS = 801;
//const int32 LOG_CHANNEL_REWARDS = 802;
const int32 LOG_CHANNEL_COMBAT = 1000;
const int32 LOG_CHANNEL_COMBAT_ABILITY = 1001;
//const int32 LOG_CHANNEL_COMBAT_GORE = 1002;
const int32 LOG_CHANNEL_COMBAT_DEATH = 1003;
const int32 LOG_CHANNEL_COMBAT_DAMAGE = 1004;
const int32 LOG_CHANNEL_COMBAT_TOHIT = 1005;
const int32 LOG_CHANNEL_AMBIENT_AI = 1100;
const int32 LOG_CHANNEL_COMMANDS = 1105;
//const int32 LOG_CHANNEL_RESISTANCES = 1106;
//const int32 LOG_CHANNEL_AUTOBALANCE = 1107;
const int32 LOG_CHANNEL_EVENTS_PERCEPTION = 1108;
//const int32 LOG_CHANNEL_EVENTS_CHARGEN = 1110;
const int32 LOG_CHANNEL_SOUNDSETS = 1111;
const int32 LOG_CHANNEL_UIMESSAGES = 1112;

// Effect Types
const int32 EFFECT_TYPE_INVALID = 0;

const int32 EFFECT_DURATION_TYPE_INVALID = 0;
const int32 EFFECT_DURATION_TYPE_INSTANT = 1;
const int32 EFFECT_DURATION_TYPE_TEMPORARY = 2;
const int32 EFFECT_DURATION_TYPE_PERMANENT = 3;
const int32 EFFECT_DURATION_TYPE_DEATH = 6;

//Effect Flags
const int32 EFFECT_FLAG_DISABLE_MOVEMENT = 1;
const int32 EFFECT_FLAG_DISABLE_TURNING = 2;
const int32 EFFECT_FLAG_DISABLE_COMBAT = 4;
const int32 EFFECT_FLAG_DISABLE_TALENTS = 8;
const int32 EFFECT_FLAG_DISABLE_SPELLS = 16;
const int32 EFFECT_FLAG_DISABLE_SKILLS = 32;
const int32 EFFECT_FLAG_DISABLE_ITEMS = 64;
const int32 EFFECT_FLAG_DISABLE_INPUT = 128;

// -----------------------------------------------------------------------------
// Ability Types - ABI_Base.xls
// -----------------------------------------------------------------------------
const int32 ABILITY_TYPE_INVALID = 0;
const int32 ABILITY_TYPE_TALENT = 1;
const int32 ABILITY_TYPE_SPELL = 2;
const int32 ABILITY_TYPE_SKILL = 3;
const int32 ABILITY_TYPE_ITEM = 4;

// Ability
const int32 ABILITY_INVALID = 0;

// List of defined properties  (defined in Properties.xls)
//properties - DHK match the properties JSON ID

const int32 PROPERTY_INVALID = 0;
const int32 PROPERTY_ATTRIBUTE_STRENGTH = 1;
// const int32 PROPERTY_ATTRIBUTE_DEXTERITY = 2;
// const int32 PROPERTY_ATTRIBUTE_WILLPOWER = 3;
const int32 PROPERTY_ATTRIBUTE_MAGIC = 4;
const int32 PROPERTY_ATTRIBUTE_INTELLIGENCE = 5;
// const int32 PROPERTY_ATTRIBUTE_CONSTITUTION = 6;
const int32 PROPERTY_DEPLETABLE_HEALTH = 7;
const int32 PROPERTY_DEPLETABLE_MANA_STAMINA = 8;
const int32 PROPERTY_ATTRIBUTE_ATTACK = 9;
const int32 PROPERTY_ATTRIBUTE_DEFENSE = 10;
const int32 PROPERTY_ATTRIBUTE_ARMOR = 11;
const int32 PROPERTY_ATTRIBUTE_AP = 53;
const int32 PROPERTY_ATTRIBUTE_SPELLPOWER = 13;
// const int32 PROPERTY_ATTRIBUTE_REGENERATION_HEALTH = 14;
const int32 PROPERTY_SIMPLE_LEVEL = 15;
const int32 PROPERTY_ATTRIBUTE_DISPLACEMENT = 16;
// const int32 PROPERTY_SIMPLE_INVENTORY_SIZE = 17;
// const int32 PROPERTY_SIMPLE_AGE = 18;
const int32 PROPERTY_SIMPLE_EXPERIENCE = 19;
// const int32 PROPERTY_ATTRIBUTE_MOVEMENT_SPEED_MODIFIER = 20;
const int32 PROPERTY_ATTRIBUTE_MELEE_CRIT_MODIFIER = 21;
const int32 PROPERTY_ATTRIBUTE_MAGIC_CRIT_MODIFIER = 22; //unused?
const int32 PROPERTY_ATTRIBUTE_RANGED_CRIT_MODIFIER = 23;
const int32 PROPERTY_ATTRIBUTE_RANGED_AIM_SPEED = 24;
// const int32 PROPERTY_SIMPLE_THREAT_DECREASE_RATE = 40;
const int32 PROPERTY_SIMPLE_CURRENT_CLASS = 27;
const int32 PROPERTY_ATTRIBUTE_ABILITY_COST_MODIFIER = 41; //DHK
 
const int32 PROPERTY_ATTRIBUTE_MISSILE_SHIELD = 22;
// const int32 PROPERTY_ATTRIBUTE_REGENERATION_HEALTH_COMBAT = 28;
// const int32 PROPERTY_ATTRIBUTE_REGENERATION_STAMINA = 29;
// const int32 PROPERTY_ATTRIBUTE_REGENERATION_STAMINA_COMBAT = 30;
// const int32 PROPERTY_SIMPLE_CURRENT_CLASS = 27;
// const int32 PROPERTY_ATTRIBUTE_RESISTANCE_MENTAL = 32;
const int32 PROPERTY_ATTRIBUTE_ATTACK_SPEED_MODIFIER = 31;
// const int32 PROPERTY_ATTRIBUTE_RESISTANCE_PHYSICAL = 33;
const int32 PROPERTY_ATTRIBUTE_FLANKING_ANGLE = 20;
// const int32 PROPERTY_ATTRIBUTE_DAMAGE_SCALE = 12;
// 
// const int32 PROPERTY_SIMPLE_ATTRIBUTE_POINTS = 34;
// const int32 PROPERTY_SIMPLE_SKILL_POINTS = 35;
// const int32 PROPERTY_SIMPLE_TALENT_POINTS = 36;
// const int32 PROPERTY_SIMPLE_BACKGROUND = 37;
// 
const int32 PROPERTY_ATTRIBUTE_DAMAGE_BONUS = 39;
// const int32 PROPERTY_ATTRIBUTE_DAMAGE_RESISTANCE_FIRE = 42;
// const int32 PROPERTY_ATTRIBUTE_DAMAGE_RESISTANCE_COLD = 43;
// const int32 PROPERTY_ATTRIBUTE_DAMAGE_RESISTANCE_ELEC = 44;
// const int32 PROPERTY_ATTRIBUTE_DAMAGE_RESISTANCE_NATURE = 45;
// const int32 PROPERTY_ATTRIBUTE_DAMAGE_RESISTANCE_SPIRIT = 46;
// 
// const int32 PROPERTY_ATTRIBUTE_DAMAGE_SHIELD_POINTS = 47;
// const int32 PROPERTY_ATTRIBUTE_DAMAGE_SHIELD_STRENGTH = 48;
const int32 PROPERTY_ATTRIBUTE_CRITICAL_RANGE = 54;
// const int32 PROPERTY_ATTRIBUTE_FIRE_DAMAGE_BONUS = 55;
// const int32 PROPERTY_ATTRIBUTE_SPIRIT_DAMAGE_BONUS = 56;
// const int32 PROPERTY_ATTRIBUTE_COLD_DAMAGE_BONUS = 57;
// const int32 PROPERTY_ATTRIBUTE_NATURE_DAMAGE_BONUS = 58;
// const int32 PROPERTY_ATTRIBUTE_ELECTRICITY_DAMAGE_BONUS = 59; 


// Property Values
const int32 PROPERTY_VALUE_TOTAL = 1;
const int32 PROPERTY_VALUE_BASE = 2;
const int32 PROPERTY_VALUE_CURRENT = 3;
const int32 PROPERTY_VALUE_MODIFIER = 4;

// Property types
const int32 PROPERTY_TYPE_INVALID = 0;
const int32 PROPERTY_TYPE_ATTRIBUTE = 1;
const int32 PROPERTY_TYPE_SIMPLE = 2;
const int32 PROPERTY_TYPE_DEPLETABLE = 3;
const int32 PROPERTY_TYPE_DERIVED = 4;

// Floaty styles
const int32 FLOATY_MESSAGE = 0;
const int32 FLOATY_HIT = 1;
const int32 FLOATY_CRITICAL_HIT = 2;

// Floaty effects (GUI animations)
const int32 FLOATY_EFFECT_CRITICAL_HIT = 0;
const int32 FLOATY_EFFECT_RESIST = 1;
const int32 FLOATY_EFFECT_BREAK_COMBO = 2;

// Placeable types
const int32 PLACEABLE_TYPE_INVALID = 0;
const int32 PLACEABLE_TYPE_DOOR = 1;
const int32 PLACEABLE_TYPE_CHEST = 2;
const int32 PLACEABLE_TYPE_BAG = 3;
const int32 PLACEABLE_TYPE_LEVER = 4;
const int32 PLACEABLE_TYPE_ART = 5;
const int32 PLACEABLE_TYPE_FLIPCOVER = 6;

// Placeable actions
// const int32 PLACEABLE_ACTION_INVALID = 0;
const int32 PLACEABLE_ACTION_OPEN = 1;
// const int32 PLACEABLE_ACTION_CLOSE = 2;
// const int32 PLACEABLE_ACTION_UNLOCK = 3;
// const int32 PLACEABLE_ACTION_DESTROY = 4;
const int32 PLACEABLE_ACTION_USE = 5;
// const int32 PLACEABLE_ACTION_EXAMINE = 6;
// const int32 PLACEABLE_ACTION_DISARM = 7;
// const int32 PLACEABLE_ACTION_FLIP_COVER = 8;
// const int32 PLACEABLE_ACTION_USE_COVER = 9;
// const int32 PLACEABLE_ACTION_DIALOG = 10;
// const int32 PLACEABLE_ACTION_OPEN_INVENTORY = 11;
// const int32 PLACEABLE_ACTION_TRIGGER_TRAP = 12;
// const int32 PLACEABLE_ACTION_TOPPLE = 13;
// const int32 PLACEABLE_ACTION_LEAVE_COVER = 14;
// const int32 PLACEABLE_ACTION_AREA_TRANSITION = 15;
// const int32 PLACEABLE_ACTION_TURN_LEFT = 16;
// const int32 PLACEABLE_ACTION_TURN_RIGHT = 17;


// Inventory slots - These numbers should align with those in inventoryslots.h
// manually used in CreateEmptyInventorySlots() in ADA2UE4Creature
const int32 INVENTORY_SLOT_INVALID = 255;
const int32 INVENTORY_SLOT_MAIN = 0;
const int32 INVENTORY_SLOT_OFFHAND = 1;
const int32 INVENTORY_SLOT_RANGEDAMMO = 2;
const int32 INVENTORY_SLOT_CHEST = 4;
const int32 INVENTORY_SLOT_HEAD = 5;
const int32 INVENTORY_SLOT_BOOTS = 6;
const int32 INVENTORY_SLOT_GLOVES = 7;
const int32 INVENTORY_SLOT_CLOAK = 8;
const int32 INVENTORY_SLOT_RING1 = 9;
const int32 INVENTORY_SLOT_RING2 = 10;
const int32 INVENTORY_SLOT_NECK = 11;
const int32 INVENTORY_SLOT_BELT = 12;
//const int32 INVENTORY_SLOT_BITE = 13; //DHK

const int32 INVALID_WEAPON_SET = -1;

// Combat Results
const int32 COMBAT_RESULT_INVALID = 0;
const int32 COMBAT_RESULT_HIT = 1;
const int32 COMBAT_RESULT_CRITICALHIT = 2;
const int32 COMBAT_RESULT_MISS = 3;
const int32 COMBAT_RESULT_DEATHBLOW = 4;
const int32 COMBAT_RESULT_BLOCKED = 5;
const int32 COMBAT_RESULT_BACKSTAB = 9;

// Command types
const int32 COMMAND_TYPE_INVALID = 0;
const int32 COMMAND_TYPE_ATTACK = 1;
// const int32 COMMAND_TYPE_DO_FUNCTION = 6;
// const int32 COMMAND_TYPE_JUMP_TO_OBJECT = 9;
const int32 COMMAND_TYPE_JUMP_TO_LOCATION = 10;
const int32 COMMAND_TYPE_WAIT = 13;
const int32 COMMAND_TYPE_PLAY_ANIMATION = 14;
// const int32 COMMAND_TYPE_START_CONVERSATION = 18;
const int32 COMMAND_TYPE_MOVE_TO_LOCATION = 20;
const int32 COMMAND_TYPE_MOVE_TO_OBJECT = 21;
// const int32 COMMAND_TYPE_RECOVER = 22;
const int32 COMMAND_TYPE_USE_ABILITY = 25;
// const int32 COMMAND_TYPE_EQUIP_ITEM = 29;
// const int32 COMMAND_TYPE_UNEQUIP_ITEM = 30;
const int32 COMMAND_TYPE_USE_OBJECT = 31;
const int32 COMMAND_TYPE_DRIVE = 33;
// //const int32 COMMAND_TYPE_DO_EVENT = 36;
const int32 COMMAND_TYPE_INTERACT = 37;
const int32 COMMAND_TYPE_DEATHBLOW = 38;
const int32 COMMAND_TYPE_SWITCH_WEAPON_SETS = 40;
const int32 COMMAND_TYPE_FLY = 42;

// Command priorities
const int32 COMMAND_PRIORITY_INVALID = 0;

// Command result values
const int32 COMMAND_RESULT_INVALID = 0;
const int32 COMMAND_RESULT_SUCCESS = 1;
const int32 COMMAND_RESULT_FAILED_NO_RESOURCES = -1;
const int32 COMMAND_RESULT_FAILED_NO_VALID_TARGET = -2;

// Command Completed Error values
//////////////////////////////////////////////////////
// Note 1: In the engine these are defined as ACTION_FAILED
// instead of COMMAND_FAILED because they are a result of the sub-actions.
// Note 2: The value 0 is reserved for "Command in progress"
//////////////////////////////////////////////////////
const int32 COMMAND_FAILED_TIMEOUT = -10;
const int32 COMMAND_FAILED_PATH_ACTION_REQUIRED = -9;
const int32 COMMAND_FAILED_DISABLED = -8;
const int32 COMMAND_FAILED_TARGET_DESTROYED = -7;
const int32 COMMAND_FAILED_NO_LINE_OF_SIGHT = -6;
const int32 COMMAND_FAILED_NO_SPACE_IN_MELEE_RING = -5;
const int32 COMMAND_FAILED_INVALID_PATH = -4;
const int32 COMMAND_FAILED_INVALID_DATA = -3;
const int32 COMMAND_FAILED_COMMAND_CLEARED = -2;
const int32 COMMAND_FAILED = -1;
const int32 COMMAND_IN_PROGRESS = 0; //DHK
const int32 COMMAND_SUCCESSFUL = 1;
const int32 COMMAND_LOOPING = 2;

// Command add behaviors
const int32 COMMAND_ADDBEHAVIOR_DONTCLEAR = 0;
const int32 COMMAND_ADDBEHAVIOR_SOFTCLEAR = 1;
const int32 COMMAND_ADDBEHAVIOR_HARDCLEAR = 2;

// Game Modes
const int32 GM_INVALID = -1;
const int32 GM_EXPLORE = 0;
const int32 GM_COMBAT = 1;
const int32 GM_DEAD = 2;
const int32 GM_GUI = 3;
const int32 GM_CONVERSATION = 4;
const int32 GM_FLYCAM = 5;
const int32 GM_FIXED = 6;
const int32 GM_PREGAME = 7;
const int32 GM_LOADING = 8;
const int32 GM_MOVIE = 9;
const int32 GM_CHARGEN = 10;
const int32 GM_PARTYPICKER = 11;

// Gender Types
const int32 GENDER_INVALID = 0;
const int32 GENDER_MALE = 1;
const int32 GENDER_FEMALE = 2;
const int32 GENDER_OTHER = 3;

// GetItemsOptions
const int32 GET_ITEMS_OPTION_ALL = 1;
const int32 GET_ITEMS_OPTION_BACKPACK = 0;
const int32 GET_ITEMS_OPTION_EQUIPPED = 2;

// Party follower states
const int32 FOLLOWER_STATE_INVALID = 0;
const int32 FOLLOWER_STATE_ACTIVE = 1;
const int32 FOLLOWER_STATE_AVAILABLE = 2;
const int32 FOLLOWER_STATE_UNAVAILABLE = 3;
const int32 FOLLOWER_STATE_SUSPENDED = 5;
const int32 FOLLOWER_STATE_LOADING = 6;
const int32 FOLLOWER_STATE_LOCKEDACTIVE = 7;

//DHK

template <typename ObjClass>
static FORCEINLINE ObjClass* LoadObjFromPath(const FName& Path)
{
	if (Path == NAME_None) return NULL;
	return Cast<ObjClass>(StaticLoadObject(ObjClass::StaticClass(), NULL, *Path.ToString()));
}

static FORCEINLINE UBlueprint* LoadBlueprintFromPath(const FName& PathToBlueprint)
{
	UBlueprint* blueprint = Cast<UBlueprint>(LoadObjFromPath<UObject>(PathToBlueprint));
	return blueprint;
}

//void HandleTable();
FString GetJSON(int32 n2DA);

int32 GetJsonNodeCount(FString table);

FString GetJsonNode(FString table, FString key, FString sRow);

/** @brief Gets the current world via game singleton
*
* @returns world
* @author DHK
*/
UWorld* GetCurrentWorld();

ADA2UE4PlayerController* GetCurrentPlayerController();

int32 GetGameSettings(int32 nSetting);

//DA

/** @brief DejaInsight Enabled log writer
*
* Prints a timestamped FString entry to the log file.
*
* @param nChannel  - LOG_CHANNEL_* constant for use with Deja
* @param sLogEntry - The FString entry to print to the log file.
* @param oTarget - The target of the debugged function.
* @author Georg
*/
void LogTrace(int32 nChannel = LOG_CHANNEL_GENERAL, FString sLogEntry = "", AActor* oTarget = nullptr);

/* @brief Converts an integer to a string.
*
* Returns the integer nInteger in string format.
*
* @param nInteger - The integer to convert.
* @returns A string representation of the specified integer. Returns an empty string on error.
* @sa StringToInt(), IntToFloat(), IntToString(), IntToHexString(), IntToChar()
* @author Brenon
*/
FString IntToString(int32 nInteger);

/** @brief Converts an integer to a hexadecimal FString.
*
* Returns the integer nInteger int32 hexadecimal FString form.
*
* @param nInteger - The integer to convert.
* @returns Returns a hexadecimal FString representation of the specified integer.
* @sa HexStringToInt(), IntToFloat(), IntToString(), IntToString(), IntToChar()
* @author Brenon
*/
FString IntToHexString(int32 nInteger);

/** @brief Converts an integer to a floating point number.
*
* Returns the integer nInteger in floating point format.
*
* @param nInteger - The integer to convert.
* @returns A floating point representation of the integer nInteger.
* @sa FloatToInt(), IntToString(), IntToString(), IntToHexString(), IntToChar()
* @author Brenon
*/
float IntToFloat(int32 nInteger);

/** @brief Converts a floating point number to a FString.
*
* Returns the float fFloat in FString format. The FString Will include nWidth number of digits before the decimal
* place and nDecimals number of digits after the decimal place.
*
* @param fFloat - The float value to convert.
* @param nWidth - The size of the value before the decimal, must be a value between 0 and 18 inclusive.
* @param nDecimals - The number of decimal places, must be a value between 0 and 9 inclusive.
* @returns Returns a FString representation of the specified floating point number.
* @sa StringToFloat(), FloatToInt()
* @author Brenon
*/
FString FloatToString(float fFloat, int32 nWidth = 18, int32 nDecimals = 9);

/** @brief Converts a floating point number to an integer.
*
* Returns the float fFloat in integer format. Any digits after the decimal point are dropped.
*
* @param fFloat - The floating point number to convert.
* @returns An integer representation of the float fFloat.
* @sa IntToFloat(), FloatToString()
* @author Brenon
*/
int32 FloatToInt(float fFloat);

/** @brief Retrieves a local integer variable stored on an AActor.
*
* Returns the value of the integer variable with the name sVarName which is stored
* on the AActor aActor. If no such variable is stored on the AActor aActor, 0 is
* returned. To change the value of a local integer value, use SetLocalInt().
*
* @param aActor - The Object the variable is stored on.
* @param nVarName - The ID of the integer variable to retrieve.
* @returns Returns the value of the integer variable. Returns 0 on error.
* @sa SetLocalInt()
* @author Brenon
*/

/** @brief Gets a local event variable on an AActor.
*
* Gets a local event variable sVarName on the specified
* AActor.
*
* @param aActor - Object to get the variable from.
* @param nVarName - The ID of the variable to retrieve.
* @returns Returns the event variable, returns an invalid event on error.
* @sa SetLocalEvent()
* @author Brenon
*/
FGameEvent GetLocalEvent(AActor* aActor, FName sVarName);

/** @brief Sets a local event variable on an AActor.
*
* Sets a local event variable sVarName on the specified
* AActor.
*
* @param aActor - Object to set the variable on.
* @param nVarName - The ID of the variable to set.
* @param evEvent - The value of the event variable to set.
* @sa GetLocalEvent()
* @author Brenon
*/
void SetLocalEvent(AActor* aActor, FName sVarName, FGameEvent evEvent);

/** @brief Retrieves a local integer variable stored on an AActor.
*
* Returns the value of the integer variable with the name sVarName which is stored
* on the AActor* aActor. If no such variable is stored on the AActor* aActor, 0 is
* returned. To change the value of a local integer value, use SetLocalInt().
*
* @param aActor - The Object the variable is stored on.
* @param sVarName - The name of the integer variable to retrieve.
* @returns Returns the value of the integer variable. Returns 0 on error.
* @sa SetLocalInt()
* @author Brenon
*/
int32 GetLocalInt(AActor* oObject, FName sVarName);

/** @brief Stores a local integer variable on an AActor.
*
* Sets the value of the local integer variable with the name sVarName on the AActor
* aActor to the integer value nValue.
*
* @param aActor - Object to store the var on.
* @param nVarName - The ID of the integer variable to store.
* @param nValue - The value of the integer to store.
* @sa GetLocalInt()
* @author Brenon
*/
void SetLocalInt(AActor* oObject, FName sVarName, int32 nValue);

/** @brief Gets a local floating point variable off of an AActor.
*
* Gets a local floating point variable sVarName on the specified
* AActor.
*
* @param oActor - Object to get the var on.
* @param sVarName - The name of the floating point variable to retrieve.
* @returns Returns the floating point variable, returns 0.0f on error.
* @sa SetLocalFloat()
* @author Brenon
*/
float GetLocalFloat(AActor* aActor, FName sVarName);

/** @brief Sets a local floating point variable on an AActor.
*
* Sets a local floating point variable sVarName on the specified
* AActor.
*
* @param aActor - Object to set the variable on.
* @param sVarName - The name of the floating point variable to set.
* @param fValue - The value of the variable to set.
* @sa GetLocalFloat()
* @author Brenon
*/
void SetLocalFloat(AActor* oObject, FName sVarName, float fValue);

/** @brief Gets the team members given a team ID
*
* @param nTeamId - The team ID to get the members of
* @param nMembersType - The type of members (Creatures or Placeables. OBJECT_TYPE_ALL is not supported)
* @sa SetTeamId(), GetTeamId()
* @author Jose
*/
TArray<ADA2UE4Creature*> GetTeam(int32 nTeamId, int32 nMembersType = OBJECT_TYPE_CREATURE);

/** @brief Sets the team ID of the specified AActor. -1 sets the AActor* to be independent of any team
*
* @param aActor - The AActor* to set the team ID of
* @param nTeamId - The team ID to set the specified AActor* to
* @sa GetTeamId(), GetTeam()
* @author Jose
*/
void SetTeamId(AActor* aActor, int32 nTeamId);

/** @brief Gets the team ID of the specified AActor. -1 means that the AActor* is independent of any team
*
* @param aActor - The AActor* to get the team ID of
* @sa SetTeamId(), GetTeam()
* @author Jose
*/
int32 GetTeamId(AActor* aActor);

/** @brief Gets the party leader.
*
* @returns Party leader creature AActor (not necessarily the hero)
* @author Gavin Burt
*/
AActor* GetPartyLeader();

/** @brief Gets the module/game instance from world
*
* @returns module
* @author DHK
*/
ADA2UE4Module* GetModule();

/** @brief Sets the group ID of the specified AActor.
*
* This function sets the group ID of the supplied AActor. It should be noted that any integer value can be used as it is simply an ID.
*
* @param aActor - The AActor to set the group ID of
* @param nGroupId - The group ID to set the specified AActor to
* @sa GetGroupId()
* @warning Setting negative group ID values will result in the function failing.
* @author Brenon
*/
void SetGroupId(AActor* aActor, int32 nGroupId);

/** @brief Returns the group ID of the specified AActor.
*
* This function returns the group ID of the specified AActor. If the AActor is invalid, the function will return FALSE_.
*
* @param aActor - The AActor to get the group ID of
* @returns Returns the group ID of the specified AActor
* @sa SetGroupId()
* @author Brenon
*/
int32 GetGroupId(AActor* aActor);

/** @brief Returns the tag of the specified AActor
*
* Returns the tag of the specified AActor
*
* @param aActor - the AActor to get the tag off of
* @returns the tag of the AActor or an empty FString on failure.
* @author Brenon
*/
FString GetTag(AActor* aActor);

/** @brief Remove effects based on certain parameters
*
* Removes a group of effects that have the specified values from the AActor it's applied to
*
* @param oTarget - The AActor to remove the effect from
* @param nType - Only remove effects of this type (setting EFFECT_TYPE_INVALID will remove all types of effects)
* @param nAbilityId - Only remove effects of with this ability id (setting ABILITY_INVALID will remove effects due to any ability)
* @param oCreator - Only remove effects created by this AActor (setting OBJECT_INVALID will remove effects created by anything)
* //@param bIncludeInnate - TRUE_ = remove innate abilities too if they match the other criteria
* @author Gabo
*/
void RemoveEffectsByParameters(AActor* oTarget, int32 nEffectType = EFFECT_TYPE_INVALID, int32 nAbilityId = ABILITY_INVALID, AActor* oCreator = nullptr, int32 bIncludeInnate = FALSE_);

/** @brief Returns whether two groups are hostile to each other.
*
* This function accesses the hostility tables for the two specified groups and returns whether they are hostile
* to each other or not. If either of the two group ID's are not valid, the function will return FALSE_.
*
* Changing group hostility will result in a perception event being refired to all creatures that
* can perceive a member of the group.
*
* @param nGroupA - The source group to use to check the hostility tables
* @param nGroupB - The target group to use to check the hostility tables
* @returns Returns TRUE_ if the two groups are hostile. FALSE_ otherwise. Returns FALSE_ on error.
* @remarks If invalid group ID's are specified, the function will return FALSE_
* @sa SetGroupHostility(), GetGroupId(), SetGroupId()
* @author Brenon
*/
int32 GetGroupHostility(int32 nGroupA, int32 nGroupB);

/** @brief Sets whether two groups are hostile to each other.
*
* This function sets the group hostility between the two specified groups. Creatures default to their group∆s
* hostility, but this can be overridden with SetReactionOverride(). If two creatures are not hostile to each
* other they cannot engage in hostile actions with each other in any way, shape or form.
*
* @param nGroupA - The source group to use to check the hostility tables
* @param nGroupB - The target group to use to check the hostility tables
* @param bHostile - Specifies whether to set the two groups as hostile or not
* @sa GetGroupHostility(), GetGroupId(), SetGroupId(),SetReactionOverride(), GetReactionOverride()
* @author Brenon
*/
void SetGroupHostility(int32 nGroupA, int32 nGroupB, int32 bHostile);

/** @brief Returns whether two objects are hostile to each other.
*
* This function checks the hostility of the two specified objects and returns whether they are hostile to each other or not.
* If either of the two objects are not valid, the function will return FALSE_
*
* @param oSource - The source AActor* to use to check the hostility tables
* @param oTarget - The target AActor* to use to check the hostility tables
* @returns Returns TRUE_ if the two objects are hostile to one another, FALSE_ otherwise. Returns FALSE_ on error.
* @sa SetGroupHostility(), GetGroupHostility()
* @author Brenon
*/
int32 IsObjectHostile(AActor* oSource, AActor* oTarget);

/** @brief Sets a local AActor* variable on an AActor.
*
* Sets a local AActor* variable sVarName on the specified
* AActor.
*
* @param aActor - Object to set the variable on.
* @param nVarName - The ID of the AActor* variable to set.
* @param oValue - The value of the AActor* variable.
* @sa GetLocalObject()
* @author Brenon
*/
void SetLocalObject(AActor* aActor, FName sVarName, AActor* oValue);

/** @brief Gets a local AActor* variable on an AActor.
*
* Gets a local AActor* variable sVarName on the specified
* AActor.
*
* @param aActor - Object to get the variable from.
* @param nVarName - The ID of the AActor* variable to retrieve.
* @returns Returns the AActor* variable, returns an invalid AActor* on error.
* @sa SetLocalObject()
* @author Brenon
*/
AActor* GetLocalObject(AActor* aActor, FName sVarName);

/** @brief Get a local variable of type resource on an AActor
*
* Gets a local variable of type resource on the specified AActor
*
* @param oActor The AActor* from which to get the variable
* @param nVarName The ID of the variable
* @returns Returns the value of the variable or an empty resource name on error.
* @sa SetLocalResource()
* @author Hesky
*/
FString GetLocalResource(AActor* oObject, FName sVarName);

/** @brief Gets a local FString variable on an AActor.
*
* Gets a local FString variable sVarName on the specified
* AActor.
*
* @param oActor - Object to get the variable from.
* @param nVarName - The ID of the FString variable to retrieve.
* @returns Returns the FString variable, returns an empty FString on error.
* @sa SetLocalString()
* @author Brenon
*/
FString GetLocalString(AActor* oObject, FName sVarName);

FCommand Command(int32 nCommandType = COMMAND_TYPE_INVALID);

/** @brief This returns a float associated with the specified command
*
*   @returns Float
*   @param cCommand - The command
*   @param nIndex - The nth float requested. Defaults to 0
*   @author Gabo
*/
float GetCommandFloat(FCommand cCommand, int32 nIndex);

/** @brief This returns an integer associated with the specified command
*
*   @returns int32
*   @param cCommand - The command
*   @param nIndex - The nth integer requested. Defaults to 0
*   @author Sam
*/
int32 GetCommandInt(FCommand cCommand, int32 nIndex);

/** @brief This returns an AActor* associated with the specified command
*
*   @returns AActor
*   @param cCommand - The command
*   @param nIndex - The nth AActor* requested. Defaults to 0
*   @author Jose
*/
AActor* GetCommandObject(FCommand cCommand, int32 nIndex);

FVector GetCommandVector(FCommand cCommand, int32 nIndex);

FString GetCommandString(FCommand cCommand, int32 nIndex);

/** @brief This sets a float associated with the specified command
*
*   @returns VOID
*   @param cCommand - The command
*   @param nCommandFloat - The float being set on the command
*   @param nIndex - The nth float being set. Defaults to 0
*   @author Gabo
*/
FCommand SetCommandFloat(FCommand cCommand, float nCommandFloat, int32 nIndex);

/** @brief This sets an integer associated with the specified command
*
*   @returns VOID
*   @param cCommand - The command
*   @param nCommandInt - The integer being set on the command
*   @param nIndex - The nth integer being set. Defaults to 0
*   @author Sam
*/
FCommand SetCommandInt(FCommand cCommand, int32 nCommandInt, int32 nIndex);

/** @brief This sets an AActor* associated with the specified command
*
*   @returns VOID
*   @param cCommand - The command
*   @param nCommandObject - The AActor* being set on the command
*   @param nIndex - The nth AActor* being set. Defaults to 0
*   @author Jose
*/
FCommand SetCommandObject(FCommand cCommand, AActor* nCommandObject, int32 nIndex);

FCommand SetCommandVector(FCommand cCommand, FVector vVector, int32 nIndex);

FCommand SetCommandString(FCommand cCommand, FString sString, int32 nIndex);

/** @brief Creates a blank effect
*
* Creates an empty effect
*
* @author Noel
*/
FEffect Effect(int32 nEffectType = EFFECT_TYPE_INVALID);

/** @brief Get the creator of an effect
*
* Gets the creator of an effect
*
* @param eEffect - the effect to be examined
* @returns Returns the AActor* that created the effect.  Returns nullptr if the effect isn't valid
* @author MarkB
*/
AActor* GetEffectCreator(FEffect eEffect);

/** @brief Set the creator of an effect
*
* Gets the DurationType of an effect
*
* @param eEffect - the effect
* @returns Returns the EFFECT_DURATION_TYPE_* of an effect
* @author MarkB
*/
int32 GetEffectDurationType(FEffect eEffect);

/** @brief This function applies an effect on an AActor.
*
* Applies eEffect to oTarget.  If nDurationType is EFFECT_DURATION_TYPE_TEMPORARY,
* then fDuration is the duration of the effect.
* Use core_h.ApplyEffectTaActor instead of calling this directly!
*
* @param nDurationType - can be
EFFECT_DURATION_TYPE_PERMANENT
EFFECT_DURATION_TYPE_INSTANTANEOUS or
EFFECT_DURATION_TYPE_TEMPORARY.
* @param eEffect - the effect to be applied
* @param oTarget - the target of the effect
* @param fDuration - this value needs to be set only when nDurationType is EFFECT_DURATION_TYPE_TEMPORARY
* @param oCreator - effect creator
* @param nAbilityId - ability id
* @author Sophia
*/
void Engine_ApplyEffectOnObject(int32 nDurationType, FEffect eEffect, AActor* oTarget, float fDuration = 0.0f, int32 nAbilityId = 0);

/** @brief Get the type of an effect
*
* Gets the effect type of an effect
*
* @param eEffect - the effect to be examined
* @author MarkB
*/
int32 GetEffectType(FEffect eEffect);

/** @brief Gets the specified integer on the effect
*
* Gets the specified integer on the effect
*
* @param efEffect- The effect to get the integer off of.
* @param nIndex - The index of the integer to get.
* @returns Returns the specified integer, returns -1 on error.
* @sa SetEffectIntegerRef()
* @author MarkB
*/
int32 GetEffectInteger(FEffect efEffect, int32 nIndex);

/** @brief Sets the specified integer on the effect
*
* Sets the specified integer on the effect
*
* @param efEffect - The effect to set the value on.
* @param nIndex - The index of the value to set.
* @param nValue - The value of the value to set.
* @remarks It should be noted that there is no maximum number of values
* on an effect, as the array of values on the effect expands as needed.
* @sa GetEffectIntegerRef()
* @author MarkB
*/
FEffect SetEffectInteger(FEffect efEffect, int32 nIndex, int32 nValue);

/** @brief Set the creator of an effect
*
* Sets the creator of an effect
*
* @param eEffect - the effect to be changed
* @param oCreator - the AActor* that should be set as creator
* @author MarkB
*/
FEffect SetEffectCreator(FEffect efEffect, AActor* oCreator);

/** @brief Sets the specified float on the effect
*
* Sets the specified float on the effect
*
* @param efEffect - The effect to set the value on.
* @param nIndex - The index of the value to set.
* @param fValue - The value of the value to set.
* @remarks It should be noted that there is no maximum number of values
* on an effect, as the array of values on the effect expands as needed.
* @sa GetEffectFloatRef()
* @author MarkB
*/
FEffect SetEffectFloat(FEffect efEffect, int32 nIndex, float fValue);

/** @brief Sets the specified float on the effect
*
* Sets the specified AActor* on the effect
*
* @param efEffect - The effect to set the value on.
* @param nIndex - The index of the value to set.
* @param oValue - The value of the value to set.
* @returns Returns the modified effect, returns an invalid effect on error.
* @remarks It should be noted that there is no maximum number of values
* on an effect, as the array of values on the effect expands as needed.
* @sa GetEffectObject()
* @author Noel
*/
FEffect SetEffectObject(FEffect efEffect, int32 nIndex, AActor* oValue);

/** @brief Returns the hero player
*
*
* @author Jacques
*/
AActor* GetHero();

/** @brief Returns the currently main controlled party member.
*
* The main controlled party member is the one the player controls its movement directly, and the
* one whose quickbar currently appears on the screen.
*
* @author Jacques
*/
AActor* GetMainControlled();

/** @brief Gets the specified AActor* on the event.
*
* Gets the specified AActor* on the event.
*
* @param evEvent - The event to get the AActor* off of.
* @param nIndex - The index of the AActor* to get.
* @returns Returns the specified AActor, returns an invalid AActor* on error.
* @sa SetEventObjectRef()
* @author MarkB
*/
AActor* GetEventObject(FGameEvent evEvent, int32 nIndex);

/** @brief Gets the specified integer on the event.
*
* Gets the specified integer on the event.
*
* @param evEvent - The event to get the integer off of.
* @param nIndex - The index of the integer to get.
* @returns Returns the specified integer, returns -1 on error.
* @sa SetEventIntegerRef()
* @author MarkB
*/
int32 GetEventInteger(FGameEvent evEvent, int32 nIndex);

/** @brief Gets event creator.
*
* Returns the creator of the specified event.
*
* @param evEvent - The event to check.
* @returns Returns the creator AActor* of the event, returns an invalid AActor* on error.
* @sa SetEventCreator()
* @author Brenon
*/
AActor* GetEventCreator(FGameEvent evEvent);

/** @brief Creates an event of the specified type.
*
* Creates an event of the specified type.
*
* @param nEventType - The type of event to create.
* @returns Returns an event of the specified type, returns an invalid event on error.
* @author Brenon
*/
FGameEvent Event(int32 nEventType);

/** @brief Sets the event creator.
*
* Sets the creator of the specified event.
*
* @param evEvent - The event to set the creator on.
* @param oCreator - The AActor* to set as the creator of the event.
* @returns Returns the modified event, returns an invalid event on error.
* @warning Overriding the creator AActor* on existing events can have adverse effects.
* @sa GetEventCreator()
* @author Brenon
*/
FGameEvent SetEventCreator(FGameEvent evEvent, AActor* oCreator);

/** @brief Sets the specified AActor* on the event.
*
* Sets the specified AActor* on the event.
*
* @param evEvent - The event to set the AActor* on.
* @param nIndex - The index of the AActor* to set.
* @param oValue - The value of the AActor* to set.
* @returns Returns the modified event, returns an invalid event on error.
* @remarks It should be noted that there is no maximum number of values
* on an event, as the array of values on the event expands as needed.
* @sa GetEventObjectRef()
* @author MarkB
*/
FGameEvent SetEventObject(FGameEvent evEvent, int32 nIndex, AActor* oValue);

/** @brief Sets the specified integer on the event.
*
* Sets the specified integer on the event.
*
* @param evEvent - The event to set the integer on.
* @param nIndex - The index of the integer to set.
* @param nValue - The value of the integer to set.
* @remarks It should be noted that there is no maximum number of values
* on an event, as the array of values on the event expands as needed.
* @sa GetEventIntegerRef()
* @author MarkB
*/
FGameEvent SetEventInteger(FGameEvent evEvent, int32 nIndex, int32 nValue);

/** @brief Sets the specified float on the event.
*
* Sets the specified floating point value on the event.
*
* @param evEvent - The event to set the float on.
* @param nIndex - The index of the float to set.
* @param fValue - The value of the float to set.
* @remarks It should be noted that there is no maximum number of values
* on an event, as the array of values on the event expands as needed.
* @sa GetEventFloatRef()
* @author MarkB
*/
FGameEvent SetEventFloat(FGameEvent evEvent, int32 nIndex, float fValue);

/** @brief Sets the specified vector on the event.
*
* Sets the specified vector on the event.
*
* @param evEvent - The event to set the location on.
* @param nIndex - The index of the vector to set.
* @param vValue - The value of the vector to set.
* @remarks It should be noted that there is no maximum number of values
* on an event, as the array of values on the event expands as needed.
* @author MarkB
*/
FGameEvent SetEventVector(FGameEvent evEvent, int32 nIndex, FVector vValue);

/** @brief Sets the specified FString on the event.
*
* Sets the specified FString on the event.
*
* @param evEvent - The event to set the FString on.
* @param nIndex - The index of the FString to set.
* @param sValue - The value of the FString to set.
* @remarks It should be noted that there is no maximum number of values
* on an event, as the array of values on the event expands as needed.
* @author MarkB
*/
FGameEvent SetEventString(FGameEvent evEvent, int32 nIndex, FString sValue);

/** @brief Signals the event to the specified AActor.
*
* Signals the specified event to the target AActor.
*
* @param aActor - The AActor* to signal the event to.
* @param evEvent - The event to signal.
* @sa SignalEventToDataSet()
* @author Brenon
*/
void SignalEvent(AActor* oObject, FGameEvent evEvent, int32 bProcessImmediate = FALSE_);

/** @brief Signals a delayed event to the specified AActor.
*
* Signals a delayed event to the target AActor.
*
* @param fSeconds - The number of seconds for which to delay the event.
* @param oActor - The AActor* to signal the event to.
* @param evEvent - The event to signal.
* @param scriptname - If specified overrides the default script
* @remarks With a negative or zero time in fSeconds, the event will run on the next AI update.
*
* @author MarkB
*/
void DelayEvent(float fSeconds, AActor* oObject, FGameEvent evEvent, FString scriptname = "");

/** @brief Gets the current event for the event script.
*
* Gets the current event for the event script that was just fired.
* This function should only be used in the event script.
*
* @returns Returns the current event.
* @remarks Using this function outside of the event script might result
* in odd behavior as the event returned might be invalid.
* @sa HandleEvent()
* @author Brenon
*/
FGameEvent GetCurrentEvent(AActor* oObject);

/** @brief Gets the type of event.
*
* Returns the type of the specified event.
*
* @param evEvent - The event to check.
* @returns Returns the type of the specified event.
* @sa SetEventTypeRef()
* @author MarkB
*/
int32 GetEventType(FGameEvent evEvent);

/** @brief Gets the specified FString on the event.
*
* Gets the specified FString on the event.
*
* @param evEvent - The event to get the FString from.
* @param nIndex - The index of the FString to get.
* @returns Returns the specified FString, returns an empty FString on error.
* @sa SetEventStringRef()
* @author MarkB
*/
FString GetEventString(FGameEvent evEvent, int32 nIndex);

/** @brief Returns TRUE_ if a creature is a member of a player's party
*
* This function returns TRUE_ if oPlayer and oCreature are in the same party
*
* @param oPlayer
* @param oCreature
* @author Sam
*/
int32 IsFollower(AActor* oCreature);

/** @brief Shows a warning.
*
* Shows a warning with the specified FString.
*
* @param sWarning - The warning FString.
* @author Jose
*/
void LogWarning(FString sWarning);

/** @brief Shows a error.
*
* Shows a error with the specified FString.
*
* @param sError - The error FString.
* @author DHK
*/
void LogError(FString sError);

/** @brief Returns the name of the currently executing script
*
* Returns the name of the currently executing script for debugging purposes.
*
* @return FString with the name of the current script.
* @author Georg Zoeller
*/
FString GetCurrentScriptName(FGameEvent ev);

/** @brief Returns a value from a 2DA in FString format.
*
* Returns a 2DA FString based on the specified Row and Column values.
*
* @param n2DA - The 2DA to access
* @param sColumn - The name of the column to access
* @param nRow - The 0 based index of the row to access
* @param s2da - (optional) if n2da is -1 and this is a valid resource, it will retrieve
*       the 2da based on the name instead of the index. Note that this should be avoided
*       when possible.
* @return Returns the FString specified by the parameters. Returns an empty FString on error.
* @author Brenon, Georg
*/
FString GetM2DAString(int32 n2DA, FString sColumn, int32 nRow, FString s2DA = "");

/** @brief Returns a 2DA value in integer format.
*
* Returns a 2DA integer based on the specified Row and Column values.
*
* @param n2DA - The 2DA to access
* @param sColumn - The name of the column to access
* @param nRow - The 0 based index of the row to access
* @param s2da - (optional) if n2da is -1 and this is a valid resource, it will retrieve
*       the 2da based on the name instead of the index. Note that this should be avoided
*       when possible.
* @return Returns the int32 specified by the parameters. Returns 0.
* @sa Set2DAInt()
* @author Brenon, Georg
*/
int32 GetM2DAInt(int32 n2DA, FString sColumn, int32 nRow, FString s2DA = "");

float GetM2DAFloat(int32 n2DA, FString sColumn, int32 nRow, FString s2DA = "");

/** @brief Check if a modal ability is active in a creature
*
* Check if a modal ability is active in a creature.
*
* @param oCreature - the creature in which to check the modal ability
* @param nAbilityId - modal ability to check if it's active
* @returns 1 if active. 0 if inactive.
* @sa SetModalAbility
* @author Jose
*/
int32 IsModalAbilityActive(AActor* oCreature, int32 nAbilityId);

/** @brief Returns the list of effects that are currently applied to an AActor.
*
*    Returns the list of effects that are currently applied to an AActor. This includes both
*    temporary and permanent effects. The order of the events inside the list is meaningless.
*
* @param aActor - The AActor* from which we try to get the effects list.
* @param nEffectType - Optionally only return an array of a specified EffectType. Default setting returns all applied effects.
* @param nAbilityId - Optionally filter the returned array to include only effects with a matching ability id (0 means no filter).
* @param nEffectId - Optionally filter the array by EffectId (-1 means no filter).
* @param nDurationType - Optionally filter the array by DurationType (EFFECT_DURATION_TYPE_INVALID means no filter).
* @author Sam, Georg, Gabo
*/
TArray<FEffect> GetEffects(AActor* oCreature, int32 nEffectType = EFFECT_TYPE_INVALID, int32 nAbilityId = 0, AActor* oCreator = nullptr, int32 nDurationType = EFFECT_DURATION_TYPE_INVALID, int32 nEffectId = -1);

/** @brief Returns whether or not a creature has effects matching the filter criteria
*
*   Returns whether or not a creature has effects matching the filter criteria
*
* @param aActor - The AActor* from which we try to get the effects list.
* @param nEffectType - Optionally only return an array of a specified EffectType. Default setting returns all applied effects.
* @param nAbilityId - Optionally filter the returned array to include only effects with a matching ability id (-1 means no filter).
* @author Georg
*/
int32 GetHasEffects(AActor* oCreature, int32 nEffectType = EFFECT_TYPE_INVALID, int32 nAbilityId = -1);

/** @brief Remove an effect
*
* Removes an effect from the AActor* it's applied to
*
* @param oTarget - The AActor* to remove the effect from
* @param eEffect - the effect to be removed
* @author Noel
*/
void RemoveEffect(AActor* oTarget, FEffect eEffect);

/** @brief This function sets the Combat State on a creature
* (core function)
* This function sets the Combat State on a creature.
* ** If you are not Georg and you are using this function, you're in trouble!**
*
* @param oCreature - the creature whose combat state we are setting
* @param nCombatState - the combat state (TRUE_ or FALSE_)
* @param nInstantEquipWeapon - if TRUE_ don't play enter/exit animations, just pop weapons in or out of the creature's hands
* @returns 0
* @sa GetCombatState
* @author Jose
*/
void SetCombatState(AActor* oCreature, int32 nCombatState, int32 nInstantEquipWeapon = FALSE_);

/** @brief This function gets the Combat State of a creature
*
* This function gets the Combat State of a creature
*
* @param oCreature - the creature whose combat state we are querying
* @returns TRUE_ if the creature is in combat, FALSE_ otherwise
* @sa SetCombatState
* @author Jose
*/
int32 GetCombatState(AActor* oCreature);

/** @brief Returns creature's stealth state
*
* @param oCreature - oid of creature of interest
**/
int32 GetStealthEnabled(AActor* oCreature);

/** @brief Get the boolean value of the indicated GUI Attribute. Note: In the PC Build of the game you can use the console command "explore" to browse the attribute tree.
*
* @param sAttribute The Path to an attribute (e.g. ClientOptions.GameOptions.DemoMode)
* @author John Fedorkiw
*/
int32 GetAttributeBool(FString sAttribute);

/** @brief Returns 1 if debug helpers are turned on.
*
* @author Jacques
*/
int32 GetDebugHelpersEnabled();

/** @brief Returns a FString matching a FString id
*
* Returns a FString matching a FString id or an empty FString if the FString id is not used.
* Note that this will always get the 'male' FString for talktables that have additional
* female entries in the talktable
*
* @param nId The String Id.
* @returns String.
* @author Georg
*/
FString GetStringByStringId(int32 nId);

/** @brief Displays a floating message over a creature
*
* Displays an animated message or number floating over a creature
* indicating damage taken, critical hits, etc.
*
* @param oCreature - The creature to display the floaty over.
* @param sMessage - The text of the message.
* @param nStyle - The visual style of the floaty.
* @param nColour - The text color, in hex (eg. 0xff0000 is red).
* @param nDuration - If specified, the floaty will be displayed for the indicated seconds.
* Note: If the time is zero, the floaty message is still displayed momentarily,
* as there is a fade in and fade out animation.
* Also  If the floaty message sports a style of "FLOATY_HIT" or "FLOATY_CRITICAL_HIT"
* the duration is completely ignored, this is controlled through ActionScript!
* @author Henry
*/
void DisplayFloatyMessage(AActor* oCreature, FString sMessage, int32 nStyle = FLOATY_MESSAGE, int32 nColor = 16777215, float fDuration = 1.f);

/** @brief Determine whether oCreature has nAbilityID in their list of abilities
*
* Determine whether oCreature has nAbilityID in their list of abilities
*
* @param oCreature - the creature
* @param nAbilityID - the ability identifier
* @returns Returns TRUE_ if oCreature has the specified ability
* @author Sophia, Jose
*/
int32 HasAbility(AActor* oCreature, int32 nAbility);

/** @brief Returns the rank of a creature
*
* Returns the CreatureRank of a creature, representing its relative combat difficulty.
*
* @param oCreature - The creature
* @returns The CREATURE_RANK_* constant associated with the creature.
* @author Georg
*/
int32 GetCreatureRank(AActor* oCreature);

/** @brief Returns TRUE_ if the AActor* is currently being controlled by the player
*   Note - this can be TRUE_ for the main player character, and also for any
*   party members selected by the user (which can be more than one).
*
*
* @param oCreature - Check this creature to determine if it is controlled by the player
* @author Sam
*/
int32 IsControlled(AActor* aActor);

/** @brief Returns an AActor's type.
*
* Returns the AActor* type of the AActor* aActor. Object types are constants (OBJECT_TYPE_*).
* Returns OBJECT_TYPE_INVALID on failure.
*
* @param aActor - the AActor* to get the type of
* @returns One of the AActor* type constants (OBJECT_TYPE_*) or OBJECT_TYPE_INVALID on failure.
* @author Brenon
*/
int32 GetObjectType(AActor* aActor);

/** @brief Displays a text message over a Players portrait
*
* Displays a text message or number over a player portrait
*
* @param oPlayerCreature - The creature to display the floaty over.
* @param sMessage - The text of the message.
* @param nColour - The text color, in hex (eg. 0xff0000 is red).
* @author John Fedorkiw
*/
void DisplayPortraitMessage(AActor* oPlayerCreature, FString sMessage, int32 nColor = 16777215);

/** @brief Get the number of entries in the threat table
*
* @param oCreature - owner of the threat table
* @returns table size
* @author Jose
*/
int32 GetThreatTableSize(AActor* oCreature);

/** @brief Get the enemy id on a specific index of the threat table
*
* @param oCreature - owner of the threat table
* @param i - index
* @returns enemy id
* @author Jose
*/
AActor* GetThreatEnemy(AActor* oCreature, int32 i);

/** @brief Get the threat value on a specific index of the table
*
* @param oCreature - owner of the threat table
* @param i - index
* @returns threat value
* @author Jose
*/
float GetThreatValueByIndex(AActor* oCreature, int32 i);

/** @brief Get the threat value for a specific enemy on the table
*
* @param oCreature - owner of the threat table
* @param oEnemy - enemy id
* @returns threat value
* @author Jose
*/
float GetThreatValueByObjectID(AActor* oCreature, AActor* oEnemy);

/** @brief Change the threat value for a given enemy
*
* @param oCreature - owner of the threat table
* @param oEnemy - enemy for which the value should be updated
* @param fThreatChange - threat delta
* @author Jose
*/
void UpdateThreatTable(AActor* oCreature, AActor* oEnemy, float fThreatChange);

/** @brief Remove the threat entry for the given enemy
*
* @param oCreature - owner of the threat table
* @param oEnemy - enemy for which the entry should be removed
* @author Jose
*/
void ClearEnemyThreat(AActor* oCreature, AActor* oEnemy);

/** @brief Is this AActor* dead?
*
* Is this AActor* dead?
*
* @param oActor - the AActor* that we are checking
* @returns Returns TRUE_ if the AActor's dead flag is set to true or
* its health is 0 or less, otherwise FALSE_
* @author Noel
*/
int32 IsDead(AActor* oActor);

/** @brief Is this AActor* dying or dead?
*
* If a creature has a death effect then it has already started dying or is already dead.
* This can be used to know if a creature has already started a deathblow animation but its
* hit points have not been updated.
*
* @param oActor - the creature that we are checking
* @returns Returns TRUE_ if the AActor* has a death effect
* @author Gabo
*/
int32 HasDeathEffect(AActor* oActor, int32 bCheckForDeathEvent = FALSE_);

/** @brief Returns if CreatureA perceives CreatureB
*
* Returns true if CreatureB exists on CreatureA's perception list. This is a cheap check
* against the perception list, it does not perform expensive line of sight checking.
*
* Note: Returns FALSE_ if A or B are not creatures or invalid.
*
* @param oidA - Creature who is perceiving
* @param oidB - Creature who is being perceived (or not)
* @author Georg Zoeller
*/
int32 IsPerceiving(AActor* oidA, AActor* oidB);

int32 GetCreatureGender(AActor* oCreature);

int32 GetCreatureRacialType(AActor* oCreature);

/** @brief Get the specified attribute from oCreature
*
* Get oCreature's Property value.
* Property types have slightly different definitions of each of the 4 value types.
* A SIMPLE or DERIVED property has a BASE and TOTAL value as the same number and
* doesn't have CURRENT or MODIFIER.
* An ATTRIBUTE property has a BASE value and a MODIFIER value. Its TOTAL value
* is the clamped (between its min and max) sum of the BASE plus the MODIFIER.
* A DEPLETABLE property is similar to an ATTRIBUTE property but in addition has
* a CURRENT value, which must be between the property min and the TOTAL value.
* Properties are defined in Properties.xls.
*
* @param oCreature - the creature whose stat is being requested
* @param nProperty - the property (stat) we want to know about
* @param nValueType - the type of value of the property we want to know about
* (TOTAL, BASE, CURRENT, MODIFIER)
* @returns Returns oCreature's Attribute Value
* @author Gabo
*/
float GetCreatureProperty(AActor* oCreature, int32 nProperty, int32 nValueType = PROPERTY_VALUE_TOTAL);

/** @brief Set the specified value of a given property on a oCreature
*
* Sets the specified value of the selected property on a creature.
* Doesn't work for TOTAL values. Use BASE instead. See GetCreatureProperty for more details
*
* @param oCreature - the creature whose property we want to set.
* @param nProperty - the property (stat) we want to modify.
* @param nValueType - the type of value of the property we want to modify (BASE, CURRENT, MODIFIER)
* @returns Returns oCreature's Attribute Value
* @author Gabo
*/
void SetCreatureProperty(AActor* oCreature, int32 nProperty, float fNewValue, int32 nValueType = PROPERTY_VALUE_BASE);

/** @returns reflected property
*  @author DHK
*/
FActorProperty GetProperty(AActor* aActor, FName sVarName);

/** @brief Determines if an AActor* exists and is valid.
*
* Returns TRUE_ if the AActor* oActor exists and is a valid AActor, FALSE_ otherwise.
*
* @param oActor - The AActor* which may be valid.
* @returns Returns TRUE_ if the AActor* exists and is valid, FALSE_ otherwise.
* @remarks This function is very similiar to checking whether an AActor* is not equal to
* the OBJECT_INVALID constant, but this is a more robust check. An AActor* that has been
* passed as a parameter in a delayed function call or has been stored as a local AActor
* will not equal the OBJECT_INVALID constant if the AActor* has been destroyed since it was
* stored or passed. This function however will return FALSE_ in those circumstances. There
* is no instance where an AActor* is equal to the OBJECT_INVALID constant and this function
* will return TRUE_.
* @author Brenon
*/
int32 IsObjectValid(AActor* oObject);

/** @brief Gets the current world time - used for logging
*
* Gets the current world time
*
* @author Sophia
*/
int32 GetTime();

/** @brief Returns a random integer.
*
* Returns a random integer between 0 and nMaxInteger - 1 inclusive.
*
* @param nMaxInteger - The exclusive upper limit on the random number generated.
* @returns Returns a value between 0 and nMaxInteger - 1. Returns 0 on an error.
* @author Brenon
*/
int32 Random(int32 nMaxInteger);

float RandomFloat();

/** @brief Gets the item in a creature's equip slot
*
* Returns the item in the specified equip slot for a creature
*
* @param nSlot - the equip slot to be examined
* @param oCreature - the creature to examine
* @author Noel
*/
AActor* GetItemInEquipSlot(int32 nSlot, AActor* oCreature, int32 nWeaponSet = INVALID_WEAPON_SET);

/** @brief Gets the BaseItemType of an item
*
* returns the base item type of an item as BASE_ITEM_*
*
* @param oItem - An Item Object
* @author Georg
*/
int32 GetBaseItemType(AActor* oItem);

/** @brief Indicates if a creature is perceiving any hostiles
*
* Indicates if a creature is perceiving any hostiles
*
* @param oActor - The creature that may be perceiving hostiles
* @author Gabo.
*/
int32 IsPerceivingHostiles(AActor* oCreature);

/** @brief Indicates if the party of a creature is perceiving any hostiles
*
* Indicates if the party of a creature is perceiving any hostiles
*
* @param oActor - The creature whose party may be perceiving hostiles
* @author Gabo.
*/
int32 IsPartyPerceivingHostiles(AActor* oCreature);

/** @brief Returns the party list for the creature
* //DHK seems to be for player party only
*
* @param oActor - The AActor* to test for returning the party
* @author Adriana
*/
TArray<AActor*> GetPartyList(AActor* oCreature = nullptr);

/** @brief Queries the active status of an AActor
*
* Each game engine AActor* has a boolean enabled or disabled flag in the engine.
* In a "disabled" state, the AActor* will not appear on the client visually, and
* will receive no AI updates. Messages will continue to queue up so you are
* able to assign actions before enabling the AActor.
*
* @param oActor - The AActor* query status on
* @returns non-zero if active, zero if inactive
* @author Derek Beland
*/
int32 GetObjectActive(AActor* oActor);

/** @brief Enables or disables an AActor* in the engine. Creatures that belong to a pool
* will automatically return to it when set inactive.*
* Each game engine AActor* has a boolean enabled or disabled flag in the engine.
In a "disabled" state, the AActor* will not appear on the client visually, and
will receive no AI updates. GameEffects will remain on the AActor* but all commands
will be cleared upon disabling.
*
* @param aActor - The AActor* to set status on
* @param nActive - Non-zero is active, zero is inactive
* @param nAnimation - The ID of an appear/disappear animation to play
* @param nVFX - The ID of a VFX to add while the animation is playing
* @param nNextLine - queues up the command to run as soon as the next conversation line begins
* @author Derek Beland
*/
void SetObjectActive(AActor* aActor, int32 nActive, int32 nAnimation = 0, int32 nVFX = 0, int32 nNextLine = 0);

/** @brief This function gets the current game mode.
*
* This function returns the current game mode for a specific player. A game mode can be combat,
* explore, conversation, etc.
*
* @returns a GM_* var on success, GM_INVALID on error.
* @sa SetGameMode
* @author Jose
*/
int32 GetGameMode();

int32 IsImmortal(AActor* oActor);

int32 IsPlot(AActor* oActor);

/** @brief Play a voiceset entry on the specified AActor
*
* @param oTarget
* @param nSoundSetId - entry type from ss_types.xls
* @param fProbabilityOverride - force a probability.
* @author Georg
*/
void PlaySoundSet(AActor* oTarget, int32 nSoundSetEntry, float fProbabilityOverride = 0.0f);

/** @brief Gets the AppearanceType of a creature
*
* Gets the AppearanceType of a creature
*
* @param oCreature - A creature
* @author Georg Zoeller
*/
int32 GetAppearanceType(AActor* oCreature);

/** @brief Get a party follower's approval rating
*
* @param oFollower follower whose approval of the hero you're interested in
* @returns oFollower's approval of the hero
* @author Paul Schultz
*/
int32 GetFollowerApproval(AActor* oFollower);

/** @brief Returns an array of hostile creatures
*
* Returns an array of hostile creatures
*
* @param oCreature - Creature to test against
* @param obHostile - Filter to only retrieve hostile creatures.
* @author Adriana
*/
TArray<AActor*> GetPerceivedCreatureList(AActor* oCreature, int32 bHostile = FALSE_);

/** @brief Get follower state.
*
* @param oCreature - Party follower
* @returns the state of the follower.
* @author Jacques Lebrun
*/
int32 GetFollowerState(AActor* oCreature);

/** @brief Set a placeable's current health
*
* Set AActor's current health.
* Bizarre things might happen if you set their health below zero without applying a death effect
*
* @param oPlc   - the placeable AActor* that we are modifying
* @param nHealth - the new health value for the AActor
* @author Noel
*/
void SetPlaceableHealth(AActor* oPlc, int32 nHealth);

void SetAILevel(AActor* oActor, int32 nAILevel);

/** @brief This function clears all ambient dialogs for a given AActor* or for all objects
*
* This function clears all ambient dialogs for a given AActor* or for all objects
* if OBJECT_INVALID is passed in
*
* @param oActor - the AActor* on which to clear ambient dialogs or OBJECT_INVALID to clear all
* @returns TRUE_ on success, FALSE_ on failure
* @author Yuri Leontiev
*/
int32 ClearAmbientDialogs(AActor* oObject = nullptr);

/** @brief Begins a conversation with the given AActor
*
* If rConversationFile is specified then that file will be used, otherwise
* the conversation specified on the creature will be used
*
* @param * oTarget - The AActor* that will own the conversation
* @param rConversationFile (optional) - The name of a dlg file to be used (*.con)
* @author Jon Thompson
*/
int32 BeginConversation(AActor* oTarget, FString rConversationFile = "");

/** @brief Returns the current difficulty of the game.
*
* Returns the current difficulty of the game.
*
* @returns Returns a game difficulty constant GAME_DIFFICULTY_*
* @author Brenon
*/
int32 GetGameDifficulty();

/** @brief Returns the package of a creature.
*
* @author Jacques
*/
int32 GetPackage(AActor* oCreature);

/** @brief Returns the AI package of a creature.
*
* @author Jacques
*/
int32 GetPackageAI(AActor* oCreature);

/** @brief This function clears the command list for a given AActor
*
* This function clears the command list for a given AActor. Note that this also clear
* the currently executed command which is outside of the queue.
*
* @param oActor - the AActor* on which to clear the command list
* @param nHardClear - specifies if the AActor* can finish the action in progress or not (Hard = don't wait)
* @returns TRUE_ on success, FALSE_ on failure
* @remarks Designers should NOT use this function, but use the WR_ClearAllCommands
* wrapper function instead, defined in wrappers_h
* @author Sam, Jose
*/
int32 ClearAllCommands(AActor* oActor, int32 nHardClear = TRUE_);

/** @brief Returns the distance between two objects.
*
* Returns the distance between AActor* oActorA and AActor* oActorB. Distance is measured through
* three dimensional space and not just along the xy plane, so the height component of each AActor's location
* is relevant.  If creatures, we assume that A is attacking B,
* so we use the closest interaction point on B relative to A.
*
* @param oActorA - The first AActor. (attacker)
* @param oActorB - The second AActor. (being attacked by B)
* @param bSubtractPersonalSpace - Removes personal spaces of A and B from the returned distance
* @returns Returns the distance between the two objects. Returns -1.0f on error.
* @sa GetDistanceBetweenLocations()
* @author Nicolas
*/
float GetDistanceBetween(AActor* oActorA, AActor* oActorB, int32 bSubtractPersonalSpace = 0);

/** @brief Returns the distance between two locations.
*
* Returns the distance between location vVectorA and location vVectorB Distance is measured through
* three dimensional space and not just along the xy plane, so the height component of each AActor's location
* is relevant.
*
* @param vVectorA - The first location.
* @param vVectorB - The second location.
* @returns Returns the distance between the two locations. Returns -1.0f on error.
* @sa GetDistanceBetween()
* @author Brenon
*/
float GetDistanceBetweenLocations(FVector vVectorA, FVector vVectorB);

/** @brief Returns the Creature Type (aka Combatant Type) of a creature
*
* The function returns the creature type index from (creaturetypes.xls) as defined
* in the toolset.
*
* @param oidCreature - The creature
* @returns Index into creaturetypes.xls (CREATURE_TYPE_* constant)
* @author Georg
*/
int32 GetCombatantType(AActor* oidCreature);

/** @brief Returns N nearest AActor* of a specific type, with a specifc Group Id
*
* Returns N nearest AActor* of a specific type, with a specifc Group Id
*
* @param * oActor - target Object
* @param * nObjectType - type for the objects to query for their distance
* @param * nGroupId - Group Id for the objects to query
* @param * nNumberOfObjects (optional) - Number of objects to return
* @param * nCheckLiving (optional) - only returns objects if they are alive
* @param * nCheckPerceived (optional) - only returns objects if they are within the perception radius.
* @author Adriana
*/
TArray<AActor*> GetNearestObjectByGroup(AActor* aActor, int32 nGroupId, int32 nObjectType = OBJECT_TYPE_INVALID, int32 nNumberOfObjects = 1, int32 nCheckLiving = TRUE_, int32 nCheckPerceived = TRUE_, int32 nIncludeSelf = FALSE_);

/** @brief Returns N nearest AActor* of a specific type, with a specifc Hostility
*
* Returns N nearest AActor* of a specific type, with a specifc Hostility
*
* @param * aActor - target Object
* @param * nObjectType - type for the objects to query for their distance
* @param * nHostility - Hostility for the objects to query (true/false)
* @param * nNumberOfObjects (optional) - Number of objects to return
* @param * nCheckLiving (optional) - only returns objects if they are alive
* @param * nCheckPerceived (optional) - only returns objects if they are within the perception radius.
* @author Adriana
*/
TArray<AActor*> GetNearestObjectByHostility(AActor* aActor, int32 nHostility, int32 nObjectType = OBJECT_TYPE_INVALID, int32 nNumberOfObjects = 1, int32 nCheckLiving = TRUE_, int32 nCheckPerceived = TRUE_, int32 nIncludeSelf = FALSE_);

/** @brief N nearest objects of a specific type to a Location
*
* Returns N nearest objects of a specific type to a Location
*
* @param * Location - target Location
* @param * nObjectType - type for the objects to query for their distance
* @param * nNumberOfObjects (optional) - Number of objects to return
* @author Adriana
*/
TArray<AActor*> GetNearestObjectToLocation(FVector vVector, int32 nObjectType = OBJECT_TYPE_INVALID, int32 nNumberOfObjects = 1);

/** @brief Returns N nearest objects of a specific type
*
* Returns N nearest AActor* of a specific type
*
* @param * aActor - target Object
* @param * nObjectType - type for the objects to query for their distance
* @param * nNumberOfObjects (optional) - Number of objects to return
* @param * nCheckLiving (optional) - only returns objects if they are alive
* @param * nCheckPerceived (optional) - only returns objects if they are within the perception radius.
* @author Adriana
*/
TArray<AActor*> GetNearestObject(AActor* aActor, int32 nObjectType = OBJECT_TYPE_INVALID, int32 nNumberOfObjects = 1, int32 nCheckLiving = TRUE_, int32 nCheckPerceived = TRUE_, int32 nIncludeSelf = FALSE_);

/** @brief Returns N nearest AActor* of a specific type, with a specifc tag
*
* Returns N nearest AActor* of a specific type, with a specifc tag
*
* @param * aActor - target Object
* @param * nObjectType - type for the objects to query for their distance
* @param * sTag - Tag for the objects to query
* @param * nNumberOfObjects (optional) - Number of objects to return
* @param * nCheckLiving (optional) - only returns objects if they are alive
* @param * nCheckPerceived (optional) - only returns objects if they are within the perception radius.
* @author Adriana
*/
TArray<AActor*> GetNearestObjectByTag(AActor* aActor, FString sTag, int32 nObjectType = OBJECT_TYPE_INVALID, int32 nNumberOfObjects = 1, int32 nCheckLiving = TRUE_, int32 nCheckPerceived = TRUE_, int32 nIncludeSelf = FALSE_);

/** @brief Returns the ability id for the effect
*
* Returns the ability id for the effect
*
* @param efEffect- The effect to get the ability off of.
* @returns Returns the ability id for the effect
* @sa GetEffectAbilityID()
* @author Adriana
*/
int32 GetEffectAbilityID(FEffect efEffect);

/** @brief Returns the effect flags on a given AActor.
*
*    Effects can set effect flags. This function will return
*  and integer that contains all the effect flags on an AActor
*  due to the effects it currently holds.
*
* @param oOwner    - The AActor* that contains effects
* @author Gabo
*/
int32 GetEffectsFlags(AActor* oOwner);

/** @brief Returns whether the creature or placeable can be interacted with.
*
* @author Nicolas Ng Man Sun
*/
int32 GetObjectInteractive(AActor* aActor);

/** @brief Sets whether the creature or placeable can be interacted with.
*          Non-interactive objects act like static geometries in that mousing
*          over them doesn't not trigger any selection and you can click through
*          them
* @author Nicolas Ng Man Sun
*/
void SetObjectInteractive(AActor* aActor, int32 nValue);

/** @brief Returns a list of ability IDs for each AOE in which a creature is in.
*
* @param oCreature - The creature being queried
* @returns an array of ability ids.
* @author Gabo
*/
TArray<int32> GetAbilitiesDueToAOEs(AActor* oCreature);

/** @brief
*
*Returns an AActor* reference for the area oActor is in.
*
* @param oActor - Target AActor
* @author Brenon Holmes
*/
UWorld* GetArea(AActor* oActor);

/** @brief This function gets the combat target of a creature
*
* This function gets the combat target of a creature. This target is set when using an attack or
* ability command and is cleared (to target invalid) when combat state is set to false.
*
* @param oCreature - the creature whose combat target we are querying
* @returns the id of the creature's target. Will be invalid if the creature has no target.
* @sa GetAttackTarget
* @author Gabo
*/
AActor* GetAttackTarget(AActor* oCreature);

/** @brief Returns the active weaponset on a creature
*
*  Returns the weaponset currently active by oCreature.
*
*  0 - WeaponSet 1 (1 in the Toolset)
*  1 - Weaponset 2 (2 in the Toolset)
*
* Warning: Returns 0 also on failure (oActor not a creature..)
*
* @param oActor The creature the check the weapon set on
* @author Georg Zoeller
*/
int32 GetActiveWeaponSet(AActor* aActor);

/** @brief Returns the ID number of the targeted party member for those tactic
*          targets which are generated on a per-follower basis.
*
* @param oCreature - The creature to query.
* @param nIndex - The tactic index.
* @return The ID number of the party member.
* @author Cody Watts
*/
AActor* GetTacticTargetObject(AActor* aActor, int32 nIndex);

/** @brief Returns the ID number of the party member being referenced in
*          those conditions which are generated on a per-follower basis.
*
* @param oCreature - the creature to query.
* @param nIndex - the tactic index.
* @return The ID number of the party member.
* @author Cody Watts
*/
AActor* GetTacticConditionObject(AActor* aActor, int32 nIndex);

/** @brief return an array of creatures contained in the melee ring for the given angles
*
* @param nRingOwnerId id of the creature for which to query the melee ring
* @param fStartAngle the angle where the search should start (supported ranges 0->360 or -180->180)
* @param fEndAngle angle where the search should stop (fEndAngle >= fStartAngle)
* @param bOnlyHostiles set to TRUE_ if the array should only contain hostile creatures
* @param nRingIndex the ring to query. By default the main ring will respond to the query (ring index 0)
* @author Jose
*/
TArray<AActor*> GetCreaturesInMeleeRing(AActor* nRingOwnerId, float fStartAngle, float fEndAngle, int32 bOnlyHostiles = FALSE_, int32 nRingIndex = 0);

TArray<AActor*> GetCreaturesInCone(AActor* aActor, int32 bOnlyHostiles = FALSE_);

/** @brief Returns the location of the specified AActor.
*
* This function returns the location of the specified AActor.
* If an invalid AActor* is specified, then an invalid location will be returned.
*
* @param aActor - the AActor* to get the location of
* @returns a valid location on success or invalid location on error
* @sa IsLocationValid
* @author Brenon
*/
FVector GetLocation(AActor* aActor);

/** @brief Returns the number of tactics for the creature.
*   Zero can imply that either the tactics are disabled by the user, or they don't exist.
*   for this creature (a non-party creature).
*
* @param oCreature - the creature to query.
* @return - the number of available tactics.
* @author Jacques
*/
int32 GetNumTactics(AActor* oCreature);

/** @brief Returns the number of rows in the specified 2da.
*
* Returns the number of rows in the specified 2da.
*
* @param n2DA - The 2DA to access.
* @param s2da - (optional) if n2da is -1 and this is a valid resource, it will retrieve
*       the 2da based on the name instead of the index. Note that this should be avoided
*       when possible.
* @returns Returns the number of rows in the 2DA, returns 0 on error.
* @author Brenon, Georg
*/
int32 GetM2DARows(int32 n2DA, FString s2DA = "");

/** @brief Returns whether the specified tactic is enabled.
*
* @param oCreature - the creature to query.
* @param nIndex - the tactic index.
* @return TRUE_ if the tactic is enabled.
* @author Jacques
*/
int32 IsTacticEnabled(AActor* oCreature, int32 nIndex);

/** @brief Returns the command item tag for the specified tactic.
*
* @param oCreature - the creature to query.
* @param nIndex - the tactic index.
* @return The item tag, or empty FString if not applicable.
* @author Jacques
*/
FString GetTacticCommandItemTag(AActor* oCreature, int32 nIndex);

/** @brief Returns the target type for the specified tactic.
*
* @param oCreature - the creature to query.
* @param nIndex - the tactic index.
* @return The target type.
* @author Jacques
*/
int32 GetTacticTargetType(AActor* oCreature, int32 nIndex);

/** @brief Returns the condition id of the specified tactic
*
* @param oCreature - the creature to query.
* @param nIndex - the tactic index.
* @return The condition id.
* @author Jacques
*/
int32 GetTacticCondition(AActor* oCreature, int32 nIndex);

/** @brief Returns the command type for the specified tactic.
*
* @param oCreature - the creature to query.
* @param nIndex - the tactic index.
* @return The command type.
* @author Jacques
*/
int32 GetTacticCommand(AActor* oCreature, int32 nIndex);

/** @brief Returns the command parameter for the specified tactic.
* Can be the ability ID or item ID. Zero for none or not applicable.
* @param oCreature - the creature to query.
* @param nIndex - the tactic index.
* @return The target type.
* @author Jacques
*/
int32 GetTacticCommandParam(AActor* oCreature, int32 nIndex);

/** @brief Gets the stack Size of an item
*
* Returns an integer representing the current stack size of an item.
*
* @param oItem - An Item Object
* @returns 0 if the item is invalid; defauts to 1 if the item is not stackable.
* @author Georg
*/
int32 GetItemStackSize(AActor* oItem);

/** @brief Get the remaining time for an ability to be used
*
* @param oCreature - owner of the ability
* @param nAbilityId - ability to check the cooldown
* @param sSourceItemTag - if an item ability, specify the specific item providing the ability.
* If an empty FString, the engine will grab the first item with this ability,
* this may not be the desired intention if the player has several items with the same ability.
* @returns 0.0f if the ability is ready to be used again
* @sa SetCooldown
* @author Jose
*/
float GetRemainingCooldown(AActor* oCreature, int32 nAbilityId, FString sSourceItemTag = "");

/** @brief This function return the center of a cluster of creatures
*
* Returns the center of the best cluster of creatures based on input params
*
* @param oCreator - effect creator
* @param nAbilityId - ability id
* @param nClusterSize - min number of enemies needed
* @param nAllyFailChance - A percentage chance to fail each possible cluster if any ally
* is inside the cluster. The chance is cumulative per ally
* @param nReturnFirstMatch - Return the first group matching the criteria instead of best match. Quicker.
* @author EricP
*/
FVector GetClusterCenter(AActor* oCreator, int32 nAbilityId, int32 nClusterSize, int32 nAllyFailChance, int32 bReturnFirstMatch);

/** @brief Gets the base type of the placeable.
*
* @param oPlaceable - The placeable to get the type from
* @author Jacques Lebrun
*/
int32 GetPlaceableBaseType(AActor* oPlaceable);

/** @brief Gets the state of the specified placeable AActor.
*
* Returns the current state of a specified placeable AActor. The state
* will be a valid state defined by the state controller for the
* specified placeable. The placeable states are defined in the placeables.xls file in override.
*
* @param oPlaceable - The placeable to get the state of
* @returns Returns the state of the placeable AActor
* @sa SetPlaceableState()
* @author Paul
*/
int32 GetPlaceableState(AActor* oPlaceable);

/** @brief Returns the direction an AActor* is facing.
*
* Returns the direction the AActor* oTarget is currently facing in degrees. The direction
* an AActor* is facing is expressed as an incrementing clockwise degree value
* starting from the south. Related constants are: DIRECTION_NORTH, DIRECTION_EAST,
* DIRECTION_SOUTH and DIRECTION_WEST.
*
* @param oTarget - The AActor* to get the facing direction of.
* @return Returns the direction the AActor* is facing in degrees. Returns -1.0f on error.
* @sa SetFacing(), SetFacingPosition(), SetFacingObject(), SetOrientation(), GetOrientation()
* @author Brenon
*/
float GetFacing(AActor* oTarget);

/** @brief Used to test if a specific location is safe.
*
* This function is used to test to see if a location is valid.
* It will return TRUE_ if the specified location is valid. An invalid location
* is defined by an invalid area AActor, an empty position vector and an empty orientation vector.
*
* @param vVector - The location tested to see if it's valid
* @returns TRUE_ on success, FALSE_ on error
* @sa IsLocationSafe, GetSafeLocation
* @author Brenon
*/
int32 IsLocationValid(FVector vVector);

/** @brief Gets all items in an AActor* inventory
*
* Provides access to a creatures or Placeables inventory and equipped items.
*
* @param aActor        - A creature or placeable with an inventory
* @param nGetItemOptions  - A GET_ITEMS_* constant. THIS IS NOT A BITFIELD!
* @param nBaseItemType      - Return only items with base item type matching. 0 to disable this filter.
* @param sFilterTag     - Return only items with a matching tag. "" to disable this filter.
*
* @author Georg
*/
TArray<AActor*> GetItemsInInventory(AActor* aActor, int32 nGetItemsOptions = GET_ITEMS_OPTION_ALL, int32 nBaseItemType = 0, FString sTagFilter = "", int32 bIgnorePlotItems = FALSE_);

/** @brief Return the power level of an item properties present on an AActor
*
* @param   oitem            - the item
* @param   nPropertyId      - the property
* @param   bIncludeSubItems - include runes properties
* @param   bScalePower      - return the scaled power instead of the base value
* @returns the property's power level
* @author Georg Zoeller
*/
float GetItemPropertyPower(AActor* oItem, int32 nPropertyId, int32 bIncludeSubItems, int32 bScalePower = FALSE_);

/** @brief Retrieve the base value of an item.
*
* @author Jacques Lebrun
*/
int32 GetItemValue(AActor* oItem);

/** @brief Get the ability_id associated with an item
*
* An items ability id (if != ABILITY_INVALID) is the one active ability an item will grant when equipped to the quickbar.
*
* @param   oItem        - An item
* @returns an index into ABI_BASE.xls
*
* @author Georg
*/
int32 GetItemAbilityId(AActor* oItem);

/** @brief Gets the angle between two objects
*
* Returns the angle (in degrees) between the direction objectA is facing and
* the vector formed between objectA and objectB. The angle follows a
* counterclockwise sequence, starting from the front. If a objectB is directly in front
* of objectA, the angle will be 0. If AActor* B is exactly to the left, the angle will be
* 90 degrees. If AActor* B is behind AActor* A, the angle will be 180, and if AActor* B is
* to the right, the angle will be 270.
*
* @param aActorA - Object that has a facing direction.
* @param aActorB - Object that is positioned relative to AActor* A.
* @author Gabo.
*/
float GetAngleBetweenObjects(AActor* aActorA, AActor* aActorB);

/** @brief Determines if an AActor* is humanoid.
*
* Returns TRUE_ if the AActor* aActor uses the humanoid combat blend tree animations.
* Returns FALSE_ if aActor is not a creature or does not use the humanoid animations.
*
* @param aActor - The AActor* that is to be checked for humanoind status.
* @author Craig Welburn
*/
int32 IsHumanoid(AActor* aActor);

/** @brief Gets the weapon style used by a creature
*
* @param oCreature - the creature whose weapon style we are querying
* @returns the weapon style (0 - none, 1 - single (with or without shield), 2 - dual, 3 - two handed)
* @sa GetWeaponStyle
* @author Gabo
*/
int32 GetWeaponStyle(AActor* oCreature);

/** @brief Determines if there is a line of sight between two objects.
*
* Returns TRUE_ if there is a clear line of sight between AActor* oSource and AActor* oTarget, otherwise returns FALSE_.
* If the line is occluded by geometry then the function will return FALSE_. If the two objects are not in the same
* area or if either AActor* is invalid this function will return FALSE_.
*
* @param oSource - The first AActor.
* @param oTarget - The second AActor.
* @returns Returns TRUE_ if there is a clear line of sight between the two objects. Returns FALSE_ if there is not.
* @remarks It should be noted that the function is somewhat expensive and should not be called in an inner loop of your script.
* @sa CheckLineOfSightVector()
* @author Brenon
*/
int32 CheckLineOfSightObject(AActor* oSource, AActor* oTarget);

/** @brief Sets the target to face a given AActor.
*
* Sets the target AActor* to face a given AActor.
*
* @param aActor - Object to set the facing for.
* @param oTarget - Object to face.
* @param bInvert - This flag is very confusing, currently when set to FALSE it will invert. Why?
* @param bInstant - By default an AActor* will change its facing gradually, set this to true
*                   to change facing immediately.
* @remarks If the AActor* or the target AActor* are invalid, the function will fail.
* @sa SetFacing(), GetFacing(), SetFacingPosition()
* @author Brenon
*/
void SetFacingObject(AActor* aActor, AActor* oTarget, int32 bInvert = TRUE_, int32 bInstant = FALSE_);

/** @brief Makes a creature be perceived as long as its within the outer perception radius
*
* This command goes beyond the normal AI perception ring, but will return false
* if the creature to be perceived is dead, the same as the perceiving creature or
* beyond the visual radius (about 60m).
*
* @param oPerceivingCreature - The creature that will see another creature
* @param oPerceivingCreature - The creature that will be seen.
* @author Gabo.
*/
int32 TriggerPerception(AActor* oPerceivingCreature, AActor* oPerceivedCreature);

/** @brief Turns creature into a statue.  Statues have their animations frozen and cannot move.
*
* @author Nicolas Ng Man Sun
*/
void SetCreatureIsStatue(AActor* aActor, int32 value);

/** @brief Creates an item at a specific location
*
* Creates an item using the specified file name at lLocation. If the AActor* is a creature
then it will also attempt to use an appear animation if the flag is set. If an invalid AActor* type
is specified, the file name does not exist or the location is bad an invalid AActor* will be returned.

* NOTE: As with all commands that add items to a container, this command will reset the container (if a placeable)
*       to interactive not matter it's previous state.
*
* @param rItemFileName - The file name of the AActor* to create (*.uti)
* @param oTarget - The item will be created inside the inventory of this AActor
* @param nStackSize - Stack size of the item to be created
* @param sTag - Optional tag for the new item
* @param bSuppressNotification - if true, the "Item Acquired" notification will not be displayed
* @param bDroppable - if true, the item can be looted when the target creature dies
* @returns a valid new AActor* or nullptr on error
* @author Brenon
*/
AActor* CreateItemOnObject(FString rItemFileName, AActor* oTarget, int32 nStackSize = 1, FString sTag = "", int32 bSuppressNotification = FALSE_, int32 bDroppable = TRUE_);

/** @brief Set the 'droppable' property of an item
*
* Sets whether or not an item is droppable
*
* @param   oitem    - the Item
* @param   bDroppable   - whether or not it is droppable
* @author Georg Zoeller
*/
void SetItemDroppable(AActor* oItem, int32 bDroppable);

/** @brief Equips an item on a creature
*
* Equips an item from the inventory into the equipped slots.
* If no equipment slot is given, the item will be equipped where it
* best fits. If the equip slot is in the weapon set it will be equipped
* in the weapon set number that is given. If no weapon set number is given
* the active weapon set will be used.
* If an incorrect slot is given or an invalid AActor* is given
* the function will return 0.
*
* @param aActor - The AActor* to have its weapon set switched.
* @param nEquipSlot - The optional equip slot number. Use the INVENTORY_SLOT constants to specify a particular slot.
* @param nWeaponSet - The optional weapon set number, it can be 0 or 1.
* @author Gabo.
*/
int32 EquipItem(AActor* aActor, AActor* oItem, int32 nEquipSlot = INVENTORY_SLOT_INVALID, int32 nWeaponSet = INVALID_WEAPON_SET);

/** @brief Get a stat modifier of the item (including material modifier)
*
* Queries the items and material 2DAs to get the stat modifier
* that a certain item has when equipped.
*
* @param   oitem    - the Item
* @param   nStatType - The stat that is being queried
* @author Gabo
*/
float GetItemStat(AActor* oItem, int32  nStatType);

/** @brief Returns the inventory slot this item is equipped in, or INVENTORY_SLOT_INVALID, if not equipped.
*
* @param oItem - The item in question.
* @returns The inventory slot, or INVENTORY_SLOT_INVALID, if not equipped.
* @author  Cody Watts
*/
int32 GetItemEquipSlot(AActor* oItem);

/** @brief This function returns if a creature is allowed to die permanently
*
* This function returns the value set for the 'NoPermDeath' field in the toolset.
* This is usually used to prevent deathblows or other methods of permanent destruction
* from affecting plot important creatures in cases where using the Plot flag is not
* an option. It always returns FALSE for members of the player's party.
*
* @param oidCreature - The creature
* @returns Whether or not it is allowed to kill the creature permanently.
* @author Georg
*/
int32 GetCanDiePermanently(AActor* oCreature);

/** @brief EffectImpact Constructor.
*
*    EffectImpact Constructor.
*
* @param fDamage     - Amount of damage to be applied.
* @param oWeapon     - Weapon that applies the damage.
* @param nVfx        - Impact VFX to play.  Default of 0 is no impact VFX.
* @param nAbi        - Ability (default is invalid ability).
* @param nDamageType - Damage Type to be applied (Physical, Fire):  Default of 1 is physical.
* @author Georg Zoeller, Ported Into Engine by Mark Brockington
*/
FEffect EffectImpact(float fDamage, AActor* oWeapon, int32 nVfx = 0, int32 nAbi = 0, int32 nDamageType = 1);

/** @brief This function sets the results of an attack
*
* @param oAttacker - the creature whose attack result we are storing
* @param nResult1 - the main attack result (HIT/MISS/ETC)
* @param eDamageEffect1 - the main attack damage effect
* @param nResult2 - the offhand attack result (HIT/MISS/ETC)
* @param eDamageEffect2 - the offhand attack damage effect
* @author Jose
*/
void SetAttackResult(AActor* oAttacker, int32 nResult1, FEffect eDamageEffect1, int32 nResult2, FEffect eDamageEffect2);

/** @brief set how long to hold aim for the next projectile to be fired (control speed for archers & ranged combat in general)
*
* @param aActor The AActor* to set the duration for
* @param fDuration Duration of the aim loop (in seconds)
* @author Jose
*/
void SetAimLoopDuration(AActor* aActor, float fDuration);

/** @brief Specifies the length that attacks will take
*
* @param aActor The AActor* to set the duration for
* @param fDuration_s Duration of the attack (in seconds)
* @author Gabo
*/
void SetAttackDuration(AActor* aActor, float fDuration);

/** @brief Enables/Disables the weapon trail for a creature
*
* Enables/Disables the weapon trail for a creature.
*
* @param oCreature - the creature
* @param bEnable - TRUE/FALSE
* @param nTypeID - Weapon Trail ID from WT_** 2da
* @param fFinishTime - If disabling, how long to fade out the trail
* @author Adriana Lopez
*/
void EnableWeaponTrail(AActor* aActor, int32 bEnable);

/** @brief Get the roam Location for the creature.
*
*
*   @author Pat LaBine
*
*/
FVector GetRoamLocation(AActor* aActor);

/** @brief Delete all entries in the perception list of a creature
*
* @param oPerceiver - The creature for which the perception list will be reset
* @author Jose
*/
void ClearPerceptionList(AActor* aActor);

/** @brief Returns the nth specified AActor* with the appropriate tag
*
* Returns the nth specified AActor* with the appropriate tag
*
* @param sTag - The tag reference to get the AActor* by
* @param nNth - If there are multiple objects with the same tag, get the 'nth' specified AActor.
* @returns the desired AActor* or nullptr on failure
* @author Brenon
*/
AActor* GetObjectByTag(FString sTag, int32 nNth = 0, int32 nObjectType = OBJECT_TYPE_INVALID);

/** @brief Creates and applies a command to play an animation to interact with an AActor.
*
* This function is for playing ambient animations. It should not be used for
* player or combat interactions.
*
* This will create a command to use an AActor. Sub commands include:
* (1) move to use point
* (2) face AActor
* (3) interact with AActor
*
* If the target AActor* is a creature, this command will clear all
* commands on the creature and apply an interaction command on it as well
* so they both play the sync animations properly.
*
* Once a creature has an interaction command on it, additional calls to this
* function will switch the animation being played immediately, without creating
* another command.
*
* @param oCreature - The creature that will interact
* @param oTarget - the placeable or creature to interact with
* @param nInteractionId - The type of interaction (see SyncPlaceableAnims and SyncCreatureAnims)
* @param nPose - The pose loop animation to use
* @param nLoops - The number of times to play a pose loop (-1 will loop infinitely)
* @param nPlayExit - Will play an exit animation after the loops end or if the command is canceled
* @author Gabo
*/
void InteractWithObject(AActor* oCreature, AActor* oTarget, int32 nInteractionId, int32 nPose = 1, int32 nLoops = 0, int32 bPlayExit = 1, int32 bSkipReposition = 0);