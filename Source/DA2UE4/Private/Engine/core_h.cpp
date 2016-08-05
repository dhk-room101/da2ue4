#include "DA2UE4.h"
#include "DA2UE4Library.h"
#include "DA2UE4Module.h"
#include "core_h.h"
#include "log_h.h"
#include "ai_constants_h.h"

void _LogDamage(FString msg, AActor* oTarget)
{
	Log_Trace(LOG_CHANNEL_COMBAT_DAMAGE, "combat_damage", msg, oTarget);
}

int32 IsPartyMember(AActor* oCreature)
{
	return IsFollower(oCreature);
}

int32 IsDying(AActor* oCreature)
{
	// Death effect is present even before the creature is considered fully dead.
	int32 bRet = HasDeathEffect(oCreature, TRUE_);
	//int32 bRet = GetCreatureFlag(oCreature, CREATURE_RULES_FLAG_DYING);

#ifdef DEBUG
	if (bRet)
		Log_Trace(LOG_CHANNEL_SYSTEMS, "core_h.GetCreatureFlag", "IsDying TRUE_", oCreature);
#endif

	return (bRet);
}

int32 IsDisabled(AActor* oCreature, int32 bGroundCheck)
{
	TArray<FEffect> effectsArray = GetEffects(oCreature, EFFECT_TYPE_KNOCKDOWN);
	if (effectsArray.Num() > 0) return TRUE_;
	return (FALSE_);
}

int32 GetCreatureCoreClass(AActor* oCreature)
{
	int32 nCurrentClass = FloatToInt(GetCreatureProperty(oCreature, PROPERTY_SIMPLE_CURRENT_CLASS));

	int32 nCoreClass = GetM2DAInt(TABLE_RULES_CLASSES, "BaseClass", nCurrentClass);

	if (nCoreClass == 0) // the current is the core
		return nCurrentClass;
	else
		return nCoreClass;
}

float GetCurrentHealth(AActor* oActor)
{
	//TODO current and max health for placeables
	float fHealth = 0.f;

	int32 nObjectType = GetObjectType(oActor);
	if (nObjectType == OBJECT_TYPE_CREATURE)
	{
		fHealth = GetCreatureProperty(oActor, PROPERTY_DEPLETABLE_HEALTH, PROPERTY_VALUE_CURRENT);
	}

	return fHealth;
}

void SetCurrentHealth(AActor* oActor, float fNewValue)
{
	if (IsObjectValid(oActor))
	{
		int32 nObjectType = GetObjectType(oActor);
		if (nObjectType == OBJECT_TYPE_PLACEABLE)
		{
			SetPlaceableHealth(oActor, FloatToInt(fNewValue));
		}
		else if (nObjectType == OBJECT_TYPE_CREATURE)
		{
			SetCreatureProperty(oActor, PROPERTY_DEPLETABLE_HEALTH, fNewValue, PROPERTY_VALUE_CURRENT);
		}
		else
		{
#ifdef DEBUG
			LogWarning("SetCurrentHealth called on AActor* [" + GetTag(oActor) + "] " + "[type: " + IntToString(nObjectType) + "] that is not a creature or placeable. Please contact Georg. Script: core_h");
#endif
		}
	}
}

float GetMaxHealth(AActor* oActor)
{
	int32 nObjectType = GetObjectType(oActor);
	if (nObjectType == OBJECT_TYPE_CREATURE)
	{
		return GetCreatureProperty(oActor, PROPERTY_DEPLETABLE_HEALTH, PROPERTY_VALUE_TOTAL);
	}
	else
	{
#ifdef DEBUG
		LogWarning("GetCurrentHealth called on AActor* [" + GetTag(oActor) + "] that is not a creature or placeable. Please contact Georg. Script: core_h.cpp");// +GetCurrentScriptName());
#endif
		return 1.0f;
	}
}

