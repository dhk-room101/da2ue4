#pragma once

class Engine;

int32 Items_GetMeleeWeaponSet(AActor* oCreature);

int32 Items_GetRangedWeaponSet(AActor* oCreature, int32 bConsiderStaffRanged = 0);

int32 Items_CheckAmmo(AActor* oAttacker, AActor* oWeapon = nullptr, int32 nWeaponSet = -1);

// Returns max size of ammo
int32 Items_CheckMaxAmmo(AActor* oAttacker, AActor* oWeapon = nullptr, int32 nWeaponSet = -1);