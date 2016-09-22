#pragma once

// -----------------------------------------------------------------------------
// Events Includes
// -----------------------------------------------------------------------------
/*

This is an include file of all events that are created via scripting

Contents:

SendXXXEvent Functions
Event structs

*/
// -----------------------------------------------------------------------------
// Owner: Georg Zoeller
// -----------------------------------------------------------------------------

#include "core_h.h"
#include "log_h.h"

/* -----------------------------------------------------------------------------
*  Designer defined events
*  ------------------------------------ ----------------------------------------*/

//------------------------------------------------------------------------------
// ** PLEASE ALSO UPDATE //tag/main/data/Source/2DA/events.xls if you add new
//    events here !!!! ** -- Georg
//------------------------------------------------------------------------------

/* -----------------------------------------------------------------------------
*  Script.nss defines
*  -----------------------------------------------------------------------------*/
const int32 EVENT_TYPE_INVALID = 0;
//const int32 EVENT_TYPE_SPELLCASTAT = 1;
//const int32 EVENT_TYPE_DAMAGED = 2;
const int32 EVENT_TYPE_SPAWN = 3;
const int32 EVENT_TYPE_DEATH = 4;
//const int32 EVENT_TYPE_MELEE_ATTACK_START = 5;
//const int32 EVENT_TYPE_INVENTORY_ADDED = 6;
//const int32 EVENT_TYPE_INVENTORY_REMOVED = 7;
//const int32 EVENT_TYPE_ENTER = 8;
//const int32 EVENT_TYPE_EXIT = 9;
//const int32 EVENT_TYPE_BLOCKED = 10;
//const int32 EVENT_TYPE_EQUIP = 11;
//const int32 EVENT_TYPE_UNEQUIP = 12;
//const int32 EVENT_TYPE_FAILTOOPEN = 13;
//const int32 EVENT_TYPE_USE = 14;
//const int32 EVENT_TYPE_CLICK = 15;
//const int32 EVENT_TYPE_TRAP_TRIGGERED = 16;
//const int32 EVENT_TYPE_TRAP_DISARMED = 17;
//const int32 EVENT_TYPE_DIALOGUE = 18;
const int32 EVENT_TYPE_CONVERSATION = 18;
//const int32 EVENT_TYPE_MODULE_START = 19;
//const int32 EVENT_TYPE_MODULE_LOAD = 20;
//const int32 EVENT_TYPE_LISTENER = 21;
//const int32 EVENT_TYPE_LOCKED = 22;
//const int32 EVENT_TYPE_UNLOCKED = 23;
//const int32 EVENT_TYPE_PLAYERLEVELUP = 24;
const int32 EVENT_TYPE_PERCEPTION_APPEAR = 25;
const int32 EVENT_TYPE_PERCEPTION_DISAPPEAR = 26;
const int32 EVENT_TYPE_SET_PLOT = 27;
const int32 EVENT_TYPE_GET_PLOT = 28;
//const int32 EVENT_TYPE_ATTACK_IMPACT = 29;
//const int32 EVENT_TYPE_COMBAT_INITIATED = 30; // was ENGAGE_TARGET
//const int32 EVENT_TYPE_ABILITY_CAST_IMPACT = 31; // was CAST
const int32 EVENT_TYPE_APPLY_EFFECT = 33;
//const int32 EVENT_TYPE_REMOVE_EFFECT = 34;
const int32 EVENT_TYPE_COMMAND_PENDING = 35; // was RESOLVE_ATTACK, WEAPON_ATTACK_PENDING
const int32 EVENT_TYPE_COMMAND_COMPLETE = 36;
//const int32 EVENT_TYPE_ABILITY_CAST_START = 32;

// Area load events
const int32 EVENT_TYPE_GAMEOBJECTSLOADED = 37;
const int32 EVENT_TYPE_AREALOAD_PRELOADEXIT = 38;
const int32 EVENT_TYPE_AREALOAD_POSTLOADEXIT = 39;
const int32 EVENT_TYPE_AREALOAD_SPECIAL = 40;
const int32 EVENT_TYPE_AREALOADSAVE_SPECIAL = 41;
const int32 EVENT_TYPE_AREALOADSAVE_PRELOADEXIT = 42;
const int32 EVENT_TYPE_AREALOADSAVE_POSTLOADEXIT = 43;

const int32 EVENT_TYPE_GAMEMODE_CHANGE = 60;

//const int32 EVENT_TYPE_ATTACKED = 1001;
//const int32 EVENT_TYPE_ALLY_ATTACKED = 1002; // an ally has got the 'attacked' event and asked for help
//const int32 EVENT_TYPE_WORLD_MAP_USED = 1003; // player used the world map to travel from point A to point B
const int32 EVENT_TYPE_DELAYED_SHOUT = 1004; // used to fire a dialog shout every few seconds
//const int32 EVENT_TYPE_TRANSITION_TO_WORLD_MAP = 1005; // player uses the generic transition system to open the world map

