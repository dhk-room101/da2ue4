#include "DA2UE4.h"
#include "DA2UE4GameInstance.h"
#include "DA2UE4Party.h"
#include "DA2UE4Creature.h"
#include "wrappers_h.h"
#include "log_h.h"
#include "commands_h.h"
#include "events_h.h"
#include "sys_stealth_h.h"
#include "ldf.h"
#include "plot_h.h"

int32 WR_ClearAllCommands(AActor* oCreature, int32 bHardClear)
{
#ifdef DEBUG
	Log_Trace(LOG_CHANNEL_SYSTEMS_WRAPPERS, "wrappers_h.WR_ClearAllCommands", "", oCreature);
#endif

	int32 nRet = ClearAllCommands(oCreature, bHardClear);
	return nRet;
}

void WR_AddCommand(AActor* aCreature, FCommand cCommand, int32 bAddToFront, int32 bStatic, int32 nOverrideAddBehavior, float fTimeout)
{
	ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aCreature);

	int32 nCommandType = GetCommandType(cCommand);

	// timeout check
	// if a timeout was selected AND this is not a follower AND we're in combat AND it's the creatures first timer command
	// then shorten the timer so if the creature moves he'll stop faster and will then have a chance to reevaluate
	// his threat towards the party
	if (fTimeout > 0.0 && !IsFollower(oCreature) && GetCombatState(oCreature) == TRUE_ &&
		oCreature->CREATURE_HAS_TIMER_ATTACK == 0)
	{
		oCreature->CREATURE_HAS_TIMER_ATTACK = 1; // applied only for first timer check
		fTimeout = 1.0; // quick timeout
	}

	if (!IsObjectValid(oCreature))
	{
#ifdef DEBUG
		Log_Trace_Scripting_Error("WR_AddCommand()", Log_GetCommandNameById(nCommandType) + " used on invalid AActor.");
#endif
		return;
	}

	if (nCommandType == COMMAND_TYPE_INVALID || nCommandType == 0)
	{
#ifdef DEBUG
		Log_Trace_Scripting_Error("WR_AddCommand()", "Invalid input parameter for cCommand (COMMAND_TYPE_INVALID)");
#endif
		LogWarning("Something is trying to add an invalid command from scripting (command_type 0). Please contact georg.)");
		return;
	}

#ifdef DEBUG
	Log_Trace_Commands("WR_AddCommand()", cCommand, oCreature);
#endif


#ifdef DEBUG
	Log_Trace(LOG_CHANNEL_COMMANDS, "WR_AddCommand", "*** START, AActor= " + GetTag(oCreature) + ". command= " + IntToString(nCommandType));
	if (bAddToFront)
		Log_Trace(LOG_CHANNEL_COMMANDS, "WR_AddCommand()", "*** Adding command to front");
	if (bStatic)
		Log_Trace(LOG_CHANNEL_COMMANDS, "WR_AddCommand()", "*** Command is static");
	if (fTimeout > 0.0)
		Log_Trace(LOG_CHANNEL_COMMANDS, "WR_AddCommand()", "*** Timeout: " + FloatToString(fTimeout));
#endif

	FCommand cCurrent = GetCurrentCommand(oCreature);
	int32 nCurrentType = GetCommandType(cCurrent);

#ifdef DEBUG
	if (nCurrentType == COMMAND_TYPE_INVALID)
		Log_Trace(LOG_CHANNEL_COMMANDS, "WR_AddCommand()", "*** No command is executing currently", oCreature);
	else
		Log_Trace(LOG_CHANNEL_COMMANDS, "WR_AddCommand()", "*** Current executing command is: " + IntToString(nCurrentType), oCreature);
