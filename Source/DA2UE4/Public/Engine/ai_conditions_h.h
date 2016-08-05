#pragma once

//////////////////////////////////////////////
// ai_conditions_h
//
// This script includes all tactics conditions related code
//
// Owner: Yaron Jakobs
//
/////////////////////////////////////////////

/** @addtogroup scripting_ai_conditions Scripting AI Conditions handling
*
* This script includes all tactics conditions related code
*/
/** @{*/

#include "ai_constants_h.h"
#include "log_h.h"
#include "ability_h.h"
#include "items_h.h"
#include "ai_threat_h.h"
//#include "party_h.h"
//#include "global_objects_h.h"
//#include "ai_behaviors_h.h"
//#include "monster_constants_h.h"

/** @brief Returns a list of creatures that are allied to the current creature
*
* Ally creatures are defined as creatures that are in the same group. This function
* does not return all the allies - but a limited number of them. The returned list
* is sorted by proximity to the current creature.
*
* @param nCommandType type of command - if ability the condition will avoid returning an enemy that has an ability applied
* @param nCommandSubType subtype of command - if ability the condition will avoid returning an enemy that has an ability applied
* @returns an array of allied creatures sorted by distance
* @author Yaron
*/
TArray<AActor*> _AI_GetAllies(AActor* OBJECT_SELF, int32 nCommandType, int32 nCommandSubType);

// Checks if the target can make use of beneficial abilities like cure poison etc.
int32 _AI_IsTargetValidForBeneficialAbility(AActor* oCreature, int32 nAbilityID);

/** @brief Checks a creature has a specific AI status applied
*
* @param oCreature the creature we are checking for an AI status
* @nAIStatus the AI status we are checking on the creature (AI_STATUS_***)
* @returns TRUE_ if the creature has the specified AI status, FALSE_ otherwise
* @author Yaron
*/
int32 _AI_HasAIStatus(AActor* oCreature, int32 nAIStatus);

/** @brief Checks if a target is valid to be hostile target
*
* The checks includes: hostility, dead check, dying check, special AI ignore flags.
*
* @param oTarget the target being checked to see if valid as hostile target
* @returns TRUE_ if target is valid as hostile target, FALSE_ otherwise
* @author Yaron
*/
int32 _AI_IsHostileTargetValid(AActor* OBJECT_SELF, AActor* oTarget);

// If target type is SELF: Returns OBJECT_SELF if I'm being attacked by the specified attack type (melee, ranged or magic)
// If target type is ALLY: Returns nearest ALLY that is being attacked by the specified attack type
AActor* _AI_Condition_BeingAttackedByAttackType(AActor* OBJECT_SELF, int32 nTargetType, int32 nAttackType, int32 nCommandType, int32 nCommandSubType, int32 nTacticID);

/** @brief Returns a list of creatures that are hostile to the current creature
*
* Enemy creatures are defined as creatures that are hostile. This function
* does not return all the allies - but a limited number of them. The returned list
* is sorted by proximity to the current creature.
*
* @param nCommandType type of command - if ability the condition will avoid returning an enemy that has an ability applied
* @param nCommandSubType subtype of command - if ability the condition will avoid returning an enemy that has an ability applied
* @param nTargetTypeOfFollower - used only for followers - if the follower already attacks someone in melee he'd prefer doing it in some cases
* @returns an array of allied creatures sorted by distance
* @author Yaron
*/
TArray<AActor*> _AI_GetEnemies(AActor* OBJECT_SELF, int32 nCommandType, int32 nCommandSubType, int32 nTargetTypeOfFollower = -1);

/** @brief Returns override target, if any
*
* An override target can be specified when using the UT_CombatStart function. This
* target will then be used instead of trying to find something with other target-looking routines
* like GetNearest or GetMostHated.
*
* @returns override target, nullptr otherwise
* @author Yaron
*/
AActor* _AI_GetTargetOverride(AActor* OBJECT_SELF);

// Returns the CURRENT attack type of the creature (using melee, ranged or magic)
int32 _AI_GetAttackType(AActor* oCreature);

/** @brief Returns the Nth most hated enemy
*
* @params n the Nth most hated enemy
* @param nCommandType type of command - if ability the condition will avoid returning an enemy that has an ability applied
* @param nCommandSubType subtype of command - if ability the condition will avoid returning an enemy that has an ability applied
* @returns the Nth most hated enemy. 
* @NO it doesn't, as 'int32 n' is never used in the function body :D
* @author Yaron
*/
AActor* _AI_Condition_GetMostHatedEnemy(AActor* OBJECT_SELF, int32 n, int32 nCommandType, int32 nCommandSubType, int32 nTacticID);

