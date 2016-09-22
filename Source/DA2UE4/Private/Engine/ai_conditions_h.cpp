#include "DA2UE4.h"
#include "DA2UE4Creature.h"
#include "ai_behaviors_h.h"
#include "ai_conditions_h.h"
#include "effect_ai_modifier_h.h"
#include "sys_stealth_h.h"

TArray<AActor*> _AI_GetAllies(AActor* OBJECT_SELF, int32 nCommandType, int32 nCommandSubType)
{
	TArray<AActor*> arAllies;
	TArray<AActor*> arAlliesFinal;

	// This is the top candidate for being in the engine.
	// It is called a lot of times and the second distance filter is not efficient.

	arAllies = GetNearestObjectByGroup(OBJECT_SELF,
		GetGroupId(OBJECT_SELF),
		OBJECT_TYPE_CREATURE,
		AI_MAX_CREATURES_NEAREST,
		TRUE_,      // Living
		TRUE_,      // Perceived
		FALSE_);    // Not including self

					// Filter distance - NOT EFFICIENT! - MOVE TO ENGINE!
					// This includes also ability filter - if the action related to the condition for which we retrieve
					// the list of allies/enemies is a duration ability then we will targeting creatures which already
					// have the specified ability active

	int32 i;
	int32 nSize = arAllies.Num();
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_GetAllies", "INITIAL LIST SIZE: " + IntToString(nSize) + ", command type: " + IntToString(nCommandType) + ", sub command: " + IntToString(nCommandSubType));
#endif

	AActor* oCurrent;
	float fDistance;
	int32 j = 0;
	for (i = 0; i < nSize; i++)
	{
		oCurrent = arAllies[i];
		fDistance = GetDistanceBetween(OBJECT_SELF, oCurrent);

		// Evaluating allies outside of combat ONLY if they are within a certain range
		if (fDistance > AI_RANGE_MAX_ALLY_HELP * SCALE_MULTIPLIER && GetCombatState(oCurrent) == FALSE_)
			continue;

		if (IsDying(oCurrent))
			continue; // dead are not included in the array already

					  // certain creatures may be filtered out if they have the ability active
		if (OBJECT_SELF, nCommandType == AI_COMMAND_USE_ABILITY)
		{
			if (Ability_IsAbilityActive(oCurrent, nCommandSubType))
				continue; // just ignore the creature
			if (!_AI_IsTargetValidForBeneficialAbility(oCurrent, nCommandSubType))
				continue;
		}
		else if (Effects_HasAIModifier(oCurrent, AI_MODIFIER_IGNORE))
			continue; // ignore this target
					  // Creature is valid to be a target - update final array:
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_GetAllies", "Adding creature to list of allies: [" + GetTag(oCurrent) + "]");
#endif
		arAlliesFinal[j] = oCurrent;
		j++;
	}

	return arAlliesFinal;
}

int32 _AI_IsTargetValidForBeneficialAbility(AActor* oCreature, int32 nAbilityID)
{
	int32 nRet = TRUE_;
	float fMaxHealth = GetMaxHealth(oCreature);
	float fCurrentHealth = GetCurrentHealth(oCreature);
	int32 nDebuf = GetM2DAInt(TABLE_AI_ABILITY_COND, "MagicalDebuf", nAbilityID);

#ifdef DEBUG
	Log_Trace_AI(oCreature, "_AI_IsTargetValidForBeneficialAbility", "Creature: " + GetTag(oCreature) + ", current health: " + FloatToString(fCurrentHealth) + ", max health: " + FloatToString(fMaxHealth));
#endif // DEBUG

	int32 nWounded = fCurrentHealth < fMaxHealth ? TRUE_ : FALSE_;

#ifdef DEBUG
	Log_Trace_AI(oCreature, "_AI_IsTargetValidForBeneficialAbility", "Wounded: " + IntToString(nWounded));
#endif // DEBUG

	switch (nAbilityID)
	{
		//TODO ARCHDEMON_DETONATE_DARKSPAWN case in AI_Conditions
		/*case ARCHDEMON_DETONATE_DARKSPAWN:
		{
		if (GetCreatureRank(oCreature) == CREATURE_RANK_LIEUTENANT || GetCreatureRank(oCreature) == CREATURE_RANK_BOSS)
		{
		nRet = FALSE_;
		break;
		}
		// Special case... a darkspawn is 'valid' if he has some enemies near him
		TArray<AActor*> arEnemies = GetNearestObjectByHostility(oCreature, TRUE_, OBJECT_TYPE_CREATURE, 5, TRUE_, FALSE_, FALSE_);
		int32 i;
		int32 nSize = GetArraySize(arEnemies);
		//Log_Trace_AI(OBJECT_SELF, "_AI_IsTargetValidForBeneficialAbility", "array size: " + IntToString(nSize));

		AActor* oCurrent;
		float fDistance;
		nRet = FALSE_;
		for (i = 0; i < nSize; i++)
		{
		oCurrent = arEnemies[i];
		fDistance = GetDistanceBetween(oCurrent, oCreature);
		if (fDistance <= ARCHDEMON_DETONATE_RADIUS)
		{
		//Log_Trace_AI(OBJECT_SELF, "_AI_IsTargetValidForBeneficialAbility", "Got enemy near detonate darkspawn: " + GetTag(oCurrent));
		nRet = TRUE_;
		break;
		}
		}
		break;
		}*/
	case ABILITY_SPELL_HEAL:
	{
		if (nWounded)
			nRet = TRUE_;
		else
			nRet = FALSE_;
		break;
	}
	case ABILITY_SPELL_CURE:
	{
		nRet = TRUE_;
		// valid if mana/stamina is not maxed

		float fCurrentManaStamina = GetCreatureProperty(oCreature, PROPERTY_DEPLETABLE_MANA_STAMINA, PROPERTY_VALUE_CURRENT);
		float fMaxManaStamina = GetCreatureProperty(oCreature, PROPERTY_DEPLETABLE_MANA_STAMINA, PROPERTY_VALUE_BASE);
		if (fCurrentManaStamina == fMaxManaStamina)
			nRet = FALSE_;

		break;
	}
	case ABILITY_SPELL_PURIFY:
	{
		// check wounds
		if (nWounded)
			nRet = TRUE_;
		else
			nRet = FALSE_;

		// .. and curable effects
		TArray<FEffect> eEffects = GetEffects(oCreature);
		int32 nAbility;

		// cycle through effects
		int32 nCount = 0;
		int32 nMax = eEffects.Num();
		for (nCount = 0; nCount < nMax; nCount++)
		{
			// get effect ability id
			nAbility = GetEffectAbilityID(eEffects[nCount]);

			// is ability curable
			if (Ability_CheckFlag(nAbility, ABILITY_FLAG_CURABLE) == TRUE_)
			{
				nRet = TRUE_;
				break;
			}
		}
		break;
	}
	case ABILITY_SPELL_SHIELD_PARTY: // dispel magic
	case ABILITY_SPELL_ANTIMAGIC_BURST:
	{
		if (nDebuf)
			nRet = TRUE_;
		else
			nRet = FALSE_;

		break;
	}
	}
	return nRet;
}

int32 _AI_HasAIStatus(AActor* oCreature, int32 nAIStatus)
{
#ifdef DEBUG
	Log_Trace_AI(oCreature, "_AI_HasAIStatus", "checking for creature: " + GetTag(oCreature) + ", status: " + IntToString(nAIStatus));
#endif
	int32 nRet = FALSE_;

	switch (nAIStatus)
	{
	case AI_STATUS_POLYMORPH: nRet = GetHasEffects(oCreature, EFFECT_TYPE_SHAPECHANGE); break;
	case AI_STATUS_CHARM: nRet = GetHasEffects(oCreature, EFFECT_TYPE_CHARM); break;
	case AI_STATUS_PARALYZE: nRet = GetHasEffects(oCreature, EFFECT_TYPE_PARALYZE); break;
	case AI_STATUS_STUN: nRet = GetHasEffects(oCreature, EFFECT_TYPE_STUN); break;
	case AI_STATUS_SLEEP: nRet = GetHasEffects(oCreature, EFFECT_TYPE_SLEEP); break;
	case AI_STATUS_ROOT: nRet = GetHasEffects(oCreature, EFFECT_TYPE_ROOT); break;
	case AI_STATUS_DAZE: nRet = GetHasEffects(oCreature, EFFECT_TYPE_DAZE); break;
	case AI_STATUS_SLOW: nRet = GetHasEffects(oCreature, EFFECT_TYPE_MOVEMENT_RATE); break;
	case AI_STATUS_STEALTH: nRet = GetHasEffects(oCreature, EFFECT_TYPE_STEALTH); break;
	case AI_STATUS_DISEASED: nRet = GetHasEffects(oCreature, EFFECT_TYPE_DISEASE); break;
	case AI_STATUS_DEAD: nRet = GetHasEffects(oCreature, EFFECT_TYPE_DEATH); break;
	case AI_STATUS_UNCONSIOUS: nRet = GetHasEffects(oCreature, EFFECT_TYPE_DEATH); break;
	case AI_STATUS_KNOCKDOWN: nRet = GetHasEffects(oCreature, EFFECT_TYPE_KNOCKDOWN) || GetHasEffects(oCreature, EFFECT_TYPE_SLIP); break;
	case AI_STATUS_GRABBED: nRet = GetHasEffects(oCreature, EFFECT_TYPE_GRABBED) || GetHasEffects(oCreature, EFFECT_TYPE_OVERWHELMED); break;
	case AI_STATUS_GRABBING: nRet = GetHasEffects(oCreature, EFFECT_TYPE_GRABBING) || GetHasEffects(oCreature, EFFECT_TYPE_OVERWHELMING); break;
	case AI_STATUS_BERSERK: nRet = IsModalAbilityActive(oCreature, ABILITY_TALENT_BERSERK); break;
	case AI_STATUS_CONFUSED: nRet = GetHasEffects(oCreature, EFFECT_TYPE_CONFUSION); break;
	case AI_STATUS_IMMOBLIZED:
	{
		int32 nFlags = GetEffectsFlags(oCreature);
		if (nFlags & EFFECT_FLAG_DISABLE_MOVEMENT)
			nRet = TRUE_;
		break;
	}
	case AI_STATUS_MOVEMENT_IMPAIRED:
	{
		int32 nFlags = GetEffectsFlags(oCreature);
		if ((nFlags & EFFECT_FLAG_DISABLE_MOVEMENT) || GetHasEffects(oCreature, EFFECT_TYPE_MOVEMENT_RATE))
			nRet = TRUE_;

		break;
	}
	case AI_STATUS_CANT_ATTACK:
	{
		int32 nFlags = GetEffectsFlags(oCreature);
		if (nFlags & EFFECT_FLAG_DISABLE_COMBAT)
			nRet = TRUE_;

		break;
	}
	}
#ifdef DEBUG
	Log_Trace_AI(oCreature, "_AI_HasAIStatus", "return: " + IntToString(nRet));
#endif

	return nRet;
}

int32 _AI_IsHostileTargetValid(AActor* OBJECT_SELF, AActor* oTarget)
{
	int32 nRet = TRUE_;
#ifdef DEBUG
	FString DEBUG_sInvalidReason = "";
#endif

	if (!IsObjectValid(oTarget))
	{
#ifdef DEBUG
		DEBUG_sInvalidReason = "INVALID OBJECT";
#endif
		nRet = FALSE_;
	}
	else if (IsDead(oTarget))
	{
#ifdef DEBUG
		DEBUG_sInvalidReason = "DEAD";
#endif
		nRet = FALSE_;
	}
	else if (IsDying(oTarget))
	{
#ifdef DEBUG
		DEBUG_sInvalidReason = "DYING";
#endif
		nRet = FALSE_;
	}
	else if (!GetObjectActive(oTarget))
	{
#ifdef DEBUG
		DEBUG_sInvalidReason = "TARGET INACTIVE";
#endif
		nRet = FALSE_;
	}
	else if (!IsObjectHostile(OBJECT_SELF, oTarget))
	{
#ifdef DEBUG
		DEBUG_sInvalidReason = "NOT HOSTILE";
#endif
		nRet = FALSE_;
	}
	else if (Effects_HasAIModifier(OBJECT_SELF, AI_MODIFIER_IGNORE))
	{
#ifdef DEBUG
		DEBUG_sInvalidReason = "AI IGNORE FLAG ACTIVE";
#endif
		nRet = FALSE_;
	}
	else if (!IsPerceiving(OBJECT_SELF, oTarget))
	{
#ifdef DEBUG
		DEBUG_sInvalidReason = "TARGET NOT PERCEIVED";
#endif
		nRet = FALSE_;
	}
	else if (IsStealthy(oTarget))
	{
#ifdef DEBUG
		DEBUG_sInvalidReason = "TARGET STEALTHY";
#endif
		nRet = FALSE_;
	}

#ifdef DEBUG
	if (nRet == FALSE_)
	{
		Log_Trace_AI(OBJECT_SELF, "_AI_IsHostileTargetValid", "Target invalid: " + DEBUG_sInvalidReason);
	}
#endif

	return nRet;
}

