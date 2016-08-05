#pragma once

#include "m2da_constants_h.h"
#include "log_h.h"
#include "core_h.h"

int32 Items_GetMeleeWeaponSet(AActor* oCreature);

int32 Items_GetRangedWeaponSet(AActor* oCreature, int32 bConsiderStaffRanged = FALSE_);

int32 Items_CheckAmmo(AActor* oAttacker, AActor* oWeapon = nullptr, int32 nWeaponSet = INVALID_WEAPON_SET);

// Returns max size of ammo
int32 Items_CheckMaxAmmo(AActor* oAttacker, AActor* oWeapon = nullptr, int32 nWeaponSet = INVALID_WEAPON_SET);