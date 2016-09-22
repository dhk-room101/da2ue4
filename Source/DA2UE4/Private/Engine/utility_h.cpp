#include "DA2UE4.h"
#include "DA2UE4GameInstance.h"
#include "DA2UE4Creature.h"
#include "utility_h.h"
#include "effect_resurrection_h.h"
#include "rules_h.h"
#include "core_h.h"

void UT_TeamGoesHostile(int32 nTeamID, int32 bHostile)
{
	int32 nIndex;
	AActor* oPC = GetPartyLeader();
	TArray<ADA2UE4Creature*> arTeam = UT_GetTeam(nTeamID);

	if (bHostile) // The team will go to GROUP_HOSTILE
	{
		for (nIndex = 0; nIndex < arTeam.Num(); nIndex++)
		{
			arTeam[nIndex]->bHostilityChanged = TRUE_;
			UT_CombatStart(arTeam[nIndex], oPC);
		}
	}
	else // The team will go to GROUP_NEUTRAL
	{
		for (nIndex = 0; nIndex < arTeam.Num(); nIndex++)
		{
			arTeam[nIndex]->bHostilityChanged = TRUE_;
			SetGroupId(arTeam[nIndex], GROUP_NEUTRAL);
		}
	}

#ifdef DEBUG
	if (!nIndex)
		Log_Trace(LOG_CHANNEL_SYSTEMS, "utility_h:UT_TeamGoesHostile",
			"No team members found for Team ID #" + IntToString(nTeamID));

	else
		Log_Trace(LOG_CHANNEL_SYSTEMS, "utility_h:UT_TeamGoesHostile",
			"Team ID #" + IntToString(nTeamID) + " has gone HOSTILE. " +
			IntToString(nIndex) + " objects have been affected");
#endif

}

TArray<ADA2UE4Creature*> UT_GetTeam(int32 nTeamID, int32 nMembersType)
{
	TArray<ADA2UE4Creature*> arNewList;

	if (nTeamID <= 0)
	{
		Log_Trace(LOG_CHANNEL_SYSTEMS, "utility_h:UT_GetTeam", "Invalid Team ID");
		return arNewList;
	}

	arNewList = GetTeam(nTeamID, nMembersType);

	return arNewList;
}

