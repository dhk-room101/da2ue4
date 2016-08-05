#include "DA2UE4.h"
#include "ability_h.h"
#include "effect_upkeep_h.h"

int32 Ability_IsAbilityActive(AActor* oCreature, int32 nAbilityID)
{
	int32 nActive = FALSE_;
	TArray<FEffect> thisEffects = GetEffects(oCreature, EFFECT_TYPE_INVALID, nAbilityID);
	int32 nSize = thisEffects.Num();

#ifdef DEBUG
	Log_Trace(LOG_CHANNEL_COMBAT_ABILITY, "Ability_IsAbilityActive", "START, abilityID: " + IntToString(nAbilityID) + ", number of effects for this ability: " + IntToString(nSize), oCreature);
#endif

	return (nSize > 0 ? TRUE_ : FALSE_);
}

int32 Ability_CheckFlag(int32 nAbility, int32 nFlag)
{
	int32 nAbiFlags = GetM2DAInt(TABLE_ABILITIES_SPELLS, "Flags", nAbility);
	return ((nAbiFlags & nFlag) == nFlag);
}

int32 Ability_GetAbilityType(int32 nAbility)
{
	return GetM2DAInt(TABLE_ABILITIES_SPELLS, "AbilityType", nAbility);
}

int32 Ability_GetAbilityTable(int32 nAbilityType)
{
	// Which 2DA to read the data from
	int32 n2DA = TABLE_ABILITIES_TALENTS;

	if (nAbilityType == ABILITY_TYPE_SPELL)
	{
		n2DA = TABLE_ABILITIES_SPELLS;
	}

	return n2DA;
}

FString Ability_GetAbilityNameById(int32 nAbilityId)
{
#ifdef DEBUG
	//int32 nType = _GetAbilityType(nAbilityId);
	int32 nType = Ability_GetAbilityType(nAbilityId);
	int32 nTable = Ability_GetAbilityTable(nType);
	//int32 nTable = _GetAbilityTable(nType);
	return (GetM2DAString(nTable, "LABEL", nAbilityId) + " (" + IntToString(nAbilityId) + ")");
#endif

	return "";

}

int32 Ability_CostCheck(AActor* oCaster, int32 nAbility, int32 nAbilityType, AActor* oItem)
{
	int32 bModal = Ability_IsModalAbility(nAbility);
	float fCost = Ability_GetAbilityCost(oCaster, nAbility, nAbilityType, bModal);
	float fMana=0.f;
	if (bModal)
	{
		fMana = GetCreatureProperty(oCaster, PROPERTY_DEPLETABLE_MANA_STAMINA, PROPERTY_VALUE_TOTAL);
		// You can use modal abilities when you have more mana available
		// than upkeep cost and at least 1 point of mana
		return fMana >= fCost && fMana >= 1.0f;
	}
	
	//unused 	int32 nNumItems;
	int32 bRet = TRUE_;

	// if the ability consumes an item, there is no cost
	if (nAbilityType == ABILITY_TYPE_ITEM && IsObjectValid(oItem))
	{
		return (GetItemStackSize(oItem) > 0);
	}

	// only abilities and spells cost...
	else if (nAbilityType == ABILITY_TYPE_SPELL || nAbilityType == ABILITY_TYPE_TALENT)
	{

		// Shapeshifted characters cast their abilities for free.
		if (IsShapeShifted(oCaster))
		{
			return TRUE_;
		}

		// blood magic doesn't use mana or stamina, it uses health and the caster can very much
		// kill himself, so we don't subtract any damage here...
		if (!Ability_IsBloodMagic(oCaster))
		{
			fMana = GetCreatureProperty(oCaster, PROPERTY_DEPLETABLE_MANA_STAMINA, PROPERTY_VALUE_CURRENT);

			if (fMana < fCost || fMana < 1.0f)
			{
				bRet = FALSE_;
			}

		}



#ifdef DEBUG
		Log_Trace(LOG_CHANNEL_COMBAT_ABILITY, "ability_h.CostCheck", "AbilityUse Cost: " + FloatToString(fCost) + ", Have:" + FloatToString(fMana));
#endif

	}

	return bRet;
}

int32 Ability_IsModalAbility(int32 nAbility)
{
	return GetM2DAInt(TABLE_ABILITIES_SPELLS, "usetype", nAbility) == 2;
}

