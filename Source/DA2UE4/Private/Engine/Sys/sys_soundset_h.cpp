#include "DA2UE4.h"
#include "DA2UE4GameInstance.h"
#include "DA2UE4Creature.h"
#include "STypes.h"
#include "log_h.h"
#include "ldf.h"
#include "m2da_data_h.h"
#include "sys_soundset_h.h"

void SSPlaySituationalSound(AActor* oCreature, int32 nSituation, AActor* oTarget, int32 nCommandType)
{

	if (IsObjectValid(oCreature) && (!IsDead(oCreature) || nSituation == SOUND_SITUATION_DYING) && GetObjectActive(oCreature) && GetObjectType(oCreature) == OBJECT_TYPE_CREATURE)
	{
		EGameMode nMode = GetGameMode();
		int32 nSound = 0;

		// -------------------------------------------------------------------------
		// Only play sound in combat or explore mode so cinematics don't get unintentional VO.
		// -------------------------------------------------------------------------
		if (nMode == EGameMode::GM_COMBAT)
		{
			nSound = _GetSituationalCombatSound(oCreature, nSituation, oTarget, nCommandType);
		}
		else if (nMode == EGameMode::GM_EXPLORE)
		{
			nSound = _GetSituationalExploreSound(oCreature, nSituation, oTarget, nCommandType);
		}

		// Immortal creatures never ask for healing.
		if (nSound == SS_COMBAT_NEAR_DEATH || nSound == SS_COMBAT_HEAL_ME ||
			nSound == SS_EXPLORE_HEAL_ME)
		{
			if (IsImmortal(oCreature) || IsPlot(oCreature))
			{
				nSound = 0;
			}
		}

		if (nSound > 0)
		{
			if (nSound == SS_COMBAT_DEATH)
			{
#ifdef DEBUG
				Log_Trace(LOG_CHANNEL_SOUNDSETS, "sys_soundsets.play", "nSituation: " + IntToString(nSituation) + ", Sound: " + IntToString(nSound));
#endif // DEBUG
				int32 nRank = GetCreatureRank(oCreature);
				if (nRank == CREATURE_RANK_BOSS || nRank == CREATURE_RANK_ELITE_BOSS)
				{
					// boss + creature always complain about dying.
					PlaySoundSet(oCreature, nSound, 1.0);
				}
				else
				{
					PlaySoundSet(oCreature, nSound);
				}
			}
			else if (!SSGetSoundSetFlag(oCreature, nSound))
			{
				if (nSituation == SOUND_SITUATION_ORDER_RECEIVED)
				{
					if (!IsStealthy(oCreature))
					{
						float fProb = GetM2DAFloat(TABLE_COMMANDS, "SoundsetProbability", nCommandType);

						// No voice chat when moving in explore mode.
						switch (nCommandType)
						{
						case COMMAND_TYPE_DRIVE:
						case COMMAND_TYPE_MOVE_TO_LOCATION:
						case COMMAND_TYPE_MOVE_TO_OBJECT:
							if (nMode == EGameMode::GM_EXPLORE)
								fProb = 0.0f;
						}

						if (fProb > 0.0f)
						{
#ifdef DEBUG
							Log_Trace(LOG_CHANNEL_SOUNDSETS, "sys_soundsets.play", "SOUND_SITUATION_ORDER_RECEIVED: Command = " + Log_GetCommandNameById(nCommandType) + ", Probability = " + FloatToString(fProb));
#endif
							PlaySoundSet(oCreature, nSound, fProb);
						}
					}
				}
				else
				{
#ifdef DEBUG
					Log_Trace(LOG_CHANNEL_SOUNDSETS, "sys_soundsets.play", "oCreature: " + GetTag(oCreature) + ", nSituation: " + IntToString(nSituation) + ", Sound: " + IntToString(nSound));
#endif
					PlaySoundSet(oCreature, nSound);
				}

				if (GetIsSoundSetEntryTypeRestricted(nSound))
				{
					SSSetSoundSetFlag(oCreature, nSound);
				}
			}
			else
			{
#ifdef DEBUG
				Log_Trace(LOG_CHANNEL_SOUNDSETS, "sys_soundsets.play", "Not playing sound " + IntToString(nSound) + " as flag was set");
#endif
			}
		}
	}
}

