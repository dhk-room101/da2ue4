#pragma once

//sys_ambient_h
//Copyright2007 BioWare Corp.
/////////////////////////////////////////////////////////////////////////////
/*
Implementation of ambient behavior system.

By default, ambient behavior begins when a creature sees the player and
ceases a short time after the party leaves the creature's sight. Ambient
behavior alternates between a movement phase and an animation phase. In a
movement phase, the creature (optionally) moves to a new destination AActor.
In an animation phase, the creature (optionally) performs one or more
animations either in sequence or at random. Exact behavior is controlled by
local variables on the creature template (see below).

The actions comprising the animation pattern used in an animation phase are
listed in ambient_ai.xls. Each row defines an animation pattern consisting
of up to nine actions. Each action is composed of an animation (left of the
decimal) and loop count (right of decimal). For example, 619.05 will play a
sleeping animation for 5 loops and 619.99 will loop infinitely.
Non-looping animations require no loop count (e.g. 808.00 = point right).

The only functions safe to call outside sys_ambient_h or creature_core are:
- Ambient_Start()
- Ambient_StartTeam()
- Ambient_StartTag()
- Ambient_Stop()
- Ambient_OverrideBehavior()
- Ambient_RestoreBehavior()

Local variables stored on the creature used by this script:
AMBIENT_SYSTEM_STATE = State bitmask of AMBIENT_SYSTEM_XXX values
AMBIENT_MOVE_PATTERN = Ambient movement pattern constant AMBIENT_MOVE_XXX (0-7)
AMBIENT_MOVE_PREFIX= Prefix of ambient movement destination (waypoint or creature)
AMBIENT_ANIM_PATTERN = Ambient animation pattern - index into ambient_ai.xls
AMBIENT_ANIM_FREQ= Animation frequency
-1.0 = play all in order
0.0 = no animations
x.y = play random number (between x and y) animations in random order

AMBIENT_ANIM_PATTERN_OVERRIDE = (Internal use) If non-zero, takes precedence over AMBIENT_ANIM_PATTERN
AMBIENT_ANIM_FREQ_OVERRIDE= (Internal use) If non-zero, takes precedence over AMBIENT_ANIM_FRE
AMBIENT_ANIM_OVERRIDE_COUNT = (Internal use) If non-zero, the number of times to play the override animation pattern before resuming the default animation pattern
AMBIENT_ANIM_STATE = (Internal use) Animation state: 0 = start move phase, -1 = start anim phase, +ve = # anims left to play
AMBIENT_MOVE_STATE = (Internal use) loword = # of the waypoint moved to last, hiword = direction of travel
AMBIENT_MOVE_COUNT = (Internal use) Number of NPC/WPs available to move to
*///////////////////////////////////////////////////////////////////////////////

class Engine;

// AMBIENT_COMMAND constants
const int32 AMBIENT_COMMAND_ATTACK_PRACTICE = 1; // Creature attacks target with 1-5s delay between attack commands.
const int32 AMBIENT_COMMAND_ATTACK = 2; // Creature attacks target with no delay between attack commands.

// AMBIENT_SYSTEM_STATE bit flags
const int32 AMBIENT_SYSTEM_DISABLED = 0x00;
const int32 AMBIENT_SYSTEM_ENABLED = 0x01;// Indicates creature uses ambient behavior.
const int32 AMBIENT_SYSTEM_SPAWNSTART = 0x02;// Indicates ambient behavior starts when creature spawns.
const int32 AMBIENT_SYSTEM_NOPLAYNEXT = 0x04;// Forces the PlayNext parameter to CommandPlayAnimation to be FALSE_.
const int32 AMBIENT_SYSTEM_NOBLEND = 0x08;// Forces the BlendIn parameter to CommandPlayAnimation to be FALSE_.
const int32 AMBIENT_SYSTEM_ALWAYSON = 0x10;// Sets ambient behavior to never stop (even during combat or when player remains out of sight).
const int32 AMBIENT_SYSTEM_RUNNING = 0x40;// (Internal use) Indicates ambient movement/animation is in progress.

