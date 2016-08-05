#include "DA2UE4.h"
#include "wrappers_h.h"
#include "log_h.h"
#include "commands_h.h"
#include "events_h.h"
#include "sys_stealth_h.h"

int32 WR_ClearAllCommands(AActor* oActor, int32 bHardClear)
{
#ifdef DEBUG
	Log_Trace(LOG_CHANNEL_SYSTEMS_WRAPPERS, "wrappers_h.WR_ClearAllCommands", "", oActor);
#endif

	int32 nRet = ClearAllCommands(oActor, bHardClear);
	return nRet;
}

void WR_AddCommand(AActor* oActor, FCommand cCommand, int32 bAddToFront, int32 bStatic, int32 nOverrideAddBehavior, float fTimeout)
{
	int32 nCommandType = GetCommandType(cCommand);

	// timeout check
	// if a timeout was selected AND this is not a follower AND we're in combat AND it's the creatures first timer command
	// then shorten the timer so if the creature moves he'll stop faster and will then have a chance to reevaluate
	// his threat towards the party
	if (fTimeout > 0.0 && !IsFollower(oActor) && GetCombatState(oActor) == TRUE_ &&
		GetLocalInt(oActor, CREATURE_HAS_TIMER_ATTACK) == 0)
	{
		SetLocalInt(oActor, CREATURE_HAS_TIMER_ATTACK, 1); // applied only for first timer check
		fTimeout = 1.0; // quick timeout
	}

	if (!IsObjectValid(oActor))
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
	Log_Trace_Commands("WR_AddCommand()", cCommand, oActor);
#endif


#ifdef DEBUG
	Log_Trace(LOG_CHANNEL_COMMANDS, "WR_AddCommand", "*** START, AActor= " + GetTag(oActor) + ". command= " + IntToString(nCommandType));
	if (bAddToFront)
		Log_Trace(LOG_CHANNEL_COMMANDS, "WR_AddCommand()", "*** Adding command to front");
	if (bStatic)
		Log_Trace(LOG_CHANNEL_COMMANDS, "WR_AddCommand()", "*** Command is static");
	if (fTimeout > 0.0)
		Log_Trace(LOG_CHANNEL_COMMANDS, "WR_AddCommand()", "*** Timeout: " + FloatToString(fTimeout));
#endif

	FCommand cCurrent = GetCurrentCommand(oActor);
	int32 nCurrentType = GetCommandType(cCurrent);

#ifdef DEBUG
	if (nCurrentType == COMMAND_TYPE_INVALID)
		Log_Trace(LOG_CHANNEL_COMMANDS, "WR_AddCommand()", "*** No command is executing currently", oActor);
	else
		Log_Trace(LOG_CHANNEL_COMMANDS, "WR_AddCommand()", "*** Current executing command is: " + IntToString(nCurrentType), oActor);
#endif // DEBUG

	int32 nSize = GetCommandQueueSize(oActor);
	if (nSize == 0)
	{
#ifdef DEBUG
		Log_Trace(LOG_CHANNEL_COMMANDS, "WR_AddCommand()", "*** Command queue is currently empty", oActor);
#endif // DEBUG
	}
	else
	{
#ifdef DEBUG
		Log_Trace(LOG_CHANNEL_COMMANDS, "WR_AddCommand()", "*** Command queue size is: " + IntToString(nSize), oActor);
#endif // DEBUG

		int32 i;
		FCommand cCurrent;
		int32 nType;
		for (i = 0; i < nSize; i++)
		{
			cCurrent = GetCommandByIndex(oActor, i);
			nType = GetCommandType(cCurrent);
#ifdef DEBUG
			Log_Trace(LOG_CHANNEL_COMMANDS, "WR_AddCommand()", "*** COMMAND[" + IntToString(i) + "]= " + IntToString(nType), oActor);
#endif // DEBUG
		}
	}

#ifdef DEBUG
	Log_Trace(LOG_CHANNEL_COMMANDS, "*** ADDING NEW COMMAND (" + IntToString(nCommandType) + ")");

	if (GetCommandType(cCommand) == COMMAND_TYPE_ATTACK)
	{
		Log_Trace_Combat("wrappers_h.h", "AddCommand(Attack) called from WR_AddCommand() + on " + GetTag(oActor));
	}
#endif // DEBUG

	if (fTimeout > 0.0 && !IsFollower(oActor)) // followers can't have command timeout
		cCommand = SetCommandFloat(cCommand, fTimeout, 5);

	AddCommand(oActor, cCommand, bAddToFront, bStatic, nOverrideAddBehavior);

#ifdef DEBUG
	nCurrentType = GetCommandType(GetCurrentCommand(oActor));

	if (nCurrentType == COMMAND_TYPE_INVALID)
		Log_Trace(LOG_CHANNEL_COMMANDS, "WR_AddCommand()", "*** VERIFY: No command is executing currently", oActor);
	else
		Log_Trace(LOG_CHANNEL_COMMANDS, "WR_AddCommand()", "*** VERIFY: executing command is: " + IntToString(nCurrentType), oActor);
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
	Log_Trace(LOG_CHANNEL_SYSTEMS_WRAPPERS, "wrappers_h.WR_SetGameMode", "nGameMode: " + IntToString(nGameMode));

	FGameEvent evModeChange = Event(EVENT_TYPE_SET_GAME_MODE);
	evModeChange = SetEventInteger(evModeChange, 0, nGameMode);

	SignalEvent(GetModule(), evModeChange);
}