float Ability_GetAbilityCost(AActor* oCaster, int32 nAbility, int32 nAbilityType, int32 bUpkeep)
{
	//This function is duplicated within the game executable. Any change made to this function will 
	//result in GUI glitches and other bugs. Sorry.
	if (nAbilityType == ABILITY_TYPE_INVALID)
	{
		nAbilityType = Ability_GetAbilityType(nAbility);
	}
	FString sCol = bUpkeep ? "costupkeep" : "cost";


	float fCost = GetM2DAFloat(TABLE_ABILITIES_SPELLS, sCol, nAbility);


	if (nAbilityType != ABILITY_TYPE_ITEM)
	{
		if (fCost > 0.0f)
		{

			float fModifier = 1.0;

#ifdef DEBUG
			Log_Trace(LOG_CHANNEL_COMBAT_ABILITY, "Ability_GetAbilityCost", "Initial Cost:" + FloatToString(fCost));
#endif

			if (!bUpkeep)
			{
				//This function is duplicated within the game executable. Any change made to this function will 
				//result in GUI glitches and other bugs. Sorry.
				fModifier += GetCreatureProperty(oCaster, PROPERTY_ATTRIBUTE_FATIGUE)*0.01f;
				fCost = FloatToInt(fCost*fModifier + 0.5) * 1.0;

			}

			if (fCost > 0.0f)
			{
				float fDiffMod = Diff_GetAbilityUseMod(oCaster);
				fCost *= (1.0 / fDiffMod);

#ifdef DEBUG
				Log_Trace(LOG_CHANNEL_COMBAT_ABILITY, "Ability_GetAbilityCost", "Difficulty Mod : * " + FloatToString(fDiffMod));
#endif
			}

#ifdef DEBUG
			Log_Trace(LOG_CHANNEL_COMBAT_ABILITY, "Ability_GetAbilityCost", "Calculating: " + FloatToString(fCost) + "*(1+(" + FloatToString(GetCreatureProperty(oCaster, PROPERTY_ATTRIBUTE_ABILITY_COST_MODIFIER)) + "*0.01))");
			Log_Trace(LOG_CHANNEL_COMBAT_ABILITY, "Ability_GetAbilityCost", "New Cost:" + FloatToString(fCost));
#endif
		}
	}

	return fCost;
}

int32 Ability_IsBloodMagic(AActor* oCaster)
{
	return IsModalAbilityActive(oCaster, ABILITY_SPELL_BLOOD_MAGIC);
}

