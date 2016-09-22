#pragma once

class Engine;

//#include "vfx_constants_h.h"

// Temporary constants that should go into the core engine
//const int32 EFFECT_DURATION_TYPE_UPKEEP = 99;


// Engine effects (Effect constructor and destructor is handled by engine)
// const int32 EFFECT_TYPE_VISUAL_EFFECT = 4;
// const int32 EFFECT_TYPE_CAMERA_SHAKE = 6;
const int32 EFFECT_TYPE_PARALYZE = 8;
const int32 EFFECT_TYPE_MOVEMENT_RATE = 9;
// const int32 EFFECT_TYPE_ALPHA = 10;
// 
// Mixed effects (Effect constructor is handled by both engine and scripts)
// const int32 EFFECT_TYPE_AOE = 5;
const int32 EFFECT_TYPE_DEATH = 7;

const int32 EFFECT_TYPE_MOVEMENT_RATE_DEBUFF = 11;

// Scripted effects
// const int32 EFFECT_TYPE_HEARTBEAT = 1001;
// const int32 EFFECT_TYPE_WALKING_BOMB = 1002;
const int32 EFFECT_TYPE_ROOT = 1003;
const int32 EFFECT_TYPE_SHAPECHANGE = 1004;
const int32 EFFECT_TYPE_RESURRECTION = 1005;
// const int32 EFFECT_TYPE_ENCHANTMENT = 1006;
// const int32 EFFECT_TYPE_LOCK_INVENTORY = 1007;
// const int32 EFFECT_TYPE_MODIFYMANASTAMINA = 1008;
// const int32 EFFECT_TYPE_SUMMON = 1009;
// const int32 EFFECT_TYPE_HEALHEALTH = 1010;
const int32 EFFECT_TYPE_DISEASE = 1011;
// const int32 EFFECT_TYPE_MODIFY_PROPERTY = 1012;
// const int32 EFFECT_TYPE_DAMAGE = 1013;
const int32 EFFECT_TYPE_KNOCKDOWN = 1014;
// const int32 EFFECT_TYPE_MODIFYATTRIBUTE = 1015;
const int32 EFFECT_TYPE_UPKEEP = 1016;
// const int32 EFFECT_TYPE_DOT = 1017;
// const int32 EFFECT_TYPE_DISPEL_MAGIC = 1018;

const int32 EFFECT_TYPE_DAZE = 1020;
const int32 EFFECT_TYPE_STEALTH = 1021;
// const int32 EFFECT_TYPE_MODIFY_CRITCHANCE = 1022;
const int32 EFFECT_TYPE_AI_MODIFIER = 1023;
const int32 EFFECT_TYPE_IMPACT = 1024;
// const int32 EFFECT_TYPE_REGENERATION = 1026;
const int32 EFFECT_TYPE_STUN = 1027;
const int32 EFFECT_TYPE_GRABBED = 1028;
const int32 EFFECT_TYPE_GRABBING = 1029;
// const int32 EFFECT_TYPE_CONECASTING = 1030;
// const int32 EFFECT_TYPE_ADDABILITY = 1031;
// const int32 EFFECT_TYPE_HEAVY_IMPACT = 1032;
const int32 EFFECT_TYPE_SLEEP = 1033;
const int32 EFFECT_TYPE_CHARM = 1034;
const int32 EFFECT_TYPE_CONFUSION = 1035;
// const int32 EFFECT_TYPE_FEAR = 1036;
// const int32 EFFECT_TYPE_SLEEP_PLOT = 1037;
// const int32 EFFECT_TYPE_LOCK_QUICKBAR = 1038;
// const int32 EFFECT_TYPE_LOCK_CHARACTER = 1039;
// const int32 EFFECT_TYPE_LIFE_WARD = 1040;
// const int32 EFFECT_TYPE_FEIGN_DEATH = 1041;
const int32 EFFECT_TYPE_OVERWHELMED = 1042;
const int32 EFFECT_TYPE_OVERWHELMING = 1043;
// const int32 EFFECT_TYPE_MARK_OF_DEATH = 1044;
const int32 EFFECT_TYPE_SIMULATE_DEATH = 1045;
// const int32 EFFECT_TYPE_CURSE_OF_MORTALITY = 1046;
// const int32 EFFECT_TYPE_ROOTING = 1047;
const int32 EFFECT_TYPE_MISDIRECTION_HEX = 1048;
const int32 EFFECT_TYPE_DEATH_HEX = 1049;
const int32 EFFECT_TYPE_FLANK_IMMUNITY = 1050;
const int32 EFFECT_TYPE_SLIP = 1051;
// const int32 EFFECT_TYPE_PETRIFY = 1052;
// const int32 EFFECT_TYPE_SPELL_WARD = 1053;
// const int32 EFFECT_TYPE_DAMAGE_WARD = 1054;
const int32 EFFECT_TYPE_EVASION = 1055;
// const int32 EFFECT_TYPE_WYNNE_REMOVAL = 1056;
// const int32 EFFECT_TYPE_KNOCKBACK = 1057;
// const int32 EFFECT_TYPE_DECREASE_PROPERTY = 1058;
// const int32 EFFECT_TYPE_SWARM = 1059;
// const int32 EFFECT_TYPE_SINGING = 1060;
// const int32 EFFECT_TYPE_DRAINING = 1061;
const int32 EFFECT_TYPE_RECENTLY_STUNNED = 1062;
// const int32 EFFECT_TYPE_DUMMY = 1063;
// const int32 EFFECT_TYPE_LUCK = 1064;
// const int32 EFFECT_TYPE_REWARD_BONUS = 1065;
// const int32 EFFECT_TYPE_TRAP_DETECTION_BONUS = 1066;
// const int32 EFFECT_TYPE_BLOOD_MAGIC_BONUS = 1067;
// const int32 EFFECT_TYPE_MABARI_DOMINANCE = 1068;
// const int32 EFFECT_TYPE_MANA_SHIELD = 1069;
// const int32 EFFECT_TYPE_DUMMY_HOSTILE = 1070;
const int32 EFFECT_TYPE_AUTOCRIT = 1071;