AActor* _AI_Condition_BeingAttackedByAttackType(AActor* OBJECT_SELF, int32 nTargetType, int32 nAttackType, int32 nTacticCommand, int32 nTacticSubCommand, int32 nTacticID)
{
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_Condition_BeingAttackedByAttackType", "START, attack type: " + IntToString(nAttackType));
#endif
	AActor* oTarget = nullptr;
	AActor* oCurrentEnemy;
	int32 i;
	int32 j;
	TArray<AActor*> arAllies = _AI_GetAllies(OBJECT_SELF, nTacticCommand, nTacticSubCommand);
	int32 nAlliesNum = arAllies.Num();
	TArray<AActor*> arEnemies = _AI_GetEnemies(OBJECT_SELF, nTacticCommand, nTacticSubCommand, nTargetType);
	int32 nEnemiesNum = arEnemies.Num();
	AActor* oCurrentAlly;
	AActor* oAlly;

	switch (nTargetType)
	{
	case AI_TARGET_TYPE_SELF:
	{
		for (i = 0; i < nEnemiesNum; i++)
		{
			oCurrentEnemy = arEnemies[i];
			if (GetAttackTarget(oCurrentEnemy) == OBJECT_SELF && (_AI_GetAttackType(oCurrentEnemy) & nAttackType))
			{
				// if melee need to check range
				if (nAttackType == AI_ATTACK_TYPE_MELEE)
				{
					float fRange = GetDistanceBetween(OBJECT_SELF, oCurrentEnemy);
					if (fRange <= AI_MELEE_RANGE * SCALE_MULTIPLIER)
					{
						oTarget = OBJECT_SELF;
						break;
					}
				}
				else // non melee
				{
					oTarget = OBJECT_SELF; // found one enemy that attacks with the specified type - that's all we need
					break;
				}
			}
		}
		break;
	}
	case AI_TARGET_TYPE_ENEMY:
	{
		// Checking all enemies - stopping when finding first ally being attack by a specific type
		for (i = 0; i < nEnemiesNum && oTarget == nullptr; i++)
		{
			oCurrentEnemy = arEnemies[i];
			for (j = 0; j < nAlliesNum && oTarget == nullptr; j++)
			{
				oCurrentAlly = arAllies[j];
				if (GetAttackTarget(oCurrentAlly) == oCurrentEnemy && (_AI_GetAttackType(oCurrentAlly) & nAttackType))
				{
					// if melee need to check range
					if (nAttackType == AI_ATTACK_TYPE_MELEE)
					{
						float fRange = GetDistanceBetween(oCurrentAlly, oCurrentEnemy);
						if (fRange <= AI_MELEE_RANGE * SCALE_MULTIPLIER)
						{
							oTarget = oCurrentEnemy;
							break;
						}
					}
					else // non melee
					{
						oTarget = oCurrentEnemy; // found one enemy that attacks with the specified type - that's all we need
						break;
					}
				}
			}
		}
		break;
	}
	case AI_TARGET_TYPE_ALLY:
	{
		// Checking all allies - stopping when finding first ally being attack by a specific type
		for (i = 0; i < nAlliesNum && oTarget == nullptr; i++)
		{
			oCurrentAlly = arAllies[i];
			for (j = 0; j < nEnemiesNum && oTarget == nullptr; j++)
			{
				oCurrentEnemy = arEnemies[j];
				if (GetAttackTarget(oCurrentEnemy) == oCurrentAlly && (_AI_GetAttackType(oCurrentEnemy) & nAttackType))
				{
					// if melee need to check range
					if (nAttackType == AI_ATTACK_TYPE_MELEE)
					{
						float fRange = GetDistanceBetween(oCurrentAlly, oCurrentEnemy);
						if (fRange <= AI_MELEE_RANGE * SCALE_MULTIPLIER)
						{
							oTarget = oCurrentAlly;
							break;
						}
					}
					else // non melee
					{
						oTarget = oCurrentAlly; // found one enemy that attacks with the specified type - that's all we need
						break;
					}
				}
			}
		}
		break;
	}
	case AI_TARGET_TYPE_MOST_HATED:
	{
		AActor* oMostHated = _AI_Condition_GetMostHatedEnemy(OBJECT_SELF, 1, nTacticCommand, nTacticSubCommand, nTacticID);
		if ((_AI_GetAttackType(oMostHated) & nAttackType) && GetAttackTarget(oMostHated) == OBJECT_SELF)
			oTarget = OBJECT_SELF;
		break;
	}
	default: // It can still be a party target
	{
		oAlly = _AI_GetPartyTarget(OBJECT_SELF, nTargetType, nTacticCommand, nTacticSubCommand, nTacticID);
		for (j = 0; j < nEnemiesNum && oTarget == nullptr; j++)
		{
			oCurrentEnemy = arEnemies[j];
			if ((_AI_GetAttackType(oCurrentEnemy) & nAttackType) && GetAttackTarget(oCurrentEnemy) == oAlly)
			{
				// if melee need to check range
				if (nAttackType == AI_ATTACK_TYPE_MELEE)
				{
					float fRange = GetDistanceBetween(oAlly, oCurrentEnemy);
					if (fRange <= AI_MELEE_RANGE * SCALE_MULTIPLIER)
					{
						oTarget = oAlly;
						break;
					}
				}
				else // non melee
				{
					oTarget = oAlly; // found one enemy that attacks with the specified type - that's all we need
					break;
				}
			}
		}
		break;
	}

	}

	return oTarget;
}

TArray<AActor*> _AI_GetEnemies(AActor* OBJECT_SELF, int32 nCommandType, int32 nCommandSubType, int32 nTargetTypeOfFollower)
{
	TArray<AActor*> arEnemies;
	TArray<AActor*> arEnemiesFinal;
	AActor* oTargetOverride = _AI_GetTargetOverride(OBJECT_SELF);

#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_GetEnemies", "nTargetTypeOfFollower: " + IntToString(nTargetTypeOfFollower) + ", commandtype: " + IntToString(nCommandType) + ", nCommandSubType: " + IntToString(nCommandSubType));
#endif
	// if there is target override, then the array of enemies has one AActor* with the overriden target
	if (IsObjectValid(oTargetOverride))
	{
		arEnemies[0] = oTargetOverride;
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_GetEnemies", "Got override target: " + GetTag(oTargetOverride));
#endif
		return arEnemies;
	}
	else if (IsFollower(OBJECT_SELF) && nTargetTypeOfFollower == AI_TARGET_TYPE_ENEMY && nCommandType == AI_COMMAND_USE_ABILITY)
	{
		AActor* oCurrentTarget = GetAttackTarget(OBJECT_SELF);

		if (IsObjectValid(oCurrentTarget))
		{
			float fRangeToTarget = GetDistanceBetween(OBJECT_SELF, oCurrentTarget);
			int32 nAbilityTargetType = GetM2DAInt(TABLE_ABILITIES_TALENTS, "range", nCommandSubType) * SCALE_MULTIPLIER;
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_GetEnemies", "Follower current target: " + GetTag(oCurrentTarget) + ", range: " + FloatToString(fRangeToTarget)
				+ ", ability range: " + IntToString(GetM2DAInt(TABLE_ABILITIES_TALENTS, "range", nCommandSubType)));
#endif
			if ((nAbilityTargetType == 1 * SCALE_MULTIPLIER || nAbilityTargetType == 0 * SCALE_MULTIPLIER) &&
				fRangeToTarget <= AI_MELEE_RANGE * SCALE_MULTIPLIER &&
				IsUsingMeleeWeapon(OBJECT_SELF))
			{
				arEnemies[0] = oCurrentTarget;
#ifdef DEBUG
				Log_Trace_AI(OBJECT_SELF, "_AI_GetEnemies", "Follower keeping melee target for melee use-ability: " + GetTag(oCurrentTarget));
#endif
				return arEnemies;
			}
		}
	}

	// This is the top candidate for being in the engine.
	// It is called a lot of times and the second distance filter is not efficient.

	arEnemies = GetNearestObjectByHostility(OBJECT_SELF,
		TRUE_,
		OBJECT_TYPE_CREATURE,
		AI_MAX_CREATURES_NEAREST,
		TRUE_,   // Living
		TRUE_,   // Perceived
		FALSE_); // Not including self

				 // Filter distance - NOT EFFICIENT! - MOVE TO ENGINE!

				 // This includes also ability filter - if the action related to the condition for which we retrieve
				 // the list of allies/enemies is a duration ability then we will targeting creatures which already
				 // have the specified ability active

	int32 i;
	int32 nSize = arEnemies.Num();

#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_GetEnemies", "Initial size: " + IntToString(nSize));
#endif

	// check what distance to evaluate targets from main controlled
	float fFollowerMaxEngageRange = AI_FOLLOWER_ENGAGE_DISTANCE_LONG * SCALE_MULTIPLIER;
	if (IsFollower(OBJECT_SELF) && AI_BehaviorCheck_ChaseEnemy(OBJECT_SELF) == FALSE_)
		fFollowerMaxEngageRange = AI_FOLLOWER_ENGAGE_DISTANCE_CLOSE * SCALE_MULTIPLIER;

	AActor* oCurrent;
	float fDistance;
	float fEnemyDistanceToMainControlled;

	for (i = 0; i < nSize; i++)
	{
		oCurrent = arEnemies[i];
		fDistance = GetDistanceBetween(OBJECT_SELF, oCurrent);
		if (IsFollower(OBJECT_SELF))
		{
			fEnemyDistanceToMainControlled = GetDistanceBetween(oCurrent, GetMainControlled());
			if (fEnemyDistanceToMainControlled > fFollowerMaxEngageRange)
			{
				// Filter only if trying melee attack or 0/touch range ability
				if (OBJECT_SELF, nCommandType == AI_COMMAND_ATTACK && IsUsingMeleeWeapon(OBJECT_SELF))
				{
#ifdef DEBUG
					Log_Trace_AI(OBJECT_SELF, "_AI_GetEnemies", "Stopping to evaluate enemies for MELEE ATTACK - too far from main controlled follower");
#endif
				}
				else if (OBJECT_SELF, nCommandType == AI_COMMAND_USE_ABILITY && GetM2DAInt(TABLE_ABILITIES_SPELLS, "Range", nCommandSubType) == 1)
				{
#ifdef DEBUG
					Log_Trace_AI(OBJECT_SELF, "_AI_GetEnemies", "Stopping to evaluate enemies for TOUCH RANGE ABILITY - too far from main controlled follower");
#endif
				}

				break;
			}
		}

		// certain creatures may be filtered out if they have the ability active
		if (OBJECT_SELF, nCommandType == AI_COMMAND_USE_ABILITY)
		{
			if (Ability_IsAbilityActive(oCurrent, nCommandSubType))
				continue; // just ignore the creature
		}
		else if (Effects_HasAIModifier(oCurrent, AI_MODIFIER_IGNORE))
			continue; // ignore this target
					  // Creature is valid to be a target - update final array:
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_GetEnemies", "Adding creature to list of enemies: [" + GetTag(oCurrent) + "]");
#endif
		arEnemiesFinal.Add(oCurrent);
	}

	nSize = arEnemiesFinal.Num();
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_GetEnemies", "Final size: " + IntToString(nSize));
#endif

	return arEnemiesFinal;
}

AActor* _AI_GetTargetOverride(AActor* aActor)
{
	ADA2UE4Creature* OBJECT_SELF = Cast<ADA2UE4Creature>(aActor);
	AActor* oTarget = nullptr;

	AActor* oTargetOverride = GetActorFromName(OBJECT_SELF->AI_TARGET_OVERRIDE); // if overridden by UT_CombatStart
	int32 nTargetOverrideCount = OBJECT_SELF->AI_TARGET_OVERRIDE_DUR_COUNT; // how long was overridden

	if (!IsObjectValid(oTargetOverride) || IsDead(oTargetOverride))
	{
		OBJECT_SELF->AI_TARGET_OVERRIDE_DUR_COUNT = 0;
		OBJECT_SELF->AI_TARGET_OVERRIDE = "";
		return nullptr;
	}

	if (nTargetOverrideCount < AI_TARGET_OVERRIDE_DURATION)
	{
		// Has override target and override duration is still valid -> use override target
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_GetTargetOverride", "Using target override: " + GetTag(oTargetOverride));
#endif
		oTarget = oTargetOverride;
		// Increase counter, only if not permanent
		if (nTargetOverrideCount != -1)
		{
			nTargetOverrideCount++;
			OBJECT_SELF->AI_TARGET_OVERRIDE_DUR_COUNT = nTargetOverrideCount;
		}
	}
	else if (nTargetOverrideCount >= AI_TARGET_OVERRIDE_DURATION)
	{
		Log_Trace_AI(OBJECT_SELF, "_AI_GetTargetOverride", "Stopping target override (timed out) - will try to pick new target. Override target: " + GetTag(oTargetOverride));
		OBJECT_SELF->AI_TARGET_OVERRIDE_DUR_COUNT = 0;
		OBJECT_SELF->AI_TARGET_OVERRIDE = "";
	}

	return oTarget;
}

int32 _AI_GetAttackType(AActor* oCreature)
{
	int32 nRet = AI_ATTACK_TYPE_INVALID;

	int32 nItemType = -1;
	int32 nHasAIStatusCantAttack = _AI_HasAIStatus(oCreature, AI_STATUS_CANT_ATTACK);

#ifdef DEBUG
	Log_Trace_AI(oCreature, "_AI_GetAttackType", "creature core class: " + IntToString(GetCreatureCoreClass(oCreature)));
	Log_Trace_AI(oCreature, "_AI_GetAttackType", "IsMelee: " + IntToString(IsUsingMeleeWeapon(oCreature)));
	Log_Trace_AI(oCreature, "_AI_GetAttackType", "IsRanged: " + IntToString(IsUsingRangedWeapon(oCreature)));
#endif

	// Removed the commandtype conditions so AI can detect the player as melee/range even before he started attacking

	if (!nHasAIStatusCantAttack &&
		(GetCreatureCoreClass(oCreature) == CLASS_WIZARD || GetCreatureCoreClass(oCreature) == 25)) // not a perfect check but covers 99% of the cases. Good enough.
	{
#ifdef DEBUG
		Log_Trace_AI(oCreature, "_AI_GetAttackType", "MAGIC attack type for: " + GetTag(oCreature));
#endif
		nRet = AI_ATTACK_TYPE_MAGIC;
	}
	else if (IsUsingRangedWeapon(oCreature) && !nHasAIStatusCantAttack)
	{
#ifdef DEBUG
		Log_Trace_AI(oCreature, "_AI_GetAttackType", "RANGED attack type for: " + GetTag(oCreature));
#endif
		nRet = AI_ATTACK_TYPE_RANGED;
	}
	else if (!nHasAIStatusCantAttack)// melee
	{
#ifdef DEBUG
		Log_Trace_AI(oCreature, "_AI_GetAttackType", "MELEE attack type for: " + GetTag(oCreature));
#endif
		nRet = AI_ATTACK_TYPE_MELEE;
	}

	return nRet;
}

