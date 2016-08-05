#pragma once
//////////////////////////////////////////////
// ai_main_h
//
// This script includes all general AI functions
//
// Owner: Yaron Jakobs
//
/////////////////////////////////////////////

/** @addtogroup scripting_ai2 Scripting AI handling
*
* Main AI interface functions
*/
/** @{*/

#include "ldf.h"
#include "ai_threat_h.h"
#include "log_h.h"
#include "wrappers_h.h"
#include "ai_threat_h.h"
#include "events_h.h"
//#include "effects_h.h"
#include "items_h.h"
//#include "ability_h.h"
#include "ai_conditions_h.h"
#include "ai_constants_h.h"
//#include "ai_ballista_h.h"
#include "ai_behaviors_h.h"

/** @brief Determines the exact action to take for the next combat round for the current creature
*
* This also includes acquiring a hostile target for offensive attacks and acquiring a friendly target
* for buffs and heals. The function assumes we handle OBJECT_SELF as the attacking AActor
* This function will run for all creature types: enemy, controlled follower and non-controlled follower
* This function does NOT care about combat mode. It can run with or without it.
* Some cases might check the combat mode, but no code should try to enable to disable combat mode.
* It is assumed that some other external check were made to make sure that OBJECT_SELF can run this function.
*
* @param oLastTarget the target the attacker attacked last round (if valid)
* @param nLastCommand the last command executed
* @param nLastCommandStatus COMMAND_SUCCESSFUL or COMMAND_FAILURE - used mostly to detect movement failures
* @param nLastSubCommand last sub command (ability ID for use ability commands)
* @author Yaron
*/
void AI_DetermineCombatRound(AActor* OBJECT_SELF, AActor* oLastTarget = nullptr, int32 nLastCommand = -1, int32 nLastCommandStatus = COMMAND_SUCCESSFUL, int32 nLastSubCommand = -1);

// returns a command to wait or play a taunt animation
FCommand _AI_DoNothing(AActor* OBJECT_SELF, int32 nLastTacticID, int32 nLastCommandStatus, int32 nAllowTaunts = FALSE_, int32 bQuick = FALSE_, AActor* oTarget = nullptr, int32 nClearThreat = TRUE_);

// Light AI for creatures fighting other creatures away from the player
void AI_DetermineCombatRound_Light(AActor* OBJECT_SELF, AActor* oLastTarget = nullptr, int32 nLastCommand = -1, int32 nLastCommandStatus = COMMAND_SUCCESSFUL, int32 nLastSubCommand = -1);

// returns TRUE_ if cooldown is clear to run again, FALSE_ otherwise
int32 _AI_CheckMoveTimer(AActor* OBJECT_SELF);

void _AI_SetMoveTimer(AActor* OBJECT_SELF);

/** @brief Returns whether AI is enabled by the user for this creature.
*
* The user can choose to selectively disable AI on a per-creature basis.
*
* @param oCreature - the creature to query.
* @return - TRUE_ if AI is enabled for this creature.
* @author Jacques
*/
int32 IsPartyAIEnabled(AActor* aActor);

/** @brief Determines the exact action to take for the next combat round for a controlled party member
*
* This includes minimal AI handling
*
* @param oLastTarget the target the attacker attacked last round (if valid)
* @param nLastCommand the command the attacker used last round
* @param nLastCommandStatus COMMAND_SUCCESSFUL or COMMAND_FAILURE - used mostly to detect movement failures
* @param nLastSubCommand last sub command (ability ID for use ability commands)
* @author Yaron
*/
void AI_DetermineCombatRound_Partial(AActor* OBJECT_SELF, AActor* oLastTarget = nullptr, int32 nLastCommand = -1, int32 nLastCommandStatus = COMMAND_SUCCESSFUL, int32 nLastSubCommand = -1);

/** @brief Check if the specified target is within melee range
*
* @param oTarget the target we check melee range for
* @returns TRUE_ if oTarget is within melee range, FALSE_ otherwise
* @author Yaron
*/
int32 _AI_IsTargetInMeleeRange(AActor* OBJECT_SELF, AActor* oTarget);

// Returns the range of the currently equipped ranged weapon
float _AI_GetEquippedWeaponRange(AActor* OBJECT_SELF);

void AI_SetPartyAllowedToAttack(AActor* OBJECT_SELF, int32 nStatus);

int32 _AI_GetWeaponSetEquipped(AActor* oTarget);

/** @brief Check if the current creature has a specific weapon set available (non-equipped)
*
* @param nWeaponSetType the weapon set we are looking for, assuming it is the non-equipped weapon set
* @returns TRUE_ if the current creature has the weapon set, FALSE_ otherwise
* @author Yaron
*/
int32 _AI_HasWeaponSet(AActor* OBJECT_SELF, int32 nWeaponSetType);

