#include "DA2UE4.h"
#include "items_h.h"

int32 Items_GetMeleeWeaponSet(AActor* oCreature)
{
	int32 nWeaponSet = GetActiveWeaponSet(oCreature);
	int32 nInactiveWeaponSet = (nWeaponSet == 1) ? 0 : 1;

	AActor* oWeapon = GetItemInEquipSlot(INVENTORY_SLOT_MAIN, oCreature, nWeaponSet);
	if (IsObjectValid(oWeapon))
	{
		if (GetItemType(oWeapon) == ITEM_TYPE_WEAPON_MELEE)
		{
			return nWeaponSet;
		}
	}

	oWeapon = GetItemInEquipSlot(INVENTORY_SLOT_MAIN, oCreature, nInactiveWeaponSet);
	if (IsObjectValid(oWeapon))
	{
		if (GetItemType(oWeapon) == ITEM_TYPE_WEAPON_MELEE)
		{
			return nInactiveWeaponSet;
		}
	}

	return -1;
}

int32 Items_GetRangedWeaponSet(AActor* oCreature, int32 bConsiderStaffRanged)
{

	int32 nWeaponSet = GetActiveWeaponSet(oCreature);
	int32 nInactiveWeaponSet = (nWeaponSet == 1) ? 0 : 1;

	AActor* oWeapon = GetItemInEquipSlot(INVENTORY_SLOT_MAIN, oCreature, nWeaponSet);
	if (IsObjectValid(oWeapon))
	{
		if (GetItemType(oWeapon) == ITEM_TYPE_WEAPON_RANGED)
		{
			//if (Items_CheckAmmo(oCreature, oWeapon))
			//{
			return nWeaponSet;
			//}
		}
		if (bConsiderStaffRanged && GetItemType(oWeapon) == ITEM_TYPE_WEAPON_WAND)
			return nWeaponSet;
	}

	oWeapon = GetItemInEquipSlot(INVENTORY_SLOT_MAIN, oCreature, nInactiveWeaponSet);
	if (IsObjectValid(oWeapon))
	{
		if (GetItemType(oWeapon) == ITEM_TYPE_WEAPON_RANGED)
		{
			//TODO Items_GetRangedWeaponSet check for ammo?
			//if (Items_CheckAmmo(oCreature, oWeapon, nInactiveWeaponSet))
			//{
			return nInactiveWeaponSet;
			//}  
		}
		if (bConsiderStaffRanged && GetItemType(oWeapon) == ITEM_TYPE_WEAPON_WAND)
			return nInactiveWeaponSet;
	}

	return -1;
}

int32 Items_CheckAmmo(AActor* oAttacker, AActor* oWeapon, int32 nWeaponSet)
{
	// -----------------------------------------------------------------
	// Ammo is in the off hand
	// -----------------------------------------------------------------
	AActor* oAmmoStack = GetItemInEquipSlot(INVENTORY_SLOT_OFFHAND, oAttacker, nWeaponSet);

	if (oWeapon == nullptr)
	{
		oWeapon = GetItemInEquipSlot(INVENTORY_SLOT_MAIN, oAttacker, nWeaponSet);
	}

	int32 nWeaponType = GetBaseItemType(oWeapon);

	// -----------------------------------------------------------------
	// need to have a valid ammo AActor* in the offhand
	// -----------------------------------------------------------------
	if (IsObjectValid(oAmmoStack))
	{
		// -----------------------------------------------------------------
		// Check that this is the right ammo type for this weapon
		// -----------------------------------------------------------------
		int32 nBaseItem = GetBaseItemType(oAmmoStack);
		if (nBaseItem == GetM2DAInt(TABLE_ITEMS, "Ammo", nWeaponType))
		{
			// -----------------------------------------------------------------
			// Check that the stack size is > 0
			// -----------------------------------------------------------------
			if (GetItemStackSize(oAmmoStack) > 0)
			{
				return GetItemStackSize(oAmmoStack);
			}

		}
	}

	// -----------------------------------------------------------------
	// for everything else ... there is FALSE_;
	// -----------------------------------------------------------------
	return FALSE_;
}

int32 Items_CheckMaxAmmo(AActor* oAttacker, AActor* oWeapon, int32 nWeaponSet)
{
	int32 nMaxAmmo = 0;
	AActor* oAmmoStack = GetItemInEquipSlot(INVENTORY_SLOT_OFFHAND, oAttacker, nWeaponSet);

	if (oWeapon == nullptr)
	{
		oWeapon = GetItemInEquipSlot(INVENTORY_SLOT_MAIN, oAttacker, nWeaponSet);
	}

	int32 nWeaponType = GetBaseItemType(oWeapon);

	if (IsObjectValid(oAmmoStack))
	{
		// -----------------------------------------------------------------
		// Check that this is the right ammo type for this weapon
		// -----------------------------------------------------------------
		int32 nBaseItem = GetBaseItemType(oAmmoStack);
		if (nBaseItem == GetM2DAInt(TABLE_ITEMS, "Ammo", nWeaponType))
		{
			nMaxAmmo = GetM2DAInt(TABLE_ITEMS, "StackSize", nBaseItem);
		}
	}
	return nMaxAmmo;
}