// const int32 EFFECT_TYPE_TEST = 2000;
// const int32 EFFECT_TYPE_RECURRING_KNOCKDOWN = 2001;

// const int32 EFFECT_TYPE_NULL = 10000; // null effect, doesn't do anything

// const int32 EFFECT_REWARD_BONUS_TYPE_CODEX = 1;
// const int32 EFFECT_REWARD_BONUS_FIELD_TYPE = 0;
// const int32 EFFECT_REWARD_BONUS_TYPE_CASH = 2;
// const int32 EFFECT_REWARD_BONUS_FIELD_MAGNITUDE = 1;
// 
// 
// const int32 UPKEEP_TYPE_MANASTAMINA = 1;
// 
// DISABLE_PHYSICS will fully disable engine physics and collision detection
// const int32 EFFECT_INTEGER_DISABLE_PHYSICS = 0;
// 
// Setting USE_INTERPOLATION_ANGLE to 1 will make the animation of the effect use
// the specified INTERPOLATION_ANGLE to indicate in which direction
// the character will rotate.
// Setting it to 2 will use the ORIGIN value. Check the description below for
// more details.
// const int32 EFFECT_INTEGER_USE_INTERPOLATION_ANGLE = 1;
// const int32 EFFECT_FLOAT_INTERPOLATION_ANGLE = 0;
// 
// POTENCY is used to define the power of effects like MOVEMENT_RATE and ALPHA
const int32 EFFECT_FLOAT_POTENCY = 1;