AActor* _AI_Condition_GetMostHatedEnemy(AActor* OBJECT_SELF, int32 n, int32 nCommandType, int32 nCommandSubType, int32 nTacticID)
{
	AActor* oTarget;
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_Condition_GetMostHatedEnemy", "START, command type: " + IntToString(nCommandType) + ", command sub type: " + IntToString(nCommandSubType));
#endif
	AActor* oTargetOverride = _AI_GetTargetOverride(OBJECT_SELF);
	if (IsObjectValid(oTargetOverride))
		return oTargetOverride;

	int32 nSize = GetThreatTableSize(OBJECT_SELF);

	oTarget = AI_Threat_GetThreatTarget(OBJECT_SELF);

	// final check - if the action is ability and already active then not return this target
	if (OBJECT_SELF, nCommandType == AI_COMMAND_USE_ABILITY && Ability_IsAbilityActive(oTarget, nCommandSubType))
	{
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_Condition_GetMostHatedEnemy", "Most hated already has this ability applied... aborting");
		oTarget = nullptr;
#endif
	}

	return oTarget;
}

AActor* _AI_GetPartyTarget(AActor* OBJECT_SELF, int32 nTargetType, int32 nCommandType, int32 nCommandSubType, int32 nTacticID)
{
	AActor* oTarget = nullptr;

	AActor* oTacticTargetObject = GetTacticTargetObject(OBJECT_SELF, nTacticID);

	if (OBJECT_SELF, nTargetType == AI_TARGET_TYPE_HERO)
		oTarget = GetHero();
	else if (OBJECT_SELF, nTargetType == AI_TARGET_TYPE_MAIN_CONTROLLED)
		oTarget = GetMainControlled();
	else
		oTarget = oTacticTargetObject;

	if (IsObjectValid(oTarget) && nCommandType == AI_COMMAND_USE_ABILITY && Ability_IsAbilityActive(oTarget, nCommandSubType))
		oTarget = nullptr;

	return oTarget;
}

AActor* _AI_Condition_GetCreatureWithAIStatus(AActor* OBJECT_SELF, int32 nAIStatus, int32 nTargetType, int32 nCommandType, int32 nCommandSubType, int32 nTacticID)
{
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_Condition_GetCreatureWithAIStatus", "[START] AI Status:" + IntToString(nAIStatus) + ", Target Type: " + IntToString(nTargetType));
#endif

	AActor* oTarget = nullptr;
	TArray<AActor*> arCreatures;
	int32 nSize = 0;
	AActor* oCurrentCreature;
	int32 i;

	switch (nTargetType)
	{
	case AI_TARGET_TYPE_SELF:
	{
		// Making sure we're not trying to apply an ability that is already active
		if (OBJECT_SELF, nCommandType == AI_COMMAND_USE_ABILITY && !Ability_IsAbilityActive(OBJECT_SELF, nCommandSubType))
			arCreatures[0] = OBJECT_SELF;
		else if (OBJECT_SELF, nCommandType != AI_COMMAND_USE_ABILITY) // not an ability - can use self as target
			arCreatures[0] = OBJECT_SELF;

		break;
	}
	case AI_TARGET_TYPE_ALLY:
	{
		// Getting a list of the nearest creatures with the same group, alive and perceived, not including self
		arCreatures = _AI_GetAllies(OBJECT_SELF, nCommandType, nCommandSubType);

		break;
	}
	case AI_TARGET_TYPE_ENEMY:
	{
		// Getting a list of the nearest hostiles, alive and perceived, not including self:
		arCreatures = _AI_GetEnemies(OBJECT_SELF, nCommandType, nCommandSubType, nTargetType);

		break;
	}
	case AI_TARGET_TYPE_MOST_HATED:
	{
		arCreatures[0] = _AI_Condition_GetMostHatedEnemy(OBJECT_SELF, 1, nCommandType, nCommandSubType, nTacticID);
		break;
	}
	default: // It can still be a party target
	{
		arCreatures[0] = _AI_GetPartyTarget(OBJECT_SELF, nTargetType, nCommandType, nCommandSubType, nTacticID);
		break;
	}
	}

	nSize = arCreatures.Num();
	for (i = 0; i < nSize; i++)
	{
		oCurrentCreature = arCreatures[i];

		if (_AI_HasAIStatus(oCurrentCreature, nAIStatus))
		{
			oTarget = oCurrentCreature;
			break;
		}
	}

	return oTarget;
}

AActor* _AI_Condition_GetCreatureWithHPLevel(AActor* OBJECT_SELF, int32 nHPLevel, int32 nTargetType, int32 nCommandType, int32 nCommandSubType, int32 nTacticID)
{
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_Condition_GetCreatureWithHPLevel", "[START] HP Level: " + IntToString(nHPLevel));
#endif
	return _AI_SubCondition_GetCreatureWithStatLevel(OBJECT_SELF, AI_STAT_TYPE_HP, nHPLevel, nTargetType, nCommandType, nCommandSubType, nTacticID);
}

AActor* _AI_Condition_SelfHPLevel(AActor* OBJECT_SELF, int32 nHPLevel, int32 nTargetType, int32 nCommandType, int32 nCommandSubType, int32 nTacticID)
{
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_Condition_SelfHPLevel", "[START] HP Level: " + IntToString(nHPLevel));
#endif

	AActor* oTarget = _AI_SubCondition_GetCreatureWithStatLevel(OBJECT_SELF, AI_STAT_TYPE_HP, nHPLevel, AI_TARGET_TYPE_SELF, nCommandType, nCommandSubType, nTacticID);
	if (oTarget != OBJECT_SELF)
		return nullptr;

	TArray<AActor*> arCreatures;

	switch (nTargetType)
	{
	case AI_TARGET_TYPE_ENEMY:
	{
		arCreatures = _AI_GetEnemies(OBJECT_SELF, nCommandType, nCommandSubType, nTargetType);
		break;
	}
	case AI_TARGET_TYPE_ALLY:
	{
		arCreatures = _AI_GetAllies(OBJECT_SELF, nCommandType, nCommandSubType);
		break;
	}
	case AI_TARGET_TYPE_MOST_HATED:
	{
		arCreatures[0] = _AI_Condition_GetMostHatedEnemy(OBJECT_SELF, 1, nCommandType, nCommandSubType, nTacticID);
		break;
	}
	default:
	{
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_Condition_SelfHPLevel", "INVALID Target Type: " + IntToString(nTargetType));
#endif
		break;
	}
	}

	return arCreatures[0];
}

AActor* _AI_Condition_SelfManaStaminaLevel(AActor* OBJECT_SELF, int32 nManaStaminaLevel, int32 nTargetType, int32 nCommandType, int32 nCommandSubType, int32 nTacticID)
{
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_Condition_SelfManaStaminaLevel", "[START] Mana/Stamina Level: " + IntToString(nManaStaminaLevel));
#endif

	AActor* oTarget = _AI_SubCondition_GetCreatureWithStatLevel(OBJECT_SELF, AI_STAT_TYPE_MANA_OR_STAMINA, nManaStaminaLevel, AI_TARGET_TYPE_SELF, nCommandType, nCommandSubType, nTacticID);
	if (oTarget != OBJECT_SELF)
		return nullptr;

	TArray<AActor*> arCreatures;

	switch (nTargetType)
	{
	case AI_TARGET_TYPE_ENEMY:
	{
		arCreatures = _AI_GetEnemies(OBJECT_SELF, nCommandType, nCommandSubType, nTargetType);
		break;
	}
	case AI_TARGET_TYPE_ALLY:
	{
		arCreatures = _AI_GetAllies(OBJECT_SELF, nCommandType, nCommandSubType);
		break;
	}
	case AI_TARGET_TYPE_MOST_HATED:
	{
		arCreatures[0] = _AI_Condition_GetMostHatedEnemy(OBJECT_SELF, 1, nCommandType, nCommandSubType, nTacticID);
		break;
	}
	default:
	{
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_Condition_SelfManaStaminaLevel", "INVALID Target Type: " + IntToString(nTargetType));
#endif
		break;
	}
	}

	return arCreatures[0];
}

AActor* _AI_Condition_GetCreatureWithManaOrStaminaLevel(AActor* OBJECT_SELF, int32 nManaOrStaminaLevel, int32 nTargetType, int32 nCommandType, int32 nCommandSubType, int32 nTacticID)
{
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_Condition_GetCreatureWithManaOrStaminaLevel", "[START] Mana or Stamina Level: " + IntToString(nManaOrStaminaLevel));
#endif
	return _AI_SubCondition_GetCreatureWithStatLevel(OBJECT_SELF, AI_STAT_TYPE_MANA_OR_STAMINA, nManaOrStaminaLevel, nTargetType, nCommandType, nCommandSubType, nTacticID);
}

AActor* _AI_SubCondition_GetCreatureWithStatLevel(AActor* OBJECT_SELF, int32 nStatType, int32 nStatLevel, int32 nTargetType, int32 nCommandType, int32 nCommandSubType, int32 nTacticID)
{
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_SubCondition_GetCreatureWithStatLevel", "[START] Stat level: " + IntToString(nStatLevel));
#endif
	AActor* oTarget = nullptr;
	TArray<AActor*> arCreatures;
	int32 nSize = 0;
	AActor* oCurrentCreature;
	int32 i;
	//unused 	float fDistance;

	switch (nTargetType)
	{
	case AI_TARGET_TYPE_SELF:
	{

		// Making sure we're not trying to apply an ability that is already active
		if (OBJECT_SELF, nCommandType == AI_COMMAND_USE_ABILITY && !Ability_IsAbilityActive(OBJECT_SELF, nCommandSubType))
			arCreatures[0] = OBJECT_SELF;
		else if (OBJECT_SELF, nCommandType != AI_COMMAND_USE_ABILITY) // not an ability - can use self as target
			arCreatures[0] = OBJECT_SELF;

		break;
	}
	case AI_TARGET_TYPE_ALLY:
	{
		// Getting a list of the nearest creatures with the same group, alive and perceived, not including self
		arCreatures = _AI_GetAllies(OBJECT_SELF, nCommandType, nCommandSubType);
		break;
	}
	case AI_TARGET_TYPE_ENEMY:
	{
		// Getting a list of the nearest hostiles, alive and perceived, not including self:
		arCreatures = _AI_GetEnemies(OBJECT_SELF, nCommandType, nCommandSubType, nTargetType);
		break;
	}
	case AI_TARGET_TYPE_MOST_HATED:
	{
		arCreatures[0] = _AI_Condition_GetMostHatedEnemy(OBJECT_SELF, 1, nCommandType, nCommandSubType, nTacticID);
		break;
	}
	default: // It can still be a party target
	{
		arCreatures[0] = _AI_GetPartyTarget(OBJECT_SELF, nTargetType, nCommandType, nCommandSubType, nTacticID);
		break;
	}
	}

	// If we have an array, and don't have a target yet, then try to find the target in the array
	nSize = arCreatures.Num();

	// This loops will be evaluated only if any array was generated above
	for (i = 0; i < nSize; i++)
	{
		oCurrentCreature = arCreatures[i];

		if (_AI_HasStatLevel(OBJECT_SELF, oCurrentCreature, nStatType, nStatLevel))
		{
			oTarget = oCurrentCreature;
			break; // got what we need
		}

	}

	return oTarget;
}

AActor* _AI_Condition_GetNthMostDamagedCreatureInGroup(AActor* OBJECT_SELF, int32 nHighLow, int32 nTargetType, int32 nCommandType, int32 nCommandSubType, int32 nTacticID)
{
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_Condition_GetNthMostDamagedCreatureInGroup (=> get creature with highest or lowest health)", "START");
#endif


	AActor* oTarget = nullptr;
	TArray<AActor*> arCreatures;

	switch (nTargetType)
	{
	case AI_TARGET_TYPE_ALLY:
	{
		// Getting a list of the nearest creatures with the same group, alive and perceived, not including self
		arCreatures = _AI_GetAllies(OBJECT_SELF, nCommandType, nCommandSubType);
		break;
	}
	case AI_TARGET_TYPE_ENEMY:
	{
		// Getting a list of the nearest hostiles, alive and perceived, not including self:
		arCreatures = _AI_GetEnemies(OBJECT_SELF, nCommandType, nCommandSubType, nTargetType);
		break;
	}
	default:
	{
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_Condition_GetNthMostDamagedCreatureInGroup", "INVALID Target Type: " + IntToString(nTargetType));
#endif
		break;
	}
	}

	oTarget = _AI_GetNthDamagedCreatureInArray(OBJECT_SELF, arCreatures, nHighLow);



	return oTarget;
}

