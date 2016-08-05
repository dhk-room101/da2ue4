#pragma once

class Engine;

//reflection variables
//DHK
const FName COMBAT_STATE = "COMBAT_STATE";//creature override for GM_COMBAT
const FString SS_NODENAME = "NODENAME"; //soundset
const FName APPROVAL = "APPROVAL";
const FName FOLLOWER_STATE = "FOLLOWER_STATE";
const FName PACKAGEAI = "PACKAGEAI";
const FName PACKAGE = "PACKAGE";
const FName PACKAGETYPE = "PACKAGETYPE";
const FName COMBATANT_TYPE = "COMBATANT_TYPE";
const FName GENDER = "GENDER";//?
const FName RACE = "RACE";
const FName ABILITY = "ABILITY";//? item
const FName ROAM_DISTANCE = "ROAM_DISTANCE";
const FName HOME_LOCATION = "HOME_LOCATION";
const FName AMBIENT_EVENT = "AMBIENT_EVENT";

//original - FName for reflection, FString for convenience :D
const FName TEAM = "TEAM";
const FName GROUP = "GROUP";
const FName RANK = "RANK";
const FName PLAYER_LEADER_STORE = "PLAYER_LEADER_STORE";
const FName CREATURE_SPAWN_DEAD = "CREATURE_SPAWN_DEAD";
const FName AI_TARGET_OVERRIDE = "AI_TARGET_OVERRIDE";
const FName AI_TARGET_OVERRIDE_DUR_COUNT = "AI_TARGET_OVERRIDE_DUR_COUNT";
const FName SPAWN_HOSTILE_LYING_ON_GROUND = "SPAWN_HOSTILE_LYING_ON_GROUND";
const FName GO_HOSTILE_ON_PERCEIVE_PC = "GO_HOSTILE_ON_PERCEIVE_PC";
const FName AI_THREAT_TARGET = "AI_THREAT_TARGET";
const FName AI_THREAT_HATED_RACE = "AI_THREAT_HATED_RACE";
const FName AI_THREAT_HATED_CLASS = "AI_THREAT_HATED_CLASS";
const FName AI_THREAT_HATED_GENDER = "AI_THREAT_HATED_GENDER";
const FName AI_THREAT_SWITCH_TIMER_MIN = "AI_THREAT_SWITCH_TIMER_MIN";
const FName AI_THREAT_TARGET_SWITCH_COUNTER = "AI_THREAT_TARGET_SWITCH_COUNTER";
const FString SOUND_SET_FLAGS = "SOUND_SET_FLAGS_";
const FName SOUND_SET_FLAGS_0 = "SOUND_SET_FLAGS_0";
const FName SOUND_SET_FLAGS_1 = "SOUND_SET_FLAGS_1";
const FName SOUND_SET_FLAGS_2 = "SOUND_SET_FLAGS_2";
const FName APP_RANGE_VALUE_CRISIS = "APP_RANGE_VALUE_CRISIS";
const FName APP_RANGE_VALUE_HOSTILE = "APP_RANGE_VALUE_HOSTILE";
const FName APP_RANGE_VALUE_NEUTRAL = "APP_RANGE_VALUE_NEUTRAL";
const FName APP_RANGE_VALUE_WARM = "APP_RANGE_VALUE_WARM";
const FName APP_RANGE_VALUE_FRIENDLY = "APP_RANGE_VALUE_FRIENDLY";
const FName SHOUTS_ACTIVE = "SHOUTS_ACTIVE"; // whether or not shouts are active for the creature
const FName SHOUTS_DIALOG_OVERRIDE = "SHOUTS_DIALOG_OVERRIDE"; // dialog to use for shouts, if not default creature dialog
const FName SHOUTS_DELAY = "SHOUTS_DELAY"; // delay between shouts, if not using default delay
const FName DISABLE_FOLLOWER_DIALOG = "DISABLE_FOLLOWER_DIALOG";
const FName CREATURE_RULES_FLAG0 = "CREATURE_RULES_FLAG0";
const FName AMB_SYSTEM_DIALOG = "AMB_SYSTEM_DIALOG";
const FName PARTY_OVERRIDE_DIALOG = "PARTY_OVERRIDE_DIALOG";
const FName PARTY_OVERRIDE_DIALOG_ACTIVE = "PARTY_OVERRIDE_DIALOG_ACTIVE";
const FName AI_THREAT_GENERATE_EXTRA_THREAT = "AI_THREAT_GENERATE_EXTRA_THREAT";
const FName CREATURE_HAS_TIMER_ATTACK = "CREATURE_HAS_TIMER_ATTACK";
const FName AI_LIGHT_ACTIVE = "AI_LIGHT_ACTIVE";
const FName AI_FLAG_STATIONARY = "AI_FLAG_STATIONARY";
const FName AI_MOVE_TIMER = "AI_MOVE_TIMER";
const FName AI_PARTY_CLEAR_TO_ATTACK = "AI_PARTY_CLEAR_TO_ATTACK";
const FName AI_HELP_TEAM_STATUS = "AI_HELP_TEAM_STATUS";
const FName AI_USE_GUI_TABLES_FOR_FOLLOWERS = "AI_USE_GUI_TABLES_FOR_FOLLOWERS";
const FName AI_LAST_TACTIC = "AI_LAST_TACTIC";
const FName AI_DISABLE_TABLES = "AI_DISABLE_TABLES";
const FName PLC_FLIP_COVER_USE_COUNT = "PLC_FLIP_COVER_USE_COUNT";
// CREATURE_COUNTER_3 used to enable/disable stomp. 1 is for disabled
const FName CREATURE_COUNTER_3 = "CREATURE_COUNTER_3"; 
const FName COMBAT_LAST_WEAPON = "COMBAT_LAST_WEAPON";
const FName PROJECTILE_OVERRIDE = "PROJECTILE_OVERRIDE";
const FName AI_DISABLE_PATH_BLOCKED_ACTION = "AI_DISABLE_PATH_BLOCKED_ACTION";
const FName AMBIENT_SYSTEM_STATE = "AMBIENT_SYSTEM_STATE";   // Bitmask of AMBIENT_SYSTEM_XXX values
const FName AMBIENT_MOVE_PATTERN = "AMBIENT_MOVE_PATTERN";   // Ambient movement pattern constant AMBIENT_MOVE_XXX (0-7)
const FName AMBIENT_MOVE_PREFIX = "AMBIENT_MOVE_PREFIX";    // Prefix of ambient movement destination (waypoint or creature)
const FName AMBIENT_ANIM_PATTERN = "AMBIENT_ANIM_PATTERN";   // Ambient animation pattern - index into ambient_ai.xls
const FName AMBIENT_ANIM_FREQ = "AMBIENT_ANIM_FREQ";      // Animation frequency x.y = min.max, -1.0 = all (in order)
const FName AMBIENT_ANIM_PATTERN_OVERRIDE = "AMBIENT_ANIM_PATTERN_OVERRIDE"; // If non-zero, takes precedence over AMBIENT_ANIM_PATTERN
const FName AMBIENT_ANIM_FREQ_OVERRIDE = "AMBIENT_ANIM_FREQ_OVERRIDE";    // If non-zero, takes precedence over AMBIENT_ANIM_FRE
const FName AMBIENT_ANIM_OVERRIDE_COUNT = "AMBIENT_ANIM_OVERRIDE_COUNT";   // If non-zero, the number of times the override animation pattern is used.
const FName AMBIENT_ANIM_STATE = "AMBIENT_ANIM_STATE";    // anim state: 0 = start move phase, -1 = start anim phase, +ve = # anims left to play
const FName AMBIENT_MOVE_STATE = "AMBIENT_MOVE_STATE";    // loword = # of the waypoint moved to last, hiword = direction of travel
const FName AMBIENT_MOVE_COUNT = "AMBIENT_MOVE_COUNT";    // number of NPC/WPs available to move to
const FName AMBIENT_COMMAND = "AMBIENT_COMMAND";
const FName AMBIENT_TICK_COUNT = "AMBIENT_TICK_COUNT";
const FName RUBBER_HOME_ENABLED = "RUBBER_HOME_ENABLED";