float GetCurrentManaStamina(AActor* aActor)
{
	return GetCreatureProperty(aActor, PROPERTY_DEPLETABLE_MANA_STAMINA, PROPERTY_VALUE_CURRENT);
}

float GetMaxManaStamina(AActor* oActor)
{
	int32 nObjectType = GetObjectType(oActor);
	if (nObjectType == OBJECT_TYPE_CREATURE)
	{
		return GetCreatureProperty(oActor, PROPERTY_DEPLETABLE_MANA_STAMINA, PROPERTY_VALUE_TOTAL);
	}
	else
	{
#ifdef DEBUG
		LogWarning("GetCurrentManaStamina called on AActor* [" + GetTag(oActor) + "] that is not a creature or placeable. Please contact Georg. Script: core_h.cpp");// +GetCurrentScriptName());
#endif
		return 1.0f;
	}
}

int32 IsUsingMeleeWeapon(AActor* oCreature, AActor* oItem)
{

	if (!IsObjectValid(oItem))
	{
		oItem = GetItemInEquipSlot(INVENTORY_SLOT_MAIN, oCreature);
	}
#ifdef DEBUG //DHK
	Log_Trace(LOG_CHANNEL_TEMP, "IsUsingMeleeWeapon", "creature: " + (IsObjectValid(oCreature) ? GetTag(oCreature) : "null") +
		", Weapon= " + (IsObjectValid(oItem) ? GetTag(oItem) : "null") + ", type: " + IntToString(GetItemType(oItem)));
#endif
	if (IsObjectValid(oItem) && !IsShapeShifted(oCreature))
	{
		return (GetItemType(oItem) == ITEM_TYPE_WEAPON_MELEE);
	}
	else    // fists
	{
		return TRUE_;
	}

	return FALSE_;
}

int32 GetItemType(AActor* oItem)
{
	int32 nBaseItemType = GetBaseItemType(oItem);

	int32 nType = GetM2DAInt(TABLE_ITEMS, "Type", nBaseItemType);

	return nType;
}

int32 IsShapeShifted(AActor* oCreature)
{
	return GetHasEffects(oCreature, EFFECT_TYPE_SHAPECHANGE);
}

int32 IsPartyDead()
{
	TArray<AActor*> partyMembers = GetPartyList();
	int32 nMembers = partyMembers.Num();
	int32 i;

	int32 bAllDead = TRUE_;
	for (i = 0; i < nMembers; i++)
	{
		if (!IsDead(partyMembers[i]))
		{
			bAllDead = FALSE_;
			break;
		}
	}
	return bAllDead;
}

AActor* GetRandomPartyMember()
{
	TArray<AActor*> party = GetPartyList();
	return party[Random(party.Num())];
}

float MaxF(float f1, float f2)
{
	return ((f1 > f2) ? f1 : f2);
}

float MinF(float f1, float f2)
{
	return ((f1 < f2) ? f1 : f2);
}

int32 Max(int32 n1, int32 n2)
{
	return ((n1 > n2) ? n1 : n2);
}

int32 Min(int32 n1, int32 n2)
{
	return ((n1 < n2) ? n1 : n2);
}

float RandFF(float fRange, float fStatic, int32 bDeterministic)
{
	if (bDeterministic) /* used for UI display purposes)*/
	{
		return fRange * 0.5 + fStatic;
	}
	else
	{
		return RandomFloat()*fRange + fStatic;
	}
}

void SetCreatureFlag(AActor* oCreature, int32 nFlag, int32 bSet)
{
	int32 nVal = GetLocalInt(oCreature, CREATURE_RULES_FLAG0);

	int32 nOld = nVal;

	if (bSet)
	{
		nVal |= nFlag;
	}
	else
	{
		nVal &= ~nFlag;
	}
#ifdef DEBUG
	Log_Trace(LOG_CHANNEL_SYSTEMS, "core_h.SetCreatureFlag", "Flag: " + IntToHexString(nFlag) + " Was: " + IntToHexString(nOld) + " Is: " + IntToHexString(nVal), oCreature);
#endif // DEBUG

	SetLocalInt(oCreature, CREATURE_RULES_FLAG0, nVal);
}

