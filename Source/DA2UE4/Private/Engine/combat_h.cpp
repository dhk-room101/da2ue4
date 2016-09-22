#include "DA2UE4.h"
#include "ai_threat_h.h"
#include "combat_h.h"
#include "log_h.h"
#include "ldf.h"
#include "core_h.h"
#include "sys_soundset_h.h"
#include "commands_h.h"
#include "combat_damage_h.h"
#include "wrappers_h.h"

void Combat_HandleCreatureDisappear(AActor* oCreature, AActor* oDisappearer)
{
#ifdef DEBUG
	Log_Trace(LOG_CHANNEL_COMBAT, "Combat_HandleCreatureDisappeart", "creature: " + GetTag(oCreature) + ", disappearer: " + GetTag(oDisappearer));
#endif

	if (IsFollower(oCreature))
	{
		AI_Threat_UpdateEnemyDisappeared(oCreature, oDisappearer);

		// ----------------------------------------------------------------------
		// If the party does no longer perceive any hostiles....
		// ----------------------------------------------------------------------
		if (!IsPartyPerceivingHostiles(oCreature))
		{

			if (!IsPartyDead())
			{
#ifdef DEBUG
				Log_Trace(LOG_CHANNEL_COMBAT, "Combat_HandleCreatureDisappear", "STOPPING COMBAT FOR PARTY!");
#endif

				SSPlaySituationalSound(GetRandomPartyMember(), SOUND_SITUATION_END_OF_COMBAT);

				DelayEvent(3.0f, GetModule(), Event(EVENT_TYPE_DELAYED_GM_CHANGE));
			}
		}
	}
	else // non party members
	{
		AI_Threat_UpdateEnemyDisappeared(oCreature, oDisappearer);
		if (!IsPerceivingHostiles(oCreature) && GetCombatState(oCreature) == TRUE_)
		{
#ifdef DEBUG
			Log_Trace(LOG_CHANNEL_COMBAT, "Combat_HandleCreatureDisappear", "STOPPING COMBAT FOR NORMAL CREATURE!");
#endif
			SetCombatState(oCreature, FALSE_);

			// Added to make a COMBAT_END event
			// FAB 9/4
			FGameEvent evCombatEnds = Event(EVENT_TYPE_COMBAT_END);
			//evCombatEnds = SetEventInteger(evCombatEnds, 0, bcombatstate);
			SignalEvent(oCreature, evCombatEnds);
		}
	}
}

