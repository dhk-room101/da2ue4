#pragma once

class Engine;

//DHK
const int32 SETTING_GAME_DIFFICULTY = 1;
const int32 SETTING_GRAPHIC_DETAIL_LEVEL = 2;

const int32 TABLE_TALK = 215;
const int32 TABLE_GAME_SETTINGS = 216;

// -----------------------------------------------------------------------------
// Groups - toolset_groups.2da
// -----------------------------------------------------------------------------
const int32 GROUP_PC = 0;
const int32 GROUP_HOSTILE = 1;
const int32 GROUP_FRIENDLY = 2;
const int32 GROUP_NEUTRAL = 3;
const int32 GROUP_HOSTILE_ON_GROUND = 36;

// -----------------------------------------------------------------------------
// 2DA tables names - 2da_server.xls
// -----------------------------------------------------------------------------
const int32 TABLE_PROPERTIES = 98; //DHK
const int32 TABLE_PACKAGES = 163;
const int32 TABLE_ITEMPRPS = 107;
const int32 TABLE_EXPERIENCE = 164;
//const int32 TABLE_EFFECT_IMMUNITIES = 165;
const int32 TABLE_RANGES = 134;
const int32 TABLE_ABILITIES_TALENTS = 1;
const int32 TABLE_ITEMS = 6;
//const int32 TABLE_MATERIAL_TYPES = 89;
//const int32 TABLE_APPROVAL_NORMAL_RANGES = 166;
//const int32 TABLE_APPROVAL_ROMANCE_RANGES = 167;
//const int32 TABLE_APPROVAL_MOTIVATIONS = 168;
const int32 TABLE_ABILITIES_SPELLS = 1;
//const int32 TABLE_CORE_RULES = 169;
const int32 TABLE_EFFECTS = 136;
const int32 TABLE_COMMANDS = 94;
const int32 TABLE_EVENTS = 114;
const int32 TABLE_AMBIENT = 170;
//const int32 TABLE_BASE_ANIMATIONS = 0;
//const int32 TABLE_COMBAT_ANIMATIONS = 0;
const int32 TABLE_TACTICS_CONDITIONS = 137;
const int32 TABLE_APPEARANCE = 2;
//const int32 TABLE_WORLD_MAPS = 171;
const int32 TABLE_RULES_RACES = 5;
const int32 TABLE_RULES_CLASSES = 3;
//const int32 TABLE_RULES_ARMOR_DATA = 172;
//const int32 TABLE_RULES_BACKGROUNDS = 97;
const int32 TABLE_SOUNDSETS = 138;
//const int32 TABLE_EFFECT_REGENERATION = 173;
const int32 TABLE_CREATURERANKS = 174;
const int32 TABLE_AUTOSCALE = 175;
//const int32 TABLE_AUTOSCALE_EASY = 279;
//const int32 TABLE_AUTOSCALE_HARD = 280;
//const int32 TABLE_AUTOSCALE_DATA = 176;
//const int32 TABLE_PLACEABLE_TYPES = 93;
//const int32 TABLE_AREA_MUSIC = 177;
//const int32 TABLE_CLOUDS = 178;
//const int32 TABLE_ATMOSPHERE = 179;
const int32 TABLE_UI_MESSAGES = 180;
//const int32 TABLE_QBAR = 181;
//const int32 TABLE_TRAPS = 182;
//const int32 TABLE_FOG = 183;
//const int32 TABLE_ITEM_SETS = 184;
const int32 TABLE_TACTICS_BASE_CONDITIONS = 139;
const int32 TABLE_AI_TACTICS_TARGET_TYPE = 140;
//const int32 TABLE_TACTICS_USER_PRESETS = 214;
//const int32 TABLE_RULES_DISEASES = 185;
//const int32 TABLE_PLOTACTIONS = 142;
const int32 TABLE_AI_ABILITY_COND = 186;
//const int32 TABLE_PLOT_TYPES = 108;
const int32 TABLE_AI_BEHAVIORS = 187;
//const int32 TABLE_ABILITY_DATA = 189;
//const int32 TABLE_ABILITY_EFFECTS = 190;
//const int32 TABLE_CHARACTERS = 191;
const int32 TABLE_ITEMSTATS = 110;
//const int32 TABLE_CRAFTING = 144;
//const int32 TABLE_TREASURES = 192;
const int32 TABLE_SHAPECHANGE = 188;
//const int32 TABLE_SCREEN_SHAKE = 193;
//const int32 TABLE_FRAMEBUFFER = 194;
//const int32 TABLE_PROVING_FIGHTS = 195;
//const int32 TABLE_PROVING_TYPES = 196;
//const int32 TABLE_SPELL_CONJURINGSPEED = 135;
//const int32 TABLE_COMMAND_TYPES = 141;
//const int32 TABLE_RULES_INJURIES = 160;
//const int32 TABLE_REWARDS = 197;
//const int32 TABLE_VFX_PERSISTENT = 96;
//const int32 TABLE_CATEGORY = 198;
//const int32 TABLE_MATERIAL = 199;
//const int32 TABLE_STEALING = 200;
//const int32 TABLE_GIFT_VALUES = 201;
const int32 TABLE_DIFFICULTY = 211;
const int32 TABLE_PROJECTILES = 1006;