FVector _AI_Condition_GetEnemyClusteredWithSameGroup(AActor* OBJECT_SELF, int32 nMinClusterSize, int32 nCommandType, int32 nCommandSubType, int32 nTacticID)
{
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_Condition_GetEnemyClusteredWithSameGroup", "START");
#endif
	int32 nAllyFailChance = 0; // will be higher the smaller the cluster is

	switch (nCommandSubType)
	{
	case ABILITY_SPELL_INFERNO: nAllyFailChance = 100; break;
	case ABILITY_SPELL_FIREBALL: nAllyFailChance = 75; break;
	case ABILITY_SPELL_MASS_PARALYSIS: nAllyFailChance = 0; break;
	case ABILITY_SPELL_GREASE: nAllyFailChance = 50; break;
	case ABILITY_SPELL_EARTHQUAKE: nAllyFailChance = 100; break;
	case ABILITY_SPELL_SLEEP: nAllyFailChance = 0; break;
	case ABILITY_SPELL_BLIZZARD: nAllyFailChance = 75; break;
	case ABILITY_SPELL_TEMPEST: nAllyFailChance = 75; break;
	case ABILITY_SPELL_DEATH_CLOUD: nAllyFailChance = 75; break;
	}
	if (IsFollower(OBJECT_SELF) && nAllyFailChance > 0)
		nAllyFailChance = 100;

	FVector lLoc = FVector(0.f);
	if (GetCombatState(OBJECT_SELF) == FALSE_)
		return lLoc;
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_Condition_GetEnemyClusteredWithSameGroup", "ability: " + IntToString(nCommandSubType) + ", cluster: " + IntToString(nMinClusterSize) + ", ally fail: " + IntToString(nAllyFailChance));
#endif
	lLoc = GetClusterCenter(OBJECT_SELF, nCommandSubType, nMinClusterSize, nAllyFailChance, FALSE_);
	TArray<AActor*> arNearest = GetNearestObjectToLocation(lLoc, OBJECT_TYPE_CREATURE, 1);
	AActor* oNearestToCluster = arNearest[0];
	if (!IsObjectValid(oNearestToCluster))
	{
		FVector lInvalid = FVector(0.f);
		return lInvalid;
	}
	float fDistance = GetDistanceBetweenLocations(lLoc, GetLocation(oNearestToCluster));
	if (fDistance > 5.0 * SCALE_MULTIPLIER)
	{
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_Condition_GetEnemyClusteredWithSameGroup", "can't perceive nearest creature to cluster - invalid cluster");
#endif
		FVector lInvalid = FVector(0.f);
		return lInvalid;
	}

	//TODO _AI_Condition_GetEnemyClusteredWithSameGroup special case for arch-demon
	/*
	// special case for arch-demon abilities - fail if they are too close
	float fClusterDistance = GetDistanceBetweenLocations(GetLocation(OBJECT_SELF), lLoc);
	if (nCommandSubType == ARCHDEMON_VORTEX || nCommandSubType == ARCHDEMON_SMITE)
	{
	if (fClusterDistance < AI_RANGE_MID_LONG * SCALE_MULTIPLIER)
	{
	#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_Condition_GetEnemyClusteredWithSameGroup", "Archdemon abilities can't trigger too close");
	#endif
	location lInvalid;
	return lInvalid;
	}
	}*/
	return lLoc;
}

AActor* _AI_Condition_GetNearestVisibleCreature(AActor* OBJECT_SELF, int32 nTargetType, int32 n, int32 nCommandType, int32 nCommandSubType, int32 nTacticID)
{
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_Condition_GetNearestVisibleCreature", "START, creature index: " + IntToString(n));
#endif
	AActor* oTarget = nullptr;
	TArray<AActor*> arCreatures;

	if (IsFollower(OBJECT_SELF) && nTargetType == AI_TARGET_TYPE_ENEMY)
	{
		// A follower should prefer to keep his selected target in this case
		AActor* oSelectedTarget = GetAttackTarget(OBJECT_SELF);
		if (_AI_IsHostileTargetValid(OBJECT_SELF, oSelectedTarget))
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_Condition_GetNearestVisibleCreature", "Follower - keeping selected target instead of picking nearest");
#endif
			return oSelectedTarget;
		}
	}

	switch (nTargetType)
	{
	case AI_TARGET_TYPE_ALLY:
	{
		// Getting a list of the nearest creatures with the same group, alive and perceived, not including self
		arCreatures = _AI_GetAllies(OBJECT_SELF, nCommandType, nCommandSubType);
		break;
	}
	case AI_TARGET_TYPE_ENEMY:
	{
		// Getting a list of the nearest hostiles, alive and perceived, not including self:
		arCreatures = _AI_GetEnemies(OBJECT_SELF, nCommandType, nCommandSubType, nTargetType);
		break;
	}
	default:
	{
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_Condition_GetNearestVisibleCreature", "INVALID Target Type: " + IntToString(nTargetType));
#endif
		break;
	}
	}


	if (arCreatures.Num() + 1 <= n)
	{
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_Condition_GetNearestVisibleCreature", "Invalid creature index requested: " + IntToString(n));
#endif
		return nullptr;
	}

	return arCreatures[n - 1]; // function asks for 1st enemy - but array starts at 0
}

AActor* _AI_Condition_GetNearestVisibleCreatureByRace(AActor* OBJECT_SELF, int32 nTargetType, int32 nRace, int32 nCommandType, int32 nCommandSubType, int32 nTacticID)
{
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_Condition_GetNearestVisibleCreatureByRace", "START, race: " + IntToString(nRace));
#endif
	TArray<AActor*> arCreatures;
	int32 nSize;
	int32 i;
	AActor* oCurrent;

	if (IsFollower(OBJECT_SELF) && nTargetType == AI_TARGET_TYPE_ENEMY)
	{
		// A follower should prefer to keep his selected target in this case
		AActor* oSelectedTarget = GetAttackTarget(OBJECT_SELF);
		if (_AI_IsHostileTargetValid(OBJECT_SELF, oSelectedTarget))
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_Condition_GetNearestVisibleCreatureByRace", "Follower - keeping selected target instead of picking nearest");
#endif
			return oSelectedTarget;
		}
	}

	switch (nTargetType)
	{
	case AI_TARGET_TYPE_ALLY:
	{
		// Getting a list of the nearest creatures with the same group, alive and perceived, not including self
		arCreatures = _AI_GetAllies(OBJECT_SELF, nCommandType, nCommandSubType);
		break;
	}
	case AI_TARGET_TYPE_ENEMY:
	{
		// Getting a list of the nearest hostiles, alive and perceived, not including self:
		arCreatures = _AI_GetEnemies(OBJECT_SELF, nCommandType, nCommandSubType);
		break;
	}
	default:
	{
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_Condition_GetNearestVisibleCreatureByRace", "INVALID Target Type: " + IntToString(nTargetType));
#endif
		break;
	}
	}

	nSize = arCreatures.Num();

	for (i = 0; i < nSize; i++)
	{
		oCurrent = arCreatures[i];
		if (GetCreatureRacialType(oCurrent) == nRace)
			return oCurrent;
	}

	return nullptr;
}

AActor* _AI_Condition_GetNearestVisibleCreatureByClass(AActor* OBJECT_SELF, int32 nTargetType, int32 nClass, int32 nCommandType, int32 nCommandSubType, int32 nTacticID)
{
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_Condition_GetNearestVisibleCreatureByClass", "START, class:" + IntToString(nClass));
#endif

	TArray<AActor*> arCreatures;
	int32 nSize;
	int32 i;
	AActor* oCurrent;

	if (IsFollower(OBJECT_SELF) && nTargetType == AI_TARGET_TYPE_ENEMY)
	{
		// A follower should prefer to keep his selected target in this case
		AActor* oSelectedTarget = GetAttackTarget(OBJECT_SELF);
		if (_AI_IsHostileTargetValid(OBJECT_SELF, oSelectedTarget))
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_Condition_GetNearestVisibleCreatureByClass", "Follower - keeping selected target instead of picking nearest");
#endif
			return oSelectedTarget;
		}
	}

	switch (nTargetType)
	{
	case AI_TARGET_TYPE_ALLY:
	{
		// Getting a list of the nearest creatures with the same group, alive and perceived, not including self
		arCreatures = _AI_GetAllies(OBJECT_SELF, nCommandType, nCommandSubType);
		break;
	}
	case AI_TARGET_TYPE_ENEMY:
	{
		// Getting a list of the nearest hostiles, alive and perceived, not including self:
		arCreatures = _AI_GetEnemies(OBJECT_SELF, nCommandType, nCommandSubType);
		break;
	}
	default:
	{
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_Condition_GetNearestVisibleCreatureByClass", "INVALID Target Type: " + IntToString(nTargetType));
#endif
		break;
	}
	}

	nSize = arCreatures.Num();

	for (i = 0; i < nSize; i++)
	{
		oCurrent = arCreatures[i];
		if (GetCreatureCoreClass(oCurrent) == nClass)
			return oCurrent;
	}

	return nullptr;
}

AActor* _AI_Condition_GetNearestVisibleCreatureByGender(AActor* OBJECT_SELF, int32 nTargetType, int32 nGender, int32 nCommandType, int32 nCommandSubType, int32 nTacticID)
{
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_Condition_GetNearestVisibleCreatureByGender", "START, gender: " + IntToString(nGender));
#endif

	TArray<AActor*> arCreatures;
	int32 nSize;
	int32 i;
	AActor* oCurrent;

	if (IsFollower(OBJECT_SELF) && nTargetType == AI_TARGET_TYPE_ENEMY)
	{
		// A follower should prefer to keep his selected target in this case
		AActor* oSelectedTarget = GetAttackTarget(OBJECT_SELF);
		if (_AI_IsHostileTargetValid(OBJECT_SELF, oSelectedTarget))
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_Condition_GetNearestVisibleCreatureByGender", "Follower - keeping selected target instead of picking nearest");
#endif
			return oSelectedTarget;
		}
	}

	switch (nTargetType)
	{
	case AI_TARGET_TYPE_ALLY:
	{
		// Getting a list of the nearest creatures with the same group, alive and perceived, not including self
		arCreatures = _AI_GetAllies(OBJECT_SELF, nCommandType, nCommandSubType);
		break;
	}
	case AI_TARGET_TYPE_ENEMY:
	{
		// Getting a list of the nearest hostiles, alive and perceived, not including self:
		arCreatures = _AI_GetEnemies(OBJECT_SELF, nCommandType, nCommandSubType);
		break;
	}
	default:
	{
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_Condition_GetNearestVisibleCreatureByGender", "INVALID Target Type: " + IntToString(nTargetType));
#endif
		break;
	}
	}

	nSize = arCreatures.Num();

	for (i = 0; i < nSize; i++)
	{
		oCurrent = arCreatures[i];
		if (GetCreatureGender(oCurrent) == nGender)
			return oCurrent;
	}

	return nullptr;
}

AActor* _AI_Condition_GetNearestEnemyAttackingPartyMember(AActor* OBJECT_SELF, int32 nCommandType, int32 nCommandSubType, int32 nPartyMemberType, int32 nTacticID)
{
	TArray<AActor*> arCreatures = _AI_GetEnemies(OBJECT_SELF, nCommandType, nCommandSubType);
	AActor* oFollower;
	if (nPartyMemberType == AI_TARGET_TYPE_HERO)
		oFollower = GetPartyLeader();
	else if (nPartyMemberType == AI_TARGET_TYPE_MAIN_CONTROLLED)
		oFollower = GetMainControlled();
	else
		oFollower = GetTacticConditionObject(OBJECT_SELF, nTacticID);
	if (!IsObjectValid(oFollower))
		return nullptr;
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_Condition_GetNearestEnemyAttackingPartyMember", "START, party member: " + GetTag(oFollower));
#endif

	int32 nSize = arCreatures.Num();
	int32 i;
	AActor* oCurrent;
	AActor* oTarget;

	for (i = 0; i < nSize; i++)
	{
		oCurrent = arCreatures[i];
		oTarget = GetAttackTarget(oCurrent);
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_Condition_GetNearestEnemyAttackingPartyMember", "Attack target of:[" + GetTag(oCurrent) +
			"] is: " + GetTag(oTarget));
#endif

		if (oTarget == oFollower)
			return oCurrent;
	}
	return nullptr;
}

AActor* _AI_Condition_GetNearestEnemyWithAnyBuffEffect(AActor* OBJECT_SELF, int32 nCommandType, int32 nCommandSubType, int32 nRangeID, int32 nTacticID, int32 nTargetType)
{
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_Condition_GetNearestEnemyWithAnyBuffEffect", "START, rangeID: " + IntToString(nRangeID));
#endif

	if (IsFollower(OBJECT_SELF))
	{
		// A follower should prefer to keep his selected target in this case
		AActor* oSelectedTarget = GetAttackTarget(OBJECT_SELF);
		if (_AI_IsHostileTargetValid(OBJECT_SELF, oSelectedTarget) && _AI_HasAnyBuffEffect(oSelectedTarget))
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_Condition_GetNearestEnemyWithAnyBuffEffect", "Follower - keeping selected target");
#endif
			return oSelectedTarget;
		}
	}

	TArray<AActor*> arCreatures;

	switch (nTargetType)
	{
	case AI_TARGET_TYPE_ENEMY:
	{
		arCreatures = _AI_GetEnemies(OBJECT_SELF, nCommandType, nCommandSubType, nTargetType);
		break;
	}
	case AI_TARGET_TYPE_MOST_HATED:
	{
		arCreatures[0] = _AI_Condition_GetMostHatedEnemy(OBJECT_SELF, 1, nCommandType, nCommandSubType, nTacticID);
		break;
	}
	}

	int32 nSize = arCreatures.Num();
	int32 i;
	AActor* oCurrent;
	float fDistance;

	for (i = 0; i < nSize; i++)
	{
		oCurrent = arCreatures[i];
		fDistance = GetDistanceBetween(OBJECT_SELF, oCurrent);
		if (nRangeID != AI_RANGE_ID_INVALID && fDistance > _AI_GetRangeFromID(nRangeID))
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_Condition_GetNearestEnemyWithAnyBuffEffect", "This target not valid (not in proper range)");
#endif
			continue;
		}
		if (_AI_HasAnyBuffEffect(oCurrent))
			return oCurrent;
	}
	return nullptr;
}