int32 Combat_HandleCommandAttack(AActor* oAttacker, AActor* oTarget, int32 nCommandSubType)
{

	FCombatAttackResultStruct stAttack1;
	FCombatAttackResultStruct stAttack2;

	AActor* oWeapon = nullptr;
	AActor* oWeapon2 = nullptr;

	int32 nHand = Combat_GetAttackHand(oAttacker);

	if (nHand == HAND_MAIN)
	{
		oWeapon = GetItemInEquipSlot(INVENTORY_SLOT_MAIN, oAttacker);
	}
	else if (nHand == HAND_OFFHAND)
	{
		oWeapon = GetItemInEquipSlot(INVENTORY_SLOT_OFFHAND, oAttacker);
	}

	// -------------------------------------------------------------------------
	// Double Weapon Strike.
	// -------------------------------------------------------------------------
	if (IsModalAbilityActive(oAttacker, ABILITY_TALENT_DUAL_WEAPON_DOUBLE_STRIKE))
	{
		nHand = HAND_BOTH;
		oWeapon = GetItemInEquipSlot(INVENTORY_SLOT_MAIN, oAttacker);
		oWeapon2 = GetItemInEquipSlot(INVENTORY_SLOT_OFFHAND, oAttacker);
	}


	int32 nAttackType = Combat_GetAttackType(oAttacker, oWeapon);



	// -------------------------------------------------------------------------
	// Handle Attack #1
	// -------------------------------------------------------------------------
	stAttack1 = Combat_PerformAttack(oAttacker, oTarget, oWeapon);

	if (nHand == HAND_BOTH)
	{
		stAttack2 = Combat_PerformAttack(oAttacker, oTarget, oWeapon2);

		if (stAttack1.nAttackResult != COMBAT_RESULT_DEATHBLOW &&  stAttack2.nAttackResult == COMBAT_RESULT_DEATHBLOW)
		{
			stAttack1 = stAttack2;
			nHand = HAND_MAIN; // Deathblows just use the main hand.
		}

	}

	// -------------------------------------------------------------------------
	// If we execute a deathblow, we gain the death fury effect for a couple of
	// seconds and apply the deathblow command
	// -------------------------------------------------------------------------
	if (stAttack1.nAttackResult == COMBAT_RESULT_DEATHBLOW)
	{

		// ----------------------------------------------------------------------
		// Georg: Do Not Modify the following section.
		// START >>
		// GM - Adding the deathblow should be the last thing done because it
		// will clear the attack command.
		// Specifically, SetAttackResult MUST be executed before adding the deathblow.
		// ----------------------------------------------------------------------
		SetAttackResult(oAttacker, stAttack1.nAttackResult, stAttack1.eImpactEffect, COMBAT_RESULT_INVALID, Effect());


		WR_AddCommand(oAttacker, CommandDeathBlow(oTarget, stAttack1.nDeathblowType), TRUE_, TRUE_);

		return COMMAND_RESULT_SUCCESS;
		// ----------------------------------------------------------------------
		// << END
		// ----------------------------------------------------------------------
	}


	// -------------------------------------------------------------------------
	// SetAttackResult requires a result in either the first or second result
	// field to determine which hand should attack.
	// -------------------------------------------------------------------------
	if (nHand == HAND_MAIN || stAttack1.nAttackResult == COMBAT_RESULT_BACKSTAB)
	{
		SetAttackResult(oAttacker, stAttack1.nAttackResult, stAttack1.eImpactEffect,
			COMBAT_RESULT_INVALID, Effect());

	}
	else if (nHand == HAND_OFFHAND)
	{
		SetAttackResult(oAttacker, COMBAT_RESULT_INVALID, Effect(),
			stAttack1.nAttackResult, stAttack1.eImpactEffect);
	}
	else if (nHand == HAND_BOTH)
	{
		SetAttackResult(oAttacker, stAttack1.nAttackResult, stAttack1.eImpactEffect, stAttack2.nAttackResult, stAttack2.eImpactEffect);
	}
	else
	{
		SetAttackResult(oAttacker, stAttack1.nAttackResult, stAttack1.eImpactEffect,
			COMBAT_RESULT_INVALID, Effect());
	}


	if (stAttack1.fAttackDuration != ATTACK_LOOP_DURATION_INVALID)
	{

		if (IsHumanoid(oAttacker))
		{

			if (nAttackType == ATTACK_TYPE_RANGED)
			{
				// the "attack duration" for ranged weapons actually overrides
				// the time spent drawing and preparing to aim
				if (GetBaseItemType(oWeapon) == BASE_ITEM_TYPE_STAFF)
				{
					SetAttackDuration(oAttacker, 0.30);
				}
				else
				{
					AActor* oArmor = GetItemInEquipSlot(INVENTORY_SLOT_CHEST, oAttacker);
					if (!IsArmorMassive(oArmor) && HasAbility(oAttacker, ABILITY_TALENT_MASTER_ARCHER))
					{
						if (IsFollower(oAttacker))
							SetAttackDuration(oAttacker, 0.8);
						else
							SetAttackDuration(oAttacker, 1.5);
					}
					else if (IsArmorHeavyOrMassive(oArmor))
					{
						if (IsFollower(oAttacker))
							SetAttackDuration(oAttacker, 2.0);
						else
							SetAttackDuration(oAttacker, 2.5);
					}
					else
					{
						if (IsFollower(oAttacker))
							SetAttackDuration(oAttacker, 0.8);
						else
							SetAttackDuration(oAttacker, 1.5);
					}
				}

				SetAimLoopDuration(oAttacker, stAttack1.fAttackDuration);

#ifdef DEBUG
				Log_Trace(LOG_CHANNEL_COMBAT, "combat_h.HandleCommandAttack", "RangedAim Loop Duration set to " + FloatToString(stAttack1.fAttackDuration));
#endif
			}
			else if (nAttackType == ATTACK_TYPE_MELEE)
			{

				SetAttackDuration(oAttacker, stAttack1.fAttackDuration);

			}
		}
	}


	return COMMAND_RESULT_SUCCESS;
}

int32 Combat_GetAttackHand(AActor* aCreature)
{
	ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aCreature);

	int32 nHand = 0;
	if (GetWeaponStyle(oCreature) == WEAPONSTYLE_DUAL)
	{
		nHand = oCreature->COMBAT_LAST_WEAPON;
		oCreature->COMBAT_LAST_WEAPON = !nHand;
	}

	return nHand;
}

