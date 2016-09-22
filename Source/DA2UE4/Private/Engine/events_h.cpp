#include "DA2UE4.h"
#include "commands_h.h"
#include "events_h.h"

void SendEventOnDelayedShout(AActor* oTarget)
{

	FGameEvent evOnDelayedShout = Event(EVENT_TYPE_DELAYED_SHOUT);
#ifdef DEBUG
	Log_Trace(LOG_CHANNEL_EVENTS, "events_h.SendEventOnDelayedShout", "", oTarget);
#endif

	SignalEvent(oTarget, evOnDelayedShout);
}

void SendEventHandleCustomAI(AActor* oTarget, AActor* oLastTarget, int32 nLastCommand, int32 nLastCommandStatus, int32 nLastSubCommand, int32 nAITargetType, int32 nAIParameter, int32 nTacticID)
{
#ifdef DEBUG
	Log_Msg(LOG_CHANNEL_EVENTS, "events_h.SendEventHandleCustomAI", oTarget);
#endif

	FGameEvent evHandleCustomAI = Event(EVENT_TYPE_HANDLE_CUSTOM_AI);
	evHandleCustomAI = SetEventObject(evHandleCustomAI, oLastTarget);
	evHandleCustomAI = SetEventInteger(evHandleCustomAI, nLastCommand);
	evHandleCustomAI = SetEventInteger(evHandleCustomAI, nLastCommandStatus);
	evHandleCustomAI = SetEventInteger(evHandleCustomAI, nLastSubCommand);
	evHandleCustomAI = SetEventInteger(evHandleCustomAI, nAITargetType);
	evHandleCustomAI = SetEventInteger(evHandleCustomAI, nAIParameter);
	evHandleCustomAI = SetEventInteger(evHandleCustomAI, nTacticID);

	SignalEvent(oTarget, evHandleCustomAI);
}

int32 Events_ValidateCommandPending(AActor* oCommandOwner, AActor* oTarget, int32 nCommandId, int32 nCommandSubType)
{
	if (!IsObjectValid(oCommandOwner))
	{
#ifdef DEBUG
		Log_Trace(LOG_CHANNEL_EVENTS, "events_h.ValidateCommandPending", "owner is INVALID!");
#endif
		SetCommandResult(oCommandOwner, COMMAND_RESULT_INVALID);
		return FALSE_;
	}

	if (IsObjectValid(oTarget) && (GetArea(oCommandOwner) != GetArea(oTarget)))
	{
#ifdef DEBUG
		Log_Trace(LOG_CHANNEL_EVENTS, "events_h.ValidateCommandPending", "Discarded Command Pending ... not in the same area as target");
#endif
		return FALSE_;
	}

	if (IsDead(oCommandOwner) || IsDying(oCommandOwner))
	{
#ifdef DEBUG
		Log_Trace(LOG_CHANNEL_EVENTS, "rules_core: (COMMAND_PENDING) - I'm dead or dying- ignoring event - ignoring event!");
#endif
		SetCommandResult(oCommandOwner, COMMAND_RESULT_INVALID);
		return FALSE_;
	}
	if (IsDisabled(oCommandOwner))
	{
#ifdef DEBUG
		Log_Trace(LOG_CHANNEL_EVENTS, "rules_core: (COMMAND_PENDING) -  I'm disabled - ignoring event!");
#endif
		SetCommandResult(oCommandOwner, COMMAND_RESULT_INVALID);
		return FALSE_;
	}

	if (!IsObjectValid(oTarget))
	{
#ifdef DEBUG
		LogWarning("rules_core, COMMAND_PENDING target is INVALID! - NOT stopping - it may be for a non-target ability (Berserk)");
		// NOT STOPPING - it may be Berserk, that has no target
#endif // DEBUG
	}
	else
		// -----------------------------------------------------------------
		// We can never attack or cast at a dying target, but
		// UseAbility (nCommandId=25) is potentially valid on dead people
		// -----------------------------------------------------------------
		if ((IsDying(oTarget) && nCommandId != COMMAND_TYPE_USE_ABILITY || (IsDead(oTarget)) && nCommandId != COMMAND_TYPE_USE_ABILITY))
		{
#ifdef DEBUG
			Log_Trace(LOG_CHANNEL_EVENTS, "rules_core, COMMAND_PENDING target is DEAD or DYING");
#endif
			SetCommandResult(oCommandOwner, COMMAND_RESULT_FAILED_NO_VALID_TARGET);
			return FALSE_;
		}
	return TRUE_;
}

FGameEvent EventSpellScriptDeactivate(AActor* oCaster, int32 nAbility, int32 nAbilityType)
{
	FGameEvent ev = Event(EVENT_TYPE_SPELLSCRIPT_DEACTIVATE);

	ev = SetEventObject(ev, oCaster);

	ev = SetEventInteger(ev, nAbility);
	ev = SetEventInteger(ev, nAbilityType);

	return ev;

}