// -----------------------------------------------------------------------------
// Target type - targettypes.xls
// -----------------------------------------------------------------------------
const int32 TARGET_TYPE_INVALID = 0;
const int32 TARGET_TYPE_SELF = 1;
const int32 TARGET_TYPE_FRIENDLY_CREATURE = 2;
const int32 TARGET_TYPE_HOSTILE_CREATURE = 4;
const int32 TARGET_TYPE_PLACEABLE = 8;
const int32 TARGET_TYPE_AOE = 16;
const int32 TARGET_TYPE_GROUND = 32;
const int32 TARGET_TYPE_BODY = 64;

// -----------------------------------------------------------------------------
// Damage Types
// -----------------------------------------------------------------------------
const int32 DAMAGE_TYPE_INVALID = 0;
const int32 DAMAGE_TYPE_PHYSICAL = 1;
const int32 DAMAGE_TYPE_FIRE = 2;
const int32 DAMAGE_TYPE_COLD = 3;
const int32 DAMAGE_TYPE_ELECTRICITY = 4;
const int32 DAMAGE_TYPE_NATURE = 5;
const int32 DAMAGE_TYPE_PLOT = 6;
const int32 DAMAGE_TYPE_TBD = 7;  //debug
const int32 DAMAGE_TYPE_SPIRIT = 8;  //debug
const int32 DAMAGE_TYPE_DOT = 9;  //DoT damage is 'pre resisted'

// ----------------------------------------------------------------------------
// Creature Types
// ----------------------------------------------------------------------------
const int32 CREATURE_TYPE_INVALID = 0;
const int32 CREATURE_TYPE_OTHER = 1;
const int32 CREATURE_TYPE_HUMANOID = 2;
const int32 CREATURE_TYPE_DARKSPAWN = 3;
const int32 CREATURE_TYPE_ANIMAL = 4;
const int32 CREATURE_TYPE_BEAST = 5;
const int32 CREATURE_TYPE_DEMON = 6;
const int32 CREATURE_TYPE_DRAGON = 7;
const int32 CREATURE_TYPE_AMBIENT = 8;
const int32 CREATURE_TYPE_GOLEM = 9;
const int32 CREATURE_TYPE_UNDEAD = 10;

// Creature ranks
const int32 CREATURE_RANK_INVALID = 0;
const int32 CREATURE_RANK_CRITTER = 1;
const int32 CREATURE_RANK_NORMAL = 2;
const int32 CREATURE_RANK_LIEUTENANT = 3;
const int32 CREATURE_RANK_BOSS = 4;
const int32 CREATURE_RANK_ELITE_BOSS = 5;
const int32 CREATURE_RANK_PLAYER = 100;
const int32 CREATURE_RANK_WEAK_NORMAL = 11;
const int32 CREATURE_RANK_ONE_HIT_KILL = 12;