int32 Combat_GetAttackType(AActor* oAttacker, AActor* oWeapon)
{
	if (IsUsingRangedWeapon(oAttacker, oWeapon))
	{
		return ATTACK_TYPE_RANGED;
	}
	else
	{
		return ATTACK_TYPE_MELEE;
	}
}

FCombatAttackResultStruct Combat_PerformAttack(AActor* oAttacker, AActor* oTarget, AActor* oWeapon, float fDamageOverride, int32 nAbility)
{
	//ADA2UE4Item* oWeapon = Cast<ADA2UE4Item>(aWeapon);

	FCombatAttackResultStruct stRet;
	float   fDamage = 0.0f;
	int32     nAttackType = Combat_GetAttackType(oAttacker, oWeapon);

	stRet.fAttackDuration = ATTACK_LOOP_DURATION_INVALID;

	// -------------------------------------------------------------------------
	// Attack check happens here...
	// -------------------------------------------------------------------------

	stRet.nAttackResult = Combat_GetAttackResult(oAttacker, oTarget, oWeapon);

	// -------------------------------------------------------------------------
	// If attack result was not a miss, go on to calculate damage
	// -------------------------------------------------------------------------
	if (stRet.nAttackResult != COMBAT_RESULT_MISS)
	{
		int32 bCriticalHit = (stRet.nAttackResult == COMBAT_RESULT_CRITICALHIT);

		// -------------------------------------------------------------------------
		// If attack result was not a miss, check if we need to handle a deathblow
		// -------------------------------------------------------------------------
		fDamage = ((fDamageOverride == 0.0f) ?
			Combat_Damage_GetAttackDamage(oAttacker, oTarget, oWeapon, stRet.nAttackResult) : fDamageOverride);


		float fTargetHealth = GetCurrentHealth(oTarget);


		// -----------------------------------------------------------------
		//  Ranged weapons attacks are not synced and therefore we never
		//  need to worry about reporting deathblows to the engine.
		// ---------------------------------------------------------------------

		// ---------------------------------------------------------------------
		// When not using a ranged weapon, there are synchronize death blows to handle
		// ---------------------------------------------------------------------
		if (nAttackType != ATTACK_TYPE_RANGED && nAbility == 0 && stRet.nAttackResult != COMBAT_RESULT_MISS)
		{

			// -----------------------------------------------------------------
			// Deathblows against doors look cool, but really...
			// -----------------------------------------------------------------
			if (GetObjectType(oTarget) == OBJECT_TYPE_CREATURE)
			{

				// ---------------------------------------------------------
				// Special conditions.
				//
				// There are a few cases in the single player campaign
				// where we want the spectacular deathblow to occur if possible.
				//
				// The following logic defines these conditions
				// ---------------------------------------------------------
				int32 nAppearance = GetAppearanceType(oTarget);
				int32 nRank = GetCreatureRank(oTarget);

				int32 bSpecial = FALSE_;

				// ---------------------------------------------------------
				// ... all boss ogres (there's 1 in the official campaign) by request
				//     from Dr. Muzyka.
				// ... all elite bosses
				// ---------------------------------------------------------
				if ((nAppearance == APR_TYPE_OGRE || (nRank == CREATURE_RANK_BOSS || nRank == CREATURE_RANK_ELITE_BOSS)) ||
					nRank == CREATURE_RANK_ELITE_BOSS)
				{
					// -------------------------------------------------
					// ... but only if they are at the health threshold
					//     required for deathblows to trigger
					// -------------------------------------------------
					if (IsHumanoid(oAttacker))
					{
						bSpecial = _GetRelativeResourceLevel(oTarget, PROPERTY_DEPLETABLE_HEALTH) < SPECIAL_BOSS_DEATHBLOW_THRESHOLD;
					}
				}

				// ---------------------------------------------------------
				// Deathblows occur when
				//  ... target isn't immortal (duh) AND
				//      ... the damage of the hit exceeds the creature's health OR
				//      ... aforementioned 'special' conditions are met.
				// ---------------------------------------------------------
				if ((!IsImmortal(oTarget) && (fDamage >= fTargetHealth || bSpecial)))
				{

					// -----------------------------------------------------
					// ... only from party members AND
					//    ... if we determine that a deathblow doesn't interrupt gameplay OR
					//    ... aforementioned 'special' conditions are met
					// -----------------------------------------------------
					if (IsPartyMember(oAttacker) && (CheckForDeathblow(oAttacker, oTarget) || bSpecial))
					{
						// -------------------------------------------------
						// Verify some more conditions...
						// -------------------------------------------------
						int32 bDeathBlow = Combat_GetValidDeathblow(oAttacker, oTarget);
						if (bDeathBlow)
						{
							stRet.nAttackResult = COMBAT_RESULT_DEATHBLOW;

							// ---------------------------------------------
							// Special treatment for ogre
							// Reason: The ogre, unlike all other special bosses
							//         has a second, non spectacular deathblow.
							//         if we specify 0, there's a 50% chance that
							//         one is played, which we don't want in this
							//         case, so we're passing the id of the
							//         spectacular one instead.
							// ---------------------------------------------
							if (bSpecial && nAppearance == APR_TYPE_OGRE)
							{
								stRet.nDeathblowType = 5; // 5 - ogre slow mo deathblow
							}
							else
							{
								stRet.nDeathblowType = 0;  // 0 - auto select in engine;
							}

						}
						else
						{
							// Failure to meet conditions: convert to hit.
							if (stRet.nAttackResult != COMBAT_RESULT_BACKSTAB)
							{
								stRet.nAttackResult = COMBAT_RESULT_HIT;
							}
						}
					}
					else
					{
						// Failure to meet conditions: convert to hit.
						if (stRet.nAttackResult != COMBAT_RESULT_BACKSTAB)
						{
							stRet.nAttackResult = COMBAT_RESULT_HIT;
						}
					}



				} /* is humanoid*/


			} /* obj_type creature*/
		}


	}
	int32 nDamageType = DAMAGE_TYPE_PHYSICAL;

	if (nAttackType == ATTACK_TYPE_RANGED)
	{
		// ---------------------------------------------------------------------
		// Certain projectiles modify the damage type done by a ranged weapon
		// This is defined in PRJ_BASE.
		// ---------------------------------------------------------------------
		//TODO PROJECTILE_OVERRIDE
		LogError("PROJECTILE_OVERRIDE!!");
		int32 nProjectileIndex = 0;//Cast<ADA2UE4Item>(oWeapon)->PROJECTILE_OVERRIDE;
		if (nProjectileIndex)
		{
			int32 nDamageTypeOverride = GetM2DAInt(TABLE_PROJECTILES, "DamageType", nProjectileIndex);
			if (nDamageTypeOverride > 0)
			{
				nDamageType = nDamageTypeOverride;
			}
		}

		// ---------------------------------------------------------------------
		// When using a ranged weapon, we need to report the duration of the
		// aim loop to the engine
		// ---------------------------------------------------------------------
		stRet.fAttackDuration = GetCreatureRangedDrawSpeed(oAttacker, oWeapon);
	}
	else
	{
		float fSpeed = CalculateAttackTiming(oAttacker, oWeapon);
		if (fSpeed > 0.0f)
		{
			stRet.fAttackDuration = fSpeed;
		}
	}

	// -------------------------------------------------------------------------
	// The Impact effect is not a real effect - it is not ever applied. Instead
	// it is used to marshal information about the attack back to the impact
	// event.
	// -------------------------------------------------------------------------
	stRet.eImpactEffect = EffectImpact(fDamage, oWeapon, 0, 0, nDamageType);

#ifdef DEBUG
	Log_Trace_Combat("combat_h.Combat_PerformAttack", " Attack Result: " + Log_GetAttackResultNameById(stRet.nAttackResult), oAttacker, oTarget, LOG_CHANNEL_COMBAT_TOHIT);
#endif

	return stRet;
}