/** @brief Switches the weapon set of the current creature to a weapon set of the specified type
*
* @param nWeaponSetType the weapon set we want to switch to
* @returns a command to switch the weapons
* @author Yaron
*/
FCommand _AI_SwitchWeaponSet(AActor* OBJECT_SELF, int32 nWeaponSetType);

/** @brief Returns the package table name ID that was set for the current creature
*
* @returns The name of the ID of table attached to the creature, or the default one if the creature has none
* @author Yaron
*/
int32 _AI_GetPackageTable(AActor* OBJECT_SELF);

// Returns TRUE_ if GUI tables should be used, FALSE_ if 2da tables should be used
int32 _AI_UseGUITables(AActor* OBJECT_SELF);

/** @brief Returns the number of tactics the creature has in it's package
*
* The number of tactics is basically the number of rows in the package 2da
*
* @param rPackageTable the package table for which we want the number of tactics in
* @returns Number of tactics the creature has in the associated package file
* @author Yaron
*/
int32 _AI_GetTacticsNum(int32 nPackageTable);

/** @brief Checks if a tactic is valid and executes it if valid
*
* @param nPackageTable the package table for the specified tactic
* @param nTacticID the tactic ID that we are trying to execute
* @param nLastCommandStatus used in AI_ExecuteAttack
* @returns TRUE_ if the tactic was executed, FALSE_ otherwise
* @author Yaron
*/
int32 _AI_ExecuteTactic(AActor* OBJECT_SELF, int32 nPackageTable, int32 nTacticID, int32 nLastCommandStatus, int32 nUseGUITables);

FString _AI_GetCommandString(int32 nAICommand);

/** @brief Verifies that a creature can execute a specific command
*
* The exact conditions depend on the command itself, but they include resources check (mana/stamina) equipment check (weapons) etc'
*
* @param nCommand the main command being checked
* @param nSubCommand the sub-command being checked
* @returns TRUE_ if the FCommand can be executed, FALSE_ otherwise
* @author Yaron
*/
int32 _AI_IsCommandValid(AActor* OBJECT_SELF, int32 nCommand, int32 nSubCommand, int32 nTacticTargetType = -1);

int32 _AI_IsAbilityValid(AActor* OBJECT_SELF, int32 nAbilityID);

// returns a potion based on criteria
AActor* _AI_GetPotionByFilter(AActor* OBJECT_SELF, int32 nPotionType, int32 nPotionPower);

// returns a return value for using a potion
FCommand _AI_GetPotionUseCommand(AActor* OBJECT_SELF, AActor* oItem);

// checks for AI-specifics conditions for using this ability on the required target
int32 _AI_CanUseAbility(AActor* OBJECT_SELF, int32 nAbility, AActor* oTarget);

FCommand _AI_GetFlyCommand(AActor* OBJECT_SELF, AActor* oTurnTo, int32 bMoveTo = FALSE_);

/** @brief Executes an attack command including possible weapon switching
*
* @param oTarget the target being attacked
* @param nLastCommandStatus used to determine what to do in case of failed movement or weapon switch
* @returns an attack or weapon switch command
* @author Yaron
*/
FCommand _AI_ExecuteAttack(AActor* OBJECT_SELF, AActor* oTarget, int32 nLastCommandStatus);

void _AI_ApplyTimerDifficultyEffects(AActor* OBJECT_SELF, AActor* oTarget);

/** @brief Sets the value of a specific AI flag on the current creature
*
* @param sFlag the flag we check for
* @author Yaron
*/
void _AI_SetFlag(AActor* OBJECT_SELF, FName sFlag, int32 nValue);

/** @brief Gets the value of a specific AI flag on the current creature
*
* @param nFlag the flag we check for
* @returns TRUE_ if the flag is set on the current creature, FALSE_ otherwise
* @author Yaron
*/
int32 _AI_GetFlag(AActor* OBJECT_SELF, FName Flag);

// Move to main controlled follower in formation
FCommand _AI_MoveToControlled(AActor* OBJECT_SELF, int32 nLastCommandStatus);

int32 AI_GetPartyAllowedToAttack(AActor* OBJECT_SELF);

// Executes the AI default action for DetermineCombatRound
void AI_ExecuteDefaultAction(AActor* OBJECT_SELF, AActor* oLastTarget = nullptr, int32 nLastCommand = 0, int32 nLastCommandStatus = COMMAND_SUCCESSFUL, int32 nLastSubCommand = -1);

void AI_HandleCowardFollower(AActor* OBJECT_SELF, AActor* oAppear = nullptr);

/** @brief Determines the exact action to take when an AActor* is blocking the way
*
* A few examples are bashing a container or door, lockpicking and then opening a gate, etc.
*
* @param oBlockingObject the AActor* blocking the path
* @returns TRUE_ if the AI found a solution to deal with the blocking AActor
* @author Jose
*/
int32 AI_DeterminePathBlockedAction(AActor* OBJECT_SELF, AActor* oBlockingObject);