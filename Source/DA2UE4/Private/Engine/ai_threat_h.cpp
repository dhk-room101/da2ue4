#include "DA2UE4.h"
#include "ai_threat_h.h"
//#include "var_constants_h.h"
#include "log_h.h"
#include "core_h.h"
#include "wrappers_h.h"
#include "core_difficulty_h.h"
#include "ldf.h"

void AI_Threat_UpdateEnemyDisappeared(AActor* oCreature, AActor* oEnemy)
{
#ifdef DEBUG
	Log_Trace_Threat(oCreature, "AI_Threat_UpdateEnemyDisappeared", "Enemy:" + GetTag(oEnemy));
	AI_Threat_Display(oCreature, "Enemy disappeared, before changing threat");
#endif
	ClearEnemyThreat(oCreature, oEnemy);
#ifdef DEBUG
	Log_Trace_Threat(oCreature, "AI_Threat_UpdateEnemyDisappeared", "ENEMY CLEARED FROM LIST");
#endif
	// Check if the enemy was my target:
	AActor* oCurrent = AI_Threat_GetThreatTarget(oCreature);
	AActor* oMostHated = GetThreatEnemy(oCreature, 0);
	if (oCurrent == oEnemy)
	{
		if (IsObjectValid(oMostHated) && !IsDead(oMostHated))
			AI_Threat_SetThreatTarget(oCreature, oMostHated);
		else // last enemy
			AI_Threat_SetThreatTarget(oCreature, nullptr);
	}
#ifdef DEBUG
	AI_Threat_Display(oCreature, "Enemy disappeared, after clearing threat");
#endif
}

void AI_Threat_Display(AActor* aCreature, FString sMes)
{
	ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aCreature);

	int32 i;
	AActor* oCurrent;
	Log_Trace(LOG_CHANNEL_THREAT_DATA, "THREAT DUMP", "START: " + sMes);
	AActor* oTarget = GetAttackTarget(oCreature);

	Log_Trace(LOG_CHANNEL_THREAT_DATA, "THREAT", "CURRENT TARGET: " + GetTag(oTarget));
	int32 nSize = GetThreatTableSize(oCreature);
	if (nSize == 0 && IsObjectValid(oTarget))
		Log_Trace(LOG_CHANNEL_THREAT_DATA, "THREAT", "ERROR! got a threat target while table is empty!");
	for (i = 0; i < nSize; i++)
	{
		oCurrent = GetThreatEnemy(oCreature, i);
		Log_Trace(LOG_CHANNEL_THREAT_DATA, "THREAT", "Enemy " + IntToString(i) + ": " + GetTag(oCurrent) + ", threat: " + FloatToString(GetThreatValueByIndex(oCreature, i)));
	}
	Log_Trace(LOG_CHANNEL_THREAT_DATA, "THREAT", "END");
}

AActor* AI_Threat_GetThreatTarget(AActor* aCreature, int32 bUpdateThreatTarget)
{
	ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aCreature);

	if (bUpdateThreatTarget)
		AI_Threat_UpdateCreatureThreat(oCreature, nullptr, 0.0);
	AActor* oTarget = GetAttackTarget(oCreature);
	if (IsObjectValid(oTarget) &&
		(IsDead(oTarget) || IsDying(oTarget) || !IsPerceiving(oCreature, oTarget)))
	{
#ifdef DEBUG
		Log_Trace_Threat(oCreature, "AI_Threat_GetThreatTarget", "ERROR: threat target is dead, dying or not perceived! (" + GetTag(oTarget) + ")", LOG_SEVERITY_CRITICAL);
#endif
		if (bUpdateThreatTarget)
			AI_Threat_UpdateCreatureThreat(oCreature, nullptr, 0.0);
	}
#ifdef DEBUG
	Log_Trace_Threat(oCreature, "AI_Threat_GetThreatTarget", "Threat Target: " + GetTag(oTarget));
#endif
	return oTarget;
}

void AI_Threat_UpdateCreatureThreat(AActor* aCreature, AActor* oEnemy, float fThreatChange)
{
	ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aCreature);
#ifdef DEBUG
	AI_Threat_Display(oCreature, "BEFORE UPDATING THREAT");