AActor* _AI_Condition_GetNearestFlipCoverByState(AActor* OBJECT_SELF, int32 nFlipState, int32 nTacticID)
{
	//TODO _AI_Condition_GetNearestFlipCoverByState Placeables to be implemented
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_Condition_GetNearestFlipCoverByState", "START, state: " + IntToString(nFlipState));
#endif
	AActor* oTarget = nullptr;

	/*if(nFlipState != 0)
	{
	Log_Trace_AI(OBJECT_SELF, "_AI_Condition_GetNearestFlipCoverByState", "Invalid state requested for flipcover (currently supporting only Idle state)");
	return nullptr;
	}*/

	//TODO _AI_Condition_GetNearestFlipCoverByState 
	/*
	TArray<AActor*> arPlaceables = GetNearestObject(OBJECT_SELF, OBJECT_TYPE_PLACEABLE, AI_MAX_NEAREST_PLACEABLE);
	int32 nSize = arPlaceables.Num();
	// Return first flip cover placeable in an Idle state, the does not yet have anyone registering a flipcover
	// on it.
	int32 i;
	AActor* oCurrent;
	for (i = 0; i < nSize; i++)
	{
		oCurrent = arPlaceables[i];
		if (GetPlaceableBaseType(oCurrent) != PLACEABLE_TYPE_FLIPCOVER)
			continue;
		if (GetPlaceableState(oCurrent) != nFlipState)
			continue;
		if (oCurrent->PLC_FLIP_COVER_USE_COUNT > 0) // Got users already
			continue;
		oTarget = oCurrent;
		break;
	}*/
	return oTarget;
}

AActor* _AI_Condition_GetEnemyVulnerableToDamage(AActor* OBJECT_SELF, int32 nDamageType, int32 nCommandType, int32 nCommandSubType, int32 nTacticID)
{
	LogError("_AI_IsVulnerableToDamage was CUT so who needs it?");
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_Condition_GetEnemyVulnerableToDamage", "START, damage type: " + IntToString(nDamageType));
#endif

	int32 i;
	AActor* oCurrent;

	if (IsFollower(OBJECT_SELF))
	{
		// A follower should prefer to keep his selected target in this case
		AActor* oSelectedTarget = GetAttackTarget(OBJECT_SELF);
		if (_AI_IsHostileTargetValid(OBJECT_SELF, oSelectedTarget) && _AI_IsVulnerableToDamage(oSelectedTarget, nDamageType))
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_Condition_GetEnemyVulnerableToDamage", "Follower - keeping selected target");
#endif
			return oSelectedTarget;
		}
		else if (_AI_IsHostileTargetValid(OBJECT_SELF, oSelectedTarget))
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_Condition_GetEnemyVulnerableToDamage", "Follower - selected target not fitting condition");
#endif
			return nullptr;
		}
	}

	TArray<AActor*> arCreatures = _AI_GetEnemies(OBJECT_SELF, nCommandType, nCommandSubType, AI_TARGET_TYPE_ENEMY);
	int32 nSize = arCreatures.Num();

	for (i = 0; i < nSize; i++)
	{
		oCurrent = arCreatures[i];
		if (_AI_IsVulnerableToDamage(oCurrent, nDamageType))
			return oCurrent;
	}
	return nullptr;
}

AActor* _AI_Condition_GetAnyTarget(AActor* OBJECT_SELF, int32 nTargetType, int32 nCommandType, int32 nCommandSubType, int32 nTacticID)
{
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_Condition_GetAnyTarget", "START, target type: " + IntToString(nTargetType));
#endif
	AActor* oTarget = nullptr;
	TArray<AActor*> arCreatures;

	if (IsFollower(OBJECT_SELF) && nTargetType == AI_TARGET_TYPE_ENEMY)
	{
		// A follower should prefer to keep his selected target in this case
		AActor* oSelectedTarget = GetAttackTarget(OBJECT_SELF);
		if (_AI_IsHostileTargetValid(OBJECT_SELF, oSelectedTarget))
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_Condition_GetAnyTarget", "Follower - keeping selected target");
#endif
			return oSelectedTarget;
		}
	}

	switch (nTargetType)
	{
	case AI_TARGET_TYPE_SELF:
	{
		// Making sure we're not trying to apply an ability that is already active
		if (OBJECT_SELF, nCommandType == AI_COMMAND_USE_ABILITY && !Ability_IsAbilityActive(OBJECT_SELF, nCommandSubType))
			oTarget = OBJECT_SELF;
		else if (OBJECT_SELF, nCommandType != AI_COMMAND_USE_ABILITY) // not an ability - can use self as target
			oTarget = OBJECT_SELF;
		break;
	}
	case AI_TARGET_TYPE_ALLY:
	{
		arCreatures = _AI_GetAllies(OBJECT_SELF, nCommandType, nCommandSubType);
		oTarget = arCreatures[0];
		break;
	}
	case AI_TARGET_TYPE_MOST_HATED:
	{
		oTarget = _AI_Condition_GetMostHatedEnemy(OBJECT_SELF, 1, nCommandType, nCommandSubType, nTacticID);
		break;
	}
	default: // It can still be a party target
	{
		oTarget = _AI_GetPartyTarget(OBJECT_SELF, nTargetType, nCommandType, nCommandSubType, nTacticID);
		break;
	}
	}

	return oTarget;
}

AActor* _AI_Condition_SelfHasAmmoLevel(AActor* OBJECT_SELF, int32 nAmmoLevel)
{
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_Condition_SelfHasAmmoLevel", "START, ammo level: " + IntToString(nAmmoLevel));
#endif
	AActor* oRet = nullptr;

	int32 nCurrentStackSize = Items_CheckAmmo(OBJECT_SELF);
	int32 nMaxStackSize = Items_CheckMaxAmmo(OBJECT_SELF);
	if (nMaxStackSize == 0)
	{
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_Condition_SelfHasAmmoLevel", "Invalid stack size for ammo: 0");
#endif
		return nullptr;
	}

	//float fStackRatio = fCurrentStackSize / fMaxStackSize * 100.0; // should give a percentage ratio
	//float fCheckRatio;
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_Condition_SelfHasAmmoLevel", "current ammo: " + IntToString(nCurrentStackSize));
#endif


	switch (nAmmoLevel)
	{
	case AI_AMMO_LEVEL_LOW: // valid if between 0 and low ration
	{
		if (nCurrentStackSize >= 0 && nCurrentStackSize <= AI_AMMO_RATIO_LOW)
			oRet = OBJECT_SELF;
		break;
	}
	case AI_AMMO_LEVEL_MEDIUM: // valid if between low and high ratio
	{
		if (nCurrentStackSize > AI_AMMO_RATIO_LOW && nCurrentStackSize < AI_AMMO_RATIO_HIGH)
			oRet = OBJECT_SELF;
		break;
	}
	case AI_AMMO_LEVEL_HIGH: // valid if high and max ratio
	{
		if (nCurrentStackSize >= AI_AMMO_RATIO_HIGH)
			oRet = OBJECT_SELF;
		break;
	}
	}

	return oRet;
}

AActor* _AI_Condition_HasArmorType(AActor* OBJECT_SELF, int32 nTargetType, int32 nArmorType, int32 nCommandType, int32 nCommandSubType)
{
	AActor* oTarget = nullptr;
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_Condition_HasArmorType", "Target type: " + IntToString(nTargetType) + ", Armor Type: " + IntToString(nArmorType));
#endif

	TArray<AActor*> arEnemies;

	switch (nTargetType)
	{
	case AI_TARGET_TYPE_SELF:
	{
		arEnemies[0] = _AI_Condition_GetMostHatedEnemy(OBJECT_SELF, 1, nCommandType, nCommandSubType, -1);
		break;
	}
	case AI_TARGET_TYPE_MOST_HATED:
	{
		arEnemies[0] = _AI_Condition_GetMostHatedEnemy(OBJECT_SELF, 1, nCommandType, nCommandSubType, -1);
		break;
	}
	case AI_TARGET_TYPE_ENEMY:
	{
		arEnemies = _AI_GetEnemies(OBJECT_SELF, nCommandType, nCommandSubType, nTargetType);
		break;
	}
	default:
	{
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_Condition_HasArmorType", "Invalid target type");
#endif
		return nullptr;
	}

	}

	int32 nSize = arEnemies.Num();
	int32 i;
	AActor* oCurrent;
	int32 nEnemyArmorType;
	for (i = 0; i < nSize; i++)
	{
		oCurrent = arEnemies[i];
		nEnemyArmorType = _AI_GetArmorType(oCurrent);
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_Condition_HasArmorType", "Enemy: " + GetTag(oCurrent) + ", Enemy's armor type: " +
			IntToString(nEnemyArmorType) + ", requested armor bit field: " + IntToString(nArmorType));
#endif

		if (nEnemyArmorType & nArmorType)
		{
			if (OBJECT_SELF, nTargetType == AI_TARGET_TYPE_SELF)
				oTarget = OBJECT_SELF; // creature's armor fits in the requested armor bit field (nArmorType)
			else
				oTarget = oCurrent;

			break;
		}
	}

	return oTarget;
}

AActor* _AI_Condition_MostEnemiesHaveArmorType(AActor* OBJECT_SELF, int32 nArmorType)
{
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_Condition_MostEnemiesHaveArmorType", "START, looking for armor type: " + IntToString(nArmorType));
#endif
	int32 nSize = GetThreatTableSize(OBJECT_SELF);
	int32 i;
	int32 nEnemiesWithArmorType = 0;
	AActor* oCurrentEnemy;
	int32 nEnemyArmorType;
	AActor* oRet = nullptr;

	for (i = 0; i < nSize; i++)
	{
		oCurrentEnemy = GetThreatEnemy(OBJECT_SELF, i);
		nEnemyArmorType = _AI_GetArmorType(oCurrentEnemy);
		if (nEnemyArmorType & nArmorType)
			nEnemiesWithArmorType++;
	}
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_Condition_MostEnemiesHaveArmorType", "number of enemies with armor type: " + IntToString(nEnemiesWithArmorType) +
		", Total number of enemies: " + IntToString(nSize));
#endif

	if (nEnemiesWithArmorType >= nSize / 2)
		oRet = OBJECT_SELF;

	return oRet;
}

AActor* _AI_Condition_AllEnemiesHaveArmorType(AActor* OBJECT_SELF, int32 nArmorType)
{
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_Condition_AllEnemiesHaveArmorType", "START, looking for armor type: " + IntToString(nArmorType));
#endif
	int32 nSize = GetThreatTableSize(OBJECT_SELF);
	int32 i;
	AActor* oCurrentEnemy;
	int32 nEnemyArmorType;
	AActor* oRet = OBJECT_SELF;

	for (i = 0; i < nSize; i++)
	{
		oCurrentEnemy = GetThreatEnemy(OBJECT_SELF, i);
		nEnemyArmorType = _AI_GetArmorType(oCurrentEnemy);
		if (!(nEnemyArmorType & nArmorType))
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_Condition_AllEnemiesHaveArmorType", "found one enemy with wrong armor type: " + GetTag(oCurrentEnemy));
#endif
			oRet = nullptr;
			break;
		}
	}

	return oRet;
}

AActor* _AI_Condition_TargetHasRank(AActor* OBJECT_SELF, int32 nTargetType, int32 nTargetRank, int32 nTacticID, int32 nTacticCommand, int32 nTacticSubCommand)
{
	AActor* oRet = nullptr;
	TArray<AActor*> arEnemies = _AI_GetEnemies(OBJECT_SELF, nTacticCommand, nTacticSubCommand, nTargetType);
	int32 nEnemyRank;
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_Condition_TargetHasRank", "START, target rank: " + IntToString(nTargetRank));
#endif

	AActor* oEnemy;
	int32 i;
	int32 nSize = arEnemies.Num();
	for (i = 0; i < nSize; i++)
	{
		oEnemy = arEnemies[i];
		nEnemyRank = GetCreatureRank(oEnemy);

		if (nTargetRank > 0) // looking for specific rank
		{
			if (nEnemyRank == nTargetRank)
				oRet = oEnemy;
		}
		else if (nTargetRank < 0) // looking for a range of ranks
		{
			switch (nTargetRank)
			{
			case AI_RANK_RANGE_BOSS_OR_HIGHER:
			{
				if (nEnemyRank == CREATURE_RANK_BOSS || nEnemyRank == CREATURE_RANK_ELITE_BOSS)
					oRet = oEnemy;
				break;
			}
			case AI_RANK_RANGE_ELITE_OR_HIGER:
			{
				if (nEnemyRank == CREATURE_RANK_LIEUTENANT || nEnemyRank == CREATURE_RANK_BOSS || nEnemyRank == CREATURE_RANK_ELITE_BOSS)
					oRet = oEnemy;
				break;
			}
			case AI_RANK_RANGE_ELITE_OR_LOWER:
			{
				if (nEnemyRank == CREATURE_RANK_LIEUTENANT || nEnemyRank == CREATURE_RANK_NORMAL || nEnemyRank == CREATURE_RANK_CRITTER ||
					nEnemyRank == CREATURE_RANK_ONE_HIT_KILL || nEnemyRank == CREATURE_RANK_WEAK_NORMAL)
					oRet = oEnemy;
				break;
			}
			case AI_RANK_RANGE_NORMAL_OR_HIGHER:
			{
				if (nEnemyRank == CREATURE_RANK_NORMAL || nEnemyRank == CREATURE_RANK_LIEUTENANT || nEnemyRank == CREATURE_RANK_BOSS || nEnemyRank == CREATURE_RANK_ELITE_BOSS)
					oRet = oEnemy;
				break;
			}
			case AI_RANK_RANGE_NORMAL_OR_LOWER:
			{
				if (nEnemyRank == CREATURE_RANK_NORMAL || nEnemyRank == CREATURE_RANK_CRITTER ||
					nEnemyRank == CREATURE_RANK_ONE_HIT_KILL || nEnemyRank == CREATURE_RANK_WEAK_NORMAL)
					oRet = oEnemy;
				break;
			}
			}
		}
	}

	return oRet;
}