// ----------------------------------------------------------------------------*
// Item Stats
// ----------------------------------------------------------------------------*
const int32 ITEM_STAT_INVALID = 0;
const int32 ITEM_STAT_DEFENSE = 1;
const int32 ITEM_STAT_DAMAGE = 2;
const int32 ITEM_STAT_SHIELD_RATING = 3;
const int32 ITEM_STAT_ATTACK = 4;
const int32 ITEM_STAT_CRIT_CHANCE_MODIFIER = 5;
const int32 ITEM_STAT_ARMOR_PENETRATION = 6;
const int32 ITEM_STAT_MISSILE_DEFLECTION = 8;
const int32 ITEM_STAT_OPTIMUM_RANGE = 9;
const int32 ITEM_STAT_ATTRIBUTE_MOD = 12;

//traits
const int32 ABILITY_TRAIT_CRITICAL_HIT_IMMUNITY = 150007;

//talents
//humanoids
const int32 ABILITY_TALENT_MASTER_ARCHER = 35;
const int32 ABILITY_TALENT_BLOOD_FRENZY = 713;
const int32 ABILITY_TALENT_SHATTERING_BLOWS = 3001;
const int32 ABILITY_TALENT_EXPLOIT_WEAKNESS = 56;
const int32 ABILITY_TALENT_BACKSTAB = 3002;
const int32 ABILITY_TALENT_HIDDEN_ROGUE = 4020;
const int32 ABILITY_TALENT_RAPIDSHOT = 3071;
const int32 ABILITY_TALENT_SHIELD_BLOCK = 3074;
const int32 ABILITY_TALENT_COMBAT_MOVEMENT = 21;
const int32 ABILITY_TALENT_SHIELD_TACTICS = 3030;
const int32 ABILITY_TALENT_BRAVERY = 17;
const int32 ABILITY_TALENT_EVASION = 3069;
const int32 ABILITY_TALENT_DUAL_WEAPON_DOUBLE_STRIKE = 11;
const int32 ABILITY_TALENT_DUAL_WEAPON_SWEEP = 3044;
const int32 ABILITY_TALENT_BERSERK = 700;
const int32 ABILITY_TALENT_STEALTH = 100075;
const int32 ABILITY_TALENT_COMBAT_STEALTH = 100077;
const int32 ABILITY_TALENT_DEVOUR = 3065;
const int32 ABILITY_TALENT_SHIELD_BASH = 617;
const int32 ABILITY_TALENT_OVERPOWER = 3073;
const int32 ABILITY_TALENT_OVERRUN = 3024;
const int32 ABILITY_TALENT_PINNING_SHOT = 803;
const int32 ABILITY_TALENT_FRIGHTENING = 3066;
const int32 ABILITY_TALENT_DIRTY_FIGHTING = 603;
const int32 ABILITY_TALENT_HOLY_SMITE = 25;
const int32 ABILITY_TALENT_SHIELD_DEFENSE = 704;
const int32 ABILITY_TALENT_SHIELD_WALL = 12;
const int32 ABILITY_TALENT_LETHALITY = 777;
const int32 ABILITY_TALENT_DUAL_WEAPON_TRAINING = 6;
const int32 ABILITY_TALENT_STRONG = 27;

//monsters
const int32 ABILITY_TALENT_MONSTER_DOG_CHARGE = 90016;
const int32 ABILITY_TALENT_MONSTER_DOG_OVERWHELM = 90015;
const int32 ABILITY_TALENT_MONSTER_SHRIEK_OVERWHLEM = 90129;
const int32 ABILITY_TALENT_MONSTER_OGRE_GRAB = 90036;
const int32 ABILITY_TALENT_BROODMOTHER_GRAB_LEFT = 90098;
const int32 ABILITY_TALENT_BROODMOTHER_GRAB_RIGHT = 90099;

//skills
const int32 ABILITY_SKILL_STEALTH_1 = 100075;
const int32 ABILITY_SKILL_STEALTH_2 = 100076;
const int32 ABILITY_SKILL_STEALTH_3 = 100077;
const int32 ABILITY_SKILL_STEALTH_4 = 100078;