int32 GetCreatureFlag(AActor * oCreature, int32 nFlag)
{
	int32 nVal = GetLocalInt(oCreature, CREATURE_RULES_FLAG0);

	Log_Trace(LOG_CHANNEL_SYSTEMS, "core_h.GetCreatureFlag", "Flag: " + IntToHexString(nFlag) + " Value: " + IntToHexString(nVal) + " Result: " + IntToString(((nVal  & nFlag) == nFlag)), oCreature);

	return ((nVal  & nFlag) == nFlag);
}

int32 GetAIBehavior(AActor* oCreature)
{
	if (GetObjectType(oCreature) == OBJECT_TYPE_CREATURE)
	{
		return FloatToInt(GetCreatureProperty(oCreature, PROPERTY_SIMPLE_AI_BEHAVIOR));
	}
	else
	{
		return AI_BEHAVIOR_DEFAULT;
	}
}

int32 IsUsingRangedWeapon(AActor* oCreature, AActor* oItem, int32 bExludeWand)
{
	// Shapeshifting currently only supports melee weapons.
	if (IsShapeShifted(oCreature))
	{
		return FALSE_;
	}

	if (!IsObjectValid(oItem))
	{
		oItem = GetItemInEquipSlot(INVENTORY_SLOT_MAIN, oCreature);
	}

	if (IsObjectValid(oItem))
	{

		return (GetItemType(oItem) == ITEM_TYPE_WEAPON_RANGED ||
			(!bExludeWand && GetItemType(oItem) == ITEM_TYPE_WEAPON_WAND));
	}

	return FALSE_;
}

int32 IsUsingShield(AActor* oCreature, AActor* oItem)
{

	if (!IsObjectValid(oItem))
	{
		oItem = GetItemInEquipSlot(INVENTORY_SLOT_OFFHAND, oCreature);
	}

	if (IsObjectValid(oItem))
	{
		return (GetItemType(oItem) == ITEM_TYPE_SHIELD);
	}

	return FALSE_;
}

void ApplyEffectOnObject(int32 nDurationType, FEffect eEffect, AActor* oTarget, float fDuration, int32 nAbilityId)
{
	// -------------------------------------------------------------------------
	// For stun specifically, we apply a marking effect for 15 secs
	// that degrades incoming stun effects to 1/4 of their potency
	// -------------------------------------------------------------------------
	if (GetEffectType(eEffect) == EFFECT_TYPE_STUN)
	{
		if (nDurationType == EFFECT_DURATION_TYPE_TEMPORARY)
		{
			if (GetHasEffects(oTarget, EFFECT_TYPE_RECENTLY_STUNNED, 0))
			{
				fDuration *= 0.25;
			}
			else
			{
				FEffect e = Effect(EFFECT_TYPE_RECENTLY_STUNNED);
				e.oCreator = eEffect.oCreator;
				Engine_ApplyEffectOnObject(nDurationType,
					e,
					oTarget,
					15.0,
					0 /*invisible*/);
			}
		}
	}


	Engine_ApplyEffectOnObject(nDurationType,
		eEffect,
		oTarget,
		fDuration,
		//oCreator, moved in the caller directly
		nAbilityId);



#ifdef DEBUG
	Log_Trace_Effects("core_h.ApplyEffectOnObject", eEffect, FloatToString(fDuration),
		oTarget, nDurationType, nAbilityId);
#endif
}