AActor*  _AI_Condition_UsingAttackType(AActor* OBJECT_SELF, int32 nTargetType, int32 nAttackType, int32 nCommandType, int32 nCommandSubType)
{
	AActor* oTarget = nullptr;

#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_Condition_UsingAttackType", "START, attack type: " + IntToString(nAttackType));
#endif
	TArray<AActor*> arTargets;

	switch (nTargetType)
	{
	case AI_TARGET_TYPE_SELF:
	{
		arTargets[0] = _AI_Condition_GetMostHatedEnemy(OBJECT_SELF, 1, nCommandType, nCommandSubType, -1);
		break;
	}
	case AI_TARGET_TYPE_MOST_HATED:
	{
		arTargets[0] = _AI_Condition_GetMostHatedEnemy(OBJECT_SELF, 1, nCommandType, nCommandSubType, -1);
		break;
	}
	case AI_TARGET_TYPE_ENEMY:
	{
		arTargets = _AI_GetEnemies(OBJECT_SELF, nCommandType, nCommandSubType, nTargetType);
		break;
	}
	case AI_TARGET_TYPE_ALLY:
	{
		arTargets = _AI_GetAllies(OBJECT_SELF, nCommandType, nCommandSubType);
		break;
	}
	default:
	{
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_Condition_UsingAttackType", "Invalid target type");
#endif
		return nullptr;
	}

	}

	int32 nSize = arTargets.Num();
	int32 i;
	AActor* oCurrent;
	int32 nEnemyAttackType;
	for (i = 0; i < nSize; i++)
	{
		oCurrent = arTargets[i];
		nEnemyAttackType = _AI_GetAttackType(oCurrent);
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_Condition_UsingAttackType", "Target: " + GetTag(oCurrent) + ", Target's attack type: " +
			IntToString(nEnemyAttackType) + ", requested attack bit field: " + IntToString(nAttackType));
#endif

		if (nEnemyAttackType & nAttackType)
		{
			if (OBJECT_SELF, nTargetType == AI_TARGET_TYPE_SELF)
				oTarget = OBJECT_SELF; // creature's armor fits in the requested armor bit field (nArmorType)
			else
				oTarget = oCurrent;

			break;
		}
	}

	return oTarget;
}

AActor* _AI_Condition_MostEnemiesUsingAttackType(AActor* OBJECT_SELF, int32 nAttackType)
{
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_Condition_MostEnemiesUsingAttackType", "START, looking for attack type: " + IntToString(nAttackType));
#endif
	TArray<AActor*> arEnemies = _AI_GetEnemies(OBJECT_SELF, -1, -1, AI_TARGET_TYPE_ENEMY);
	int32 nSize = arEnemies.Num();
	int32 i;
	int32 nEnemiesWithAttackType = 0;
	AActor* oCurrentEnemy;
	int32 nEnemyAttackType;
	AActor* oRet = nullptr;

	for (i = 0; i < nSize; i++)
	{
		oCurrentEnemy = arEnemies[i];
		nEnemyAttackType = _AI_GetAttackType(oCurrentEnemy);
		if (nEnemyAttackType & nAttackType)
			nEnemiesWithAttackType++;
	}
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_Condition_MostEnemiesUsingAttackType", "number of enemies with attack type: " + IntToString(nEnemiesWithAttackType) +
		", Total number of enemies: " + IntToString(nSize));
#endif

	if (nEnemiesWithAttackType > nSize - nEnemiesWithAttackType)
		oRet = OBJECT_SELF;

	return oRet;
}

AActor* _AI_Condition_AllEnemiesUsingAttackType(AActor* OBJECT_SELF, int32 nAttackType)
{
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_Condition_AllEnemiesUsingAttackType", "START, looking for attack type: " + IntToString(nAttackType));
#endif
	int32 nSize = GetThreatTableSize(OBJECT_SELF);
	int32 i;
	AActor* oCurrentEnemy;
	int32 nEnemyAttackType;
	AActor* oRet = OBJECT_SELF;

	for (i = 0; i < nSize; i++)
	{
		oCurrentEnemy = GetThreatEnemy(OBJECT_SELF, i);
		nEnemyAttackType = _AI_GetAttackType(oCurrentEnemy);
		if (!(nEnemyAttackType & nAttackType))
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_Condition_AllEnemiesUsingAttackType", "found one enemy with wrong attack type: " + GetTag(oCurrentEnemy));
#endif
			oRet = nullptr;
			break;
		}
	}

	return oRet;
}

AActor* _AI_Condition_AtLeastXEnemiesAreAlive(AActor* OBJECT_SELF, int32 nTargetType, int32 nNum)
{
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_Condition_AtLeastXEnemiesAreAlive", "START, X=: " + IntToString(nNum));
#endif
	TArray<AActor*> arEnemies = _AI_GetEnemies(OBJECT_SELF, -1, -1, nTargetType);

	AActor* oTarget = nullptr;
	switch (nTargetType)
	{
	case AI_TARGET_TYPE_SELF: oTarget = OBJECT_SELF; break;
	case AI_TARGET_TYPE_ENEMY: oTarget = arEnemies[0]; break;
	case AI_TARGET_TYPE_MOST_HATED: oTarget = _AI_Condition_GetMostHatedEnemy(OBJECT_SELF, 1, -1, -1, -1); break;
	}

	int32 nCount = 0;
	int32 nEnemiesNum = GetThreatTableSize(OBJECT_SELF);
	if (nEnemiesNum < nNum)
		oTarget = nullptr;
	return oTarget;
}

AActor* _AI_Condition_AtLeastXCreaturesAreDead(AActor* OBJECT_SELF, int32 nTargetType, int32 nNum, int32 nTacticCommand, int32 nTacticSubCommand)
{
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_Condition_AtLeastXCreaturesAreDead", "START, X=: " + IntToString(nNum));
#endif

	TArray<AActor*> arEnemies = _AI_GetEnemies(OBJECT_SELF, -1, -1, nTargetType);

	AActor* oTarget = nullptr;
	switch (nTargetType)
	{
	case AI_TARGET_TYPE_SELF: oTarget = OBJECT_SELF; break;
	case AI_TARGET_TYPE_ENEMY: oTarget = arEnemies[0]; break;
	case AI_TARGET_TYPE_MOST_HATED: oTarget = _AI_Condition_GetMostHatedEnemy(OBJECT_SELF, 1, -1, -1, -1); break;
	}

	TArray<AActor*> arCorpses = GetNearestObject(OBJECT_SELF,
		OBJECT_TYPE_CREATURE,
		AI_MAX_CREATURES_NEAREST);

	float fRange = 0.f;
	if (nTacticCommand == AI_COMMAND_USE_ABILITY &&
		(nTacticSubCommand == ABILITY_TALENT_DEVOUR || nTacticSubCommand == ABILITY_SPELL_ANIMATE_DEAD))
	{
		int32 nRangeID = GetM2DAInt(TABLE_ABILITIES_SPELLS, "range", nTacticSubCommand);
		fRange = GetM2DAFloat(TABLE_RANGES, "PrimaryRange", nRangeID) * SCALE_MULTIPLIER;
	}

	int32 nSize = arCorpses.Num();

#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_Condition_AtLeastXCreaturesAreDead", "Total corpses found: " + IntToString(nSize) +
		", looking for range: " + FloatToString(fRange));
#endif

	int32 i;
	AActor* oCurrent;
	int32 nCorpseCount = 0;

	for (i = 0; i < nSize; i++)
	{
		oCurrent = arCorpses[i];
		if (IsDead(oCurrent))
		{
			if (fRange > 0.0) // range check
			{
				float fDistance = GetDistanceBetween(OBJECT_SELF, oCurrent);
				if (fDistance <= fRange)
					nCorpseCount++;
			}
			else // no need for range check
				nCorpseCount++;
		}
	}

	if (nCorpseCount < nNum)
		oTarget = nullptr;

	return oTarget;
}

AActor* _AI_Condition_AtLeastXAlliesAreAlive(AActor* OBJECT_SELF, int32 nTargetType, int32 nNum, int32 nParam2)
{
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_Condition_AtLeastXAlliesAreAlive", "START, X=: " + IntToString(nNum));
#endif

	TArray<AActor*> arEnemies = _AI_GetEnemies(OBJECT_SELF, -1, -1, nTargetType);

	AActor* oTarget = nullptr;
	switch (nTargetType)
	{
	case AI_TARGET_TYPE_SELF: oTarget = OBJECT_SELF; break;
	case AI_TARGET_TYPE_ENEMY: oTarget = arEnemies[0]; break;
	case AI_TARGET_TYPE_MOST_HATED: oTarget = _AI_Condition_GetMostHatedEnemy(OBJECT_SELF, 1, -1, -1, -1); break;
	}

	int32 nCount = 0;
	TArray<AActor*> arAllies = _AI_GetAllies(OBJECT_SELF, COMMAND_TYPE_INVALID, -1);
	int32 nAlliesNum = arAllies.Num();

	if (nParam2 == 0) // normal check of "at least alive"
	{
		if (nAlliesNum < nNum)
			oTarget = nullptr;
	}
	else if (nParam2 == 1) // special check if at least wounded 50%
	{
		int32 i;
		AActor* oCurrent;
		for (i = 0; i < nAlliesNum; i++)
		{
			oCurrent = arAllies[i];
			if (_AI_HasStatLevel(OBJECT_SELF, oCurrent, AI_STAT_TYPE_HP, -50))
				nCount++;
		}
		if (nCount < nNum)
			oTarget = nullptr;
	}
	else if (nParam2 == 2) // special check if at least has curable effect
	{
		int32 i;
		AActor* oCurrent;
		for (i = 0; i < nAlliesNum; i++)
		{
			oCurrent = arAllies[i];
			if (_AI_IsTargetValidForBeneficialAbility(oCurrent, ABILITY_SPELL_CURE))
				nCount++;
		}
		if (nCount < nNum)
			oTarget = nullptr;
	}
	return oTarget;
}

AActor* _AI_Condition_GetTargetUsingRangedWeaponsAtRange(AActor* OBJECT_SELF, int32 nTargetType, int32 nRange, int32 nCommandType, int32 nCommandSubType)
{
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_Condition_GetTargetUsingRangedWeaponsAtRange", "START, looking for range: " + IntToString(nRange));
#endif

	TArray<AActor*> arEnemies;
	if (OBJECT_SELF, nTargetType == AI_TARGET_TYPE_ENEMY)
		arEnemies = _AI_GetEnemies(OBJECT_SELF, nCommandType, nCommandSubType, nTargetType);
	else // most hated
		arEnemies[0] = _AI_Condition_GetMostHatedEnemy(OBJECT_SELF, 1, nCommandType, nCommandSubType, -1);

	int32 nSize = arEnemies.Num();
	int32 i;
	AActor* oCurrent;
	float fDistance;

	for (i = 0; i < nSize; i++)
	{
		oCurrent = arEnemies[i];
		fDistance = GetDistanceBetween(OBJECT_SELF, oCurrent);

#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_Condition_GetTargetUsingRangedWeaponsAtRange", "Current Target: " + GetTag(oCurrent) + ", Distance: " + FloatToString(fDistance) + ", attack type: " +
			IntToString(_AI_GetAttackType(oCurrent)));
#endif

		if (_AI_GetAttackType(oCurrent) == AI_ATTACK_TYPE_RANGED || _AI_GetAttackType(oCurrent) == AI_ATTACK_TYPE_MAGIC)
		{

#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_Condition_GetTargetUsingRangedWeaponsAtRange", "Target using ranged or magic! - checking range");
#endif

			// Target is ranged or magic
			switch (nRange)
			{
			case AI_RANGE_ID_SHORT:
			{
#ifdef DEBUG
				Log_Trace_AI(OBJECT_SELF, "_AI_Condition_GetTargetUsingRangedWeaponsAtRange", "Checking for range lower than: " + FloatToString(AI_RANGE_SHORT * SCALE_MULTIPLIER));
#endif
				if (fDistance <= AI_RANGE_SHORT * SCALE_MULTIPLIER)
					return oCurrent;
				break;
			}
			case AI_RANGE_ID_MEDIUM:
			{
#ifdef DEBUG
				Log_Trace_AI(OBJECT_SELF, "_AI_Condition_GetTargetUsingRangedWeaponsAtRange", "Checking for range between: " + FloatToString(AI_RANGE_SHORT * SCALE_MULTIPLIER) + " and " + FloatToString(AI_RANGE_MEDIUM * SCALE_MULTIPLIER));
#endif
				if (fDistance > AI_RANGE_SHORT * SCALE_MULTIPLIER && fDistance <= AI_RANGE_MEDIUM * SCALE_MULTIPLIER)
					return oCurrent;
				break;
			}
			case AI_RANGE_ID_LONG:
			{
#ifdef DEBUG
				Log_Trace_AI(OBJECT_SELF, "_AI_Condition_GetTargetUsingRangedWeaponsAtRange", "Checking for range between: " + FloatToString(AI_RANGE_MEDIUM * SCALE_MULTIPLIER) + " and " + FloatToString(AI_RANGE_LONG * SCALE_MULTIPLIER));
#endif
				if (fDistance > AI_RANGE_MEDIUM * SCALE_MULTIPLIER && fDistance <= AI_RANGE_LONG * SCALE_MULTIPLIER)
					return oCurrent;
				break;
			}

			}
		}
	}

	return nullptr;
}