//spells
//humanoids
const int32 ABILITY_SPELL_COMBAT_MAGIC = 17023;
const int32 ABILITY_SPELL_ARCANE_FOCUS = 200256;
const int32 ABILITY_SPELL_MANA_CLASH = 11000;
const int32 ABILITY_SPELL_WYNNES_SEAL_PORTAL = 10704;
const int32 ABILITY_SPELL_CHAIN_LIGHTNING = 10211;
const int32 ABILITY_SPELL_ANIMATE_DEAD = 10508;
const int32 ABILITY_SPELL_HEAL = 10104;
const int32 ABILITY_SPELL_CURE = 10200;
const int32 ABILITY_SPELL_PURIFY = 10207;
const int32 ABILITY_SPELL_SHIELD_PARTY = 10401;
const int32 ABILITY_SPELL_ANTIMAGIC_BURST = 11003;
const int32 ABILITY_SPELL_BEAR = 17010;
const int32 ABILITY_SPELL_SPIDER_SHAPE = 17008;
const int32 ABILITY_SPELL_BLOOD_MAGIC = 10700;
const int32 ABILITY_SPELL_INFERNO = 10002;
const int32 ABILITY_SPELL_FIREBALL = 10003;
const int32 ABILITY_SPELL_MASS_PARALYSIS = 11110;
const int32 ABILITY_SPELL_GREASE = 11113;
const int32 ABILITY_SPELL_EARTHQUAKE = 11116;
const int32 ABILITY_SPELL_SLEEP = 11121;
const int32 ABILITY_SPELL_BLIZZARD = 13000;
const int32 ABILITY_SPELL_TEMPEST = 14002;
const int32 ABILITY_SPELL_DEATH_CLOUD = 15003;
const int32 ABILITY_SPELL_WALKING_BOMB = 12005;

//monsters
const int32 ABILITY_SPELL_MONSTER_OGRE_HURL = 90061;

const int32 MONSTER_DRAGON_BREATH = 90094; 
const int32 MONSTER_BEAR_OVERWHELM = 90130;
const int32 MONSTER_STALKER_OVERWHLEM = 90131;
const int32 MONSTER_DRAGON_OVERWHELM = 90132;
const int32 MONSTER_SPIDER_OVERWHELM = 90133;
const int32 MONSTER_HIGH_DRAGON_FIRE_SPIT = 90168;
const int32 ARCHDEMON_DETONATE_DARKSPAWN = 90002;
const int32 ARCHDEMON_CORRUPTION_BLAST = 90172;
const int32 ARCHDEMON_VORTEX = 90000;
const int32 ARCHDEMON_SMITE = 90001;
const int32 MONSTER_HIGH_DRAGON_GRAB_LEFT = 90169;
const int32 MONSTER_HIGH_DRAGON_GRAB_RIGHT = 90170;


// -----------------------------------------------------------------------------
// BITEM_base.xls
// -----------------------------------------------------------------------------
const int32 BASE_ITEM_TYPE_SHORTBOW = 19;
const int32 BASE_ITEM_TYPE_LONGBOW = 20;
const int32 BASE_ITEM_TYPE_CROSSBOW = 21;
const int32 BASE_ITEM_TYPE_ARMOR_LIGHT = 9;
const int32 BASE_ITEM_TYPE_ARMOR_MEDIUM = 10;
const int32 BASE_ITEM_TYPE_ARMOR_HEAVY = 11;
const int32 BASE_ITEM_TYPE_ARMOR_MASSIVE = 12;
const int32 BASE_ITEM_TYPE_ARMOR_SUPERMASSIVE = 22;
const int32 BASE_ITEM_TYPE_STAFF = 16;
const int32 BASE_ITEM_TYPE_QUICK = 39;
const int32 BASE_ITEM_TYPE_GIFT = 109;

// -----------------------------------------------------------------------------
// BITEM_base.xls::_ItemTypes
// -----------------------------------------------------------------------------
const int32 ITEM_TYPE_INVALID = 0;
const int32 ITEM_TYPE_MISC = 1;
const int32 ITEM_TYPE_WEAPON_MELEE = 2;
const int32 ITEM_TYPE_SHIELD = 3;
const int32 ITEM_TYPE_ARMOR = 4;
const int32 ITEM_TYPE_WEAPON_WAND = 5;
const int32 ITEM_TYPE_WEAPON_RANGED = 6;