int32 Combat_GetAttackResult(AActor* oAttacker, AActor* oTarget, AActor* oWeapon, float fBonus, int32 nAbility)
{
	// -------------------------------------------------------------------------
	// Debug
	// -------------------------------------------------------------------------
	int32 nForcedCombatResult = GetForcedCombatResult(oAttacker);
	if (nForcedCombatResult != -1)
	{
#ifdef DEBUG
		Log_Trace_Combat("combat_h.GetAttackResult", " Skipped rules, FORCED RESULT IS:" + IntToString(nForcedCombatResult), oTarget);
#endif
		return nForcedCombatResult;
	}



	// -------------------------------------------------------------------------
	// Placeables are always hit
	// -------------------------------------------------------------------------
	if (GetObjectType(oTarget) == OBJECT_TYPE_PLACEABLE)
	{
#ifdef DEBUG
		Log_Trace_Combat("combat_h.GetAttackResult", " Placeable, automatic result : COMBAT_RESULT_HIT", oTarget);
#endif
		return COMBAT_RESULT_HIT;
	}


	// -------------------------------------------------------------------------
	// Displacement
	// -------------------------------------------------------------------------
	float fDisplace = GetCreatureProperty(oTarget, PROPERTY_ATTRIBUTE_DISPLACEMENT);
	float fRand = RandomFloat()*100.0;
	if (fRand < fDisplace)
	{
#ifdef DEBUG
		Log_Trace_Combat("combat_h.GetAttackResult", " Displacement effect kicked in, automatic result : COMBAT_RESULT_MISS", oTarget);
#endif

		// ---------------------------------------------------------------------
		// if the target has the evasion talent, attribute this miss to the talent
		// (random 50% because the anim is interrupting)
		// ---------------------------------------------------------------------
		if (HasAbility(oTarget, ABILITY_TALENT_EVASION) && fRand < (20.0f - (RandomFloat()*10.0f)))
		{
			FCommand currentCmd = GetCurrentCommand(oTarget);
			int32 nCmdType = GetCommandType(currentCmd);
			// Evasion only plays during attack and wait commands.
			if (nCmdType == COMMAND_TYPE_WAIT || nCmdType == COMMAND_TYPE_ATTACK || nCmdType == COMMAND_TYPE_INVALID)
			{
				FEffect effect = Effect(EFFECT_TYPE_EVASION);
				effect.oCreator = oTarget;
				ApplyEffectOnObject(EFFECT_DURATION_TYPE_INSTANT, effect, oTarget, 0.0f, ABILITY_TALENT_EVASION);
			}
		}

		return COMBAT_RESULT_MISS;
	}



	int32  nAttackType = Combat_GetAttackType(oAttacker, oWeapon);
	int32 nRet;

	// -------------------------------------------------------------------------
	// Get the attackers attack rating (includes effects and equipment stats)
	// -------------------------------------------------------------------------
	float   fAttackRating;

	if (GetBaseItemType(oWeapon) == BASE_ITEM_TYPE_STAFF)
	{
		// ---------------------------------------------------------------------
		// Staffs always hit
		// ---------------------------------------------------------------------
		return COMBAT_RESULT_HIT;
	}



	fAttackRating = GetCreatureAttackRating(oAttacker);

	// -------------------------------------------------------------------------
	// Add item stat (usually 0) along with scripted bonus and attack bias.
	// -------------------------------------------------------------------------

	fAttackRating += GetItemStat(oWeapon, ITEM_STAT_ATTACK) + fBonus + ATTACK_HIT_BIAS;

	// -------------------------------------------------------------------------
	// Easier difficulties grant the player a bonus.
	// -------------------------------------------------------------------------
	fAttackRating += Diff_GetRulesAttackBonus(oAttacker);


	// -------------------------------------------------------------------------
	// This section deals with figuring out which critical hit modifier (melee, ranged, etc)
	// to use for this attack.
	// -------------------------------------------------------------------------
	float   fCriticalHitModifier;
	int32     nCriticalHitModifier = (nAttackType == ATTACK_TYPE_RANGED) ? PROPERTY_ATTRIBUTE_RANGED_CRIT_MODIFIER : PROPERTY_ATTRIBUTE_MELEE_CRIT_MODIFIER;



	fCriticalHitModifier = GetCreatureCriticalHitModifier(oAttacker, nCriticalHitModifier);
	fCriticalHitModifier += GetItemStat(oWeapon, ITEM_STAT_CRIT_CHANCE_MODIFIER);

	// -------------------------------------------------------------------------
	//  Bravery grants +3.5 critical hit per enemy past the first 2
	// -------------------------------------------------------------------------
	if (HasAbility(oAttacker, ABILITY_TALENT_BRAVERY))
	{

		int32 nEnemies = Max(0, GetCreaturesInMeleeRing(oAttacker, 0.0, 359.99f, TRUE_, 0).Num() - 2);
		fCriticalHitModifier += nEnemies * 3.5f;
	}


	// -------------------------------------------------------------------------
	// Calculate Flanking Bonus
	// -------------------------------------------------------------------------
	float fFlanking = Combat_GetFlankingBonus(oAttacker, oTarget);
	if (fFlanking > 0.0)
	{
		// ---------------------------------------------------------------------
		// Also increase chance for critical hits by 1/5th of the flanking bonus
		// ---------------------------------------------------------------------
		fCriticalHitModifier *= (1.0 + (fFlanking / 5.0));
	}

	// -------------------------------------------------------------------------
	// Range plays a role too.
	// -------------------------------------------------------------------------
	float fDistance = GetDistanceBetween(oAttacker, oTarget);
	float fNoPenaltyDistance = MaxF(POINT_BLANK_RANGE * SCALE_MULTIPLIER, GetItemStat(oWeapon, ITEM_STAT_OPTIMUM_RANGE));
	fDistance = MaxF(fDistance - fNoPenaltyDistance, 0.0f);

	float fAttackRoll = 50.0;
	float fPenalties = fDistance; // every meter distance past the free range is -1!
	float fAttack = fAttackRating + fAttackRoll + fFlanking - fPenalties;

	// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	// BEGIN SECTION CRITICAL HITS
	// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

	int32 bThreatenCritical = (RandomFloat()*100.0f < fCriticalHitModifier);

	if (!bThreatenCritical)
	{
		// ---------------------------------------------------------------------
		// Attacking out of stealth always threatens crit.
		// ---------------------------------------------------------------------
		if (IsModalAbilityActive(oAttacker, ABILITY_SKILL_STEALTH_1))
		{
			bThreatenCritical = TRUE_;
		}

		// -----------------------------------------------------------------
		// Death hex effect ... all hits are auto crit
		// -----------------------------------------------------------------
		if (GetHasEffects(oTarget, EFFECT_TYPE_DEATH_HEX))
		{
			bThreatenCritical = TRUE_;
		}

		// -----------------------------------------------------------------
		// Autocrit effect
		// -----------------------------------------------------------------
		if (GetHasEffects(oAttacker, EFFECT_TYPE_AUTOCRIT))
		{
			bThreatenCritical = TRUE_;
		}
	}
	else
	{
		// ---------------------------------------------------------------------
		// Double strike does not allow crits
		// ---------------------------------------------------------------------
		if (IsModalAbilityActive(oAttacker, ABILITY_TALENT_DUAL_WEAPON_DOUBLE_STRIKE))
		{
			bThreatenCritical = FALSE_;
		}

		// rapid shot doesn't allow critical strikes
		if (IsModalAbilityActive(oAttacker, ABILITY_TALENT_RAPIDSHOT))
		{
			bThreatenCritical = FALSE_;
		}
	}


	// ---------------------------------------------------------------------
	// Targets that have critical hit immunity can not be crit...
	// ---------------------------------------------------------------------
	if (bThreatenCritical)
	{
		Log_Trace_Combat("combat_h.GetAttackResult", " Critical hit averted, target has critical hit immunity", oTarget);
		bThreatenCritical = !HasAbility(oTarget, ABILITY_TRAIT_CRITICAL_HIT_IMMUNITY);
	}



	// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	// END SECTION CRITICAL HITS
	// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<




	// -------------------------------------------------------------------------
	// This section deals with calculating the defense values of the attack target
	// -------------------------------------------------------------------------
	float fDefenseRating = GetCreatureDefense(oTarget) + ((nAttackType == ATTACK_TYPE_RANGED) ? GetCreatureProperty(oTarget, PROPERTY_ATTRIBUTE_MISSILE_SHIELD) : 0.0f);

	// -------------------------------------------------------------------------
	// Easier difficulties grant the player a bonus.
	// -------------------------------------------------------------------------
	fDefenseRating += Diff_GetRulesDefenseBonus(oTarget);

	// -------------------------------------------------------------------------
	// A hit is successful if the attack rating exceeds the defense rating
	// -------------------------------------------------------------------------
	if (RandomFloat()*100.0f < fAttack - fDefenseRating)
	{

		// ---------------------------------------------------------------------
		// If we threatened a critical, we crit here, otherwise just report normal hit
		// ---------------------------------------------------------------------
		nRet = ((bThreatenCritical) ? COMBAT_RESULT_CRITICALHIT : COMBAT_RESULT_HIT);

		if (nAttackType == ATTACK_TYPE_MELEE)
		{
			// -----------------------------------------------------------------
			// If we are backstabbing, we change the result here if it
			// was a crit. Abilities never bs (anim priority)
			// -----------------------------------------------------------------
			if (nAbility == 0 && Combat_CheckBackstab(oAttacker, oTarget, oWeapon, fFlanking))
			{
				nRet = COMBAT_RESULT_BACKSTAB;
			}
		}

	}
	// -------------------------------------------------------------------------
	// Miss...
	// -------------------------------------------------------------------------
	else
	{
		nRet = COMBAT_RESULT_MISS;

	}

	// -------------------------------------------------------------------------
	// Misdirection Hex
	// -------------------------------------------------------------------------
	if (GetHasEffects(oAttacker, EFFECT_TYPE_MISDIRECTION_HEX))
	{
#ifdef DEBUG
		Log_Trace_Combat("combat_h.GetAttackResult", " Attacker under misdirection hex: Hits are misses, crits are hits.", oTarget);
#endif

		if (nRet == COMBAT_RESULT_HIT || nRet == COMBAT_RESULT_BACKSTAB)
		{
			nRet = COMBAT_RESULT_MISS;
		}
		else if (nRet == COMBAT_RESULT_CRITICALHIT)
		{
			nRet = COMBAT_RESULT_HIT;
		}
	}


	//  return COMBAT_RESULT_BACKSTAB;

#ifdef DEBUG
	Log_Trace_Combat("combat_h.GetAttackResult",
		" ToHit Calculation: fAttack:" + IntToString(fAttack) +
		" = (fAttackRating: " + IntToString(fAttackRating) +
		" fAttackRoll:" + IntToString(fAttackRoll) +
		" (range penalty:" + IntToString(fPenalties) + ")" +
		" fFlanking: " + IntToString(fFlanking) +
		" fBonus(script): " + IntToString(fBonus) +
		")", oAttacker, oTarget, LOG_CHANNEL_COMBAT_TOHIT);

	Log_Trace_Combat("combat_h.GetAttackResult",
		" ToHit Calculation (2):  fDefenseRating: " + IntToString(fDefenseRating) +
		" fCriticalHitModifier: " + IntToString(fCriticalHitModifier) +
		" bThreatenCritical: " + IntToString(bThreatenCritical), oAttacker, oTarget, LOG_CHANNEL_COMBAT_TOHIT);

#endif

	return nRet;




}