// Returns a party member target, if valid
AActor* _AI_GetPartyTarget(AActor* OBJECT_SELF, int32 nTargetType, int32 nCommandType, int32 nCommandSubType, int32 nTacticID);

/** @brief Returns a creature with a specifc AI Status
*
* When looking for a target of type SELF: the function will return OBJECT_SELF if the status is valid
* When looking for a target of type ALLY: the function will return the nearest ally with the status
* When looking for a target of type ENEMY: the function will return the nearest enemy with the status
*
* @param nAIStatus the status being checked for if active on a creature
* @param nTargetType the target type of creature we are looking for that might have the specified status
* @param nCommandType type of command - if ability the condition will avoid returning an enemy that has an ability applied
* @param nCommandSubType subtype of command - if ability the condition will avoid returning an enemy that has an ability applied
* @returns A creature of a specific type (self, ally, hostile) that has the status applied, nullptr otherwise
* @author Yaron
*/
AActor* _AI_Condition_GetCreatureWithAIStatus(AActor* OBJECT_SELF, int32 nAIStatus, int32 nTargetType, int32 nCommandType, int32 nCommandSubType, int32 nTacticID);

/** @brief Returns a creature with a specific HP level
*
* When looking for a target of type SELF: the function will return OBJECT_SELF if the creature HP at the specified level
* When looking for a target of type ALLY: the function will return the nearest ally HP with the specified level
* When looking for a target of type ENEMY: the function will return the nearest enemy with HP of the specified level
*
* @params nHPLevel a percentage value (25, 50, etc') representing a wounded level. A creature is considered 'wounded'
* for that level if his HP are below the level specified. For positive values: the check is greater or equal (50% ->
* creature >= 50%), for negative values: the check is lower (-50% -> creature < 50%)
* @params nTargetType the target type of creature we are looking for that might have his HP at the specified level
* @param nCommandType type of command - if ability the condition will avoid returning an enemy that has an ability applied
* @param nCommandSubType subtype of command - if ability the condition will avoid returning an enemy that has an ability applied
* @returns A creature of a specific type (self, ally, hostile) that has his HP are at the specified level, nullptr otherwise
* @author Yaron
*/
AActor* _AI_Condition_GetCreatureWithHPLevel(AActor* OBJECT_SELF, int32 nHPLevel, int32 nTargetType, int32 nCommandType, int32 nCommandSubType, int32 nTacticID);

/** @brief Returns a creature with a specific Mana or Stamina level
*
* When looking for a target of type SELF: the function will return OBJECT_SELF if the creature Mana or Stamina at the specified level
* When looking for a target of type ALLY: the function will return the nearest ally Mana or Stamina with the specified level
* When looking for a target of type ENEMY: the function will return the nearest enemy with Mana or Stamina of the specified level
*
* @params nManaOrStaminaLevel a percentage value (25, 50, etc') representing a wounded level. A creature is considered 'wounded'
* for that level if his Mana are below the level specified. For positive values: the check is greater or equal (50% ->
* creature >= 50%), for negative values: the check is lower (-50% -> creature < 50%)
* @params nTargetType the target type of creature we are looking for that might have his Mana or Stamina at the specified level
* @param nCommandType type of command - if ability the condition will avoid returning an enemy that has an ability applied
* @param nCommandSubType subtype of command - if ability the condition will avoid returning an enemy that has an ability applied
* @returns A creature of a specific type (self, ally, hostile) that has his Mana or Stamina are at the specified level, nullptr otherwise
* @author Yaron
*/
AActor* _AI_Condition_GetCreatureWithManaOrStaminaLevel(AActor* OBJECT_SELF, int32 nManaOrStaminaLevel, int32 nTargetType, int32 nCommandType, int32 nCommandSubType, int32 nTacticID);

/** @brief A sub-condition function for the HPLevel and ManaStaminaLevel condition functions
*
* Contains all the logic for all related condition functions
*
* @params nStatType the stat type we are checking the level for (AI_STAT_TYPE_*)
* @params nStatLevel a percentage value (25, 50, etc') representing a wounded level. A creature is considered 'wounded'
* for that level if his Mana are below the level specified. For positive values: the check is greater or equal (50% ->
* creature >= 50%), for negative values: the check is lower (-50% -> creature < 50%)
* @params nTargetType the target type of creature we are looking for that might have his Mana or Stamina at the specified level
* @param nCommandType type of command - if ability the condition will avoid returning an enemy that has an ability applied
* @param nCommandSubType subtype of command - if ability the condition will avoid returning an enemy that has an ability applied
* @returns A creature of a specific type (self, ally, hostile) that has his stat at the specified level, nullptr otherwise
* @author Yaron
*/
AActor* _AI_SubCondition_GetCreatureWithStatLevel(AActor* OBJECT_SELF, int32 nStatType, int32 nStatLevel, int32 nTargetType, int32 nCommandType, int32 nCommandSubType, int32 nTacticID);