#endif

	if (IsObjectValid(oEnemy))
	{
		if (IsDead(oEnemy)) // no threat towards dead enemy
		{
#ifdef DEBUG
			Log_Trace_Threat(oCreature, "AI_Threat_UpdateCreatureThreat", "Can not update threat against a dead enemy");
#endif
			return;
		}

		if (!IsObjectHostile(oCreature, oEnemy))
			return; // doing nothing if both objects are not hostile towards each other
		fThreatChange = AI_Threat_GetHatedThreat(oCreature, oEnemy, fThreatChange);

		if (fThreatChange > 0.0 && fThreatChange < AI_THREAT_MIN_CHANGE)
			fThreatChange = AI_THREAT_MIN_CHANGE;

		float fCurrentThreat = GetThreatValueByObjectID(oCreature, oEnemy);
		if (fThreatChange < 0.0 && fCurrentThreat + fThreatChange <= 0.0)
			fThreatChange = (-1) * (fCurrentThreat - 1.0);

		if (fThreatChange > 0.0 && fCurrentThreat + fThreatChange > AI_THREAT_MAX)
			fThreatChange = AI_THREAT_MAX - fCurrentThreat;

#ifdef DEBUG
		Log_Trace_Threat(oCreature, "AI_Threat_UpdateCreatureThreat", "Enemy: " + GetTag(oEnemy) + ", Threat Change: " + FloatToString(fThreatChange));
#endif

		UpdateThreatTable(oCreature, oEnemy, fThreatChange);

#ifdef DEBUG
		AI_Threat_Display(oCreature, "AFTER ADDING NEW CREATURE TO TABLE");
#endif
	}
	else
	{
#ifdef DEBUG
		Log_Trace_Threat(oCreature, "AI_Threat_UpdateCreatureThreat", "Update check for threat target (no specific enemy)");
#endif
	}


	// Check if we can update the threat target (if different from current)
	AActor* oMostHated = GetThreatEnemy(oCreature, 0); // 0 is the most hated
	AActor* oCurrentTarget = AI_Threat_GetThreatTarget(oCreature, FALSE_); // current threat target
	int32 nNewMaxTimer;
	int32 nCurrentTime = GetTime();
	int32 nCurrentMaxTimer = oCreature->AI_THREAT_SWITCH_TIMER_MIN;

#ifdef DEBUG
	Log_Trace_Threat(oCreature, "AI_Threat_UpdateCreatureThreat", "Most Hated: " + GetTag(oMostHated) + ", Current Target: " + GetTag(oCurrentTarget));
#endif


	if (!IsObjectValid(oCurrentTarget)) // setting target for the first time
	{
		AI_Threat_SetThreatTarget(oCreature, oMostHated);
		oCreature->AI_THREAT_TARGET_SWITCH_COUNTER = GetTime();
	}
	else if (oMostHated != oCurrentTarget) // if they are different - try to update threat target to be most hated
	{
		// After the first switch - set a timer great then 0 (it was 0 until now to allow initial quick switching)

		// Check if to keep wounded target
		if (IsObjectValid(oEnemy) && AI_Threat_KeepWoundedTarget(oEnemy))
		{
#ifdef DEBUG
			Log_Trace_Threat(oCurrentTarget, "AI_Threat_UpdateCreatureThreat", "Keeping wounded target (not checking any other threat rules)");
#endif
		}
		else // target not wounded enough, or decided not to keep it
		{
			if (AI_Threat_ClearToSwitchTarget(oCreature) || !IsPerceiving(oCreature, oCurrentTarget) || IsDead(oCurrentTarget) || IsDying(oCurrentTarget))
			{
#ifdef DEBUG
				Log_Trace_Threat(oCreature, "AI_Threat_UpdateCreatureThreat", "Clear to switch threat target");
#endif
				// init timer
				oCreature->AI_THREAT_TARGET_SWITCH_COUNTER = nCurrentTime;
				// Update target to be the most hated target
				AI_Threat_SetThreatTarget(oCreature, oMostHated);

				nNewMaxTimer = nCurrentMaxTimer + AI_THREAT_SWITCH_TIMER_JUMP;
				if (nNewMaxTimer > AI_THREAT_SWITCH_TIME_MAX)
					nNewMaxTimer = 0;
#ifdef DEBUG
				Log_Trace_Threat(oCreature, "AI_Threat_UpdateCreatureThreat", "New threat timer value: " + IntToString(nNewMaxTimer));
#endif

				oCreature->AI_THREAT_SWITCH_TIMER_MIN = nNewMaxTimer;
			}
			else
			{
#ifdef DEBUG
				Log_Trace_Threat(oCreature, "AI_Threat_UpdateCreatureThreat", "NOT Clear to switch threat target");
#endif
			}
		}
	}

	if (IsObjectValid(oEnemy))
	{
		float fNewThreat = GetThreatValueByObjectID(oCreature, oEnemy);
#ifdef DEBUG
		Log_Trace_Threat(oCreature, "AI_Threat_UpdateCreatureThreat", "Threat updated against enemy: " + GetTag(oEnemy) + ", new value: " + FloatToString(fNewThreat));
#endif
	}
#ifdef DEBUG
	AI_Threat_Display(oCreature, "AFTER UPDATING THREAT");