AActor* _AI_Condition_GetTargetAtRange(AActor* OBJECT_SELF, int32 nTargetType, int32 nRange, int32 nCommandType, int32 nCommandSubType)
{
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_Condition_GetTargetAtRange", "START, looking for range: " + IntToString(nRange));
#endif

	float fDistance = 0.f;

	TArray<AActor*> arTargets;
	if (OBJECT_SELF, nTargetType == AI_TARGET_TYPE_ALLY)
		arTargets = _AI_GetAllies(OBJECT_SELF, nCommandType, nCommandSubType);
	else if (OBJECT_SELF, nTargetType == AI_TARGET_TYPE_ENEMY)
		arTargets = _AI_GetEnemies(OBJECT_SELF, nCommandType, nCommandSubType, nTargetType);
	else // most hated OR self
		arTargets[0] = _AI_Condition_GetMostHatedEnemy(OBJECT_SELF, 1, nCommandType, nCommandSubType, -1);

	int32 nSize = arTargets.Num();
	int32 i = 0;
	AActor* oCurrent = nullptr;
	AActor* oTarget = nullptr;


	for (i = 0; i < nSize && oTarget == nullptr; i++)
	{
		oCurrent = arTargets[i];
		fDistance = GetDistanceBetween(OBJECT_SELF, oCurrent);

#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_Condition_GetTargetAtRange", "Current Target: " + GetTag(oCurrent) + ", Distance: " + FloatToString(fDistance));
#endif

		switch (nRange)
		{
		case AI_RANGE_ID_SHORT:
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_Condition_GetTargetAtRange", "Checking for range lower than: " + FloatToString(AI_RANGE_SHORT * SCALE_MULTIPLIER));
#endif
			if (fDistance <= AI_RANGE_SHORT * SCALE_MULTIPLIER)
				oTarget = oCurrent;
			break;
		}
		case AI_RANGE_ID_MEDIUM:
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_Condition_GetTargetAtRange", "Checking for range between: " + FloatToString(AI_RANGE_SHORT * SCALE_MULTIPLIER) + " and " + FloatToString(AI_RANGE_MEDIUM * SCALE_MULTIPLIER));
#endif
			if (fDistance > AI_RANGE_SHORT * SCALE_MULTIPLIER && fDistance <= AI_RANGE_MEDIUM * SCALE_MULTIPLIER)
				oTarget = oCurrent;
			break;
		}
		case AI_RANGE_ID_LONG:
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_Condition_GetTargetAtRange", "Checking for range between: " + FloatToString(AI_RANGE_MEDIUM * SCALE_MULTIPLIER) + " and " + FloatToString(AI_RANGE_LONG * SCALE_MULTIPLIER));
#endif
			if (fDistance > AI_RANGE_MEDIUM * SCALE_MULTIPLIER && fDistance <= AI_RANGE_LONG * SCALE_MULTIPLIER)
				oTarget = oCurrent;
			break;
		}

		}

	}

	if (OBJECT_SELF, nTargetType == AI_TARGET_TYPE_SELF && IsObjectValid(oTarget))
		oTarget = OBJECT_SELF;

	if (IsFollower(OBJECT_SELF) && nTargetType == AI_TARGET_TYPE_ENEMY)
	{
		AActor* oSelectedTarget = nullptr;
		if (_AI_IsHostileTargetValid(OBJECT_SELF, oSelectedTarget) && oSelectedTarget != oTarget)
			oTarget = nullptr; // for followers this is valid only if they current target is in the specified range
	}

	return oTarget;
}

AActor* _AI_Condition_GetTargetAtFlankLocation(AActor* OBJECT_SELF, int32 nFlankType, int32 nTargetType)
{
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_Condition_GetTargetAtFlankLocation", "START, flank type: " + IntToString(nFlankType));
#endif
	float fAngleStart = 0.f;
	float fAngleEnd = 0.f;
	AActor* oTarget = nullptr;
	float fAbilityFacing = 0.f;
	int32 nMeleeRing = 0; // for creatures with multiple rings (high dragon)
	int32 nApp = GetAppearanceType(OBJECT_SELF);
	float fRange = 0.0;
	int32 bCheckShape = FALSE_;
	float fMinRange = 0.0;

	switch (nFlankType)
	{
	case AI_FLANK_DRAGON_FR:
	case AI_FLANK_RIGHT:
	{
		if (nApp == APR_TYPE_BROODMOTHER)
		{
			fAngleStart = ABILITY_FLANK_FACING_RIGHT - ABILITY_FLANK_SIZE_SIDE / 2;
			fAngleEnd = ABILITY_FLANK_FACING_RIGHT + ABILITY_FLANK_SIZE_SIDE / 2;
			fAbilityFacing = ABILITY_FLANK_FACING_RIGHT;
		}
		else
		{
			float fAttackFR = GetM2DAFloat(TABLE_APPEARANCE, "AttackFR", nApp);
			float fAttackFRWidth = GetM2DAFloat(TABLE_APPEARANCE, "AttackFRWidth", nApp);
			fAngleStart = fAttackFR - fAttackFRWidth / 2;
			fAngleEnd = fAttackFR + fAttackFRWidth / 2;
			fAbilityFacing = fAttackFR;
			nMeleeRing = GetM2DAInt(TABLE_APPEARANCE, "AttackFRRing", nApp);
		}
		break;
	}
	case AI_FLANK_DRAGON_FL:
	case AI_FLANK_LEFT:
	{
		if (nApp == APR_TYPE_BROODMOTHER)
		{
			fAngleStart = ABILITY_FLANK_FACING_LEFT - ABILITY_FLANK_SIZE_SIDE / 2;
			fAngleEnd = ABILITY_FLANK_FACING_LEFT + ABILITY_FLANK_SIZE_SIDE / 2;
			fAbilityFacing = ABILITY_FLANK_FACING_LEFT;
		}
		else
		{
			float fAttackFL = GetM2DAFloat(TABLE_APPEARANCE, "AttackFL", nApp);
			float fAttackFLWidth = GetM2DAFloat(TABLE_APPEARANCE, "AttackFLWidth", nApp);
			fAngleStart = fAttackFL - fAttackFLWidth / 2;
			fAngleEnd = fAttackFL + fAttackFLWidth / 2;
			fAbilityFacing = fAttackFL;
			nMeleeRing = GetM2DAInt(TABLE_APPEARANCE, "AttackFLRing", nApp);
		}
		break;
	}
	case AI_FLANK_DRAGON_BR:
	case AI_FLANK_BRIGHT:
	{
		float fAttackBR = GetM2DAFloat(TABLE_APPEARANCE, "AttackBR", nApp);
		float fAttackBRWidth = GetM2DAFloat(TABLE_APPEARANCE, "AttackBRWidth", nApp);
		fAngleStart = fAttackBR - fAttackBRWidth / 2;
		fAngleEnd = fAttackBR + fAttackBRWidth / 2;
		fAbilityFacing = fAttackBR;
		nMeleeRing = GetM2DAInt(TABLE_APPEARANCE, "AttackBRRing", nApp);
		break;
	}
	case AI_FLANK_DRAGON_BL:
	case AI_FLANK_BLEFT:
	{
		float fAttackBL = GetM2DAFloat(TABLE_APPEARANCE, "AttackBL", nApp);
		float fAttackBLWidth = GetM2DAFloat(TABLE_APPEARANCE, "AttackBLWidth", nApp);
		fAngleStart = fAttackBL - fAttackBLWidth / 2;
		fAngleEnd = fAttackBL + fAttackBLWidth / 2;
		fAbilityFacing = fAttackBL;
		nMeleeRing = GetM2DAInt(TABLE_APPEARANCE, "AttackBLRing", nApp);
		break;
	}
	case AI_FLANK_LARGE_LEFT:
	{
		fAngleStart = ABILITY_FLANK_FACING_LARGE_LEFT - ABILITY_FLANK_SIZE_LARGE_SIDE / 2;
		fAngleEnd = ABILITY_FLANK_FACING_LARGE_LEFT + ABILITY_FLANK_SIZE_LARGE_SIDE / 2;
		fAbilityFacing = ABILITY_FLANK_FACING_LARGE_LEFT;
		break;
	}
	case AI_FLANK_LARGE_RIGHT:
	{
		fAngleStart = ABILITY_FLANK_FACING_LARGE_RIGHT - ABILITY_FLANK_SIZE_LARGE_SIDE / 2;
		fAngleEnd = ABILITY_FLANK_FACING_LARGE_RIGHT + ABILITY_FLANK_SIZE_LARGE_SIDE / 2;
		fAbilityFacing = ABILITY_FLANK_FACING_LARGE_RIGHT;
		break;
	}
	case AI_FLANK_FRONT:
	{
		fAngleStart = ABILITY_FLANK_FACING_FRONT - ABILITY_FLANK_SIZE_FRONT / 2;
		fAngleEnd = ABILITY_FLANK_FACING_FRONT + ABILITY_FLANK_SIZE_FRONT / 2;
		fAbilityFacing = ABILITY_FLANK_FACING_FRONT;
		break;
	}
	case AI_FLANK_BRIGHT2:
	{
		fAngleStart = ABILITY_FLANK_FACING_BACK_RIGHT2 - ABILITY_FLANK_SIZE_BACK2 / 2;
		fAngleEnd = ABILITY_FLANK_FACING_BACK_RIGHT2 + ABILITY_FLANK_SIZE_BACK2 / 2;
		fAbilityFacing = ABILITY_FLANK_FACING_BACK_RIGHT2;
		break;
	}
	case AI_FLANK_BLEFT2:
	{
		fAngleStart = ABILITY_FLANK_FACING_BACK_LEFT2 - ABILITY_FLANK_SIZE_BACK2 / 2;
		fAngleEnd = ABILITY_FLANK_FACING_BACK_LEFT2 + ABILITY_FLANK_SIZE_BACK2 / 2;
		fAbilityFacing = ABILITY_FLANK_FACING_BACK_LEFT2;
		break;
	}
	case AI_FLANK_DRAGON_BACK:
	case AI_FLANK_DRAGON_BACK_2:
	case AI_FLANK_DRAGON_BACK_3:
	{
		fAngleStart = HIGH_TAIL_FLAP_ANGLE_START;
		fAngleEnd = HIGH_TAIL_FLAP_ANGLE_END;
		fAbilityFacing = GetFacing(OBJECT_SELF) - HIGH_TAIL_FLAP_FACING;
		fRange = HIGH_TAIL_FLAP_RANGE;
		bCheckShape = TRUE_;
		fMinRange = HIGH_TAIL_MIN_RANGE;
		break;
	}
	case AI_FLANK_DRAGON_FRONT_FAR:
	case AI_FLANK_DRAGON_FRONT_FAR_2:
	case AI_FLANK_DRAGON_FRONT_FAR_3:
	{
		fAngleStart = HIGH_BREATH_ANGLE_START;
		fAngleEnd = HIGH_BREATH_ANGLE_END;
		fAbilityFacing = GetFacing(OBJECT_SELF) - HIGH_BREATH_FACING;
		fRange = HIGH_BREATH_RANGE;
		fMinRange = HIGH_BREATH_MIN_RANGE;
		bCheckShape = TRUE_;
		break;
	}
	case AI_FLANK_DRAGON_FRONT_CLOSE:
	case AI_FLANK_DRAGON_FRONT_CLOSE_2:
	case AI_FLANK_DRAGON_FRONT_CLOSE_3:
	{
		fAngleStart = HIGH_SWEEP_ANGLE_START;
		fAngleEnd = HIGH_SWEEP_ANGLE_END;
		fAbilityFacing = GetFacing(OBJECT_SELF) - HIGH_SWEEP_FACING;
		//fAbilityFacing = HIGH_SWEEP_FACING;
		fRange = HIGH_SWEEP_RANGE;
		bCheckShape = TRUE_;
		break;
	}


	}

	if (fRange == 0.0)
	{
		switch (nApp)
		{
		case APR_TYPE_OGRE: fRange = MONSTER_MELEE_RANGE_OGRE; break;
		case APR_TYPE_BROODMOTHER: fRange = MONSTER_MELEE_RANGE_BROODMOTHER; break;
		case APP_TYPE_ARCHDEMON: fRange = MONSTER_MELEE_RANGE_HIGHDRAGON; break;
		case APP_TYPE_HIGHDRAGON: fRange = MONSTER_MELEE_RANGE_HIGHDRAGON; break;
		case APP_TYPE_WILD_SYLVAN: fRange = MONSTER_MELEE_RANGE_WILD_SYLVAN; break;
		case APP_TYPE_DRAGON: fRange = MONSTER_MELEE_RANGE_DRAGON; break;
		case APP_TYPE_DRAKE: fRange = MONSTER_MELEE_RANGE_DRAGON; break;
		case APP_TYPE_PRIDE_DEMON: fRange = MONSTER_MELEE_RANGE_PRIDE_DEMON; break;
		}
	}
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_Condition_GetTargetAtFlankLocation", "ability facing: " + FloatToString(fAbilityFacing));
#endif

	//FVector lConeLocation = Location(GetArea(OBJECT_SELF), GetPosition(OBJECT_SELF), fAbilityFacing);
	TArray<AActor*> arEnemies;
	if (bCheckShape)
	{
		arEnemies = GetCreaturesInCone(OBJECT_SELF, TRUE_);
		//arEnemies = GetObjectsInShape(OBJECT_TYPE_CREATURE, SHAPE_SPHERE, lConeLocation, fRange);
		//arEnemies = FilterObjectsInShape(arEnemies, OBJECT_TYPE_CREATURE, SHAPE_CONE, lConeLocation, fAngleEnd - fAngleStart, fRange, 0.0, TRUE_);
	}
	else
	{
		if (fAngleEnd == 360.0)
			fAngleEnd = 359.9;
		arEnemies = GetCreaturesInMeleeRing(OBJECT_SELF, fAngleStart, fAngleEnd, TRUE_, nMeleeRing);
	}

	// Main filter - no need to check anything if not surrounded by enough enemies

	int32 nSize = arEnemies.Num();
#ifdef DEBUG
	if (nMeleeRing != 0)
		Log_Trace_AI(OBJECT_SELF, "_AI_Condition_GetTargetAtFlankLocation", "Melee Ring: " + IntToString(nMeleeRing));

	Log_Trace_AI(OBJECT_SELF, "_AI_Condition_GetTargetAtFlankLocation", "Looking between angle: " + IntToString(FloatToInt(fAngleStart)) +
		", and: " + IntToString(FloatToInt(fAngleEnd)) + ", number of creatures between angles: " + IntToString(nSize));
#endif
	if (nSize == 1 && arEnemies[0] == OBJECT_SELF)
	{
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_Condition_GetTargetAtFlankLocation", "ERROR: found self in target AOE", LOG_SEVERITY_CRITICAL);
#endif
		return nullptr;
	}

	// filter min range if needed
	if (fMinRange > 0.0)
	{
		int32 i;
		AActor* oCurrent;
		int32 nNewSize = 0;
		float fDistance;
		for (i = 0; i < nSize; i++)
		{
			oCurrent = arEnemies[i];
			fDistance = GetDistanceBetween(OBJECT_SELF, oCurrent);
			if (fDistance > fMinRange)
				nNewSize++;
		}
		nSize = nNewSize;
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_Condition_GetTargetAtFlankLocation", "Ability has min range. Num of targets after filter: " + IntToString(nSize));
#endif

	}

	int32 nMinTargets = 1;
	if (nFlankType == AI_FLANK_DRAGON_BACK_2 || nFlankType == AI_FLANK_DRAGON_FRONT_CLOSE_2 || nFlankType == AI_FLANK_DRAGON_FRONT_FAR_2)
		nMinTargets = 2;
	else if (nFlankType == AI_FLANK_DRAGON_BACK_3 || nFlankType == AI_FLANK_DRAGON_FRONT_CLOSE_3 || nFlankType == AI_FLANK_DRAGON_FRONT_FAR_3)
		nMinTargets = 2;

	if (nSize >= nMinTargets && nTargetType == AI_TARGET_TYPE_SELF)
		oTarget = OBJECT_SELF;
	else if (OBJECT_SELF, nTargetType == AI_TARGET_TYPE_ENEMY)
		oTarget = arEnemies[0];
	return oTarget;
}