// KNOCKBACK_DISTANCE will move a character back the value specified.
// If this value is negative and there is a valid creator of the effect
// different than the character the effect has been placed on, then
// the affected character will move towards the effect creator and
// stop the specified distance from the creator.
// const int32 EFFECT_FLOAT_KNOCKBACK_DISTANCE = 2;
// 
// VFX is used for when an effect will have an accompanying VFX.
// This works for all script effects and AOE effects but not
// for the other engine game effects.
// const int32 EFFECT_INTEGER_VFX = 2;
// 
// DONT_INTERPOLATE is used to stop the animation engine from positioning
// a character under an effect to the position where its "supposed to be"
// Use this if you want to force a character to stay in its place. For example,
// In the Ogre grab, the ogre should stay in its place and only the target of the
// grab should reposition. Therefore, set the grabbing effect to not interpolate.
// const int32 EFFECT_INTEGER_DONT_INTERPOLATE = 3;
// 
// SCALE is used to specify a scaling parameter for VFXs. It works for standalone VFXs
// as well as additional VFXs on scripted game effects.
// const int32 EFFECT_FLOAT_SCALE = 3;
// 
// The ORIGIN value is used to specify a point to which a character will
// align to while playing an effect animation. For example, in the case of a
// knockback effect, specifying a point with ORIGIN will make the character be
// knocked back from that point. To make an effect use the ORIGIN value, set the
// USE_INTERPOLATION_ANGLE value to 2.
// const int32 EFFECT_VECTOR_ORIGIN = 2;
// 
// To tint an effect of a certain color first set USE_TINTING to 1, then
// specify a Red/Green/Blue (RGB) value in the TINTING vector.
// const int32 EFFECT_INTERGER_USE_TINTING = 4;
// const int32 EFFECT_VECTOR_TINTING = 3;
// 
// 
// 
// const int32 VFX_CRUST_STUN = 1009;
// const int32 VFX_CRUST_SLEEP = 1104;
// const int32 VFX_CRUST_BLOODMAGIC = 1090;
// const int32 VFX_CRUST_WEB = 1097;
// const int32 VFX_CRUST_GHOST = 3007;
// const int32 VFX_IMP_DEATH = 1100;
// const int32 VFX_CRUST_WBOMB = 1054;
// const int32 VFX_CRUST_ELECTRICITY = 1005;
// 
// const int32 VFX_IMP_DEACTIVATE = 1120;
// const int32 VFX_IMP_BLOOD_SMALL = 1016;
// 
// 
// const int32 VFX_AOE_BLIZZARD = 1018;
// const int32 VFX_AOE_TEMPEST = 1089;
// const int32 VFX_GROUND_ICE_SHEET = 1074;
// 
// const int32 VFX_PLC_LIGHTNING_IMPACT = 1118;
// 
// const int32 VFX_GROUND_BLOODPOOL_S = 1127;
// const int32 VFX_GROUND_BLOODPOOL_M = 1126;
// const int32 VFX_GROUND_BLOODPOOL_L = 1125;
// 
// const int32 VFX_TARGET_HOSTILE = 30012;
// 
// Constants for the CONFUSION effect
// CONFUSION: Behavior percentage
// const int32 EFFECT_CONFUSION_SWITCH_FACTION_PERCENTAGE = 50; // chance that the enemy will switch to the caster faction (and attack his friends) when confused. This % if modified based on the target, just like a spell duration is
// 															 // CONFUSION: VFXs and animation
// 															 // neutral circle VFX to play under confused enemy who turns against his friends. This is in addition to the VFX specified for each spell using the confusion effect
// const int32 EFFECT_CONFUSION_NEUTRAL_VFX_SMALL = 30002;
// const int32 EFFECT_CONFUSION_NEUTRAL_VFX_MEDIUM = 30003;
// const int32 EFFECT_CONFUSION_NEUTRAL_VFX_LARGE = 30004;
// const int32 EFFECT_CONFUSION_DAZE_VFX = 1009; // daze VFX to play on confused enemy. This is in addition to the VFX specified for each spell using the confusion effect
// const int32 EFFECT_CONFUSION_ANIMATION_ID = 271; // which animation to play when the enemy is randomly dazed
// 												 // CONFUSION: Game Parameters
// const float EFFECT_CONFUSION_WANDER_RADIUS = 5.0; // if the enemy wanders away, how far will he go? This should be kept small to avoid pathfinding issue (e.g.: finding a wander spot behind a wall)
// 												  // CONFUSION: Other constants
// const int32 EFFECT_CONFUSION_EFFECT_INTEGER_INDEX = 0; // the index used to store the enemy faction on the confusion effect itself.
// 
// const int32 STINGING_SWARM_VFX = 90100;
// 
// const float WYNNE_ATTACK_PENALTY = -20.0f;
// const float WYNNE_DEFENSE_PENALTY = -20.0f;
// const float WYNNE_SPEED_PENALTY = 0.6f;
// const int32 WYNNE_WEAKNESS_VFX = 90288;
// const float WYNNE_WEAKNESS_DURATION = 60.0f;
// const float WYNNE_STUN_DURATION = 10.0f;
// 