int32 GetForcedCombatResult(AActor* oCreature)
{
	// bitmask as follows:
	// 0 1 = miss
	// 1 0 = critical
	// 1 1 = deathblow

	int32 nVal = GetLocalInt(oCreature, CREATURE_RULES_FLAG0);
	int32 nMask = (CREATURE_RULES_FLAG_FORCE_COMBAT_0 | CREATURE_RULES_FLAG_FORCE_COMBAT_1);
	int32 nResult = (nVal & nMask);


	if (nResult == nMask)
	{
		return COMBAT_RESULT_DEATHBLOW;
	}
	if (nResult == CREATURE_RULES_FLAG_FORCE_COMBAT_0)
	{
		return COMBAT_RESULT_MISS;
	}
	else if (nResult == CREATURE_RULES_FLAG_FORCE_COMBAT_1)
	{
		return COMBAT_RESULT_CRITICALHIT;
	}
	else
	{
		return -1;
	}
}

float GetCreatureAttackRating(AActor* oCreature)
{
	return GetCreatureProperty(oCreature, PROPERTY_ATTRIBUTE_ATTACK);
}

float GetCreatureCriticalHitModifier(AActor* oCreature, int32 nCritModifier)
{
	float fRet = GetCreatureProperty(oCreature, nCritModifier, PROPERTY_VALUE_TOTAL);
#ifdef DEBUG
	float fMod = GetCreatureProperty(oCreature, nCritModifier, PROPERTY_VALUE_MODIFIER);
	Log_Trace(LOG_CHANNEL_CHARACTER, "core_h.GetCreatureCriticalHitModifier", "Prop (" + IntToString(nCritModifier) + ") on " + GetTag(oCreature) + " is " + FloatToString(fRet) + " modifier is: " + FloatToString(fMod));
#endif

	return fRet;
}

float GetCreatureDefense(AActor* oCreature)
{
	float fRet = GetCreatureProperty(oCreature, PROPERTY_ATTRIBUTE_DEFENSE);
#ifdef DEBUG
	float fMod = GetCreatureProperty(oCreature, PROPERTY_ATTRIBUTE_DEFENSE, PROPERTY_VALUE_MODIFIER);
	Log_Trace(LOG_CHANNEL_CHARACTER, "core_h.GetCreatureDefense", "Defense on " + GetTag(oCreature) + " is " + FloatToString(fRet) + " modifier is: " + FloatToString(fMod));
#endif

	return fRet;
}

float Combat_Damage_GetMageStaffDamage(AActor* oAttacker, AActor* oTarget, AActor* oWeapon, int32 bDeterministic)
{
	int32 nProjectileIndex = GetLocalInt(oWeapon, PROJECTILE_OVERRIDE);
	float fDamageBonus = 1.0 + GetM2DAFloat(TABLE_PROJECTILES, "DamageBonus", nProjectileIndex);

	float fArcaneFocus = 1.0;
	if (HasAbility(oAttacker, ABILITY_SPELL_ARCANE_FOCUS)) /**ABILITY_SPELL_ARCANE_FOCUS*/
	{
		fArcaneFocus = 1.0 + (1.0 / 3.0);
	}

	float fSpellPowerComponent = (GetCreatureSpellPower(oAttacker) / 4.0f) * fArcaneFocus  * fDamageBonus;


	return DmgGetWeaponDamage(oWeapon, bDeterministic) + RandFF(fSpellPowerComponent * 0.25, fSpellPowerComponent * 0.75, bDeterministic);

}

float GetCreatureSpellPower(AActor* oCreature)
{
	return GetCreatureProperty(oCreature, PROPERTY_ATTRIBUTE_SPELLPOWER, PROPERTY_VALUE_TOTAL);
}

