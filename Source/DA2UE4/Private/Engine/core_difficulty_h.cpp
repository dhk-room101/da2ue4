#include "DA2UE4.h"
#include "core_difficulty_h.h"
#include "ldf.h"

int32 Diff_GetAutoScaleTable()
{
	return TABLE_AUTOSCALE;/*  TABLE_Autos GetM2DAInt( TABLE_DIFFICULTY, "AUTOSCALE", GetGameDifficulty() );*/
}

float Diff_GetAbilityUseMod(AActor* oCreature)
{
	int32 nRank = GetCreatureRank(oCreature);
	if (nRank == CREATURE_RANK_PLAYER || nRank == CREATURE_RANK_CRITTER || nRank == CREATURE_RANK_WEAK_NORMAL)
	{
		return 1.0f;
	}

	// --------------------------------------------------------------------------
	// Up to level 3, we don't enforce this rule on HARD
	// --------------------------------------------------------------------------
	float fLevel = GetCreatureProperty(oCreature, PROPERTY_SIMPLE_LEVEL);
	if (fLevel <= 3.0f || GetGameDifficulty() <= 2)
	{
		return 1.0f;
	}


	float fBase = GetM2DAFloat(TABLE_DIFFICULTY, "AIAbilityUseMod", GetGameDifficulty());

	// capping at 0.5, just in case someone adds bad 2da data
	if (fBase > 0.75)
	{
		return fBase;
	}
	return 0.75;
}

float Diff_GetRulesAttackBonus(AActor* oAttacker)
{
	if (GetCreatureRank(oAttacker) != CREATURE_RANK_PLAYER)
	{
		return 0.0;
	}

	return GetM2DAFloat(TABLE_DIFFICULTY, "AttackBonus", GetGameDifficulty());

}

float Diff_GetRulesDefenseBonus(AActor* oCreature)
{
	if (GetCreatureRank(oCreature) != CREATURE_RANK_PLAYER)
	{
		return 0.0;
	}

	return GetM2DAFloat(TABLE_DIFFICULTY, "DefenseBonus", GetGameDifficulty());
}

float Diff_GetRulesDamageBonus(AActor* oCreature)
{
	if (GetCreatureRank(oCreature) != CREATURE_RANK_PLAYER)
	{
		return 0.0;
	}

	return GetM2DAFloat(TABLE_DIFFICULTY, "DamageBonus", GetGameDifficulty());

}

float GetDamageScalingThreshold()
{
	float fRet = GetM2DAFloat(TABLE_DIFFICULTY, "DmgScalingThresh", GetGameDifficulty());

	if (fRet > 0.0f)
	{
		return fRet;
	}
	else /* outdated resources */
	{
		return 5.0f;
	}
}