//------------------------------------------------------------------------------
// Redirector events for spellscripts in ability_core
//------------------------------------------------------------------------------
//const int32 EVENT_TYPE_SPELLSCRIPT_PENDING = 1005;
//const int32 EVENT_TYPE_SPELLSCRIPT_CAST = 1006;  // cast
//const int32 EVENT_TYPE_SPELLSCRIPT_IMPACT = 1007;  // impact
const int32 EVENT_TYPE_SPELLSCRIPT_DEACTIVATE = 1008; // talent deactivated

//const int32 EVENT_TYPE_DOT_TICK = 1010;
//const int32 EVENT_TYPE_CAST_AT = 1011; //an ability has been cast on me
//const int32 EVENT_TYPE_STAT_REGEN = 1012; // stat regeneration. might be changed in the future.
const int32 EVENT_TYPE_RESURRECTION = 1013; // creature resurrected.
const int32 EVENT_TYPE_SET_OBJECT_ACTIVE = 1014; // needed since CommandDoFunction was removed and we're using CommandDoEvent
// 1015 used by ambient AI
const int32 EVENT_TYPE_HANDLE_CUSTOM_AI = 1016; // handle any custom AI before handling the built-in AI
//const int32 EVENT_TYPE_OUT_OF_AMMO = 1017;
//const int32 EVENT_TYPE_HEARTBEAT = 1018; // runs 2 seconds after death to set AI level to low
//const int32 EVENT_TYPE_TEAM_DESTROYED = 1019; // fires when an entire team of creatures is destroyed. The event is fired to the last living creature on the team
//const int32 EVENT_TYPE_CAMPAIGN_ITEM_ACQUIRED = 1020;  // fires when items with the ITEM_ACQUIRED_EVENT_ID variable set are picked up. The event is fired to the module.
const int32 EVENT_TYPE_SET_GAME_MODE = 1021; // fires to the module when the game mode is set with the game mode being set carried as an integer on the event.
const int32 EVENT_TYPE_COMBAT_END = 1022; // fires to creature_core when combat ends
//const int32 EVENT_TYPE_DYING = 1023; // fired by effect_death_h when a creature received the killing blow.
//const int32 EVENT_TYPE_PLAYER_LEVELUP = 1024; // fired by the levelup system into player_core.
//const int32 EVENT_TYPE_... = 1025; // unused
const int32 EVENT_TYPE_AMBIENT_CONTINUE = 1026; // fired by engine when conversation ends or player is nearby to resume ambient behavior
//const int32 EVENT_TYPE_MODULE_CHARGEN_DONE = 1027; // fired the the core chargen script into the module
//const int32 EVENT_TYPE_PARTY_MEMBER_HIRED = 1028; // fired when a party member is added to the party pool
//const int32 EVENT_TYPE_SUMMON_DIED = 1029; // fired when the summon died.
//const int32 EVENT_TYPE_CONFUSION_CALLBACK = 1030;
//const int32 EVENT_TYPE_PARTY_MEMBER_FIRED = 1031;
//const int32 EVENT_TYPE_UNIQUE_POWER = 1032;
//const int32 EVENT_TYPE_APPROACH_TRAP = 1033; // Trap triggered and the creature receiving this event should approach the trap
//const int32 EVENT_TYPE_CAMPAIGN_ITEM_LOST = 1034; // fire to the module when the ITEM_LOST_EVENT variable is set on an item removed form player inventory.

//const int32 EVENT_TYPE_TRAP_TRIGGER_DISARMED = 1045; // Fired to trap's signal target(s) when the trap is disarmed.
//const int32 EVENT_TYPE_UNLOCK_FAILED = 1046; // Fired to placeable when unlock attempt fails.
//const int32 EVENT_TYPE_OPENED = 1047; // Fired to placeable when it has been opened.
//const int32 EVENT_TYPE_TRAP_RESET = 1048; // Fired to trap's signal target(s) so it releases/retracts/resets when trap is triggered.
//const int32 EVENT_TYPE_TRAP_DISARM = 1049; // Fired to a trap to cause itself to disarm.
//const int32 EVENT_TYPE_TRAP_ARM = 1050; // Fired to a trap to cause itself to arm.
//const int32 EVENT_TYPE_TRAP_TRIGGER_ENTER = 1051;
//const int32 EVENT_TYPE_TRAP_TRIGGER_EXIT = 1052;
//const int32 EVENT_TYPE_TRAP_TRIGGER_ARMED = 1053;
const int32 EVENT_TYPE_CUSTOM_COMMAND_COMPLETE = 1054;
//const int32 EVENT_TYPE_MODULE_HANDLE_GIFT = 1055;
//const int32 EVENT_TYPE_MODULE_HANDLE_FOLLOWER_DEATH = 1056; // yes! its only here for Wynne!
//const int32 EVENT_TYPE_SET_INTERACTIVE = 1057;
const int32 EVENT_TYPE_OBJECT_ACTIVE = 1058;
//const int32 EVENT_TYPE_SPELLSCRIPT_INDIVIDUAL_IMPACT = 1066;  // impact
//const int32 EVENT_TYPE_DROP_STEALTH = 1090;
//const int32 EVENT_TYPE_CREATURE_SHAPESHIFTED = 1100;