/** @brief Returns the nth most damaged creature in a group
*
* When looking for a target of type ALLY: the function will return the Nth most damaged ally
* When looking for a target of type ENEMY: the function will return the Nth most damaged enemy
*
* @params nMostDamagedNum to specify N for the Nth most damaged enemy
* @params nTargetType the target type of group we're looking for the most damaged enemy in
* @param nCommandType type of command - if ability the condition will avoid returning an enemy that has an ability applied
* @param nCommandSubType subtype of command - if ability the condition will avoid returning an enemy that has an ability applied
* @returns the Nth most damaged creature in the group
* @author Yaron
*/
AActor* _AI_Condition_GetNthMostDamagedCreatureInGroup(AActor* OBJECT_SELF, int32 nMostDamagedNum, int32 nTargetType, int32 nCommandType, int32 nCommandSubType, int32 nTacticID);

/** @brief Returns the nearest creature clustered with the same group of a minimum size
*
* It is assumed the cluster is for a generic AOE size, a bit smaller than a fireball.
* The function only looks for enemies and does not care for distance or if the enemies are moving or not.
*
* @params nMinClusterSize the minimum number of enemies to be considered clustered
* @returns the center location of the cluster
* @author Yaron
*/
FVector _AI_Condition_GetEnemyClusteredWithSameGroup(AActor* OBJECT_SELF, int32 nMinClusterSize, int32 nCommandType, int32 nCommandSubType, int32 nTacticID);

/** @brief Returns the Nth nearest visible enemy
*
* @params n Nth nearest visible
* @returns the Nth nearest visible enemy
* @param nCommandType type of command - if ability the condition will avoid returning an enemy that has an ability applied
* @param nCommandSubType subtype of command - if ability the condition will avoid returning an enemy that has an ability applied
* @author Yaron
*/
AActor* _AI_Condition_GetNearestVisibleCreature(AActor* OBJECT_SELF, int32 nTargetType, int32 n, int32 nCommandType, int32 nCommandSubType, int32 nTacticID);

/** @brief Returns the nearest visible enemy of a specific race
*
* @params nRace the race of the enemy being looked for
* @param nCommandType type of command - if ability the condition will avoid returning an enemy that has an ability applied
* @param nCommandSubType subtype of command - if ability the condition will avoid returning an enemy that has an ability applied
* @returns the nearest visible enemy of the specified race
* @author Yaron
*/
AActor* _AI_Condition_GetNearestVisibleCreatureByRace(AActor* OBJECT_SELF, int32 nTargetType, int32 nRace, int32 nCommandType, int32 nCommandSubType, int32 nTacticID);

/** @brief Returns the nearest visible enemy of a specific class
*
* @params nClass the class of the enemy being looked for
* @param nCommandType type of command - if ability the condition will avoid returning an enemy that has an ability applied
* @param nCommandSubType subtype of command - if ability the condition will avoid returning an enemy that has an ability applied
* @returns the nearest visible enemy of the specified class
* @author Yaron
*/
AActor* _AI_Condition_GetNearestVisibleCreatureByClass(AActor* OBJECT_SELF, int32 nTargetType, int32 nClass, int32 nCommandType, int32 nCommandSubType, int32 nTacticID);

/** @brief Returns the nearest visible enemy of a specific gender
*
* @params nGender the gender of the enemy being looked for
* @param nCommandType type of command - if ability the condition will avoid returning an enemy that has an ability applied
* @param nCommandSubType subtype of command - if ability the condition will avoid returning an enemy that has an ability applied
* @returns the nearest visible enemy of the specified gender
* @author Yaron
*/
AActor* _AI_Condition_GetNearestVisibleCreatureByGender(AActor* OBJECT_SELF, int32 nTargetType, int32 nGender, int32 nCommandType, int32 nCommandSubType, int32 nTacticID);