float Combat_GetFlankingBonus(AActor* oAttacker, AActor* oTarget)
{


	if (HasAbility(oTarget, ABILITY_TALENT_SHIELD_TACTICS))
	{
		if (IsUsingShield(oTarget))
			return 0.0;
	}


	if (GetHasEffects(oTarget, EFFECT_TYPE_FLANK_IMMUNITY))
	{
		return 0.0;
	}




	float fAngle = GetAngleBetweenObjects(oTarget, oAttacker);
	float fFactor = 0.0;
	float fMaxModifier = 15.0;

	// The attackers ability to flank is stored in a creature property
	float fFlankingAngle = GetCreatureProperty(oAttacker, PROPERTY_ATTRIBUTE_FLANKING_ANGLE);

	if (fFlankingAngle <= 10.0) /*old savegames have this at 10*/
	{
		fFlankingAngle = 60.0; // old savegames need this to avoid divby0 later
	}
	else if (fFlankingAngle > 180.0)
	{
		fFlankingAngle = 180.0;
	}


	// -------------------------------------------------------------------------
	if (HasAbility(oAttacker, ABILITY_TALENT_COMBAT_MOVEMENT))
	{
		fMaxModifier = 20.0;
	}

	if (fMaxModifier <= 0.0)
	{
		return 0.0;
	}

	if ((fAngle >= (180.0 - fFlankingAngle) && fAngle <= (180.0 + fFlankingAngle)))
	{
		// Shield block negates flanking on the left.

		int32 bShieldBlock = HasAbility(oTarget, ABILITY_TALENT_SHIELD_BLOCK);
		int32 bUsingShield = IsUsingShield(oTarget);

		if (!bShieldBlock || fAngle < 180.0 || (bShieldBlock && !bUsingShield))
		{
			fFactor = (fFlankingAngle - fabs(180.0 - fAngle)) / fFlankingAngle;
		}

	}

	// Only rogues get the full positional benefits on the battlefield,
	// everyone else gets half
	float fClassModifier = GetCreatureCoreClass(oAttacker) == CLASS_ROGUE ? 1.0f : 0.5f;


	return fFactor * fMaxModifier * fClassModifier;
}