// AMBIENT_MOVE_PATTERN types
const int32 AMBIENT_MOVE_INVALID = -1;
const int32 AMBIENT_MOVE_NONE = 0;// No movement
const int32 AMBIENT_MOVE_PATROL = 1;// Patrol waypoints (1, 2, 3, 2, 1, ...)
const int32 AMBIENT_MOVE_LOOP = 2;// Loop through waypoints (1, 2, 3, 1, 2, 3, ...)
const int32 AMBIENT_MOVE_WARP = 3;// Jump from the last waypoint to the first (1, 2, 3, jump to 1, 2, 3, ...)
const int32 AMBIENT_MOVE_RANDOM = 4;// Go to random waypoint
const int32 AMBIENT_MOVE_WANDER = 5;// Go to random location within AMBIENT_RANGE_NEAR meters of home location
const int32 AMBIENT_MOVE_WANDER_FAR = 6;// Go to random location within AMBIENT_RANGE_FAR meters of home location
const int32 AMBIENT_MOVE_PATH_PATROL = 7;// Follow waypoints using CommandMoveToMultiLocation() in order (1, 2, 3, 2, 1, ...)
const int32 AMBIENT_MOVE_PATH_LOOP = 8;// Follow waypoints using CommandMoveToMultiLocation() in order (1, 2, 3, 1, 2, 3, ...)
const int32 AMBIENT_MOVE_ONCE = 9;// Follow waypoints in order once (1, 2, 3)

const float AMBIENT_ANIM_FREQ_NONE = 0.0f;
const float AMBIENT_ANIM_FREQ_ORDERED = -1.0f;
const float AMBIENT_ANIM_FREQ_RANDOM = 3.5f;

// AMBIENT_ANIM_STATE constants
const int32 AMBIENT_ANIM_NONE = 0;// Indicates no animations remaining to play, ergo movement phase should begin next.
const int32 AMBIENT_ANIM_RESET = -1;// Indicates movement phase complete and animation phase should begin next.

// AMBIENT_MOVE_STATE direction constants
const int32 AMBIENT_MOVE_NEXT = 0x00000; // Indicates waypoint travel in increasing numerical order
const int32 AMBIENT_MOVE_PREV = 0x10000; // Indicates waypoint travel in decreasing numerical order

// AMBIENT_MOVE_COUNT constants
const int32 AMBIENT_MOVE_COUNT_INVALID = -1; // Indicates destination count needs to be (re)done.

const float AMBIENT_SMALL_DELTA = 0.01f;// Small value used for floating point comparison.
const float AMBIENT_SMALL_DELAY = 0.1f; // Period to wait between ambient actions (in seconds).
const float AMBIENT_MOVE_TOLERANCE = 2.0f; // Move to target tolerance in meters
const float AMBIENT_RESUME_PERIOD = 15.0f; // Period to wait before attempting to resume ambient behavior (in seconds).
const int32 AMBIENT_PAUSE_PERIOD = 30;// Number of seconds the party must be out of range for a creature to pause ambient behavior.
const int32 AMBIENT_RANGE_NEAR = 10;// Range limit for wandering 'near' in meters.
const int32 AMBIENT_RANGE_FAR = 30;// Range limit for wandering 'far' in meters.
const int32 AMBIENT_LOOP_FOREVER = 90;// Any number of loops greater than this is considered infinite looping.

// String constants
// const FString AMBIENT_MOVE_PREFIX_NONE = "";

// 2DA column names in ambient_ai.xls
const FString COL_AMBIENT_ANIM = "Action";
const FString COL_AMBIENT_ANIM_TOTAL = "ActionTotal";
const FString COL_AMBIENT_ANIM_NOPLAYNEXT = "NoPlayNext";

