#pragma once

// -----------------------------------------------------------------------------
// combat_damage_h - Damage Include
// -----------------------------------------------------------------------------
/*
Damage Utility Include

Contains Damage calculation and resistance logic

*/
// -----------------------------------------------------------------------------
// Owner: Georg Zoeller
// -----------------------------------------------------------------------------

#include "ldf.h"

class Engine;

float Combat_Damage_GetAttackDamage(AActor* oAttacker, AActor* oTarget, AActor* oWeapon, int32 nAttackResult, float fArmorPenetrationBonus = 0.0, int32 bForceMaxWeaponDamage = FALSE_);

float DmgGetArmorRating(AActor* oDefender);

float DmgGetArmorPenetrationRating(AActor* oAttacker, AActor* oWeapon);

float GetCriticalDamageModifier(AActor* oAttacker);

float Combat_Damage_GetBackstabDamage(AActor* oAttacker, AActor* oWeapon, float fDamage);

float Combat_Damage_GetTalentBoni(AActor* oAttacker, AActor* oDefender, AActor* oWeapon);