#endif // DEBUG

	int32 nSize = GetCommandQueueSize(oCreature);
	if (nSize == 0)
	{
#ifdef DEBUG
		Log_Trace(LOG_CHANNEL_COMMANDS, "WR_AddCommand()", "*** Command queue is currently empty", oCreature);
#endif // DEBUG
	}
	else
	{
#ifdef DEBUG
		Log_Trace(LOG_CHANNEL_COMMANDS, "WR_AddCommand()", "*** Command queue size is: " + IntToString(nSize), oCreature);
#endif // DEBUG

		int32 i;
		//FCommand cCurrent;
		int32 nType;
		for (i = 0; i < nSize; i++)
		{
			cCurrent = GetCommandByIndex(oCreature, i);
			nType = GetCommandType(cCurrent);
#ifdef DEBUG
			Log_Trace(LOG_CHANNEL_COMMANDS, "WR_AddCommand()", "*** COMMAND[" + IntToString(i) + "]= " + IntToString(nType), oCreature);
#endif // DEBUG
		}
	}

#ifdef DEBUG
	Log_Trace(LOG_CHANNEL_COMMANDS, "*** ADDING NEW COMMAND (" + IntToString(nCommandType) + ")");

	if (GetCommandType(cCommand) == COMMAND_TYPE_ATTACK)
	{
		Log_Trace_Combat("wrappers_h.h", "AddCommand(Attack) called from WR_AddCommand() + on " + GetTag(oCreature));
	}
#endif // DEBUG

	if (fTimeout > 0.0 && !IsFollower(oCreature)) // followers can't have command timeout
		cCommand = SetCommandFloat(cCommand, fTimeout);

	AddCommand(oCreature, cCommand, bAddToFront, bStatic, nOverrideAddBehavior);

#ifdef DEBUG
	nCurrentType = GetCommandType(GetCurrentCommand(oCreature));

	if (nCurrentType == COMMAND_TYPE_INVALID)
		Log_Trace(LOG_CHANNEL_COMMANDS, "WR_AddCommand()", "*** VERIFY: No command is executing currently", oCreature);
	else
		Log_Trace(LOG_CHANNEL_COMMANDS, "WR_AddCommand()", "*** VERIFY: executing command is: " + IntToString(nCurrentType), oCreature);
#endif // DEBUG


}

void WR_SetObjectActive(AActor* aActor, int32 bActive, int32 nAnimation, int32 nVFX)
{
	if (GetObjectActive(aActor) == bActive)
	{
		Log_Trace(LOG_CHANNEL_SYSTEMS_WRAPPERS, "wrappers_h.WR_SetObjectActive",
			"Object already same state as requested - doing nothing");
		return;
	}
	// Handle default appear/disappear animation if one is not defined.
	if (nAnimation == -1)
	{
		nAnimation = 0;
		// Only use default if:
		//  - Object is a Creature
		//  - Creature is Alive
		//  - We are going from Active -> Inactive or Inactive -> Active
		if (GetObjectType(aActor) == OBJECT_TYPE_CREATURE && !IsDead(aActor) && GetObjectActive(aActor) != bActive)
		{
			if (bActive && GetM2DAInt(TABLE_APPEARANCE, "bHasAppearAnim", GetAppearanceType(aActor)))
			{
				nAnimation = 648;
			}
			else if (!bActive && GetM2DAInt(TABLE_APPEARANCE, "bHasDisappearAnim", GetAppearanceType(aActor)))
			{
				nAnimation = 649;
			}
		}
	}

	// Handle default appear/disappear VFX if one is not defined
	if (nVFX == -1)
	{
		if (bActive)
		{
			nVFX = GetM2DAInt(TABLE_APPEARANCE, "AppearVFX", GetAppearanceType(aActor));
		}
		else
		{
			nVFX = 0;
		}
	}

	SetObjectActive(aActor, bActive, nAnimation, nVFX);

	if (bActive)
	{
		SignalEvent(aActor, Event(EVENT_TYPE_OBJECT_ACTIVE));
	}

	Log_Trace(LOG_CHANNEL_SYSTEMS_WRAPPERS, "wrappers_h.WR_SetObjectActive",
		"bActive: " + IntToString(bActive) + ", nAnimation: " + IntToString(nAnimation) + ", nVFX: " + IntToString(nVFX),
		aActor);
}