int32 Combat_CheckBackstab(AActor* oAttacker, AActor* oTarget, AActor* oWeapon, float fFlankingBonus)
{

	// -------------------------------------------------------------------------
	// If we we are a rogue
	// -------------------------------------------------------------------------
	if (!HasAbility(oAttacker, ABILITY_TALENT_HIDDEN_ROGUE))
	{
		return FALSE_;
	}

	if (!IsHumanoid(oAttacker))
	{
		return FALSE_;
	}

	// -------------------------------------------------------------------------
	// And target is not immune
	// -------------------------------------------------------------------------
	if (HasAbility(oTarget, ABILITY_TRAIT_CRITICAL_HIT_IMMUNITY))
	{
		return FALSE_;
	}

	// -------------------------------------------------------------------------
	// And attacker does not use double strike mode
	// -------------------------------------------------------------------------
	if (IsModalAbilityActive(oAttacker, ABILITY_TALENT_DUAL_WEAPON_DOUBLE_STRIKE))
	{
		return FALSE_;
	}


	// -------------------------------------------------------------------------
	// We can only backstab if we are flanking.
	// -------------------------------------------------------------------------
	if (fFlankingBonus > 0.0)
	{
		return TRUE_;
	}
	else
	{
		/* Coup de grace*/
		if (HasAbility(oAttacker, ABILITY_TALENT_BACKSTAB))
		{
			if (GetHasEffects(oTarget, EFFECT_TYPE_STUN))
			{
				return TRUE_;
			}
			else if (GetHasEffects(oTarget, EFFECT_TYPE_PARALYZE))
			{
				return TRUE_;
			}
		}

	}

	return FALSE_;
}