#endif
}

float AI_Threat_GetHatedThreat(AActor* aCreature, AActor* oThreatTarget, float fThreat)
{
	ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aCreature);
	int32 nHatedRace = oCreature->AI_THREAT_HATED_RACE;
	int32 nHatedClass = oCreature->AI_THREAT_HATED_CLASS;
	int32 nHatedGender = oCreature->AI_THREAT_HATED_GENDER;


	if (nHatedRace == GetCreatureRacialType(oThreatTarget))
		fThreat *= AI_THREAT_HATED_ENEMY_COEFFICIENT;

	if (nHatedClass == GetCreatureCoreClass(oCreature))
		fThreat *= AI_THREAT_HATED_ENEMY_COEFFICIENT;

	if (nHatedGender == GetCreatureGender(oThreatTarget))
		fThreat *= AI_THREAT_HATED_ENEMY_COEFFICIENT;

	return fThreat;
}

void AI_Threat_SetThreatTarget(AActor* aCreature, AActor* oTarget)
{
	ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aCreature);
#ifdef DEBUG
	Log_Trace_Threat(oCreature, "AI_Threat_SetThreatTarget", "New threat target: " + GetTag(oTarget));
	AI_Threat_Display(oCreature, "Before setting threat target");
#endif
	SetAttackTarget(oCreature, oTarget);//DHK
#ifdef DEBUG
	AI_Threat_Display(oCreature, "After setting threat target");
#endif
}

int32 AI_Threat_KeepWoundedTarget(AActor* oTarget)
{
	float fCurrentHealth = GetCurrentHealth(oTarget);
	float fMaxHealth = GetMaxHealth(oTarget);

	if (fMaxHealth <= 0.0) fMaxHealth = 1.0;

	if (fCurrentHealth < 0.0) fCurrentHealth = 0.0;

	float fHealthRatio = fCurrentHealth / fMaxHealth;
	int32 nRand;
#ifdef DEBUG
	Log_Trace_Threat(oTarget, "AI_Threat_KeepWoundedTarget", "Target health ratio:" + FloatToString(fHealthRatio) + "%");
#endif

	if (fHealthRatio <= 0.2 && fHealthRatio > 0.1)
	{
#ifdef DEBUG
		Log_Trace_Threat(oTarget, "AI_Threat_KeepWoundedTarget", "Target health lower than 20%, chance of keeping target:" + IntToString(AI_THREAT_CHANCE_KEEP_TARGET_20_HEALTH) + "%");
#endif
		nRand = Random(100) + 1;
		if (nRand <= AI_THREAT_CHANCE_KEEP_TARGET_20_HEALTH)
			return TRUE_;

	}
	else if (fHealthRatio <= 0.1)
	{
#ifdef DEBUG
		Log_Trace_Threat(oTarget, "AI_Threat_KeepWoundedTarget", "Target health lower than 10%, chance of keeping target:" + IntToString(AI_THREAT_CHANCE_KEEP_TARGET_10_HEALTH) + "%");
#endif
		nRand = Random(100) + 1;
		if (nRand <= AI_THREAT_CHANCE_KEEP_TARGET_10_HEALTH)
			return TRUE_;
	}

	return FALSE_;
}

int32 AI_Threat_ClearToSwitchTarget(AActor* aCreature)
{
	ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aCreature);

	int32 nSwitchTime = oCreature->AI_THREAT_TARGET_SWITCH_COUNTER; // the time stamp when the target switched last time
	int32 nCurrentTime = GetTime();
	int32 nCurrentMaxTimer;
	if (IsUsingMeleeWeapon(oCreature))
		nCurrentMaxTimer = oCreature->AI_THREAT_SWITCH_TIMER_MIN;
	else // using ranged attack -> can switch much faster
		nCurrentMaxTimer = 0;

#ifdef DEBUG
	Log_Trace_Threat(oCreature, "AI_Threat_ClearToSwitchTarget", "creature: " + GetTag(oCreature) + ", current time: " + IntToString(nCurrentTime)
		+ ", switch time: " + IntToString(nSwitchTime) + ", current max: " + IntToString(nCurrentMaxTimer));
#endif

	return(nCurrentTime - nSwitchTime >= nCurrentMaxTimer);
}

void AI_Threat_UpdateEnemyAppeared(AActor* aCreature, AActor* aEnemy)
{
	ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aCreature);
	ADA2UE4Creature* oEnemy = Cast<ADA2UE4Creature>(aEnemy);

	//int32 nMaxRand = FloatToInt(AI_THREAT_VALUE_ENEMY_PERCEIVED);
	//int32 nRand = Random(nMaxRand) + 1; // randomized so not the first perceived is always attacked
	//float fThreatChange = IntToFloat(nRand);
