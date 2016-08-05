#pragma once

#include "STypes.h"
#include "ldf.h"

class Engine;

/** @brief Returns the command type.
*
* Returns the command type.
*
*   @returns int32 - the type of command.
*   @param cCommand - The command.
*   @author Jacques Lebrun
*/
int32 GetCommandType(FCommand cCommand);

/** @brief This function returns the previously processed command for the specified AActor
*
*   @returns command - the previous command, use GetCommandType to see if it is COMMAND_INVALID
*   @author Jose
*/
FCommand GetPreviousCommand(AActor* aActor);

/** @brief This function returns current command for the specified AActor
*
* The 'current' command is the command that is currently being executed. It is considered
* outside of the command queue.
*
*   @returns command - the current command, use GetCommandType to see if it is COMMAND_INVALID
*   @author Sam
*/
FCommand GetCurrentCommand(AActor* aActor);

/** @brief This function returns the size of an AActor* command queue, note that the currently active command doesn't belong in the queue.
*
* This function returns the size of an AActor* command queue, note that the currently active command doesn't belong in the queue.
*
*   @param aActor - returns the size of this objects command queue
*   @returns int32
*   @author Sam
*/
int32 GetCommandQueueSize(AActor* aActor);

/** @brief Returns the command at the specified index in the command queue.
*
* Index '0' is the command at the top of the queue, but not being executed yet.
*
*   @returns command - the command at the specified index
*   @param oActor - the AActor* to get the command from
*   @param nIndex - the index to get the command from
*   @author Sam
*/
FCommand GetCommandByIndex(AActor* aActor, int32 nIndex);

/** @brief This function adds the specified command to the AActor.
*
* This function adds the specified command to the AActor* command queue.
* The command can be added to the back of the queue or to the front, and can also be
* flagged as static (must-finish command).
*
* @param oActor - the AActor* to add the specified command
* @param cCommand - the command to add to the AActor
* @param bAddToFront - specifies if the command should be added to the front of the queue or not
* @param bStatic - whether or not the command will be added as a static command. Static commands are flagged in a special manner and cannot be removed via regular clearing functionality. An override must specifically be specified to remove static commands. As such, commands should only be specified as static if they absolutely must finish.
* @param nOverrideAddBehavior - replace the default add behavior by specifying a new behavior here
* @returns TRUE_ on success, FALSE_ on failure
* @remarks Any duplicate commands in the queue that are adjacent to one another are deleted.
* @author Brenon
*/
int32 AddCommand(AActor* aActor, FCommand cCommand, int32 bAddToFront = FALSE_, int32 bStatic = FALSE_, int32 nOverrideAddBehavior = -1);

/** @brief Set the result of a command.
*
* @param aActor - the AActor* in which to set the result of the command
* @param nResult - the result of the command (success, failure)
* @author Jose
*/
void SetCommandResult(AActor* aActor, int32 nResult);

/** @brief This function is a play animation command constructor.
*
* This function is a play animation command constructor.
* It creates a play animation command which can then be added to any
* AActor's command queue. This command, when processed will attempt
* to play the specified animation on the AActor.
*
* @param nAnimation - the animation that the AActor* should play
* @param nLoops - The number of loops to play if its a looping animation or the next looping animation, if its a transition animation.
* @param bPlayNext - Indicates if the engine will automatically play the next animation after the initial animation and its looping animation.
* @param bBlendIn - 1 = fast blend (default), 2 = immediate (no blending)
* @param bRandomizeOffset - Start playing the animation at a random position
* @returns a valid command
* @author Brenon
*/
FCommand CommandPlayAnimation(int32 nAnimation, int32 nLoops = 0, int32 bPlayNext = 0, int32 bBlendIn = 1, int32 bRandomizeOffset = 0);

/** @brief Moves creature away from specified target AActor* at the specified distance
*
* If no clear line-of-sight exists between our current position and any position around the target
* at the specified distance, the AI command will return failure.
* If we're currently further away from the target than the specified distance, the command
* will actually bring us closer up to the specified distance.
*
* @param oTarget - the AActor* from whom to move away
* @param fAwayDistance - The distance away from the target we want to be
* @param bRunToLocation - specifies whether the AActor* should run or not
* @returns a valid command
* @author Nicolas Ng Man Sun
*/
FCommand CommandMoveAwayFromObject(AActor* oTarget, float fAwayDistance, int32 bRunToLocation = TRUE_);

/** @brief This function is an attack FCommand constructor.
*
* This function is an attack FCommand constructor.
* It creates an attack command which can then be added to any
* AActor's command queue. This command, when processed will attempt to
* make the creature attack the target AActor.
* This command will also move the attacker towards the target, if the creature
* has a melee weapon equipped.
*
* @param oTarget - the AActor* the command make the creature attack.
* @param nForcedResult - the command will be executed without processing scripts
* @returns a valid command, or invalid if the target is dead.
* @remarks This command should not be used to initiate combat.
* @author Adriana/Brenon
*/
FCommand CommandAttack(AActor* oTarget, int32 nForcedResult = COMBAT_RESULT_INVALID);

