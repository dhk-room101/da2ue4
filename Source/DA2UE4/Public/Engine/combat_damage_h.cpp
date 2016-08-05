#include "DA2UE4.h"
#include "core_h.h"
#include "m2da_constants_h.h"
#include "effects_h.h"
#include "combat_damage_h.h"
//#include "sys_disease"
//#include "plt_cod_aow_spellcombo9"

float Combat_Damage_GetAttackDamage(AActor* oAttacker, AActor* oTarget, AActor* oWeapon, int32 nAttackResult, float fArmorPenetrationBonus, int32 bForceMaxWeaponDamage)
{

	int32 nHand = HAND_MAIN;
	int32 nSlot = GetItemEquipSlot(oWeapon);

	// -------------------------------------------------------------------------
	// special case: one hit kill forms generally don't do damage...
	// -------------------------------------------------------------------------
	if (IsShapeShifted(oAttacker))
	{
		if (GetM2DAInt(TABLE_APPEARANCE, "OneShotKills", GetAppearanceType(oAttacker)))
		{
			return 1.0f;
		}
	}

	if (IsObjectValid(oWeapon))
	{
		if (nSlot == INVENTORY_SLOT_MAIN)// CUT || nSlot == INVENTORY_SLOT_BITE)
		{
			nHand = HAND_MAIN;
		}
		else if (nSlot == INVENTORY_SLOT_OFFHAND)
		{
			nHand = HAND_OFFHAND;
		}

		// Mage staffs have their own rules
		if (nAttackResult != COMBAT_RESULT_DEATHBLOW && GetBaseItemType(oWeapon) == BASE_ITEM_TYPE_STAFF)
		{
			if (!GetHasEffects(oAttacker, EFFECT_TYPE_SHAPECHANGE))
			{
				return Combat_Damage_GetMageStaffDamage(oAttacker, oTarget, oWeapon);
			}
			else
			{
				oWeapon = nullptr;
			}
		}

	}

	// Weapon Attribute Bonus Factor
	float fFactor = GetWeaponAttributeBonusFactor(oWeapon);

	// Attribute Modifier
	float fStrength = Combat_Damage_GetAttributeBonus(oAttacker, nHand, oWeapon) * fFactor;

	// Weapon Damage
	float fWeapon = IsObjectValid(oWeapon) ? DmgGetWeaponDamage(oWeapon, bForceMaxWeaponDamage) : COMBAT_DEFAULT_UNARMED_DAMAGE;

	// Game Difficulty Adjustments
	float fDiffBonus = Diff_GetRulesDamageBonus(oAttacker);


	float fDamage = fWeapon + fStrength + fDiffBonus;
	float fDamageScale = GetM2DAFloat(TABLE_AUTOSCALE, "fDamageScale", GetCreatureRank(oAttacker));

	float fAr = DmgGetArmorRating(oTarget);

	// TODO GXA Override
	if (HasAbility(oAttacker, 401101) == TRUE_) // GXA Spirit Damage
	{
		if (IsModalAbilityActive(oAttacker, 401100) == TRUE_) // GXA Spirit Warrior
		{
			// bypass armor for normal attacks
			fAr = 0.0f;
		}
	}
	// GXA Override

	float fAp = DmgGetArmorPenetrationRating(oAttacker, oWeapon) + fArmorPenetrationBonus;
	float fDmgBonus = GetCreatureProperty(oAttacker, PROPERTY_ATTRIBUTE_DAMAGE_BONUS);





#ifdef DEBUG
	_LogDamage("Total: " + FloatToString(fDamage), oTarget);
	_LogDamage("  fStrength: " + FloatToString(fStrength));
	_LogDamage("  fWeapon  : " + FloatToString(fWeapon));
	_LogDamage("  fDmgBonus: " + FloatToString(fDmgBonus));
	_LogDamage("        fAr: " + FloatToString(fAr));
	_LogDamage("        fAp: " + FloatToString(fAp));
	_LogDamage(" fRankScale: " + FloatToString(fDamageScale));
#endif


	if (nAttackResult == COMBAT_RESULT_CRITICALHIT)
	{
		fDamage *= GetCriticalDamageModifier(oAttacker);
#ifdef DEBUG
		_LogDamage("Crit:        " + FloatToString(fDamage));
#endif
	}
	else if (nAttackResult == COMBAT_RESULT_BACKSTAB)
	{
		fDamage = Combat_Damage_GetBackstabDamage(oAttacker, oWeapon, fDamage);
#ifdef DEBUG
		_LogDamage("Backstab:        " + FloatToString(fDamage));
#endif
	}
	else if (nAttackResult == COMBAT_RESULT_DEATHBLOW)
	{
		fDamage = GetMaxHealth(oTarget) + 1.0f;
#ifdef DEBUG
		_LogDamage("Deathblow damage:" + FloatToString(fDamage));
#endif
	}

	fDamage = fDamage - MaxF(0.0f, fAr - fAp);

	fDamage += fDmgBonus + Combat_Damage_GetTalentBoni(oAttacker, oTarget, oWeapon);


	// -------------------------------------------------------------------------
	// Damage scale only kicks in on 'significant' damage.
	// -------------------------------------------------------------------------
	if (fDamageScale > 0.0 && fDamage > GetDamageScalingThreshold())
	{
		fDamage *= fDamageScale;
	}

	// -------------------------------------------------------------------------
	// Weapon damage is always at least 1, even with armor. This is intentional
	// to avoid deadlocks of creatures that are both unable to damage each other
	// -------------------------------------------------------------------------
	fDamage = MaxF(1.0f, fDamage);



	return (fDamage);
}