// Animation pattern constants corresponding to rows in ambient_ai.xls
const int32 AMBIENT_ANIM_PATTERN_NONE = 0;
const int32 AMBIENT_ANIM_PATTERN_TWITCHES = 1;
// const int32 AMBIENT_ANIM_PATTERN_RELAXED_LOOP = 2;
// const int32 AMBIENT_ANIM_PATTERN_DOG_EATING = 3;
// const int32 AMBIENT_ANIM_PATTERN_RELAXED = 4;
// const int32 AMBIENT_ANIM_PATTERN_GUARD_POST =5;
// const int32 AMBIENT_ANIM_PATTERN_GUARD_POST =6;
// const int32 AMBIENT_ANIM_PATTERN_DEAD_LOOP = 7;
// const int32 AMBIENT_ANIM_PATTERN_BLESSING = 8;
// const int32 AMBIENT_ANIM_PATTERN_CROUCHPRAY_1 = 9;
// const int32 AMBIENT_ANIM_PATTERN_LECTURER_1 = 10;
// const int32 AMBIENT_ANIM_PATTERN_LISTENER_1 = 11;
// const int32 AMBIENT_ANIM_PATTERN_LECTURER_2 = 12;
// const int32 AMBIENT_ANIM_PATTERN_CHATTER_1 = 13;
// const int32 AMBIENT_ANIM_PATTERN_LISTENER_2 = 14;
// const int32 AMBIENT_ANIM_PATTERN_SHOPPER = 15;
// const int32 AMBIENT_ANIM_PATTERN_NURSE = 16;
// const int32 AMBIENT_ANIM_PATTERN_WRITHING = 17;
// const int32 AMBIENT_ANIM_PATTERN_TABLE_MAP = 18;
// const int32 AMBIENT_ANIM_PATTERN_CROSSARMS_LOOP = 19;
// const int32 AMBIENT_ANIM_PATTERN_MERCHANT_BECKON = 20;
// const int32 AMBIENT_ANIM_PATTERN_CLEAN_FLOOR = 22;
// const int32 AMBIENT_ANIM_PATTERN_SITGROUND_LOOP = 23;
// const int32 AMBIENT_ANIM_PATTERN_WANDER_LR = 24;
// const int32 AMBIENT_ANIM_PATTERN_HANDSBACK_LOOP_1 = 25; // without enter animation
// const int32 AMBIENT_ANIM_PATTERN_ATTENTION = 26;
// const int32 AMBIENT_ANIM_PATTERN_PRE_LOGHAIN = 27;
// const int32 AMBIENT_ANIM_PATTERN_WANDER_R = 28;
// const int32 AMBIENT_ANIM_PATTERN_WANDER_L = 29;
// const int32 AMBIENT_ANIM_PATTERN_DARKSPAWN_WANDER = 30;
// const int32 AMBIENT_ANIM_PATTERN_CROUCH_LOOP = 31;
// const int32 AMBIENT_ANIM_PATTERN_PRAY_LOOP = 32;
// const int32 AMBIENT_ANIM_PATTERN_CROUCHPRAY_2 = 33;
// const int32 AMBIENT_ANIM_PATTERN_HANDSBACK_LOOP_2 = 34; // with enter animation
// const int32 AMBIENT_ANIM_PATTERN_RUMMAGING = 35;
// const int32 AMBIENT_ANIM_PATTERN_DEER_GRAZING_1 = 59;
// const int32 AMBIENT_ANIM_PATTERN_DEER_GRAZING_2 = 60;
// 
// const int32 AMBIENT_ANIM_PATTERN_TEST = 1000;
// const int32 AMBIENT_ANIM_PATTERN_TEST_LONG = 1001;

/** @brief Disables ambient behavior for a creature.
*
* @param oCreature The creature.
**/
void Ambient_Stop(AActor* oCreature);

/** @brief Called from event handler for EVENT_TYPE_COMMAND_COMPLETE
*
* @param nCommandType The type of the command that completed.
* @param nCommandStatus The status of the command that completed.
* @param oCreatureThe creature that completed the command.
**/
void Ambient_CommandComplete(int32 nCommandType, int32 nCommandStatus, AActor* oCreature);

/** @briefCauses a creature to perform an ambient behavior (i.e. a single movement or animation).
*
* @paramoCreatureThe creature.
* @parambUpdateTimer
**/
void Ambient_DoSomething(AActor* oCreature, int32 bUpdateTimer = FALSE_);

/** @briefChecks period since last EVENT_AMBIENT_CONTINUE event was received.
*
* @paramoCreature The creature performing the ambient behavior.
* @parambUpdateTimerTRUE_ if timer should be updated with current timestamp.
* @returnsTRUE_ if timer expired (meaning creature should cease ambient animations)
**/
int32 Ambient_TimerExpired(AActor* oCreature, int32 bUpdateTimer);

/** @brief Triggers ambient commands.
*
*   @param oCreature  The creature to assign an ambient command.
*   @param nCommand   The command to perform (AMBIENT_COMMAND_***). If zero, uses the AMBIENT_COMMAND local integer set on oCreature.
*   @returns          TRUE_ if oCreature has an ambient command to perform. FALSE_ otherwise.
**/
int32 Ambient_DoCommand(AActor* oCreature, int32 nCommand = 0);