float DmgGetWeaponDamage(AActor* oWeapon, int32 bForceMaxDamage)
{

	float fBase = DmgGetWeaponBaseDamage(oWeapon);

	if (GetItemType(oWeapon) == ITEM_TYPE_SHIELD)
	{
		fBase += 5.0f;
	}

	float fMax = DmgGetWeaponMaxDamage(oWeapon);

	float fDmg = fBase + ((bForceMaxDamage) ? fMax - fBase : RandFF(fMax - fBase));

#ifdef DEBUG
	_LogDamage("  fWeapon   " + GetTag(oWeapon) + ": base: " + FloatToString(fBase) + " + max: " + FloatToString(fMax) + " max damage:" + FloatToString(bForceMaxDamage));
	_LogDamage("  fWeapon   " + GetTag(oWeapon) + ":" + FloatToString(fDmg) + " = " + FloatToString(fBase) + " + Rand(" + FloatToString(fMax - fBase) + ")");
#endif

	return fDmg;

}

float DmgGetWeaponBaseDamage(AActor* oWeapon)
{
	if (!IsObjectValid(oWeapon))
	{
		return COMBAT_DEFAULT_UNARMED_DAMAGE;
	}
	float fBase = GetItemStat(oWeapon, ITEM_STAT_DAMAGE);
	return fBase;

}

float DmgGetWeaponMaxDamage(AActor* oWeapon)
{
	int32 nType = GetBaseItemType(oWeapon);

	if (!IsObjectValid(oWeapon))
	{
		return COMBAT_DEFAULT_UNARMED_DAMAGE * 1.5f;
	}

	float fMax = DmgGetWeaponBaseDamage(oWeapon) * MaxF(1.0, GetM2DAFloat(TABLE_ITEMSTATS, "DamageRange", nType));
	return fMax;
}

float GetWeaponAttributeBonusFactor(AActor* oWeapon)
{

	if (!IsObjectValid(oWeapon))
	{
		// ---------------------------------------------------------------------
		// Unarmed
		// ---------------------------------------------------------------------
		return UNARMED_ATTRIBUTE_BONUS_FACTOR;
	}

	int32 nBase = GetBaseItemType(oWeapon);
	float fFactor = GetItemStat(oWeapon, ITEM_STAT_ATTRIBUTE_MOD);


	return fFactor;
}

float Combat_Damage_GetAttributeBonus(AActor* oCreature, int32 nHand, AActor* oWeapon, int32 bDeterministic)
{
	int32 nAttribute = PROPERTY_ATTRIBUTE_STRENGTH;
	int32 nAttribute1 = 0;
	// -------------------------------------------------------------------------
	// some
	// -------------------------------------------------------------------------
	if (IsObjectValid(oWeapon))
	{
		int32 nBaseItemType = GetBaseItemType(oWeapon);
		if (nBaseItemType > 0)
		{
			nAttribute = GetM2DAInt(TABLE_ITEMSTATS, "Attribute0", nBaseItemType);
			nAttribute1 = GetM2DAInt(TABLE_ITEMSTATS, "Attribute1", nBaseItemType);
		}
	}


	// -------------------------------------------------------------------------
	// Combat Magic: Using SpellPower (magic modifier) instead.
	// -------------------------------------------------------------------------
	if (IsModalAbilityActive(oCreature, ABILITY_SPELL_COMBAT_MAGIC))
	{
		nAttribute = PROPERTY_ATTRIBUTE_MAGIC;
	}

	// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---
	// Lethality: If the talent is present and the attribute tested is strength
	// then change the attribute to cunning if cunning is higher than int32
	// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---
	else if (nAttribute == PROPERTY_ATTRIBUTE_STRENGTH && HasAbility(oCreature, ABILITY_TALENT_LETHALITY))
	{
		if (GetAttributeModifier(oCreature, nAttribute) < GetAttributeModifier(oCreature, PROPERTY_ATTRIBUTE_INTELLIGENCE))
		{
			nAttribute = PROPERTY_ATTRIBUTE_INTELLIGENCE;
		}
	}



	float fDmg = 0.0f;

	if (nAttribute1 == 0)
	{
		fDmg = MaxF(0.0, GetAttributeModifier(oCreature, nAttribute));
	}
	else
	{
		fDmg = MaxF(0.0, (GetAttributeModifier(oCreature, nAttribute) + GetAttributeModifier(oCreature, nAttribute1)) / 2.0f);

	}


	// Main hand attacks to 50-75% of attribute modifier
	if (nHand == HAND_MAIN)
	{
		if (GetWeaponStyle(oCreature) == WEAPONSTYLE_DUAL)
		{
			return RandFF(fDmg * 0.25, fDmg*0.25, bDeterministic);
		}
		else
		{
			return RandFF(fDmg * 0.25, fDmg*0.5, bDeterministic);
		}
	}
	else
	{
		// dual weapon training adds 1/4th of attribute modifier to damage / so does using a shield (offhand damage with shield is shield abilities only anyway)
		if (HasAbility(oCreature, ABILITY_TALENT_DUAL_WEAPON_TRAINING) || IsUsingShield(oCreature))
		{
			return RandFF(fDmg * 0.25, fDmg*0.25, bDeterministic);
		}
		else
		{
			return RandFF(fDmg * 0.25, 0.0f, bDeterministic);
		}
	}
}