#ifdef DEBUG
	Log_Trace_Threat(oCreature, "AI_Threat_UpdateEnemyAppeared", "Enemy:" + GetTag(oEnemy));
#endif

	AActor* oChestItem = GetItemInEquipSlot(INVENTORY_SLOT_CHEST, oEnemy);
	AActor* oWeaponItem = GetItemInEquipSlot(INVENTORY_SLOT_MAIN, oEnemy);
	int32 nChestItemType = GetBaseItemType(oChestItem);
	int32 nWeaponItemType = GetBaseItemType(oWeaponItem);
	float fThreatExtra = 0.0;
	if (GetGameDifficulty() < 3) // apply this for difficulty levels lower than nightmare
	{
		switch (nChestItemType)
		{
		case BASE_ITEM_TYPE_ARMOR_LIGHT: fThreatExtra = AI_THREAT_PERCEIVED_ARMOR_LIGHT; break;
		case BASE_ITEM_TYPE_ARMOR_MEDIUM: fThreatExtra = AI_THREAT_PERCEIVED_ARMOR_MEDIUM; break;
		case BASE_ITEM_TYPE_ARMOR_HEAVY: fThreatExtra = AI_THREAT_PERCEIVED_ARMOR_HEAVY; break;
		case BASE_ITEM_TYPE_ARMOR_MASSIVE:
		case BASE_ITEM_TYPE_ARMOR_SUPERMASSIVE:
			fThreatExtra = AI_THREAT_PERCEIVED_ARMOR_MASSIVE; break;
		}
	}
	float fThreatChange = AI_THREAT_VALUE_ENEMY_PERCEIVED + fThreatExtra;

	float fMoreExtra = oEnemy->AI_THREAT_GENERATE_EXTRA_THREAT;

	fThreatChange += fMoreExtra;

	// weapon check
	if (GetGameDifficulty() < 3 && // extra threat only for difficulty lower than nightmare
		nChestItemType != BASE_ITEM_TYPE_LONGBOW && nChestItemType != BASE_ITEM_TYPE_SHORTBOW && nChestItemType != BASE_ITEM_TYPE_CROSSBOW
		&& nChestItemType != BASE_ITEM_TYPE_STAFF)
		fThreatChange += AI_THREAT_PERCEIVED_WEAPON_MELEE;

	// last - if this is a 'coward' creature, give it minimal hate
	if (GetPackageAI(oEnemy) == AI_TABLE_COWARD)
		fThreatChange = 1.0;

	//TODO ai_threat force-target-switch-check
	// Check to see if we need to do a force-target-switch-check (ClearAllCommands)
	// This is so enemies attack newly perceived party members who have higher threat 
	// while still trying to attack an older target
	/*if(!IsFollower(oCreature) && AI_Threat_ClearToSwitchTarget(oCreature))
	{
	AActor* oCurrentTarget = AI_Threat_GetThreatTarget(oCreature); // current threat target
	float fCurrentTargetThreat = GetThreatValueByObjectID(oCreature, oCurrentTarget);
	Log_Trace_Threat("AI_Threat_UpdateEnemyAppeared", "Current target threat: " + FloatToString(fCurrentTargetThreat));

	if(IsObjectValid(oCurrentTarget) && fThreatChange > fCurrentTargetThreat)
	{
	Log_Trace_Threat("AI_Threat_UpdateEnemyAppeared", "Newly perceived creature is more threatening then existing threat target - clearing all actions");
	WR_ClearAllCommands(oCreature);
	}
	}*/

#ifdef DEBUG
	AI_Threat_Display(oCreature, "Enemy appeared, before changing threat");
#endif
	AI_Threat_UpdateCreatureThreat(oCreature, oEnemy, fThreatChange);
}

void AI_Threat_UpdateCantAttackTarget(AActor* aCreature, AActor* oEnemy)
{
	ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aCreature);

	if (!IsObjectHostile(oCreature, oEnemy))
		return; // doing nothing if both objects are not hostile towards each other
#ifdef DEBUG
	AI_Threat_Display(oCreature, "cant attack target before changing threat");
	Log_Trace_Threat(oCreature, "AI_Threat_UpdateCantAttackTarget", "oEnemy:" + GetTag(oEnemy));
#endif

	float fThreatChange = (-1) * GetThreatValueByObjectID(oCreature, oEnemy) * AI_THREAT_CANT_ATTACK_TARGET_REDUCTION;

	// clear the switch timer (can't attack current anyways)
	oCreature->AI_THREAT_TARGET_SWITCH_COUNTER = 0;
	AI_Threat_UpdateCreatureThreat(oCreature, oEnemy, fThreatChange);
}