#pragma once

class ADA2UE4Creature;
//------------------------------------------------------------------------------
// ui_h
//------------------------------------------------------------------------------
/*
Any function interacting with the UI is wrapped here.

*/
//------------------------------------------------------------------------------
// @author georg zoeller
//------------------------------------------------------------------------------

// const int32 UI_MESSAGE_OUT_OF_AMMO = 1;
// const int32 UI_MESSAGE_DEATHBLOW = 2;
// const int32 UI_MESSAGE_ABILITY_CONDITION_NOT_MET = 3;
const int32 UI_MESSAGE_MISSED = 4;
// const int32 UI_MESSAGE_BLOCKED = 5000;
// const int32 UI_MESSAGE_BACKSTAB = 5;
// const int32 UI_MESSAGE_OUT_OF_MANA = 6;
// const int32 UI_MESSAGE_OUT_OF_STAMINA = 7;
const int32 UI_MESSAGE_IMMUNE = 8;
// const int32 UI_MESSAGE_RESISTED = 9;
// const int32 UI_MESSAGE_LEVELUP = 10;
// const int32 UI_MESSAGE_STUNNED = 11;
// const int32 UI_MESSAGE_GRAB_BROKEN = 12;
// const int32 UI_MESSAGE_TRAP_TRIGGERED = 13;
// const int32 UI_MESSAGE_TRAP_PLACED = 14;
// const int32 UI_MESSAGE_TRAP_DETECTED = 15;
// const int32 UI_MESSAGE_CANT_DO_IN_COMBAT = 16;
// const int32 UI_MESSAGE_LOCKED = 17;
// const int32 UI_MESSAGE_UNLOCKED = 18;
// const int32 UI_MESSAGE_CAN_NOT_USE_OBJECT = 19;
// const int32 UI_MESSAGE_APPROVAL_INCREASED = 20;
// const int32 UI_MESSAGE_APPROVAL_DECREASED = 21;
// const int32 UI_MESSAGE_INTERRUPTED = 22;
// const int32 TRAP_DISARM_SUCCEEDED = 23;
// const int32 TRAP_DISARM_FAILED = 24;
// const int32 UI_MESSAGE_SHATTERED = 25;
// const int32 UI_MESSAGE_EVASION = 27;
// const int32 UI_MESSAGE_NO_EFFECT = 28;
// const int32 UI_MESSAGE_UNLOCK_SKILL_LOW = 29;
// const int32 UI_MESSAGE_KEY_REQUIRED = 30;
// const int32 UI_MESSAGE_SPELL_IMMUNITY = 31;
// const int32 UI_MESSAGE_UNLOCKED_BY_KEY = 32;
// const int32 UI_MESSAGE_NOT_AT_THIS_LOCATION = 33;
// 
// const int32 UI_MESSAGE_LOCKPICK_NOT_POSSIBLE = 3516;
// const int32 UI_MESSAGE_DISARM_NOT_POSSIBLE = 3517;
// const int32 UI_MESSAGE_LEVER_PULLED = 3520;
// const int32 UI_MESSAGE_VALVE_OPEN = 3521;
// const int32 UI_MESSAGE_VALVE_CLOSE = 3522;

const int32 UI_DEBUG_LAST_MOVEMENT_FAILED = 1001;
const int32 UI_DEBUG_LAST_ATTACK_FAILED = 1002;
const int32 UI_DEBUG_LAST_WEAPON_SWITCH_FAILED = 1003;
const int32 UI_DEBUG_LAST_WAIT_FAILED = 1004;
const int32 UI_DEBUG_CREATURE_WAITING_NOW = 1005;
const int32 UI_DEBUG_NO_LOS = 1006;
const int32 UI_DEBUG_INVALID_PATH = 1007;
const int32 UI_DEBUG_INVALID_DATA = 1008;
const int32 UI_DEBUG_COMMAND_CLEARED = 1009;
const int32 UI_DEBUG_COMMAND_FAILED = 1010;
const int32 UI_DEBUG_NO_SPACE_IN_MELEE_RING = 1011;
const int32 UI_DEBUG_TARGET_DESTROYED = 1012;
const int32 UI_DEBUG_MOVEMENT_DISABLED = 1013;
const int32 UI_DEBUG_CREATURE_IMMORTAL = 1014;
const int32 UI_DEBUG_EVENT_IMPACT_ATTACK = 1015;
const int32 UI_DEBUG_EVENT_IMPACT_CAST = 1016;
const int32 UI_DEBUG_COMMAND_TIMED_OUT = 1017;

const int32 UI_MESSAGE_TYPE_SPECIAL = 10000;
const int32 UI_SPECIAL_MSG_DAMAGE = 10001;
const int32 UI_SPECIAL_MSG_DAMAGE_CRIT = 10002;
const int32 UI_SPECIAL_MESSAGE_HEAL = 10003;

const int32 UI_MESSAGE_TYPE_PORTRAIT_FOLLOWER = 4;
const int32 UI_MESSAGE_TYPE_PORTRAIT = 3;

const int32 UI_DISPLAY_MASK_PLAYER = 0x00000001;
const int32 UI_DISPLAY_MASK_PARTY = 0x00000002;
const int32 UI_DISPLAY_MASK_HOSTILE = 0x00000004;
const int32 UI_DISPLAY_MASK_ALL = 0x000000ff;

const float UI_DISPLAY_DURATION_DEFAULT = 3.0;
const float UI_DISPLAY_DURATION_CODEX = 3.0;
const float UI_DISPLAY_DURATION_POPUP = 3.0;
const float UI_DISPLAY_DURATION_ABILITY = 1.5;
const float UI_DISPLAY_DURATION_DAMAGE = 1.5;
const float UI_DISPLAY_DURATION_XP = 1.5;

const int32 UI_DISPLAY_COLOR_DEFAULT = 0xFFFFFF;
const int32 UI_DISPLAY_COLOR_CODEX = 0xFFFFFF;
const int32 UI_DISPLAY_COLOR_POPUP = 0xFFFFFF;
const int32 UI_DISPLAY_COLOR_ABILITY = 0xFFFF00;
const int32 UI_DISPLAY_COLOR_DAMAGE = 0xFFFFFF;
const int32 UI_DISPLAY_COLOR_XP = 0xFFFFFF;
const int32 UI_DISPLAY_COLOR_ERROR = 0xFF0000;

void UI_DisplayAbilityMessage(AActor* oTarget, int32 nAbility, int32 nDeactivate = FALSE_);

void UI_DisplayMessage(AActor* oTarget, int32 nMessage, FString sParam = "", int32 nColor = 0);

// -----------------------------------------------------------------------------
// @author: georg
//
// Check the AActor* mask setting for the message to determine whether or not
// it can be shown on the selected AActor* type
// -----------------------------------------------------------------------------
int32 UI_CheckObjectMask(int32 nMessage, AActor* oTarget);

int32 UI_CheckDisplayMask(int32 nMessage, AActor* oTarget);