AActor* _AI_Condition_SurroundedByAtLeastXEnemies(AActor* OBJECT_SELF, int32 nTacticCommand, int32 nTacticSubCommand, int32 nNumOfTargets, int32 nTacticID)
{
	AActor* oTarget = nullptr;
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_Condition_SurroundedByAtLeastXEnemies", "START, Min num of enemies: " + IntToString(nNumOfTargets));
#endif

	float fStart = 0.0;
	float fEnd = 359.0;

	if (nTacticCommand == AI_COMMAND_USE_ABILITY && nTacticSubCommand == ABILITY_TALENT_DUAL_WEAPON_SWEEP)
	{
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_Condition_SurroundedByAtLeastXEnemies", "Using a SWEEP talent - narrowing angles");
#endif
		fStart = 0.0;
		fEnd = 180.0;
	}
	else if (nTacticCommand == AI_COMMAND_USE_ABILITY && nTacticSubCommand == MONSTER_DRAGON_BREATH)
	{
		fStart = BREATH_ANGLE_START;
		fEnd = BREATH_ANGLE_END;
	}
	int32 nApp = GetAppearanceType(OBJECT_SELF);
	TArray<AActor*> arEnemies;
	//TODO _AI_Condition_SurroundedByAtLeastXEnemies for Dragon and Arcane Horror
	/*if (nApp == APP_TYPE_ARCHDEMON || nApp == APP_TYPE_HIGHDRAGON)
	{
	arEnemies = GetObjectsInShape(OBJECT_TYPE_CREATURE, SHAPE_SPHERE, GetLocation(OBJECT_SELF), MONSTER_MELEE_RANGE_HIGHDRAGON);
	arEnemies = FilterObjectsInShape(arEnemies, OBJECT_TYPE_CREATURE, SHAPE_SPHERE, GetLocation(OBJECT_SELF), MONSTER_MELEE_RANGE_HIGHDRAGON, 0.0, 0.0, TRUE_);
	}
	else if (nTacticCommand == AI_COMMAND_USE_ABILITY && nTacticSubCommand == ABILITY_TALENT_MONSTER_ARCANEHORROR_AOE)
	{
	arEnemies = GetObjectsInShape(OBJECT_TYPE_CREATURE, SHAPE_SPHERE, GetLocation(OBJECT_SELF), ARCANEHORROR_AOE_RADIUS);
	arEnemies = FilterObjectsInShape(arEnemies, OBJECT_TYPE_CREATURE, SHAPE_SPHERE, GetLocation(OBJECT_SELF), ARCANEHORROR_AOE_RADIUS, 0.0, 0.0, TRUE_);
	}
	else*/
	arEnemies = GetCreaturesInMeleeRing(OBJECT_SELF, fStart, fEnd, TRUE_);

	int32 nSize = arEnemies.Num();
	int32 nEnemiesCount = nSize;
	if (nApp == APP_TYPE_ARCHDEMON || nApp == APP_TYPE_HIGHDRAGON)
	{
		nEnemiesCount = 0;
		int32 i;
		AActor* oCurrent;
		for (i = 0; i < nSize; i++)
		{
			oCurrent = arEnemies[i];
			if (oCurrent != OBJECT_SELF && IsObjectHostile(OBJECT_SELF, oCurrent))
				nEnemiesCount++;
		}
	}

#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_Condition_SurroundedByAtLeastXEnemies", "Number of enemies around self: " + IntToString(nEnemiesCount));
#endif

	// If nNumofTargets is 0 - then we want NO enemies
	if (nNumOfTargets == 0 && nEnemiesCount == 0)
		oTarget = OBJECT_SELF;
	else if (nNumOfTargets != 0 && nEnemiesCount >= nNumOfTargets)
		oTarget = OBJECT_SELF;
	return oTarget;
}

AActor* _AI_Condition_GetPartyMemberTarget(AActor* OBJECT_SELF, int32 nTacticCommand, int32 nTacticSubCommand, int32 nPartyMemberType, int32 nTacticID)
{
	AActor* oFollower;
	if (nPartyMemberType == AI_TARGET_TYPE_HERO)
		oFollower = GetPartyLeader();
	else if (nPartyMemberType == AI_TARGET_TYPE_MAIN_CONTROLLED)
		oFollower = GetMainControlled();
	else
		oFollower = GetTacticConditionObject(OBJECT_SELF, nTacticID);
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_Condition_GetPartyMemberTarget", "START, looking for target of party member: " + GetTag(oFollower));
#endif

	return GetAttackTarget(oFollower);
}

AActor* _AI_GetNthDamagedCreatureInArray(AActor* OBJECT_SELF, TArray<AActor*> arCreatures, int32 nHighLow)
{
	// NOTE: this function actually returns the creature with highest or lower hp
	int32 nSize = arCreatures.Num();
	AActor* oMostDamagedCreature = nullptr;
	AActor* oCurrent = nullptr;
	int32 i = 0;
	float fCurrentHP;
	float fLowestHP = -1.0;
	float fHighestHP = 0.0;
	AActor* oLowestHealthCreature = nullptr;
	AActor* oHighestHealthCreature = nullptr;


	if (nHighLow != 0 && nHighLow != 1)
	{
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_GetNthDamagedCreatureInArray (=> get creature with highest or lowest health)",
			"ERROR! wrong value", LOG_SEVERITY_WARNING);
#endif

		return nullptr;
	}

	for (i = 0; i < nSize; i++)
	{
		oCurrent = arCreatures[i];
		fCurrentHP = GetCurrentHealth(oCurrent);
		if (fCurrentHP < fLowestHP || fLowestHP == -1.0)
		{
			fLowestHP = fCurrentHP;
			oLowestHealthCreature = oCurrent;
		}
		if (fCurrentHP > fHighestHP)
		{
			fHighestHP = fCurrentHP;
			oHighestHealthCreature = oCurrent;
		}
	}

	if (nHighLow == 0) // => return lowest health
		return oLowestHealthCreature;
	else // 1 => return highest health
		return oHighestHealthCreature;
}

int32 _AI_HasStatLevel(AActor* OBJECT_SELF, AActor* oCreature, int32 nStatType, int32 nStatLevel)
{
	float fCurrentStat = 0.f;
	float fMaxStat = 0.f;
	int32 nCurrentStatLevel = 0;
	int32 nRet = FALSE_;

	switch (nStatType)
	{
	case AI_STAT_TYPE_HP:
	{
		fCurrentStat = GetCurrentHealth(oCreature);
		fMaxStat = GetMaxHealth(oCreature);
		break;
	}
	case AI_STAT_TYPE_MANA_OR_STAMINA:
	{
		fCurrentStat = GetCreatureProperty(oCreature, PROPERTY_DEPLETABLE_MANA_STAMINA, PROPERTY_VALUE_CURRENT);
		fMaxStat = GetCreatureProperty(oCreature, PROPERTY_DEPLETABLE_MANA_STAMINA, PROPERTY_VALUE_TOTAL);

		if (fMaxStat <= 0.0)
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_HasStatLevel", "ERROR! bad value for max stamina or mana: " + FloatToString(fMaxStat));
#endif
			return FALSE_;
		}
		break;
	}
	}


	// Determine what type of stat we are dealing with
	nCurrentStatLevel = FloatToInt(fCurrentStat / fMaxStat * 100);

#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_HasStatLevel", "Creature: " + GetTag(oCreature) + ", MAX: " + FloatToString(fMaxStat) +
		", Current: " + FloatToString(fCurrentStat));
#endif

	if (nStatLevel > 0) // For example: creature >= 50%
	{
		if (nCurrentStatLevel >= nStatLevel)
			nRet = TRUE_;
	}
	else // For example: creature < 50% - use absolute value of function parameter
	{
		if (nCurrentStatLevel < abs(nStatLevel))
			nRet = TRUE_;
	}

#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_HasStatLevel", "Creature: " + GetTag(oCreature) + ", Current: " + IntToString(nCurrentStatLevel) +
		", Required: " + IntToString(nStatLevel) + ", return: " + IntToString(nRet));
#endif

	return nRet;

}

int32 _AI_HasAnyBuffEffect(AActor* oCreature)
{
	TArray<FEffect> arEffects = GetEffects(oCreature);
	int32 nSize = arEffects.Num();
	int32 i;
	FEffect eCurrent;
	int32 nEffectAbility;
#ifdef DEBUG
	Log_Trace_AI(oCreature, "_AI_HasAnyBuffEffect", "START, number of effects: " + IntToString(nSize));
#endif

	for (i = 0; i < nSize; i++)
	{
		eCurrent = arEffects[i];
		nEffectAbility = GetEffectAbilityID(eCurrent);
		if (GetM2DAInt(TABLE_AI_ABILITY_COND, "MagicalBuff", nEffectAbility) == 1)
		{
#ifdef DEBUG
			Log_Trace_AI(oCreature, "_AI_HasAnyBuffEffect", "Creature has buff ability applied: " + Ability_GetAbilityNameById(nEffectAbility));
#endif
			return TRUE_;
		}
	}
	return FALSE_;
}

float _AI_GetRangeFromID(int32 nRangeID)
{
	float fRet = 0.0;
	switch (nRangeID)
	{
	case AI_RANGE_ID_SHORT: fRet = AI_RANGE_SHORT * SCALE_MULTIPLIER; break;
	case AI_RANGE_ID_MEDIUM: fRet = AI_RANGE_MEDIUM * SCALE_MULTIPLIER; break;
	case AI_RANGE_ID_LONG: fRet = AI_RANGE_LONG * SCALE_MULTIPLIER; break;
	}
	return fRet;
}

int32 _AI_IsVulnerableToDamage(AActor* oCreature, int32 nDamageType)
{
	//TODO _AI_IsVulnerableToDamage was CUT. why?
	// CUT
	int32 nRet = FALSE_;

	return nRet;
}

int32 _AI_GetArmorType(AActor* oCreature)
{
	int32 nArmorType = AI_ARMOR_TYPE_INVALID;

	// Trying to find if humanoid
	AActor* oArmor;
	int32 nBaseItemType;

	oArmor = GetItemInEquipSlot(INVENTORY_SLOT_CHEST, oCreature);
	if (!IsObjectValid(oArmor))
		nArmorType = AI_ARMOR_TYPE_LOW;
	else
	{
		nBaseItemType = GetBaseItemType(oArmor);
		switch (nBaseItemType)
		{
		case BASE_ITEM_TYPE_ARMOR_LIGHT:
		{
			nArmorType = AI_ARMOR_TYPE_LOW;
			break;
		}
		case BASE_ITEM_TYPE_ARMOR_MEDIUM:
		{
			nArmorType = AI_ARMOR_TYPE_MEDIUM;
			break;
		}
		case BASE_ITEM_TYPE_ARMOR_HEAVY:
		case BASE_ITEM_TYPE_ARMOR_MASSIVE:
		case BASE_ITEM_TYPE_ARMOR_SUPERMASSIVE:
		{
			nArmorType = AI_ARMOR_TYPE_HIGH;
			break;
		}
		}
	}

	return nArmorType;
}