int32 Ability_CheckUseConditions(AActor* oCaster, AActor* oTarget, int32 nAbility, AActor* oItem)
{

	FString sItemTag = IsObjectValid(oItem) ? GetTag(oItem) : "";
	int32 nAbilityType = GetAbilityType(nAbility);

	int32 nCondition = GetM2DAInt(TABLE_ABILITIES_SPELLS, "conditions", nAbility);
	int32 bRet = TRUE_;
	FString sRet;

	float fCooldown = GetRemainingCooldown(oCaster, nAbility, sItemTag);
	if (fCooldown > 0.0f)
	{
#ifdef DEBUG
		Log_Trace(LOG_CHANNEL_COMBAT_ABILITY, "Ability_CheckUseConditions", "Trying to execute ability with cooldown " + FloatToString(fCooldown) + " remaining ability which is already active. FALSE_.");
#endif
		return FALSE_;
	}
	else

		// If nAbility is modal and active already - do nothing
		if (IsModalAbilityActive(oCaster, nAbility))
		{
#ifdef DEBUG
			Log_Trace(LOG_CHANNEL_COMBAT_ABILITY, "Ability_CheckUseConditions", "Trying to execute modal ability which is already active. FALSE_.");
#endif
			return FALSE_;
		}


	// -------------------------------------------------------------------------
	// No conditions? bail right here
	// -------------------------------------------------------------------------
	if (nCondition == 0)
	{
#ifdef DEBUG
		Log_Trace(LOG_CHANNEL_COMBAT_ABILITY, "CheckUseConditions", "TRUE_ (no condition)");
#endif
		return TRUE_;
	}

	// -------------------------------------------------------------------------
	// CONDITION_MELEE_WEAPON - Caster needs a melee weapon in main hand
	// -------------------------------------------------------------------------
	if ((nCondition & 1) == 1)
	{

#ifdef DEBUG
		sRet = ((bRet) ? "TRUE_" : "FALSE_");
		Log_Trace(LOG_CHANNEL_COMBAT_ABILITY, "CheckUseConditions", "Melee: " + sRet);
#endif
		bRet = bRet &&   IsUsingMeleeWeapon(oCaster);

		if (!bRet)
		{
			return FALSE_;
		}

	}

	// -------------------------------------------------------------------------
	// CONDITION_SHIELD - Caster needs a shield in the offhand
	// -------------------------------------------------------------------------
	if ((nCondition & 2) == 2)
	{
		bRet = bRet && IsUsingShield(oCaster);
#ifdef DEBUG
		sRet = ((bRet) ? "TRUE_" : "FALSE_");
		Log_Trace(LOG_CHANNEL_COMBAT_ABILITY, "CheckUseConditions", "Shield: " + sRet);
#endif

		if (!bRet)
		{
			return FALSE_;
		}

	}

	// -------------------------------------------------------------------------
	// CONDITION_RANGED_WEAPON - Caster needs a ranged weapon in main hand
	// -------------------------------------------------------------------------
	if ((nCondition & 4) == 4)
	{
		bRet = bRet && IsUsingRangedWeapon(oCaster);
#ifdef DEBUG
		sRet = ((bRet) ? "TRUE_" : "FALSE_");
		Log_Trace(LOG_CHANNEL_COMBAT_ABILITY, "CheckUseConditions", "Ranged: " + sRet);
#endif

		if (!bRet)
		{
			return FALSE_;
		}

	}


	// -------------------------------------------------------------------------
	// CONDITION_BEHIND_TARGET - Caster needs to be located behind the target
	// -------------------------------------------------------------------------
	if ((nCondition & 8) == 8)
	{
		float fAngle = GetAngleBetweenObjects(oTarget, oCaster);

		bRet = bRet && (fAngle >= 90.0f && fAngle <= 270.0f);
#ifdef DEBUG
		sRet = ((bRet) ? "TRUE_" : "FALSE_");
		Log_Trace(LOG_CHANNEL_COMBAT_ABILITY, "CheckUseConditions", "Back: " + sRet);
#endif

		if (!bRet)
		{
			return FALSE_;
		}

	}

	// -------------------------------------------------------------------------
	// CONDITION_ACTIVE_MODAL_ABILITY - A specific modal ability needs to be active
	// -------------------------------------------------------------------------
	if ((nCondition & 16) == 16)
	{
		int32 nModalAbility = GetM2DAInt(TABLE_ABILITIES_TALENTS, "condition_mode", nAbility);
		if (nModalAbility != 0)
		{
			bRet = bRet && IsModalAbilityActive(oCaster, nModalAbility);
#ifdef DEBUG
			sRet = ((bRet) ? "TRUE_" : "FALSE_");
			Log_Trace(LOG_CHANNEL_COMBAT_ABILITY, "CheckUseConditions", "Mode Active: " + sRet);
#endif
		}

		if (!bRet)
		{
			return FALSE_;
		}

	}

	// -------------------------------------------------------------------------
	// CONDITION_TARGET_HUMANOID - Target is humanoid
	// -------------------------------------------------------------------------
	if ((nCondition & 32) == 32)
	{
		bRet = bRet && IsHumanoid(oTarget);

#ifdef DEBUG
		sRet = ((bRet) ? "TRUE_" : "FALSE_");
		Log_Trace(LOG_CHANNEL_COMBAT_ABILITY, "CheckUseConditions", "IsHumanoid: " + sRet);
#endif

		if (!bRet)
		{
			return FALSE_;
		}


	}

	// -------------------------------------------------------------------------
	// CONDITION_DUAL_WEAPONS
	// -------------------------------------------------------------------------
	if ((nCondition & 64) == 64)
	{
		bRet = bRet && GetWeaponStyle(oCaster) == WEAPONSTYLE_DUAL;
#ifdef DEBUG
		sRet = ((bRet) ? "TRUE_" : "FALSE_");
		Log_Trace(LOG_CHANNEL_COMBAT_ABILITY, "CheckUseConditions", "UsingDualWeapons: " + sRet);
#endif

		if (!bRet)
		{
			return FALSE_;
		}
	}

	// -------------------------------------------------------------------------
	// CONDITION_DUAL_WEAPONS
	// -------------------------------------------------------------------------
	if ((nCondition & 128) == 128)
	{
		bRet = bRet && GetWeaponStyle(oCaster) == WEAPONSTYLE_TWOHANDED;
#ifdef DEBUG
		sRet = ((bRet) ? "TRUE_" : "FALSE_");
		Log_Trace(LOG_CHANNEL_COMBAT_ABILITY, "CheckUseConditions", "Using2HWeapon: " + sRet);
#endif
		if (!bRet)
		{
			return FALSE_;
		}

	}


#ifdef DEBUG
	sRet = ((bRet) ? "TRUE_" : "FALSE_");
	Log_Trace(LOG_CHANNEL_COMBAT_ABILITY, "CheckUseConditions", sRet + " condition: " + IntToHexString(nCondition));
#endif

	return bRet;

}

int32 Ability_GetAbilityTargetType(int32 nAbility, int32 nAbilityType)
{
	int32 n2DA = Ability_GetAbilityTable(nAbilityType);
	int32 nTargetType = GetM2DAInt(n2DA, "TargetType", nAbility);
	return nTargetType;
}