float GetAttributeModifier(AActor* oCreature, int32 nAttribute)
{

	float fValue = GetCreatureProperty(oCreature, nAttribute) - RULES_ATTRIBUTE_MODIFIER;

	return MaxF(fValue, 0.0f);

}

int32 IsMeleeWeapon2Handed(AActor* oMeleeWeapon)
{
	int32 nBitm = GetBaseItemType(oMeleeWeapon);
	return (GetM2DAInt(TABLE_ITEMS, "WeaponWield", nBitm) == WEAPON_WIELD_TWO_HANDED_MELEE);
}

float _GetRelativeResourceLevel(AActor* oCreature, int32 nResource)
{
	float fCur = GetCreatureProperty(oCreature, nResource, PROPERTY_VALUE_CURRENT);
	float fMax = GetCreatureProperty(oCreature, nResource, PROPERTY_VALUE_TOTAL);

	return (fMax > 0.0 ? fCur / fMax : 0.0);
}

int32 GetCreatureAppearanceFlag(AActor* oCreature, int32 nFlag)
{
	int32 nVal = GetM2DAInt(TABLE_APPEARANCE, "AprRulesFlags", GetAppearanceType(oCreature));
	return ((nVal  & nFlag) == nFlag);
}

int32 GetLevel(AActor* oCreature)
{
#ifdef DEBUG
	LogTrace(LOG_CHANNEL_TEMP, "GetLevel: " + GetTag(oCreature) + " - " + FloatToString(GetCreatureProperty(oCreature, PROPERTY_SIMPLE_LEVEL, PROPERTY_VALUE_TOTAL)));
#endif
	return FloatToInt(GetCreatureProperty(oCreature, PROPERTY_SIMPLE_LEVEL, PROPERTY_VALUE_TOTAL));
}

float GetCreatureRangedDrawSpeed(AActor* oCreature, AActor* oWeapon)
{
	float fRet = 0.0f;

	// -- Get Total Draw Speed
	float fTotal = GetCreatureProperty(oCreature, PROPERTY_ATTRIBUTE_RANGED_AIM_SPEED, PROPERTY_VALUE_TOTAL);

	// -- Get Mod (for debug output only)
	float fMod = GetCreatureProperty(oCreature, PROPERTY_ATTRIBUTE_RANGED_AIM_SPEED, PROPERTY_VALUE_MODIFIER);

	// -- GetWeapon Draw Speed (can't be negative)
	float fWeapon = 0.0f;

	if (IsObjectValid(oWeapon))
	{
		fWeapon = MaxF(GetM2DAFloat(TABLE_ITEMSTATS, "BaseAimDelay", GetBaseItemType(oWeapon)), 0.0f);
	}

	fRet = fTotal + fWeapon;

#ifdef DEBUG
	Log_Trace(LOG_CHANNEL_CHARACTER, "core_h.GetCreatureRangedDrawSpeed", "DrawSpeed on " + GetTag(oCreature) + " is " + FloatToString(fRet) + " modifier is: " + FloatToString(fMod) + " Weapon (" + GetTag(oWeapon) + "):" + FloatToString(fWeapon));
#endif

	return MaxF(fRet, 0.0f);
}