int32 CheckForDeathblow(AActor* oAttacker, AActor* oTarget)
{
	int32 nRank = GetCreatureRank(oTarget);
	int32 nLevel = GetLevel(oAttacker);
	float fChance = GetM2DAFloat(TABLE_CREATURERANKS, "fDeathblow", nRank);


	// -------------------------------------------------------------------------
	// Any rank flagged 1.0 or higher triggers a deathblow.
	// -------------------------------------------------------------------------
	if (fChance >= 1.0f)
	{
		return TRUE_;
	}

	// -------------------------------------------------------------------------
	// If we perceive more than 1 creature, then half the chance of triggering
	// a deathblow with each perceived hostile.
	// -------------------------------------------------------------------------
	int32 nCount = GetPerceivedCreatureList(oAttacker, TRUE_).Num();
	if (nCount > 1)
	{
		fChance *= (2.0 / IntToFloat(nCount));
	}
	//Increase chance of death blow in origin stories (level 1 and 2)
	//1.5 times the chance
	if (nLevel < 3)
	{
		fChance *= 1.5;
	}
	return (RandomFloat() < fChance);
}

int32 Combat_GetValidDeathblow(AActor* oAttacker, AActor* oTarget)
{

	// ------------------------------------------------------------------------
	// First, let's check if we can even perform deathblows with the attacker
	// ------------------------------------------------------------------------
	int32 bCanDeathblow = CanPerformDeathblows(oAttacker);

	if (!bCanDeathblow)
	{
		return FALSE_;
	}

	int32 nValid = GetM2DAInt(TABLE_APPEARANCE, "ValidDeathblows", GetAppearanceType(oTarget));

	int32 bImmortal = IsImmortal(oTarget);
	int32 bPlot = IsPlot(oTarget);
	int32 bCanDiePermanently = GetCanDiePermanently(oTarget);
	int32 bAlreadyDead = HasDeathEffect(oTarget);

#ifdef DEBUG
	Log_Trace(LOG_CHANNEL_COMBAT_DEATH, "GetValidDeathblow", "nValid: " + IntToString(nValid) + " bImmortal: " + IntToString(bImmortal) + " bPlot:" +
		IntToString(bPlot) + " bCanDiePermanently:" + IntToString(bCanDiePermanently));

#endif

	// ------------------------------------------------------------------------
	// If we are immortal, Plot, or can not die permanently (e.g. a party member),
	// we return 0
	// ------------------------------------------------------------------------
	if (bImmortal || bPlot || !bCanDiePermanently || IsPartyMember(oTarget) || bAlreadyDead)
	{
		return FALSE_;
	}

	return TRUE_;
}