void UT_CombatStart(AActor* aAttacker, AActor* oTarget, int32 bTargetSelectionOverride, int32 nOverridePermanent)
{
	ADA2UE4Creature* oAttacker = Cast<ADA2UE4Creature>(aAttacker);

	int32 nAttackGroup = GetGroupId(oAttacker);
	int32 nTargetGroup = GetGroupId(oTarget);
	int32 bHostilityChanged = FALSE_;

#ifdef DEBUG
	Log_Trace(LOG_CHANNEL_SYSTEMS, "UT_CombatStart", "oAttacker: " + GetTag(oAttacker) + ", oTarget: " + GetTag(oTarget));
#endif

	// Handle special case:
	// If both objects have the same group, there is nothing that can be done.
	if (nAttackGroup == nTargetGroup)
	{
#ifdef DEBUG
		Log_Trace(LOG_CHANNEL_SYSTEMS, "UT_CombatStart",
			"Creatures have the same group: [" +
			IntToString(nAttackGroup) + "] [" + IntToString(nTargetGroup) + "]",
			oAttacker, LOG_SEVERITY_CRITICAL);
#endif
		return;
	}

	// If creature was lying on the ground then need to remove fake-death effect
	if (oAttacker->CREATURE_SPAWN_DEAD == 2)
	{
		oAttacker->CREATURE_SPAWN_DEAD = 0;
		RemoveEffectsByParameters(oAttacker, EFFECT_TYPE_SIMULATE_DEATH);
	}

	// [PC] attacking [Neutral/Friendly]
	if (((nAttackGroup == GROUP_PC) && (nTargetGroup == GROUP_NEUTRAL || nTargetGroup == GROUP_FRIENDLY)))
	{
		SetGroupId(oTarget, GROUP_HOSTILE);
#ifdef DEBUG
		Log_Trace(LOG_CHANNEL_SYSTEMS, "UT_CombatStart",
			"Changing the following creature's group to be HOSTILE: " + GetTag(oTarget));
#endif
		bHostilityChanged = TRUE_;
	}

	// [Neutral/Friendly] attacking [PC]
	else if (((nTargetGroup == GROUP_PC) && (nAttackGroup == GROUP_NEUTRAL || nAttackGroup == GROUP_FRIENDLY)))
	{
		SetGroupId(oAttacker, GROUP_HOSTILE);
#ifdef DEBUG
		Log_Trace(LOG_CHANNEL_SYSTEMS, "UT_CombatStart",
			"Changing the following creature's group to be HOSTILE: " + GetTag(oAttacker));
#endif
		bHostilityChanged = TRUE_;
	}

	// [The Rest]
	else if (!(nTargetGroup == GROUP_PC || nTargetGroup == GROUP_NEUTRAL || nTargetGroup == GROUP_FRIENDLY || nTargetGroup == GROUP_HOSTILE) ||
		!(nAttackGroup == GROUP_PC || nAttackGroup == GROUP_NEUTRAL || nAttackGroup == GROUP_FRIENDLY || nAttackGroup == GROUP_HOSTILE))
	{
#ifdef DEBUG
		Log_Trace(LOG_CHANNEL_SYSTEMS, "UT_CombatStart",
			"Setting the following groups to be hostile towards each other: [" +
			IntToString(nAttackGroup) + "] [" + IntToString(nTargetGroup) + "]");
#endif
		SetGroupHostility(nAttackGroup, nTargetGroup, TRUE_);
		bHostilityChanged = TRUE_;
	}

	// Check for overriding the Attackers Target.
	if (bTargetSelectionOverride)
	{
#ifdef DEBUG
		Log_Trace(LOG_CHANNEL_SYSTEMS, "UT_CombatStart",
			"Forcing attacker to attack target");
#endif
		oAttacker->AI_TARGET_OVERRIDE = FName(*oTarget->GetName());
		if (nOverridePermanent)
			oAttacker->AI_TARGET_OVERRIDE_DUR_COUNT = -1; // -1 flags permanent override
		else
			oAttacker->AI_TARGET_OVERRIDE_DUR_COUNT = 0;
	}

	// Hostility Change Failed
	if (!bHostilityChanged)
	{
#ifdef DEBUG
		if (((nAttackGroup == GROUP_PC || nAttackGroup == GROUP_FRIENDLY) &&
			(nTargetGroup == GROUP_HOSTILE || nTargetGroup == GROUP_HOSTILE_ON_GROUND)) ||
			((nAttackGroup == GROUP_HOSTILE || nAttackGroup == GROUP_HOSTILE_ON_GROUND) &&
				nTargetGroup == GROUP_PC || nTargetGroup == GROUP_FRIENDLY)) {
			Log_Trace(LOG_CHANNEL_SYSTEMS, "UT_CombatStart",
				"creatures already hostile toward each other: [" +
				IntToString(nAttackGroup) + "] [" + IntToString(nTargetGroup) + "]"); //TODO DHK: may not be needed: already hostile creatures may start to fight somewhere else
		}

		else {
			Log_Trace(LOG_CHANNEL_SYSTEMS, "UT_CombatStart",
				"Could not turn creatures hostile towards each other: [" +
				IntToString(nAttackGroup) + "] [" + IntToString(nTargetGroup) + "]",
				oAttacker, LOG_SEVERITY_CRITICAL);
		}
#endif
	}
}

int32 UT_GetShoutsFlag(AActor* aCreature)
{
	ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aCreature);

	return oCreature->SHOUTS_ACTIVE;
}

void UT_SetShoutsFlag(AActor* oActor, int32 bEnable = TRUE_)
{
	ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(oActor);
	oCreature->SHOUTS_ACTIVE = bEnable;
}