int32 _GetSituationalCombatSound(AActor* oCreature, int32 nSituation, AActor* oTarget, int32 nCommandType)
{
	int32 nSound = 0;

	switch (nSituation)
	{
	case SOUND_SITUATION_COMBAT_BATTLECRY:
	{
		int32 nRand = Random(12);
		if (nRand < 1)
			nSound = SS_COMBAT_TAUNT;
		else if (nRand < 7)
			nSound = SS_COMBAT_ATTACK;
		else
			nSound = SS_COMBAT_BATTLE_CRY;

		break;
	}
	case SOUND_SITUATION_GOT_DAMAGED:
		nSound = SS_COMBAT_PAIN_GRUNT;
		break;
		/*case SOUND_SITUATION_COMBAT_CHARGE:
		nSound = SS_COMBAT_ATTACK;
		break;*/
	case SOUND_SITUATION_ENEMY_KILLED:
		nSound = SS_COMBAT_ENEMY_KILLED;
		break;
	case SOUND_SITUATION_ATTACK_IMPACT:
		nSound = SS_COMBAT_ATTACK_GRUNT;
		break;
	case SOUND_SITUATION_END_OF_COMBAT:
		nSound = SS_COMBAT_CHEER_PARTY;
		break;
	case SOUND_SITUATION_SPELL_INTERRUPTED:
		nSound = SS_SPELL_FAILED;
		break;
	case SOUND_SITUATION_PARTY_MEMBER_SLAIN:
		if (IsObjectValid(oTarget) && IsObjectHostile(oTarget, oCreature))
			nSound = SS_COMBAT_MONSTER_SLEW_PARTY_MEMBER;
		break;
	case SOUND_SITUATION_COMMAND_COMPLETE:
	{
		if (_GetRelativeResourceLevel(oCreature, PROPERTY_DEPLETABLE_HEALTH) < SOUND_THRESH_HEALTH_NEAR_DEATH)
			nSound = SS_COMBAT_NEAR_DEATH;
		else if (_GetRelativeResourceLevel(oCreature, PROPERTY_DEPLETABLE_HEALTH) < SOUND_THRESH_HEALTH_LOW)
			nSound = SS_COMBAT_HEAL_ME;
		else if (_GetRelativeResourceLevel(oCreature, PROPERTY_DEPLETABLE_MANA_STAMINA) < SOUND_THRESH_MANA_STAMINA_LOW
			&& IsControlled(oCreature))
		{
			nSound = (GetCreatureCoreClass(oCreature) == CLASS_WIZARD) ? SS_MANA_LOW : SS_COMBAT_STAMINA_LOW;
		}
		break;
	}
	case SOUND_SITUATION_DYING:
		nSound = SS_COMBAT_DEATH;
		break;
	case SOUND_SITUATION_SELECTED:
		nSound = SS_COMBAT_SELECT_NEUTRAL;
		break;
	case SOUND_SITUATION_ORDER_RECEIVED:
		nSound = SS_EXPLORE_START_TASK;
		break;
	}
	return nSound;
}

int32 _GetSituationalExploreSound(AActor* oCreature, int32 nSituation, AActor* oTarget, int32 nCommandType)
{
	//ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aCreature);

	int32 nSound = 0;

	switch (nSituation)
	{
	case SOUND_SITUATION_COMMAND_COMPLETE:
		if (_GetRelativeResourceLevel(oCreature, PROPERTY_DEPLETABLE_HEALTH) < SOUND_THRESH_HEALTH_LOW)
			nSound = SS_EXPLORE_HEAL_ME;
		break;
	case SOUND_SITUATION_ENEMY_SIGHTED:
	{
		if (IsPartyMember(oCreature))
		{
			switch (GetCreatureTypeClassification(GetAppearanceType(oTarget)))
			{
			case CREATURE_TYPE_DARKSPAWN:
				nSound = SS_EXPLORE_ENEMIES_SIGHTED_DARKSPAWN;
				break;
			case CREATURE_TYPE_ANIMAL:
				nSound = SS_EXPLORE_ENEMIES_SIGHTED_ANIMAL;
				break;
			case CREATURE_TYPE_BEAST:
				nSound = SS_EXPLORE_ENEMIES_SIGHTED_BEAST;
				break;
			case CREATURE_TYPE_UNDEAD:
				nSound = SS_EXPLORE_ENEMIES_SIGHTED_UNDEAD;
				break;
			case CREATURE_TYPE_DEMON:
				nSound = SS_EXPLORE_ENEMIES_SIGHTED_DEMON;
				break;
			case CREATURE_TYPE_DRAGON:
				nSound = SS_EXPLORE_ENEMIES_SIGHTED_DRAGON;
				break;
			case CREATURE_TYPE_INVALID:
				nSound = 0;
				break;
			default:
				nSound = SS_EXPLORE_ENEMIES_SIGHTED_OTHER;
				break;
			}
			break;
		}
		else
		{
			nSound = RandomFloat() > 0.5 ? SS_WARCRY : SS_COMBAT_BATTLE_CRY;
		}
		break;
	}
	case SOUND_SITUATION_SELECTED:
	{
		nSound = SS_EXPLORE_SELECT_NEUTRAL;
		if (IsFollower(oCreature))
		{
			nSound = SS_EXPLORE_SELECT_HATE;
			int32 nApproval = GetFollowerApproval(oCreature);

			if (nApproval >= GetModule()->APP_RANGE_VALUE_NEUTRAL) //DHK GetModule()
				nSound = SS_EXPLORE_SELECT_NEUTRAL;
			if (nApproval >= GetModule()->APP_RANGE_VALUE_WARM)
				nSound = SS_EXPLORE_SELECT_FRIENDLY;
			if (nApproval >= GetModule()->APP_RANGE_VALUE_FRIENDLY)
				nSound = SS_EXPLORE_SELECT_LOVE;
		}
		break;
	}
	case SOUND_SITUATION_DYING:
		nSound = SS_COMBAT_DEATH;
		break;
	case SOUND_SITUATION_ORDER_RECEIVED:
	{
		nSound = SS_EXPLORE_START_TASK;
		break;
	}
	case SOUND_SITUATION_SKILL_FAILURE:
	{
		nSound = SS_SKILL_FAILURE;
		break;
	}
	}
	return nSound;
}