/** @brief Returns the nearest enemy attacking any or a specific party member
*
* @params nTargetType the target follower
* @param nCommandType type of command - if ability the condition will avoid returning an enemy that has an ability applied
* @param nCommandSubType subtype of command - if ability the condition will avoid returning an enemy that has an ability applied
* @returns the nearest enemy attacking the specified party member
* @author Yaron
*/
AActor* _AI_Condition_GetNearestEnemyAttackingPartyMember(AActor* OBJECT_SELF, int32 nCommandType, int32 nCommandSubType, int32 nPartyMemberType, int32 nTacticID);

/** @brief Returns the nearest enemy with any magical buff applied to it
*
* @param nCommandType type of command - if ability the condition will avoid returning an enemy that has an ability applied
* @param nCommandSubType subtype of command - if ability the condition will avoid returning an enemy that has an ability applied
* @param nRangeID the type of range the enemy must be in
* @returns the nearest enemy with any magical buff applied to it
* @author Yaron
*/
AActor* _AI_Condition_GetNearestEnemyWithAnyBuffEffect(AActor* OBJECT_SELF, int32 nCommandType, int32 nCommandSubType, int32 nRangeID, int32 nTacticID, int32 nTargetType);

/** @brief Returns the nearest flip-cover placeable by state
*
* Can be used to look for placeables that are ready for flip-cover or being used by someone as flip cover
*
* @returns the nearest flip cover by the specified state, or nullptr if none
* @param nFlipState State can be AI_FLIP_COVER_READY_FOR_USE or AI_FLIP_COVER_USED_BY_ENEMY.
* @author Yaron
*/
AActor* _AI_Condition_GetNearestFlipCoverByState(AActor* OBJECT_SELF, int32 nFlipState, int32 nTacticID);

/** @brief Returns the nearest enemy vulnerable to a specific damage type
*
* 'Vulnerable' means the creature will be damaged more than usual with being hit by a damage of the specified type
*
* @returns the nearest enemy vulnerable to the specified damage
* @param nDamageType the type of damage the creature should be vulnerable to
* @param nCommandType type of command - if ability the condition will avoid returning an enemy that has an ability applied
* @param nCommandSubType subtype of command - if ability the condition will avoid returning an enemy that has an ability applied
* @author Yaron
*/
AActor* _AI_Condition_GetEnemyVulnerableToDamage(AActor* OBJECT_SELF, int32 nDamageType, int32 nCommandType, int32 nCommandSubType, int32 nTacticID);

/** @brief Returns any target that fits the type
*
* If more than one target is optional than the nearest is returned
*
* @returns any target of a specific type
* @param nTargetType the type of the target we are looking for
* @param nCommandType type of command - if ability the condition will avoid returning an enemy that has an ability applied
* @param nCommandSubType subtype of command - if ability the condition will avoid returning an enemy that has an ability applied
* @author Yaron
*/
AActor* _AI_Condition_GetAnyTarget(AActor* OBJECT_SELF, int32 nTargetType, int32 nCommandType, int32 nCommandSubType, int32 nTacticID);

// Returns OBJECT_SELF if ammo level is appropriate
AActor* _AI_Condition_SelfHasAmmoLevel(AActor* OBJECT_SELF, int32 nAmmoLevel);

// Returns OBJECT_SELF if the most hated enemy has the specified armor type
AActor* _AI_Condition_HasArmorType(AActor* OBJECT_SELF, int32 nTargetType, int32 nArmorType, int32 nCommandType, int32 nCommandSubType);

// Returns OBJECT_SELF if the most enemies have the specified armor type
AActor* _AI_Condition_MostEnemiesHaveArmorType(AActor* OBJECT_SELF, int32 nArmorType);

// Returns OBJECT_SELF if all enemies have the specified armor type
AActor* _AI_Condition_AllEnemiesHaveArmorType(AActor* OBJECT_SELF, int32 nArmorType);

// If target type is SELF: returns OBJECT_SELF if the most hated enemy is at least at the specified rank
// If target type is ENEMY: returns the most hated enemy if his rank is at least as the specified rank
AActor* _AI_Condition_TargetHasRank(AActor* OBJECT_SELF, int32 nTargetType, int32 nRank, int32 nTacticID, int32 nCommandType, int32 nCommandSubType);

// Returns OBJECT_SELF if most hated enemy is using the specified attack type
AActor*  _AI_Condition_UsingAttackType(AActor* OBJECT_SELF, int32 nTacticTargetType, int32 nTacticCondition_Parameter, int32 nTacticCommand, int32 nTacticSubCommand);

// Returns OBJECT_SELF if most enemies are using the specified attack type
AActor* _AI_Condition_MostEnemiesUsingAttackType(AActor* OBJECT_SELF, int32 nAttackType);