float DmgGetArmorRating(AActor* oDefender)
{
	float fAr = GetCreatureProperty(oDefender, PROPERTY_ATTRIBUTE_ARMOR);
	// Armor is calculated 70 (fixed) /30 (random)
	float fArRolled = (RandFF(fAr) * COMBAT_ARMOR_RANDOM_ELEMENT) + (fAr * (1.0f - COMBAT_ARMOR_RANDOM_ELEMENT));

	AActor* oArmor = GetItemInEquipSlot(INVENTORY_SLOT_CHEST, oDefender);

#ifdef DEBUG
	_LogDamage("  fAr:  " + GetTag(oArmor) + ":" + FloatToString(fArRolled) + " = " + FloatToString(fAr * 0.75) + " + Rand(" + FloatToString(fAr * 0.25) + ")");
#endif

	return fArRolled;
}

float DmgGetArmorPenetrationRating(AActor* oAttacker, AActor* oWeapon)
{

	float fBase = GetItemStat(oWeapon, ITEM_STAT_ARMOR_PENETRATION) + GetCreatureProperty(oAttacker, PROPERTY_ATTRIBUTE_AP);

	if (IsMeleeWeapon2Handed(oWeapon))
	{
		// -----------------------------------------------------------------
		// STRONG: Armor Rating
		// -----------------------------------------------------------------
		if (HasAbility(oAttacker, ABILITY_TALENT_STRONG))
		{
#ifdef DEBUG
			_LogDamage("  fAP: (Modified +25% by TALENT_STRONG) ");
#endif
			fBase *= 1.25;
		}
	}


	return fBase;
}

float GetCriticalDamageModifier(AActor* oAttacker)
{
	return COMBAT_CRITICAL_DAMAGE_MODIFIER + (GetCreatureProperty(oAttacker, PROPERTY_ATTRIBUTE_CRITICAL_RANGE) / 100.0);
}

float Combat_Damage_GetBackstabDamage(AActor* oAttacker, AActor* oWeapon, float fDamage)
{

	//  ------------------------------------------------------------------------
	// Each backstab is an auto crit.
	//  ------------------------------------------------------------------------
	fDamage *= GetCriticalDamageModifier(oAttacker);


	// -------------------------------------------------------------------------
	// Exploit Weakness:  Backstab Damage = Int / 3
	// -------------------------------------------------------------------------
	if (HasAbility(oAttacker, ABILITY_TALENT_EXPLOIT_WEAKNESS))
	{
		float fBase = MaxF(0.0, (GetAttributeModifier(oAttacker, PROPERTY_ATTRIBUTE_INTELLIGENCE) / 3.0));
		float fMod = MaxF(0.2, RandomFloat());
		fDamage += (fBase * fMod);
	}

	// GXA Override
	if (HasAbility(oAttacker, 401312) == TRUE_) // GXA Deep Striking
	{
		if (IsModalAbilityActive(oAttacker, 401310) == TRUE_) // GXA Shadow Striking
		{
			fDamage *= 1.5f;
		}
	}
	// GXA Override

	return fDamage;
}

float Combat_Damage_GetTalentBoni(AActor* oAttacker, AActor* oDefender, AActor* oWeapon)
{

	float fBase = 0.0;


	if (HasAbility(oAttacker, ABILITY_TALENT_SHATTERING_BLOWS))
	{
		if (IsObjectValid(oDefender))
		{
			if (GetCreatureAppearanceFlag(oDefender, APR_RULES_FLAG_CONSTRUCT))
			{
				if (IsUsingMeleeWeapon(oAttacker, oWeapon) && IsMeleeWeapon2Handed(oWeapon))
				{
					fBase += (GetAttributeModifier(oAttacker, PROPERTY_ATTRIBUTE_STRENGTH) * 0.5f);
				}
			}
		}
	}

	if (IsModalAbilityActive(oAttacker, ABILITY_TALENT_BLOOD_FRENZY))
	{
		float fMod = (10.0 *  MaxF(0.0, 1.0 - _GetRelativeResourceLevel(oAttacker, PROPERTY_DEPLETABLE_HEALTH)));
#ifdef DEBUG
		_LogDamage("-- BLOOD_FRENZY DAMAGE BONUS: " + FloatToString(fMod));
#endif

		fBase += fMod;
	}


	return fBase;
}