int32 WR_TriggerPerception(AActor* oPerceiver, AActor* oPerceivedCreature)
{
	DropStealth(oPerceivedCreature);

#ifdef DEBUG
	FString sOut = oPerceiver->GetName() + " group: " + IntToString(GetGroupId(oPerceiver)) + " perceived ";
	sOut += oPerceivedCreature->GetName() + " group: " + IntToString(GetGroupId(oPerceivedCreature));
	Log_Trace(LOG_CHANNEL_EVENTS_PERCEPTION, " TriggerPerception ", sOut);
#endif

	return TriggerPerception(oPerceiver, oPerceivedCreature);
}

void WR_SetGameMode(int32 nGameMode)
{
	FGameEvent evModeChange = Event(EVENT_TYPE_SET_GAME_MODE);
	evModeChange = SetEventInteger(evModeChange, nGameMode);

	int32 nNewGameMode = GetEventInteger(evModeChange, 0);
	int32 nOldGameMode = GetEventInteger(evModeChange, 1);

	if (nNewGameMode == nOldGameMode) 
		return;

	Log_Trace(LOG_CHANNEL_SYSTEMS_WRAPPERS, "wrappers_h.WR_SetGameMode", "nGameMode: " + IntToString(nGameMode));

	SignalEvent(GetModule(), evModeChange);
}

int32 WR_GetPlotFlag(int64 nPlotHash, int32 nFlag)
{
#ifdef DEBUG
	//int32 nPlotName = GetPlotEntryName(nPlotName);
	FString sPlotName = GetPlotResRef(nPlotHash);
	//if ((sPlotName) == "") sPlotName = nPlotName;
	if (sPlotName == "")
	{
		LogError("WR_GetPlotFlag: didn't find plot name from hash " + IntToString(nPlotHash));
	}
#endif // DEBUG
	
	int32 nCurrent = GetPartyPlotFlag(nPlotHash, nFlag);

#ifdef DEBUG
	FString sFlagName = GetPlotFlagName(nPlotHash, nFlag);

	if (sFlagName == "")
	{
		LogError("WR_GetPlotFlag: didn't find plot flag name from hash " + IntToString(nPlotHash));
	}
#endif // DEBUG

#ifdef DEBUG	
	LogTrace(LOG_CHANNEL_PLOT, "GetPlot [" + sPlotName + "] ["
		+ sFlagName + "] = [" + IntToString(nCurrent) + "]");
	//Log_Trace_Plot("wrappers_h.WR_GetPartyPlotFlag", sPlotName, nFlag, nCurrent);
#endif // DEBUG

	return nCurrent;
}

void WR_SetPlotFlag(int64 nPlotHash, int32 nFlag, int32 nValue)
{
#ifdef DEBUG
	//int32 nPlotName = GetPlotEntryName(nPlotName);
	FString sPlotName = GetPlotResRef(nPlotHash);
	//if ((sPlotName) == "") sPlotName = nPlotName;
	if (sPlotName == "")
	{
		LogError("WR_SetPlotFlag: didn't find plot name from hash " + IntToString(nPlotHash));
	}
#endif // DEBUG

#ifdef DEBUG
	FString sFlagName = GetPlotFlagName(nPlotHash, nFlag);

	if (sFlagName == "")
	{
		LogError("WR_SetPlotFlag: didn't find plot flag name from hash " + IntToString(nPlotHash));
	}
#endif // DEBUG

#ifdef DEBUG
	LogTrace(LOG_CHANNEL_PLOT, "SetPlot [" + sPlotName + "] ["
		+ sFlagName + "] -> [" + IntToString(nValue) + "]");
#endif // DEBUG

	SetPlotFlag(nPlotHash, nFlag, nValue);
}