int32 SSGetSoundSetFlag(AActor* aCreature, int32 nSSEntry)
{
	ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aCreature);

	// -------------------------------------------------------------------------
	// We got more than 32 SS entries, so we need find out in which variable
	// this particular ssentry is stored. We do this by dividing the entry
	// by 32 and appending the result to the base flag variable name.
	// -------------------------------------------------------------------------
	int32 nVar = nSSEntry / 32;
	FString sVar = "SOUND_SET_FLAGS_" + IntToString(nVar);

	int32 nFlag = (0x00000001 << (nSSEntry % 32));

	int32 nVal = 0;
	if (sVar == "SOUND_SET_FLAGS_0")
		nVal = oCreature->SOUND_SET_FLAGS_0;
	else if (sVar == "SOUND_SET_FLAGS_1")
		nVal = oCreature->SOUND_SET_FLAGS_1;
	else if (sVar == "SOUND_SET_FLAGS_2")
		nVal = oCreature->SOUND_SET_FLAGS_2;
	else LogError("SSGetSoundSetFlag!!");

#ifdef DEBUG
	Log_Trace(LOG_CHANNEL_SOUNDSETS, "sys_soundsets_h.GetFlag." + sVar, "Flag: " + IntToString(nSSEntry) + "(" + IntToHexString(nFlag) + ")" + " Value: " + IntToHexString(nVal) + " Result: " + IntToString(((nVal  & nFlag) == nFlag)), oCreature);
#endif // DEBUG

	return ((nVal  & nFlag) == nFlag);
}

void SSSetSoundSetFlag(AActor* aCreature, int32 nSSEntry, int32 bSet)
{
	ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aCreature);

	// -------------------------------------------------------------------------
	// We got more than 32 SS entries, so we need find out in which variable
	// this particular ssentry is stored. We do this by dividing the entry
	// by 32 and appending the result to the base flag variable name.
	// -------------------------------------------------------------------------
	int32 nVar = nSSEntry / 32;
	FString sVar = "SOUND_SET_FLAGS_" + IntToString(nVar);

	int32 nFlag = (0x00000001 << (nSSEntry % 32));

	int32 nVal = 0;
	if (sVar == "SOUND_SET_FLAGS_0")
		nVal = oCreature->SOUND_SET_FLAGS_0;
	else if (sVar == "SOUND_SET_FLAGS_1")
		nVal = oCreature->SOUND_SET_FLAGS_1;
	else if (sVar == "SOUND_SET_FLAGS_2")
		nVal = oCreature->SOUND_SET_FLAGS_2;
	else
	{
#ifdef DEBUG
		LogError("SSGetSoundSetFlag!!");

#endif // DEBUG

	}

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
	Log_Trace(LOG_CHANNEL_SOUNDSETS, "sys_soundset_h.SetFlag." + sVar + "." + IntToString(bSet), "Flag: " + IntToString(nSSEntry) + "(" + IntToHexString(nFlag) + ")" + " Was: " + IntToHexString(nOld) + " Is: " + IntToHexString(nVal), oCreature);
#endif // DEBUG


	if (sVar == "SOUND_SET_FLAGS_0")
		oCreature->SOUND_SET_FLAGS_0 = nVal;
	else if (sVar == "SOUND_SET_FLAGS_1")
		oCreature->SOUND_SET_FLAGS_1 = nVal;
	else if (sVar == "SOUND_SET_FLAGS_2")
		oCreature->SOUND_SET_FLAGS_2 = nVal;
	else
	{
#ifdef DEBUG
		LogError("SSSetSoundSetFlag!!");

#endif // DEBUG

	}

}

void SSPartyResetSoundsetRestrictions()
{
	TArray<AActor*> aParty = GetPartyList();
	int32 nSize = aParty.Num();
	int32 i;

	for (i = 0; i < nSize; i++)
	{
		SSResetSoundsetRestrictions(aParty[i]);
	}

}

void SSResetSoundsetRestrictions(AActor* aActor)
{
	SSSetSoundSetFlag(aActor, SS_COMBAT_NEAR_DEATH, FALSE_);
	SSSetSoundSetFlag(aActor, SS_COMBAT_HEAL_ME, FALSE_);
	SSSetSoundSetFlag(aActor, SS_EXPLORE_HEAL_ME, FALSE_);
	SSSetSoundSetFlag(aActor, SS_MANA_LOW, FALSE_);
	SSSetSoundSetFlag(aActor, SS_COMBAT_STAMINA_LOW, FALSE_);
}