float CalculateAttackTiming(AActor* oAttacker, AActor* oWeapon)
{
	// -------------------------------------------------------------------------
	// Without a weapon or if we are not a humanoid (which covers shapeshift)
	// we return 0.0, which translates to animation controlled timing in the
	// engine when passed into attack speed.
	// -------------------------------------------------------------------------
	if (IsObjectValid(oWeapon) && IsHumanoid(oAttacker))
	{

		float fSpeedMod = GetM2DAFloat(TABLE_ITEMSTATS, "dspeed", GetBaseItemType(oWeapon));

		// ---------------------------------------------------------------------
		// Calculate weapon speed.
		// ---------------------------------------------------------------------
		int32 nStyle = GetWeaponStyle(oAttacker);
		float fSpeed = 0.0f;

		switch (nStyle)
		{
		case WEAPONSTYLE_NONE:
			fSpeed = BASE_TIMING_WEAPON_SHIELD;
			break;
		case WEAPONSTYLE_DUAL:
			fSpeed = BASE_TIMING_DUAL_WEAPONS;
			break;
		case WEAPONSTYLE_SINGLE:
			fSpeed = BASE_TIMING_WEAPON_SHIELD;
			break;
		case WEAPONSTYLE_TWOHANDED:
			fSpeed = BASE_TIMING_TWO_HANDED;
			break;
		}


		// ---------------------------------------------------------------------
		// Only attacks that rely on timing can be modified
		// ---------------------------------------------------------------------
		if (fSpeed > 0.0f)
		{
			// -----------------------------------------------------------------
			// We're capping the actual values here to avoid animations breaking
			// down when played too fast or too slow.
			// -----------------------------------------------------------------
			float fSpeedEffects = MinF(1.5f, MaxF(0.5f, GetCreatureProperty(oAttacker, PROPERTY_ATTRIBUTE_ATTACK_SPEED_MODIFIER)));

			// -----------------------------------------------------------------
			// compatibility with some old savegames.
			// -----------------------------------------------------------------
			if (GetCreatureProperty(oAttacker, PROPERTY_ATTRIBUTE_ATTACK_SPEED_MODIFIER) < 0.5f)
			{
				fSpeedEffects = 1.0f;
			}

#ifdef DEBUG
			Log_Trace(LOG_CHANNEL_COMBAT, "combat_performattack", "weapon base speed :" + FloatToString(fSpeed) + " mod:" + FloatToString(fSpeedMod) + " effects: " + FloatToString(fSpeedEffects));
#endif

			fSpeed += (fSpeedMod);
			if (fSpeedEffects > 0.0f)
			{
				fSpeed *= fSpeedEffects;
			}
			return fSpeed;
		}
	}
	return 0.0;
}

int32 IsArmorMassive(AActor* oArmor)
{
	if (!IsObjectValid(oArmor))
	{
		return FALSE_;
	}
	int32 nType = GetBaseItemType(oArmor);

	return (nType == BASE_ITEM_TYPE_ARMOR_MASSIVE || nType == BASE_ITEM_TYPE_ARMOR_SUPERMASSIVE);

}

int32 IsArmorHeavyOrMassive(AActor* oArmor)
{
	if (!IsObjectValid(oArmor))
	{
		return FALSE_;
	}

	int32 nType = GetBaseItemType(oArmor);
	return (nType == BASE_ITEM_TYPE_ARMOR_HEAVY || nType == BASE_ITEM_TYPE_ARMOR_MASSIVE || nType == BASE_ITEM_TYPE_ARMOR_SUPERMASSIVE);

}