// -----------------------------------------------------------------------------
// SoundSets
// -----------------------------------------------------------------------------
const int32 SS_COMBAT_ATTACK = 1;
const int32 SS_COMBAT_BATTLE_CRY = 2;
const int32 SS_COMBAT_STAMINA_LOW = 3;
const int32 SS_MANA_LOW = 4;
const int32 SS_COMBAT_HEAL_ME = 5;
const int32 SS_EXPLORE_HEAL_ME = 6;
// const int32 SS_SCRIPTED_HELP = 7;
const int32 SS_EXPLORE_ENEMIES_SIGHTED_UNDEAD = 8;
const int32 SS_EXPLORE_ENEMIES_SIGHTED_DARKSPAWN = 9;
const int32 SS_EXPLORE_ENEMIES_SIGHTED_DRAGON = 10;
const int32 SS_EXPLORE_ENEMIES_SIGHTED_ANIMAL = 11;
const int32 SS_EXPLORE_ENEMIES_SIGHTED_BEAST = 12;
const int32 SS_EXPLORE_ENEMIES_SIGHTED_OTHER = 13;
const int32 SS_COMBAT_TAUNT = 14;
const int32 SS_COMBAT_ATTACK_GRUNT = 15;
const int32 SS_COMBAT_PAIN_GRUNT = 16;
const int32 SS_COMBAT_NEAR_DEATH = 17;
const int32 SS_COMBAT_DEATH = 18;
// const int32 SS_POISONED = 19;
const int32 SS_SPELL_FAILED = 20;
const int32 SS_COMBAT_ENEMY_KILLED = 21;
const int32 SS_COMBAT_MONSTER_SLEW_PARTY_MEMBER = 22;
const int32 SS_COMBAT_CHEER_PARTY = 23;
// const int32 SS_COMBAT_WEAPON_INEFFECTIVE = 24;
// const int32 SS_EXPLORE_TRAP_DETECTED = 25;
// const int32 SS_EXPLORE_LOOK_HERE = 26;
// const int32 SS_EXPLORE_MOVE_OVER = 27;
const int32 SS_EXPLORE_START_TASK = 28;
// const int32 SS_EXPLORE_STEALTH = 29;
// const int32 SS_CANNOT_DO = 30;
// const int32 SS_TASK_COMPLETE = 31;
const int32 SS_COMBAT_SELECT_NEUTRAL = 32;
// //const int32 SS_COMBAT_SELECT_FRIENDLY         = 33;     // CUT
// //const int32 SS_COMBAT_SELECT_HATE             = 34;     // CUT
// //const int32 SS_COMBAT_SELECT_LOVE             = 35;     // CUT
const int32 SS_EXPLORE_SELECT_NEUTRAL = 36;
const int32 SS_EXPLORE_SELECT_FRIENDLY = 37;
const int32 SS_EXPLORE_SELECT_HATE = 38;
const int32 SS_EXPLORE_SELECT_LOVE = 39;
// //const int32 SS_ARMOR_IMPROVEMENT              = 40;     // CUT
// //const int32 SS_WEAPON_IMPROVEMENT             = 41;     // CUT
// const int32 SS_GIFT_NEUTRAL = 42;
// const int32 SS_GIFT_NEGATIVE = 43;
// const int32 SS_GIFT_POSITIVE = 44;
// const int32 SS_GIFT_ECSTATIC = 45;
// const int32 SS_HELLO = 46;
const int32 SS_YES = 47;
// const int32 SS_NO = 48;
// const int32 SS_STOP = 49;
// const int32 SS_BORED = 50;
// const int32 SS_GOODBYE = 51;
// const int32 SS_THANK_YOU = 52;
// const int32 SS_LAUGH = 53;
// const int32 SS_CUSS = 54;
// const int32 SS_CHEER = 55;
// const int32 SS_SOMETHING_TO_SAY = 56;
// const int32 SS_GOOD_IDEA = 57;
// const int32 SS_BAD_IDEA = 58;
// const int32 SS_THREATEN = 59;
// const int32 SS_BERSERK = 60;
const int32 SS_WARCRY = 61;
// const int32 SS_CAUGHT_STEALING = 62;
// const int32 SS_NO_WEAPON = 63;
// const int32 SS_ORDER_RECIEVED = 64;
const int32 SS_EXPLORE_ENEMIES_SIGHTED_DEMON = 65;
const int32 SS_SKILL_FAILURE = 66;