void UT_Talk(AActor* oTarget, int32 nConversation, int32 nPartyResurrection)
{
	Log_Trace(LOG_CHANNEL_SYSTEMS, "utility_h.UT_Talk", "speaker: " + GetTag(oTarget) + ", listener: " + GetTag(GetHero()) + ", conversation: " + IntToString(nConversation));

	if (GetGameMode() == EGameMode::GM_DEAD)
	{
		Log_Trace(LOG_CHANNEL_SYSTEMS, "utility_h.UT_Talk", "PARTY IS DEAD - CANT TRIGGER DIALOG!", nullptr, LOG_SEVERITY_CRITICAL);
		return;
	}

	int32 n = GetModule()->DISABLE_FOLLOWER_DIALOG;
	Log_Trace(LOG_CHANNEL_SYSTEMS, "utility_h.UT_Talk", "disable follower dialog = " + IntToString(n), nullptr, LOG_SEVERITY_CRITICAL);

	if (GetFollowerState(oTarget) != FOLLOWER_STATE_INVALID && GetModule()->DISABLE_FOLLOWER_DIALOG == 1 && GetHero() != GetPartyLeader())
	{
		Log_Trace(LOG_CHANNEL_SYSTEMS, "utility_h.UT_Talk", "Party leader is not hero when clicking on follower - running soundset instead", nullptr, LOG_SEVERITY_CRITICAL);
		PlaySoundSet(oTarget, SS_YES);
		return;
	}

	if (nPartyResurrection)
		ResurrectPartyMembers(FALSE_);

	ClearAmbientDialogs(oTarget); // this makes sure an already running ambient dialog triggers its plot flag action

	if (IsFollower(oTarget))
		oTarget = GetPartyLeader(); //DHK not needed, as we initiate directly with oSpeaker/target

	int32 rOverrideConversation = GetModule()->PARTY_OVERRIDE_DIALOG.IsNone() ? 0 : 1;
	int32 bOverride = GetModule()->PARTY_OVERRIDE_DIALOG_ACTIVE;

	if (IsFollower(oTarget)
		&& bOverride
		&& GetPartyLeader() != GetHero())
	{
		Log_Trace(LOG_CHANNEL_SYSTEMS, "utility_h.UT_Talk", "overriding dialog with: " + rOverrideConversation);
		nConversation = rOverrideConversation;
		//sDialogResRef = rConversation;
	}


	Log_Trace(LOG_CHANNEL_SYSTEMS, "utility_h.UT_Talk", "Triggering BeginConversation NOW");

	//TrackDialogEvent(EVENT_TYPE_DIALOGUE, oSpeaker, oListener, sDialogResRef); //DHK

	// Track the number of dialogs initiated
	//TODO implement STATS
	//STATS_TrackStartedDialogues(oListener);

	BeginConversation(oTarget, nConversation);
}

AActor* UT_GetNearestCreature(AActor* aActor, int32 nIncludeSelf)
{
	TArray<AActor*> arCreatures = GetNearestObject(aActor, OBJECT_TYPE_CREATURE, 1, FALSE_, FALSE_, nIncludeSelf);
	return arCreatures[0];
}

AActor* UT_GetNearestCreatureByTag(AActor* aActor, FString sTag, int32 nIncludeSelf)
{
	TArray<AActor*> arCreatures = GetNearestObjectByTag(aActor, sTag, OBJECT_TYPE_CREATURE, 1, FALSE_, FALSE_, nIncludeSelf);
	return arCreatures[0];
}

AActor* UT_GetNearestCreatureByGroup(AActor* aActor, int32 nGroup, int32 nIncludeSelf)
{
	TArray<AActor*> arCreatures = GetNearestObjectByGroup(aActor, nGroup, OBJECT_TYPE_CREATURE, 1, FALSE_, FALSE_, nIncludeSelf);
	return arCreatures[0];
}

AActor* UT_GetNearestHostileCreature(AActor* aActor, int32 nCheckLiving)
{
	TArray<AActor*> arCreatures = GetNearestObjectByHostility(aActor, TRUE_, OBJECT_TYPE_CREATURE, MAX_CREATURES_IN_COMBAT, nCheckLiving);
	int32 i;
	AActor* oCreature = nullptr;
	int32 nSize = arCreatures.Num();
	for (i = 0; i < nSize; i++)
	{
		oCreature = arCreatures[i];
		if (!IsDead(oCreature))
			break;
	}

#ifdef DEBUG
	Log_Trace(LOG_CHANNEL_SYSTEMS, "UT_GetNearestHostileCreature", "returning: " + GetTag(oCreature));
#endif
	return oCreature;
}

AActor* UT_GetNearestObjectByTag(AActor* aActor, FString sTag, int32 nIncludeSelf)
{
	TArray<AActor*> arCreatures = GetNearestObjectByTag(aActor, sTag, OBJECT_TYPE_WAYPOINT, 1, FALSE_, FALSE_, nIncludeSelf);
	return arCreatures[0];
}