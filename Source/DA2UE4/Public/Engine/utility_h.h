#pragma once

#include "ldf.h"
#include "log_h.h"

/** @brief Makes a team go hostile.
*
* The specified team joins the hostile faction.
* 0 is not a valid parameter (because that's the default value of the variable).
*
* @param nTeamID - This is the team ID number (stored in CREATURE_TEAM_ID)
* @param bHostile - Default behavior is to turn the team to GROUP_HOSTILE. If this is set to FALSE_, then instead the team will turn to GROUP_NEUTRAL.
* @author Ferret
**/
void UT_TeamGoesHostile(int32 nTeamID, int32 bHostile = TRUE_);

/** @brief Returns an array of all team members in the area.
*
* This function searches through all the creatures in the area and returns
* any creature that has it's team set to nTeamID
*
* @param nTeamID - This is the team ID number (stored in CREATURE_TEAM_ID)
* @param nMembersType - The type of members to retrieve (OBJECT_TYPE_CREATURE, OBJECT_TYPE_PLACEABLE)
* @author joshua
**/
TArray<ADA2UE4Creature*> UT_GetTeam(int32 nTeamID, int32 nMembersType = OBJECT_TYPE_CREATURE);

/** @brief Gets two creature to start combat.
*
* This function starts combat by turning 2 creatures hostile towards each other.
* It is assumed that the perception system will trigger combat once both sides are hostile.
* This function will switch the creature's group to be the 'hostile' group if it's current group is 'non-hostile'
* No other groups will be switched | for these cases the function will just set the 2 groups hostile.
*
* @param oAttacker - The attacking creature or the creature who initiates combat. This will not matter most of the time
* unless we are using the bTargetSelectionOverride parameter.
* @param oTarget - The target creature or the creature who is being attacked. This will not matter most of the time
* unless we are using the bTargetSelectionOverride parameter.
* @param bTargetSelectionOverride - if TRUE_ this will override the default target selection for the attacker for the first few rounds
* @param nOverridePermanent - if TRUE_ the attacker will not leave the specified target until it is dead.
* and will force the attacker to target the specified target. Otherwise they will just turn hostile and the AI
* system will decide who attacks who.
* @author Yaron
**/
void UT_CombatStart(AActor* oAttacker, AActor* oTarget, int32 bTargetSelectionOverride = FALSE_, int32 nOverridePermanent = FALSE_);

/** @brief Returns the shout flag for an AActor
*
* brief Returns the shout flag for an AActor
*
* @param oActor - The AActor* whose shouts flag is being changed
* @param bEnable - TRUE_ - enable shouts, FALSE_ - disable
* @returns the shouts flag for the creature (TRUE_ or FALSE_)
* @sa UT_GetShoutsFlag
* @author Yaron
*/
int32 UT_GetShoutsFlag(AActor* oActor);

/** @brief Sets an AActor* to use shouts in his dialog file.
*
* This function sets an AActor* to use shouts whenever dialog is triggered.
* The function sets a variable on the AActor, that is read and cleared by
* Generic conversation flags. The flags can then be used to flag a shouts tree
* in the conversation.
*
* @param oActor - The AActor* whose shouts flag is being changed
* @param bEnable - TRUE_ - enable shouts, FALSE_ - disable
* @sa UT_GetShoutsFlag
* @author Yaron
*/
void UT_SetShoutsFlag(AActor* oActor, int32 bEnable);

/** @brief Instantly initiate dialog with 2 objects
*
* Calling this function will instantly trigger dialog between 2 objects. The dialog
* can be ambient or not.
*
* @param oInitiator - The main talking creature - owner of the default dialog file, if any
* @param oTarget    - The creature being spoken to. Should be the player AActor* most of the time
* @returns TRUE_ on success, FALSE_ on error
* @author Yaron
*/
void UT_Talk(AActor* oTarget, int32 nConversation, int32 nPartyResurrection = TRUE_);

/** @brief Returns the nearest creature
*
* Returns the nearest creature
*
* @param aActor - the AActor* that we try to find a nearest creature from
* @returns the nearest creature to aActor
* @sa UT_GetNearestCreatureByTag, UT_GetNearestObjectByTag, UT_GetNearestCreatureByGroup, UT_GetNearestHostileCreature
* @author Yaron

*/
AActor* UT_GetNearestCreature(AActor* aActor, int32 nIncludeSelf = FALSE_);

/** @brief Returns the nearest creature with a specific tag
*
* Returns the nearest creature with a specific tag
*
* @param aActor - the AActor* that we try to find a nearest creature from
* @param sTag - the tag of the creature we are looking for
* @returns the nearest creature to aActor with the specified tag
* @sa UT_GetNearestCreature, UT_GetNearestObjectByTag, UT_GetNearestCreatureByGroup, UT_GetNearestHostileCreature
* @author Yaron
*/
AActor* UT_GetNearestCreatureByTag(AActor* aActor, FString sTag, int32 nIncludeSelf = FALSE_);

/** @brief Returns the nearest creature from a specific group
*
* Returns the nearest creature from a specific group
*
* @param aActor - the AActor* from which we are trying to find a nearest creature from
* @param nGroup - the group of the creature we are looking for
* @returns the nearest creature to aActor from a specific group
* @sa UT_GetNearestCreature, UT_GetNearestCreatureByTag, UT_GetNearestObjectByTag, UT_GetNearestHostileCreature
* @author Yaron
*/
AActor* UT_GetNearestCreatureByGroup(AActor* aActor, int32 nGroup, int32 nIncludeSelf = FALSE_);

/** @brief Returns the nearest living hostile creature
*
* Returns the nearest living hostile creature
*
* @param aActor - the AActor* from which we are trying to find a nearest creature from
* @param nGroup - the group of the creature we are looking for
* @returns the nearest creature to aActor from a specific group
* @sa UT_GetNearestCreature, UT_GetNearestCreatureByTag, UT_GetNearestObjectByTag, UT_GetNearestCreatureByGroup
* @author Yaron
*/
AActor* UT_GetNearestHostileCreature(AActor* aActor, int32 nCheckLiving = FALSE_);

/** @brief Returns the one nearest AActor* to the specified AActor
*
* @param * aActor - target Object
* @param * nIncludeSelf - TRUE if the query should check the target AActor
* @param * sTag - Tag for the objects to query
*/
AActor* UT_GetNearestObjectByTag(AActor* aActor, FString sTag, int32 nIncludeSelf = 0);
