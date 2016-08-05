#pragma once
#include "ldf.h"

const int32 AI_THREAT_SIZE = 5;                           // Max number of creatures in threat list
const float AI_THREAT_MIN = 0.0;                        // Min threat value
const float AI_THREAT_MAX = 1000.0;                       // Max threat value
const float AI_THREAT_DISSOLVE = -0.5;                    // Amount of threat lowered each second
const float AI_THREAT_ROGUE_DAMAGE_RATIO = 0.8;         // rogues takes less threat from damage
const float AI_THREAT_VALUE_DIRECT_DAMAGE = 1.0;        // Multiplier for damage threat
const float AI_THREAT_VALUE_DIRECT_HEALING = 1.0;       // Multiplier for healing threat
const float AI_THREAT_VALUE_ALLY_DAMAGE = 0.5;          // Multiplier for ally damage threat
const float AI_THREAT_VALUE_ATTACKED = 1.0;             // A constant value of threat to apply only when being attacked
const float AI_THREAT_VALUE_ENEMY_PERCEIVED = 10.0;     // A max setting for a random value that is set when first perceived a hostile enemy
const float AI_THREAT_PERCEIVED_ARMOR_LIGHT = 5.0;       // A constant value to add to perception threat when perceiving someone in light armor
const float AI_THREAT_PERCEIVED_ARMOR_MEDIUM = 10.0;       // A constant value to add to perception threat when perceiving someone in light armor
const float AI_THREAT_PERCEIVED_ARMOR_HEAVY = 10.0;       // A constant value to add to perception threat when perceiving someone in light armor
const float AI_THREAT_PERCEIVED_ARMOR_MASSIVE = 10.0;       // A constant value to add to perception threat when perceiving someone in light armor
const float AI_THREAT_PERCEIVED_WEAPON_MELEE = 5.0;    // A constant value to add to perception threat
const float AI_THREAT_HATED_ENEMY_COEFFICIENT = 2.0;    // Multiply any threat chance by this value if coming from a hated target
const float AI_THREAT_ABILITY_USE_THREAT_COEFF = 2.0;         // The amount of threat per point assigned to the ability to apply when the ability is fired
const float AI_THREAT_ABILITY_IMPACT_THREAT_COEFF = 10.0;      // The amount of threat per point assign to the ability to apply when the ability hits the target
const int32 AI_THREAT_SWITCH_TIMER_JUMP = 5;               // The amount of seconds the switch timer increases each time there is a switch
const int32 AI_THREAT_SWITCH_TIME_MAX = 25;               // The maximum amount of switch timer. The timer revers to 0 afterwards
const float AI_THREAT_MIN_CHANGE = 1.0;                 // The minimum value of each threat change
const float AI_THREAT_PASSIVE_RANGE = 30.0;              // The range other creatures need to be in order to hate something that happens in the middle of that area
const int32 AI_THREAT_CHANCE_KEEP_TARGET_20_HEALTH = 50; // the chance to keep the current target if it has 20% or less health (on top of other timer restrictions)
const int32 AI_THREAT_CHANCE_KEEP_TARGET_10_HEALTH = 90; // the chance to keep the current target is it has 10% or less health (on top of other timer restrictions)
const float AI_THREAT_CANT_ATTACK_TARGET_REDUCTION = 0.75;  // how much  to reduce the threat if can't attack the target
const float AI_THREAT_NON_DAMAGING_AOE_EXTRA = 20.0; // extra threat fired when enemies go on non-hostile AOE and only on hard difficulty.														

/** @brief Updates threat based on losing sight of a creature
*
* Remove the enemy creature from the current creature's threat list and remove the current creature
* from the enemy's inverse threat list.
*
* @param oCreature the creature perceiving the enemy
* @param oEnemy the hostile creature by perceived by oCreature
* @author Yaron
*/
void AI_Threat_UpdateEnemyDisappeared(AActor* oCreature, AActor* oEnemy);

void AI_Threat_Display(AActor* oCreature, FString sMes);

/** @brief Returns most hated enemy
*
* This function is the main interface for the threat system. It will mostly return
* the most hated enemy, but it sometimes can be not the most hated (to avoid too many target switching)
*
* @param oCreature the creature for which we need a target for
* @param bUpdateThreatTarget whether or not to update the threat target during the check
* @returns most hated enemy
* @author Yaron
*/
AActor* AI_Threat_GetThreatTarget(AActor* oCreature, int32 bUpdateThreatTarget = TRUE_);

/** @brief Handle the update of threat for a single creature
*
* Do any global handling and call the engine function that handles most of the logic
*
* @param oCreature the hating creature
* @param oEnemy the hated creature
* @param fThreatChange the threat before being changed
* @author Yaron
*/
void AI_Threat_UpdateCreatureThreat(AActor* oCreature, AActor* oEnemy, float fThreatChange);

/** @brief returns updates threat value based on hated targets
*
* If the creature hates the threat target because of race, gender or class - the threat may increase.
*
* @param oCreature the hating creature
* @param oThreatTarget the hated creature
* @param fThreat the threat before being changed
* @returns the updated threat, if any, based on hated target
* @author Yaron
*/
float AI_Threat_GetHatedThreat(AActor* oCreature, AActor* oThreatTarget, float fThreat);

// Updated the threat target for oCreature
void AI_Threat_SetThreatTarget(AActor* oCreature, AActor* oTarget);

// Returns TRUE_ if this target should be kept, on top of any other threat rules
int32 AI_Threat_KeepWoundedTarget(AActor* oTarget);

// switch check
int32 AI_Threat_ClearToSwitchTarget(AActor* oCreature);

/** @brief Updates threat based perceiving a creature
*
* Update the threat for the current creature based on the enemy that was perceived.
*
* @param oCreature the creature perceiving the enemy
* @param oEnemy the hostile creature by perceived by oCreature
* @author Yaron
*/
void AI_Threat_UpdateEnemyAppeared(AActor* oCreature, AActor* oEnemy);

// This is fired when a stationary creature is trying to attack a target that is beyond 
// the range of his abilities or with no line of sight.
void AI_Threat_UpdateCantAttackTarget(AActor* oCreature, AActor* oTarget);