/** @brief This function is a switch weapon set FCommand constructor.
*
* Makes the creature executing this command switch weapon sets
*
* if the WeaponSet used is INVALID_WEAPON_SET, then the next available
* weapon set is made active.
*
* @param nWeaponSet - The weapon set to make active.
* @returns a valid command
* @author Gabo
*/
FCommand CommandSwitchWeaponSet(int32 nWeaponSet = INVALID_WEAPON_SET);

/** @brief This function is a move to object command constructor. The object executing the command will only use the position and disregard the target object orientation.
*
* This function is a move to object command constructor.
* It creates a move to object command which can then be added to any
* object's command queue. This command, when processed will attempt to
* move the creature to target object.
*
* @param oTarget - the object the command should move the creature to
* @param bRunToLocation - specifies whether the object should run or not
* @param fMinRange - The closest to the object we can be
* @param bUseOriginalPosition - Even if the target is moving, use the original position
* @returns a valid command
* @remarks Only creatures can move, non-creature objects that have a move command assigned to them will fail the command when attempting to process it.
* @author Noel, Jose
*/
FCommand CommandMoveToActor(AActor* oTarget, int bRunToLocation = TRUE_, float fMinRange = 0.0f, int bUseOriginalPosition = FALSE_, float fMaxRange = 0.0f);

/** @brief This function is a move to location FCommand constructor. The AActor* executing the command will use both the position and orientation of the location.
*
* This function is a move to location FCommand constructor.
* It creates a move to location command which can then be added to any
* AActor's command queue. This command, when processed will attempt to
* move the creature to the specified location.
*
* @param vVector - the location the command should move the creature to
* @param bRunToLocation - specifies whether the AActor* should run or not
* @param bDeactivateAtEnd - deactivate the AActor* at the end of the movement. Guaranteed to happen even if the movement doesn't complete
* @returns a valid command
* @author Brenon, Jose
*/
FCommand CommandMoveToLocation(FVector vVector, int32 bRunToLocation = TRUE_, int32 bDeactivateAtEnd = FALSE_);

FCommand CommandMoveToMultiLocations(TArray<FVector> vVectors, int32 bRunToLocation = TRUE_, int32 nStartingWP = 0, int32 bLoop = FALSE_);

/** @brief This function is a "use ability" FCommand constructor.
*
* This function is a "use ability" FCommand constructor.
* It creates a "use ability" command which can then be added to any AActor's
* command queue. This command, when processed will cause the AActor* to
* use the ability if they're able
*
* @param nAbilityId - The ability to perform
* @param oTarget - Object to perform the ability on (optional)
* @param vTarget - ground target location to perform the ability on (optional)
* @param nConjureTime - sets the conjure time of the ability (optional)
* @param AbilitySourceItem - sets the item that's granting the ability.  If none is specified but the ability does come from an item, the the one in the creature's inventory with that ability is used.
* @returns a valid command
* @author Noel, Jose
*/
FCommand CommandUseAbility(int32 nAbilityId, AActor* oTarget = nullptr, FVector vTarget = FVector(0.f), float fConjureTime = -1.f, FString sAbilitySourceItemTag = "");

/** @brief This function is a fly FCommand constructor. The flying creature will use both the position and orientation of the target location.
*
* @param vVector - the location the command should fly the creature to
* @param bIgnorePathing - (optional) set to true if being able to path to the vVector is not a requirement
* @returns a valid command
* @author Jose
*/
FCommand CommandFly(FVector vVector, int32 bIgnorePathing = FALSE_);

/** @brief Create a command to use an AActor.
*
* This will create a command to use an AActor. Sub commands include:
* (1) move to use point
* (2) face AActor
* (3) use AActor
*
* @param oTarget - the AActor* to use
* @param nAction - the action to use on the AActor
* @returns a valid command
* @remarks See PLACEABLE_ACTION_*
* @author Jacques Lebrun
*/
FCommand CommandUseObject(AActor* oTarget, int32 nAction);

/** @brief This function simply waits for the specified amount of time to pass.
*
* This function simply waits for the specified amount of time to pass.
* If a negative value is specified, the command will be given a wait time of zero.
*
* @param fDelay - The amount of time in seconds the command should delay
* @returns a valid command
* @author Brenon
*/
FCommand CommandWait(float fSeconds);

/** @brief Deathblow FCommand constructor.
*
* This creates a deathblow command which will play a deathblow animation on the
* AActor* that executes it. If the attacker is the same as the AActor* executing
* a deathblow attack animation will be played. If its not, then a deathblow
* damage animation will be played.
*
* @param oAttacker - The AActor* that represents the attacker.
* @param nDeathType - Indicates what deathblow will be used use (e.i. unsync, sword and shield, dog, etc.)
* @returns a valid command
* @author Gabo
*/
FCommand CommandDeathBlow(AActor* oTarget, int32 nDeathType = 0);

/** @brief Adds a command to move to a location
*
* Adds a command to move to a location
*
* @param vVector - The location the AActor* will move to
* @returns a valid command
* @author EricP
*/
FCommand CommandJumpToLocation(FVector vVector);