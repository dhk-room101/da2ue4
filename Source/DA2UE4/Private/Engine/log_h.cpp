#include "DA2UE4.h"
#include "DA2UE4Creature.h"
#include "log_h.h"
#include "events_h.h"
#include "commands_h.h"
#include "ability_h.h"

void Log_Trace(int32 nLogChannel, FString sOrigin, FString sMessage, AActor* oTarget, int32 nPriority)
{
#ifdef DEBUG
	if (LOG_ENABLED)
	{
		LogTrace(nLogChannel, "[" + sOrigin + "] " + sMessage, oTarget);
	}
#endif
}

void Log_Events(FString sOrigin, FGameEvent ev, FString sMessage, int32 nLogChannel)
{
#ifdef DEBUG
	if (!LOG_ENABLED)
		return;

	int32 nEventType = GetEventType(ev);
	FString sDetails;
	AActor* oTarget = nullptr;

	switch (nEventType)
	{
	case EVENT_TYPE_PERCEPTION_APPEAR:
	case EVENT_TYPE_PERCEPTION_DISAPPEAR:
	{
		if (GetEventObject(ev, 0)->IsA(ADA2UE4Creature::StaticClass()))
		{
			sDetails = "Object: " + Cast<ADA2UE4Creature>(GetEventObject(ev, 0))->PlayerState->PlayerName;
		}
		else LogError("log_h: EVENT_TYPE_PERCEPTION: unknown actor type");
		break;
	}
	default: break;
	}
	FString sFinalMessage = "[" + GetCurrentScriptName(ev) + ((sOrigin == "") ? "" : ".") + sOrigin + "] "
		+ Log_GetEventNameById(nEventType) + sDetails + " " + sMessage;

	LogTrace(nLogChannel, sFinalMessage, oTarget);
#endif
}

// -----------------------------------------------------------------------------
// Get a human readable FString for an event
// -----------------------------------------------------------------------------
FString Log_GetEventNameById(int32 nEventType)
{

	FString sRet = "";

#ifdef DEBUG
	sRet = GetM2DAString(TABLE_EVENTS, "Label", nEventType) + " (" + IntToString(nEventType) + ") ";
#endif

	return sRet;

}

void Log_Trace_Threat(AActor* OBJECT_SELF, FString sOrigin, FString sMessage, int32 nSeverity)
{
#ifdef DEBUG
	if (LOG_ENABLED)
	{
		FString sTag = GetTag(OBJECT_SELF) + "::";
		Log_Trace(LOG_CHANNEL_THREAT, sTag + sOrigin, sMessage, nullptr, nSeverity);
	}
#endif
}

FString Log_GetCommandNameById(int32 nCommandId)
{
	FString sRet = "";

#ifdef DEBUG
	sRet = GetM2DAString(TABLE_COMMANDS, "Label", nCommandId) + "(" + IntToString(nCommandId) + ") ";
#endif

	return sRet;

}

void Log_Trace_AI(AActor* aActor, FString sOrigin, FString sMessage, int32 nSeverity)
{
#ifdef DEBUG
	if (LOG_ENABLED)
	{
		FString sTag = GetTag(aActor) + "::";
		Log_Trace(LOG_CHANNEL_AI, sTag + sOrigin, sMessage, nullptr, nSeverity);
	}
#endif
}

void Log_Msg(int32 nLogChannel, FString sMessage, AActor* oTarget, int32 nPriority)
{
#ifdef DEBUG
	if (LOG_ENABLED)
	{
		LogTrace(nLogChannel, sMessage, oTarget);
	}
#endif
}

void Log_Trace_Scripting_Error(FString sOrigin, FString sMessage, AActor* oTarget)
{
#ifdef DEBUG
	if (LOG_ENABLED)
	{
		Log_Trace(LOG_CHANNEL_DESIGN_SCRIPTERROR, sOrigin, sMessage, oTarget);
	}
#endif
}

void Log_Trace_Commands(FString sOrigin, FCommand cmd, AActor* oTarget)
{
#ifdef DEBUG
	if (LOG_ENABLED)
	{
		int32 nCommandType = GetCommandType(cmd);
		Log_Trace(LOG_CHANNEL_COMMANDS, sOrigin, Log_GetCommandNameById(nCommandType), oTarget);
	}
#endif
}

void Log_Trace_Combat(FString sOrigin, FString sMessage, AActor* oAttacker, AActor* oTarget, int32 nLogChannel, int32 nSeverity)
{
#ifdef DEBUG
	if (LOG_ENABLED)
	{
		LogTrace(nLogChannel, sOrigin + " " + sMessage + (IsObjectValid(oAttacker) ? ". Attacker: " + GetTag(oAttacker) : ""), oTarget);
	}
#endif
}

void Log_Trace_Effects(FString sOrigin, FEffect eEffect, FString sMessage, AActor* oTarget, int32 nDurationType, int32 nAbilityId)
{
#ifdef DEBUG
	if (LOG_ENABLED)
	{
		int32    nEffectId = GetEffectType(eEffect);
		FString sCreator = GetEffectCreator(eEffect)->GetName();

		if (nDurationType == -1)
		{
			nDurationType = GetEffectDurationType(eEffect);
		}

		if (nAbilityId == -1)
		{
			nAbilityId = GetEffectAbilityID(eEffect);
		}

		FString sInfo = Log_GetEffectNameById(nEffectId) + ", duration: " + IntToString(nDurationType) + ", ability: " + IntToString(nAbilityId) + ", creator: " + sCreator;

		Log_Trace(LOG_CHANNEL_EFFECTS, sOrigin, sMessage + " " + sInfo, oTarget);

	}
#endif
}

FString Log_GetEffectNameById(int32 nEffectId)
{

	FString sRet = "";

#ifdef DEBUG
	sRet = GetM2DAString(TABLE_EFFECTS, "Label", nEffectId) + " (" + IntToString(nEffectId) + ") ";
#endif

	return sRet;
}

FString Log_GetAttackResultNameById(int32 nAttackResult)
{
#ifdef DEBUG
	if (!LOG_ENABLED)
	{
		return "";
	}

	if (nAttackResult == COMBAT_RESULT_HIT)
	{
		return IntToString(nAttackResult) + " = COMMAND_RESULT_HIT";
	}
	else if (nAttackResult == COMBAT_RESULT_CRITICALHIT)
	{
		return IntToString(nAttackResult) + " = COMMAND_RESULT_CRITICALHIT";
	}
	else if (nAttackResult == COMBAT_RESULT_DEATHBLOW)
	{
		return IntToString(nAttackResult) + " = COMMAND_RESULT_DEATHBLOW";
	}
	else if (nAttackResult == COMBAT_RESULT_MISS)
	{
		return IntToString(nAttackResult) + " = COMMAND_RESULT_MISS";
	}
	else if (nAttackResult == COMBAT_RESULT_BACKSTAB)
	{
		return IntToString(nAttackResult) + " = COMMAND_RESULT_BACKSTAB";
	}
	else
	{
		return IntToString(nAttackResult) + " = COMMAND_RESULT_INVALID";
	}
#endif

	return "";

}

