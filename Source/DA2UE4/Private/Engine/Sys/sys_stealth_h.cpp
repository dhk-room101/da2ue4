#include "DA2UE4.h"
#include "DA2UE4Creature.h"
#include "ui_h.h"
#include "sys_stealth_h.h"

void DropStealth(AActor* oTarget)
{
	if (IsModalAbilityActive(oTarget, ABILITY_SKILL_STEALTH_1))
	{
		Effects_RemoveUpkeepEffect(oTarget, ABILITY_SKILL_STEALTH_1);
	}
}

int32 Stealth_CheckSuccess(AActor* oStealther, AActor* oPerceiver)
{
	//TODO implement Stealth_CheckSuccess fully
	LogTrace(LOG_CHANNEL_COMBAT_ABILITY, "Perception check for " + GetTag(oPerceiver) + " detecting " + GetTag(oStealther));

	int32 bCombatState = GetCombatState(oStealther);
	int32 nStealthRank = Stealth_GetStealthRank(oStealther);
	int32 nPerceptionRank = Stealth_GetPerceptionRank(oPerceiver);

	if ((bCombatState == FALSE_) || (nStealthRank >= nPerceptionRank))
	{
		return TRUE_;
	}
	else
	{
#ifdef DEBUG
		LogTrace(LOG_CHANNEL_COMBAT_ABILITY, "DETECTED by " + GetTag(oPerceiver));
#endif

		// display stealth detected UI message
		UI_DisplayMessage(oStealther, 3511);
		UI_DisplayMessage(oPerceiver, 3513);

		return FALSE_;
	}
}

int32 Stealth_GetStealthRank(AActor* oCreature)
{
	if (oCreature->IsA(ADA2UE4Creature::StaticClass()))
	{
		int32 nStealthRank = -1;
		if (HasAbility(oCreature, ABILITY_SKILL_STEALTH_4) == TRUE_)
		{
			nStealthRank = 4;
		}
		else if (HasAbility(oCreature, ABILITY_SKILL_STEALTH_3) == TRUE_)
		{
			nStealthRank = 3;
		}
		else if (HasAbility(oCreature, ABILITY_SKILL_STEALTH_2) == TRUE_)
		{
			nStealthRank = 2;
		}
		else if (HasAbility(oCreature, ABILITY_SKILL_STEALTH_1) == TRUE_)
		{
			nStealthRank = 1;
		}
#ifdef DEBUG
		LogTrace(LOG_CHANNEL_COMBAT_ABILITY, "Stealth Rank = " + IntToString(nStealthRank));
#endif

		return nStealthRank;
	}
	else LogError("Stealth_GetStealthRank: unknown actor type");
	return 0;
}

int32 Stealth_GetPerceptionRank(AActor* oCreature)
{
	if (oCreature->IsA(ADA2UE4Creature::StaticClass()))
	{
		int32 nCreatureRank = GetCreatureRank(oCreature);
		LogTrace(LOG_CHANNEL_COMBAT_ABILITY, "Creature Rank = " + IntToString(nCreatureRank));
		int32 nPerceptionRank = GetM2DAInt(Diff_GetAutoScaleTable(), "nStealthRank", nCreatureRank);
		LogTrace(LOG_CHANNEL_COMBAT_ABILITY, "Perception Rank = " + IntToString(nPerceptionRank));

		return nPerceptionRank;
	}
	else LogError("Stealth_GetPerceptionRank: unknown actor type");
	return 0;
}

int32 IsStealthy(AActor* oTarget)
{
	return IsModalAbilityActive(oTarget, ABILITY_SKILL_STEALTH_1);
}