/** @brief Forces a creature to repeatedly attack an AActor.
*
*   @param oAttacker    The attacking creature.
*   @param fDelay       Delay to wait (in seconds) before attacking again.
**/
void Ambient_DoCommandAttack(AActor* oAttacker, float fDelay);

/** @brief Checks whether a creature has reached to its final destination.
*
*   @param   oCreature  The ambient creature.
*   @returns TRUE_ if oCreature is within a reasonable tolerance of the destination
*            to which it was last told to move.
**/
int32 Ambient_ReachedDestination(AActor* oCreature);

/** @brief Gets the Nth ambient destination for a creature.
*
*   See Ambient_GetDestinations() for a description of the expected tag format.
*
*   @param oCreature    The creature moving.
*   @param n            The number of the destination.
*   @returns            The Nth destination AActor* or nullptr.
**/
AActor* Ambient_GetDestination(AActor* oCreature, int32 n);

/** @brief Gets the number of objects a creature can move to through ambient movement.
*
*   Gets the number of objects a creature can move to through ambient movement.
*   The destinations can be waypoints, placeables, or creatures. Expected tag
*   convention is either sequential unique objects (XXX_01, XXX_02, XXX_03, etc.) or
*   all objects sharing a single common tag (XXX) where XXX is the AMBIENT_MOVE_PREFIX
*   local variable set on the creature.
*
*   @param    oCreature  The creature whose destinations to count.
*   @returns             The number of valid destinations for oCreature.
**/
TArray<AActor*> Ambient_GetDestinations(AActor* oCreature);

/** @brief Causes a creature to follow a prescribed path using CommandMoveToMultiLocations().
*
*   @param  oCreature   The ambient creature.
*   @param  bLoop       Whether the creature should loop or patrol waypoints.
*   @param  bRun        Whether the creature should walk or run between waypoints.
*   @returns            TRUE_ if function succeeds (i.e. creature moves to a valid location).
**/
int32 Ambient_MovePath(AActor* oCreature, int32 bLoop, int32 bRun);

/** @brief Triggers next action in an ambient behavior animation phase.
*
*   Causes oCreature to perform the next action in an ambient behavior animation phase.
*   Once oCreature runs out of actions to perform, a movement phase is triggered.
*
*   @param oCreature  The creature animating.
**/
void Ambient_Animate(AActor* oCreature);

/** @brief Triggers an ambient behavior movement phase.
*
*   Causes oCreature to move to a new location/waypoint. Once oCreature
*   reaches its destination it will begin an ambient behavior animation phase.
*
*   @param oCreature  The creature moving.
**/
void Ambient_Move(AActor* oCreature);

/** @brief Causes a creature to walk between a set of objects (typically waypoints).
*
*   @param oCreature        The creature moving.
*   @param nMovementPattern The pattern of movement between objects (loop, patrol, random, etc).
*   @param bRun             Whether the creature should walk or run to the next waypoint.
*   @returns                TRUE_ if function succeeds (i.e. creature moves to a valid location).
**/
int32 Ambient_WalkWaypoints(AActor* oCreature, int32 nMovementPattern, int32 bRun = FALSE_);

/** @brief Causes a creature to move a random distance away from its home location.
*
*   @param  oCreature   The creature moving.
*   @param  nRange      Maximum distance (in meters) oCreature can wander from home location.
*   @param  bRun        Whether the creature should walk or run.
*   @returns            TRUE_ if function succeeds (i.e. creature moves to a valid location).
**/
int32 Ambient_MoveRandom(AActor* oCreature, int32 nRange, int32 bRun = FALSE_);

/** @brief  Checks whether to start ambient behavior after creature spawns.
*
*   Called from creature's EVENT_TYPE_SPAWN handler. Starts ambient behavior
*   if the local variable AMBIENT_SYSTEM_STATE bit mask contains 0x02
*   (AMBIENT_SYSTEM_SPAWNSTART). Generally, only creatures with a single
*   infinitely looping animation should have this bit set.
*
*   @param  oCreature        The creature.
**/
void Ambient_SpawnStart(AActor* oCreature);