const int32 EVENT_TYPE_DELAYED_GM_CHANGE = 2000;
const int32 EVENT_TYPE_AUTOPAUSE = 2001;

//const int32 EVENT_TYPE_CHARGEN_AUTOLEVEL = 56;
//const int32 EVENT_TYPE_DESTROY_OBJECT = 1070; // AActor* should destroy itself
//const int32 EVENT_TYPE_COMBO_IGNITE = 1080;
//const int32 EVENT_TYPE_PARTY_MEMBER_RES_TIMER = 1201;// party members are rezzed on this event if it happens outside of combat.

// QA is using events 30200-30300
//const int32 EVENT_TYPE_QA_EVENT = 30200;
//const int32 EVENT_TYPE_QA_EVENT_BLA = 30201;

// Events 50000 to 60000 are reserved for plot events
//const int32 EVENT_TYPE_PROVING_ENTER = 50001;
//const int32 EVENT_TYPE_PROVING_START = 50002;
//const int32 EVENT_TYPE_PROVING_WIN = 50003;
//const int32 EVENT_TYPE_PROVING_LOSE = 50004;
//const int32 EVENT_TYPE_PROVING_EXIT = 50005;
//const int32 EVENT_TYPE_CUSTOM_EVENT_01 = 50006;
//const int32 EVENT_TYPE_CUSTOM_EVENT_02 = 50007;
//const int32 EVENT_TYPE_CUSTOM_EVENT_03 = 50008;
//const int32 EVENT_TYPE_CUSTOM_EVENT_04 = 50009;
//const int32 EVENT_TYPE_CUSTOM_EVENT_05 = 50010;
//const int32 EVENT_TYPE_STEALING_SUCCESS = 50011;
//const int32 EVENT_TYPE_STEALING_FAILURE = 50012;
//const int32 EVENT_TYPE_CUSTOM_EVENT_06 = 50013;
//const int32 EVENT_TYPE_CUSTOM_EVENT_07 = 50014;
//const int32 EVENT_TYPE_CUSTOM_EVENT_08 = 50015;

//const int32 EVENT_TYPE_DEBUG_KICKSTART_AI = 9998; // (georg) kickstart the AI if it was frozen.
//const int32 EVENT_TYPE_DEBUG_RESURRECTION = 9999; // (georg) resurrection button handler for death panel/

//------------------------------------------------------------------------------
// Events 80000-81000 are reserved for run database events,
//
//   *** See design_tracking_h for their actual definition ***
//
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// ** PLEASE ALSO UPDATE //tag/main/data/Source/2DA/events.xls if you add new
//    events here !!!! ** -- Georg
//------------------------------------------------------------------------------

/**
* @brief Signals a delayed shout event to oTarget.
*
* @param oTarget   the target to signal the event to
*
* @returns  nothing
* @author   Yaron
*
**/
void SendEventOnDelayedShout(AActor* oTarget);

/**
* @brief Signals a handle custom AI event
*
* @param oTarget   the target to signal the event to
*
* @returns  nothing
* @author   Yaron
*
**/
void SendEventHandleCustomAI(AActor* oTarget, AActor* oLastTarget, int32 nLastCommand, int32 nLastCommandStatus, int32 nLastSubCommand, int32 nAITargetType = -1, int32 nAIParameter = 0, int32 nTacticID = -1);

/**
*  @brief Decides whether or not a command_pending should be processed or discarded
*
*  @param oCommandOwner   The command owner, usually OBJECT_SELF
*  @param oTarget         The Target of the command
*  @param nCommandId      The command Id
*  @param nCommandSubType The command subtype
*
*  @author Georg Zoeller
**/
int32 Events_ValidateCommandPending(AActor* oCommandOwner, AActor* oTarget, int32 nCommandId, int32 nCommandSubType);

/** ----------------------------------------------------------------------------
* @brief Creates a EVENT_TYPE_SPELLSCRIPT_DEACTIVATE event and returns it
*
* @param oCaster      The caster using the ability (EventObject 0)
* @param nAbility     The ability id               (EventInteger 0)
* @param nAbilityType The ability type             (EventInteger 1)

* @returns  event with populated event parameters
* @author   Georg Zoeller
*  -----------------------------------------------------------------------------
**/
FGameEvent EventSpellScriptDeactivate(AActor* oCaster, int32 nAbility, int32 nAbilityType);