// Returns OBJECT_SELF if all enemies are using the specified attack type
AActor* _AI_Condition_AllEnemiesUsingAttackType(AActor* OBJECT_SELF, int32 nAttackType);

// Returns OBJECT_SELF if at least X enemies are alive
AActor* _AI_Condition_AtLeastXEnemiesAreAlive(AActor* OBJECT_SELF, int32 nTargetType, int32 nNum);

// Returns OBJECT_SELF if at least X enemies are dead
AActor* _AI_Condition_AtLeastXCreaturesAreDead(AActor* OBJECT_SELF, int32 nTacticTargetType, int32 nNum, int32 nTacticCommand, int32 nTacticSubCommand);

// Returns OBJECT_SELF if at least X allies are alive
AActor* _AI_Condition_AtLeastXAlliesAreAlive(AActor* OBJECT_SELF, int32 nTargetType, int32 nNum, int32 nParam2);

// Returns the nearest target within the specified range
AActor* _AI_Condition_GetTargetAtRange(AActor* OBJECT_SELF, int32 nTargetType, int32 nRange, int32 nCommandType, int32 nCommandSubType);

// Returns a target at the specified flank or nullptr if none
AActor* _AI_Condition_GetTargetAtFlankLocation(AActor* OBJECT_SELF, int32 nFlankType, int32 nTargetType);

// Returns OBJECT_SELF if surrounded by the specified number of enemies
AActor* _AI_Condition_SurroundedByAtLeastXEnemies(AActor* OBJECT_SELF, int32 nTacticCommand, int32 nTacticSubCommand, int32 nNumOfTargets, int32 nTacticID);

// Returns a creature that currently uses a ranged attack while being at a certain distance
AActor* _AI_Condition_GetTargetUsingRangedWeaponsAtRange(AActor* OBJECT_SELF, int32 nTargetType, int32 nRange, int32 nCommandType, int32 nCommandSubType);

// Returns the current target of a specific party member (nTargetType)
AActor* _AI_Condition_GetPartyMemberTarget(AActor* OBJECT_SELF, int32 nTacticCommand, int32 nTacticSubCommand, int32 nPartyMemberType, int32 nTacticID);

// Returns the nearest target of the specified type if SELF HP level is valid
AActor* _AI_Condition_SelfHPLevel(AActor* OBJECT_SELF, int32 nHPLevel, int32 nTargetType, int32 nCommandType, int32 nCommandSubType, int32 nTacticID);

// Returns the nearest target of the specified type if SELF Mana or stamina level is valid
AActor* _AI_Condition_SelfManaStaminaLevel(AActor* OBJECT_SELF, int32 nManaStaminaLevel, int32 nTargetType, int32 nCommandType, int32 nCommandSubType, int32 nTacticID);

// Returns creature with highest or lowest health in a group
// This is a very good candidate for being in the engine
// Currently supports only 1st most damaged, anything else will fail
AActor* _AI_GetNthDamagedCreatureInArray(AActor* OBJECT_SELF, TArray<AActor*> arCreatures, int32 nHighLow);

/** @brief Checks a creature has a specific stat (HP, mana or stamina) within a specific range
*
* This function should be used when a creature has HP >= 50% or mana < 20% etc'.
*
* @param oCreature the creature we are checking for a stat level
* nStatType the stat type we are looking for a range (AI_STAT_TYPE_*)
* nStatLevel the level of the stat we are checking for. This should be a percentage number (50 for 50%).
* A positive number should be for checks like HP >= 50%, a negative number should be for checks like HP < 50%.
* @returns TRUE_ if the creature has the stat in the required level, FALSE_ otherwise
* @author Yaron
*/
int32 _AI_HasStatLevel(AActor* OBJECT_SELF, AActor* oCreature, int32 nStatType, int32 nStatLevel);

/** @brief Checks if a creature has any normal buff effect
*
* Normal buff effects are effects that can be dispelled.
*
* @returns TRUE_ if the creature has any buff effect active, FALSE_ otherwise
* @author Yaron
*/
int32 _AI_HasAnyBuffEffect(AActor* oCreature);

float _AI_GetRangeFromID(int32 nRangeID);

/** @brief Checks if a creature is vulnerable to a specific damage type
*
* @returns TRUE_ if the creature is vulnerable, FALSE_ otherwise
* @author Yaron
*/
int32 _AI_IsVulnerableToDamage(AActor* oCreature, int32 nDamageType);

int32 _AI_GetArmorType(AActor* oCreature);