//TODO  class and coreclass in same variable?
//Class Constants
const int32 CLASS_WIZARD = 2;
const int32 CLASS_ROGUE = 3;
const int32 CLASS_WARRIOR = 1;
// const int32 CLASS_ARCANE_WARRIOR = 10;
// const int32 CLASS_SHAPESHIFTER = 4;
// const int32 CLASS_SPIRITHEALER = 5;
// const int32 CLASS_CHAMPION = 6;
// const int32 CLASS_TEMPLAR = 7;
// const int32 CLASS_REAVER = 9;
// const int32 CLASS_DOG = 17;
// const int32 CLASS_ASSASSIN = 11;
// const int32 CLASS_BLOOD_MAGE = 12;
// const int32 CLASS_BARD = 13;
// const int32 CLASS_RANGER = 14;
// const int32 CLASS_DUELIST = 15;
// const int32 CLASS_SHALE = 16;
// const int32 CLASS_BERSERKER = 8;
// const int32 CLASS_MONSTER_ANIMAL = 18;

// -----------------------------------------------------------------------------
// Races -- RACE_base.xls
// -----------------------------------------------------------------------------
const int32 RACE_INVALID = 0;
const int32 RACE_DWARF = 1;
const int32 RACE_ELF = 2;
const int32 RACE_HUMAN = 3;
// const int32 RACE_QUNARI = 4;
// const int32 RACE_ANIMAL = 5;
// const int32 RACE_BEAST = 6;
// const int32 RACE_DARKSPAWN = 7;
// const int32 RACE_DRAGON = 8;
// const int32 RACE_GOLEM = 9;
// const int32 RACE_SPIRIT = 10;
// const int32 RACE_UNDEAD = 11;

// ----------------------------------------------------------------------------*
// AI Levels - leaving the constants like in code as I'm the only one supposed
//             to ever deal with this -- Georg
// ----------------------------------------------------------------------------*
const int32 CSERVERAIMASTER_AI_LEVEL_VERY_HIGH = 4;
const int32 CSERVERAIMASTER_AI_LEVEL_HIGH = 3;
const int32 CSERVERAIMASTER_AI_LEVEL_NORMAL = 2;
const int32 CSERVERAIMASTER_AI_LEVEL_LOW = 1;
const int32 CSERVERAIMASTER_AI_LEVEL_VERY_LOW = 0;
const int32 CSERVERAIMASTER_AI_LEVEL_INVALID = -1;

// APR_base
const int32 APR_TYPE_OGRE = 47;
const int32 APR_TYPE_BROODMOTHER = 10;
const int32 APR_TYPE_WEREWOLF_A = 22;
const int32 APP_TYPE_ARCHDEMON = 82;
const int32 APP_TYPE_HIGHDRAGON = 66;
const int32 APP_TYPE_WILD_SYLVAN = 65;
const int32 APP_TYPE_DRAGON = 4;
const int32 APP_TYPE_DRAKE = 80;
const int32 APP_TYPE_PRIDE_DEMON = 72;
const int32 APP_TYPE_HURLOCK = 16;
const int32 APP_TYPE_GENLOCK = 50;

// itemprps.xls

// const int32 ITEM_PROPERTY_ONHIT_SLAY_DARKSPAWN = 4005;
// const int32 ITEM_PROPERTY_ONHIT_MAGESLAYER = 4006;
// const int32 ITEM_PROPERTY_ONHIT_VICIOUS = 10002;
// const int32 ITEM_PROPERTY_NO_ATTRIBUTE_REQUIREMENTS = 10000;
const int32 ITEM_PROPERTY_IS_HEALING_POTION = 6116;
const int32 ITEM_PROPERTY_IS_MANA_POTION = 6117;
// const int32 ITEM_PROPERTY_ONHIT_SLOW = 6114;

const int32 PLC_STATE_DOOR_UNLOCKED = 0;
const int32 PLC_STATE_DOOR_LOCKED = 1;