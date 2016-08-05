#include "DA2UE4.h"
#include "m2da_data_h.h"
#include "ldf.h"

int32 GetCreatureTypeClassification(int32 nAppearanceType)
{
	return GetM2DAInt(TABLE_APPEARANCE, APPEARANCE_DATA_CREATURE_TYPE, nAppearanceType);
}

int32 GetIsSoundSetEntryTypeRestricted(int32 nSSEntry)
{
	return GetM2DAInt(TABLE_SOUNDSETS, SS_DATA_RESTRICT, nSSEntry);
}

float GetItemRange(AActor* oItem)
{
	int32 nBaseItemType = GetBaseItemType(oItem);
	return GetM2DAFloat(TABLE_ITEMS, ITEM_DATA_RANGE, nBaseItemType) * SCALE_MULTIPLIER;
}

int32 GetAbilityType(int32 nAbility)
{
	return GetM2DAInt(TABLE_ABILITIES_SPELLS, "AbilityType", nAbility);
}

int32 CanPerformDeathblows(AActor* oCreature)
{
	int32 nApp = GetAppearanceType(oCreature);
	return (GetM2DAInt(TABLE_APPEARANCE, APPEARANCE_DATA_CAN_DO_DEATHBLOWS, nApp));

}