#include "DA2UE4.h"
#include "DA2UE4Creature.h"
#include "var_constants_h.h"
#include "log_h.h"
#include "events_h.h"
#include "utility_h.h"
#include "wrappers_h.h"
#include "sys_rubberband_h.h"
#include "sys_ambient_h.h"
#include "commands_h.h"

void Ambient_Stop(AActor* oCreature)
{
#ifdef DEBUG
	Log_Trace(LOG_CHANNEL_AMBIENT_AI, "Ambient_Stop()");
#endif // DEBUG

	int32 nAmbientState = GetLocalInt(oCreature, AMBIENT_SYSTEM_STATE);
	SetLocalInt(oCreature, AMBIENT_SYSTEM_STATE, nAmbientState & ~AMBIENT_SYSTEM_ENABLED);
}

void Ambient_CommandComplete(int32 nCommandType, int32 nCommandStatus, AActor* oCreature)
{
	if (IsPartyMember(oCreature) || GetCombatState(oCreature))
		return;

	if (nCommandType == COMMAND_TYPE_INTERACT || nCommandType == COMMAND_TYPE_PLAY_ANIMATION)
	{
		if (nCommandStatus == COMMAND_LOOPING)
			return;
		if (nCommandStatus == COMMAND_SUCCESSFUL)
		{
			int32 nAmbientState = GetLocalInt(oCreature, AMBIENT_SYSTEM_STATE);
			SetLocalInt(oCreature, AMBIENT_SYSTEM_STATE, nAmbientState & ~AMBIENT_SYSTEM_RUNNING);
		}
	}

	Ambient_DoSomething(oCreature);
}

void Ambient_DoSomething(AActor* oCreature, int32 bUpdateTimer)
{
	if (!GetObjectActive(oCreature) || IsDead(oCreature) || IsPartyMember(oCreature))
		return;

	int32 nAmbientState = GetLocalInt(oCreature, AMBIENT_SYSTEM_STATE);

	// Check whether ambient behavior is enabled.
	if (!(nAmbientState & AMBIENT_SYSTEM_ENABLED))
	{
		Log_Trace(LOG_CHANNEL_AMBIENT_AI, "Ambient_DoSomething()", "AMBIENT_SYSTEM_ENABLED bit not set (nAmbientState: " + IntToHexString(nAmbientState) + ")");
		return;
	}

	// Check if creature already doing or about to do something.
	if (GetCommandType(GetCurrentCommand(oCreature)) != COMMAND_TYPE_INVALID || GetCommandQueueSize(oCreature) > 0)
	{
		Log_Trace(LOG_CHANNEL_AMBIENT_AI, "Ambient_DoSomething()", "Busy - command(s) in queue: " + Log_GetCommandNameById(GetCommandType(GetCurrentCommand(oCreature))));
		return;
	}

	// The AMBIENT_SYSTEM_ALWAYSON flag skips timer and combat checks (ergo, use it sparingly).
	if (!(nAmbientState & AMBIENT_SYSTEM_ALWAYSON))
	{
		// Check whether creature or player is in combat
		if (GetGameMode() == GM_COMBAT || GetCombatState(oCreature))
		{
			Log_Trace(LOG_CHANNEL_AMBIENT_AI, "Ambient_DoSomething()", "Busy - in combat");
			return;
		}

		// Check whether ambient behavior should start/continue or time out.
		if (Ambient_TimerExpired(oCreature, bUpdateTimer))
			return;
	}

	if (Ambient_DoCommand(oCreature))
		return;

	//    if (GetGroupId(oCreature) == GROUP_HOSTILE)
	//        return;

	if (Ambient_ReachedDestination(oCreature))
	{
		// Play animations if in an animation phase, otherwise attempt a movement phase.
		if (GetLocalInt(oCreature, AMBIENT_ANIM_STATE) != AMBIENT_ANIM_NONE)
			Ambient_Animate(oCreature);
		else
			Ambient_Move(oCreature);
	}
	else
	{
		Log_Trace(LOG_CHANNEL_AMBIENT_AI, "Ambient_DoSomething()", "Do nothing");
	}
}

int32 Ambient_TimerExpired(AActor* oCreature, int32 bUpdateTimer)
{
	int32 nTimeNow = GetCurrentWorld()->GetTimeSeconds();
	int32 nTimeLast = GetLocalInt(oCreature, AMBIENT_TICK_COUNT);

	if (nTimeLast == 0)
		nTimeLast = nTimeNow;

	if (bUpdateTimer)
	{
		SetLocalInt(oCreature, AMBIENT_TICK_COUNT, nTimeNow);
	}
	else if (abs(nTimeNow - nTimeLast) > AMBIENT_PAUSE_PERIOD * 1000)
	{
		// Pause ambient behavior if party out of range for too long.
		Log_Trace(LOG_CHANNEL_AMBIENT_AI, "Ambient_TimerExpired()", "Pausing (" + IntToString(nTimeNow) + " - " + IntToString(nTimeLast) + " = " + IntToString(nTimeNow - nTimeLast));
		SetLocalInt(oCreature, AMBIENT_TICK_COUNT, 0);
		return TRUE_;
	}
	return FALSE_;
}

int32 Ambient_DoCommand(AActor* oCreature, int32 nCommand)
{
	if (nCommand == 0)
	{
		nCommand = GetLocalInt(oCreature, AMBIENT_COMMAND);
	}
	else
	{
		SetLocalInt(oCreature, AMBIENT_COMMAND, nCommand);

		// Hack: Originally AMBIENT_COMMAND didn't require the AMBIENT_SYSTEM_ENABLED bit set. 
		// Now it does, so flip the bit here.
		int32 nAmbientState = GetLocalInt(oCreature, AMBIENT_SYSTEM_STATE);
		SetLocalInt(oCreature, AMBIENT_SYSTEM_STATE, nAmbientState | AMBIENT_SYSTEM_ENABLED);
	}

	if (nCommand)
	{
		switch (nCommand)
		{
		case AMBIENT_COMMAND_ATTACK_PRACTICE:
			Ambient_DoCommandAttack(oCreature, RandomFloat() * 5.0f);
			break;
		case AMBIENT_COMMAND_ATTACK:
			Ambient_DoCommandAttack(oCreature, 0.05f);
			break;
		default:
			Log_Trace(LOG_CHANNEL_AMBIENT_AI, "Ambient_DoCommand()", "Unhandled AMBIENT_COMMAND value.");
			break;
		}
	}
	return nCommand;
}

void Ambient_DoCommandAttack(AActor* oAttacker, float fDelay)
{
	FGameEvent ev = GetLocalEvent(oAttacker, AMBIENT_EVENT); //GetCurrentEvent();
	switch (GetEventType(ev))
	{
		// Ambient_DoCommand() can be invoked by the event handler script of an
		// area or a creature. Hence the odd collection of events below.

	case EVENT_TYPE_AREALOAD_PRELOADEXIT:   // Invoked from area's event handler script.
	case EVENT_TYPE_AREALOAD_POSTLOADEXIT:  // Invoked from area's event handler script.
	case EVENT_TYPE_SPAWN:                  // Invoked from creature's event handler script.
	case EVENT_TYPE_OBJECT_ACTIVE:          // Invoked from creature's event handler script.
	case EVENT_TYPE_AMBIENT_CONTINUE:       // Invoked from creature's event handler script.
	{
		Log_Trace(LOG_CHANNEL_AMBIENT_AI, "Ambient_DoCommandAttack()", "Kickstarting " + GetTag(oAttacker));

		WR_AddCommand(oAttacker, CommandWait(fDelay));
		break;
	}

	case EVENT_TYPE_COMMAND_COMPLETE:       // Invoked from creature's event handler script.
	{
		switch (GetEventInteger(ev, 0))
		{
		case COMMAND_TYPE_WAIT:
		{
			// Attack target if player is wandering around. Otherwise, wait a bit.
			// Expected attacker tag format: XXXcr_XXX.
			// Expected target tag format:   XXXip_XXX_target.
			switch (GetGameMode())
			{
			case GM_EXPLORE:
			case GM_FIXED:
			case GM_FLYCAM:
			{
				//FString sTargetTag = ReplaceString(GetTag(oAttacker), "cr_", "ip_") + "_target";
				//TArray<AActor*> aTarget = GetNearestObjectByTag(oAttacker, sTargetTag);
				//DHK
				ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(oAttacker);
				TArray<AActor*> aTarget = GetNearestObjectByHostility(oAttacker, TRUE_, OBJECT_TYPE_CREATURE, oCreature->SensedPawns.Num());
				//END DHK

				if (aTarget.Num() == 0)
					aTarget = GetNearestObjectByTag(oAttacker, GetTag(oAttacker) + "_target");

				if (aTarget.Num() > 0)
				{
					if (IsDead(aTarget[0]))
						Log_Trace(LOG_CHANNEL_AMBIENT_AI, "Ambient_DoCommandAttack()", "WARNING: Target is dead - " + GetTag(aTarget[0]));

					// Only shoot if player isn't in the way.
					float fAngleToPlayer = GetAngleBetweenObjects(oAttacker, GetMainControlled());
					if ((fAngleToPlayer < 20.0 || fAngleToPlayer > 340.0) && GetDistanceBetween(oAttacker, GetMainControlled()) < GetDistanceBetween(oAttacker, aTarget[0]))
						WR_AddCommand(oAttacker, CommandWait(4.0f));
					else
						WR_AddCommand(oAttacker, CommandAttack(aTarget[0]));
				}
				else
				{
					Log_Trace(LOG_CHANNEL_AMBIENT_AI, "Ambient_DoCommandAttack()", "Failed to find valid target");
				}
				break;
			}
			default:
			{
				// Wait a bit till combat or conversation ends.
				WR_AddCommand(oAttacker, CommandWait(6.0f));
			}
			}
			break;
		}
		case COMMAND_TYPE_ATTACK:
		{
			Log_Trace(LOG_CHANNEL_AMBIENT_AI, "Ambient_DoCommandAttack()", "Wait.");

			// Wait a bit before attacking again.
			WR_AddCommand(oAttacker, CommandWait(fDelay));
			break;
		}
		}
		break;
	}
	}
}

int32 Ambient_ReachedDestination(AActor* oCreature)
{
	int32 nMovePattern = GetLocalInt(oCreature, AMBIENT_MOVE_PATTERN);
	int32 nRunChance = nMovePattern / 10;     // 2nd digit is the likelihood (0-9) of running (0 = 0%, 1 = 10%, etc).
	nMovePattern = nMovePattern % 10;     // 1st digit is the 'real' move pattern.
	int32 nMoveState = GetLocalInt(oCreature, AMBIENT_MOVE_STATE);
	int32 nWP = nMoveState & 0x0000FFFF;    // loword = The number of the AActor/waypoint last moved to.

	switch (nMovePattern)
	{
	case AMBIENT_MOVE_PATROL:
	case AMBIENT_MOVE_LOOP:
	case AMBIENT_MOVE_WARP:
	case AMBIENT_MOVE_ONCE:
	case AMBIENT_MOVE_RANDOM:
	case AMBIENT_MOVE_WANDER:
	case AMBIENT_MOVE_WANDER_FAR:
	{
		if (nWP)
		{
			AActor* oWP = Ambient_GetDestination(oCreature, nWP);
			if (IsObjectValid(oWP))
			{
				// Keep moving if too far from destination.
				if (GetDistanceBetween(oCreature, oWP) > AMBIENT_MOVE_TOLERANCE)
				{
					Log_Trace(LOG_CHANNEL_AMBIENT_AI, "Ambient_ReachedDestination()", "Moving to: " + GetTag(oWP));
					WR_AddCommand(oCreature, CommandWait(5.0f));
					WR_AddCommand(oCreature, CommandMoveToLocation(GetLocation(oWP), (nRunChance > Random(10))));
					return FALSE_;
				}
			}
		}
		break;
	}
	case AMBIENT_MOVE_PATH_PATROL:
	case AMBIENT_MOVE_PATH_LOOP:
	{
		int32 cWP = GetLocalInt(oCreature, AMBIENT_MOVE_COUNT);
		if (cWP == AMBIENT_MOVE_COUNT_INVALID)
		{
			TArray<AActor*> aWP = Ambient_GetDestinations(oCreature);
			cWP = aWP.Num();
			SetLocalInt(oCreature, AMBIENT_MOVE_COUNT, cWP);
		}

		if (cWP > 0 && (nWP + 1) < cWP)
		{
			Log_Trace(LOG_CHANNEL_AMBIENT_AI, "Ambient_ReachedDestination()", "nWP + 1 < cWP (" + IntToString(nWP) + " + 1 < " + IntToString(cWP) + ") -> not there yet");
			WR_AddCommand(oCreature, CommandWait(5.0f));
			Ambient_MovePath(oCreature, (nMovePattern == AMBIENT_MOVE_PATH_LOOP), (nRunChance > Random(10)));
			return FALSE_;
		}

		Log_Trace(LOG_CHANNEL_AMBIENT_AI, "Ambient_ReachedDestination()", "nWP + 1 >= cWP (" + IntToString(nWP) + " + 1 >= " + IntToString(cWP) + ") -> reached destination");
		Log_Trace(LOG_CHANNEL_AMBIENT_AI, "Ambient_ReachedDestination()", "AMBIENT_ANIM_STATE: " + IntToString(GetLocalInt(oCreature, AMBIENT_ANIM_STATE)));
		break;
	}
	}
	return TRUE_;
}

AActor* Ambient_GetDestination(AActor* oCreature, int32 n)
{
	//FString sWP = ReplaceString(GetLocalString(oCreature, AMBIENT_MOVE_PREFIX), "<tag>", GetTag(oCreature));
	FString sWP = (GetLocalString(oCreature, AMBIENT_MOVE_PREFIX)).Replace(TEXT("<tag>"), *(GetTag(oCreature)));
	AActor* oWP = GetObjectByTag(sWP + "_" + (n < 10 ? "0" : "") + IntToString(n));
	if (!IsObjectValid(oWP))
		oWP = GetObjectByTag(sWP, n - 1);   // subtract 1 since GetObjectByTag() is zero-based.

	if (!IsObjectValid(oWP))
		Log_Trace(LOG_CHANNEL_AMBIENT_AI, "Ambient_GetDestination()", "Failed to find valid destination: " + sWP);

	return oWP;
}

TArray<AActor*> Ambient_GetDestinations(AActor* oCreature)
{
	//FString   sWP = ReplaceString(GetLocalString(oCreature, AMBIENT_MOVE_PREFIX), "<tag>", GetTag(oCreature));
	FString sWP = (GetLocalString(oCreature, AMBIENT_MOVE_PREFIX)).Replace(TEXT("<tag>"), *(GetTag(oCreature)));
	TArray<AActor*> aWP;
	AActor*   oWP;
	int32      i;

	for (i = 0; IsObjectValid(oWP = GetObjectByTag(sWP + "_" + (i + 1 < 10 ? "0" : "") + IntToString(i + 1))); i++)
		aWP[i] = oWP;

	if (aWP.Num() == 0)
	{
		// Hack for non-existent engine function GetObjectsByTag()
		for (i = 0; IsObjectValid(oWP = GetObjectByTag(sWP, i)); i++)
			aWP[i] = oWP;
	}
	return aWP;
}

int32 Ambient_MovePath(AActor* oCreature, int32 bLoop, int32 bRun)
{
	TArray<AActor*> aWP = Ambient_GetDestinations(oCreature);
	int32 cWP = aWP.Num();

	if (cWP == 0)
	{
		Log_Trace(LOG_CHANNEL_AMBIENT_AI, "Ambient_MovePath()", "No valid destinations.");
		WR_AddCommand(oCreature, CommandWait(AMBIENT_SMALL_DELAY));
		return FALSE_;
	}

	int32    nMoveState = GetLocalInt(oCreature, AMBIENT_MOVE_STATE);
	int32    nWP = nMoveState & 0x0000FFFF;    // loword = The number of the AActor/waypoint last moved to.
	int32    nMoveDir = nMoveState & 0xFFFF0000;    // hiword = Direction of travel.

	if ((nWP + 1) >= cWP)
	{
		Log_Trace(LOG_CHANNEL_AMBIENT_AI, "Ambient_MovePath()", "nWP + 1 >= cWP (" + IntToString(nWP) + " + 1 >= " + IntToString(cWP) + ") -> changing direction");
		nMoveDir = (nMoveDir == AMBIENT_MOVE_NEXT) ? AMBIENT_MOVE_PREV : AMBIENT_MOVE_NEXT;
		nWP = 0;
	}

	TArray<FVector> lWP;
	int32        i;
	for (i = 0; i < cWP; i++)
	{
		if (!bLoop && nMoveDir == AMBIENT_MOVE_PREV)
			lWP[i] = GetLocation(aWP[cWP - i - 1]);
		else
			lWP[i] = GetLocation(aWP[i]);
	}

	Log_Trace(LOG_CHANNEL_AMBIENT_AI, "Ambient_MovePath()", "Moving to destination " + IntToString(nWP) + " of " + IntToString(cWP));
	SetLocalInt(oCreature, AMBIENT_MOVE_STATE, nMoveDir + nWP);
	WR_AddCommand(oCreature, CommandMoveToMultiLocations(lWP, bRun, nWP));
	return TRUE_;
}

void Ambient_Animate(AActor* oCreature)
{
	int32   nAnimsToDo = GetLocalInt(oCreature, AMBIENT_ANIM_STATE);
	int32   nAnimPattern = GetLocalInt(oCreature, AMBIENT_ANIM_PATTERN_OVERRIDE);
	float fAnimFreq = GetLocalFloat(oCreature, AMBIENT_ANIM_FREQ_OVERRIDE);

	// Decrement number of times to play override animations
	int32 nOverrideCount = GetLocalInt(oCreature, AMBIENT_ANIM_OVERRIDE_COUNT);
	if (nAnimsToDo == AMBIENT_ANIM_RESET && nOverrideCount > 0)
	{
		SetLocalInt(oCreature, AMBIENT_ANIM_OVERRIDE_COUNT, --nOverrideCount);
	}

	// If override count reaches zero then clear override animation pattern & frequency.
	if (nOverrideCount == 0)
	{
		int32   nAnimPattern = AMBIENT_ANIM_PATTERN_NONE;
		float fAnimFreq = AMBIENT_ANIM_FREQ_NONE;
		SetLocalInt(oCreature, AMBIENT_ANIM_PATTERN_OVERRIDE, AMBIENT_ANIM_PATTERN_NONE);
		SetLocalFloat(oCreature, AMBIENT_ANIM_FREQ_OVERRIDE, AMBIENT_ANIM_FREQ_NONE);
	}

	// Load default animation pattern and frequency if no override values
	if (nAnimPattern == AMBIENT_ANIM_PATTERN_NONE)
		nAnimPattern = GetLocalInt(oCreature, AMBIENT_ANIM_PATTERN);
	if (fabs(fAnimFreq) < AMBIENT_SMALL_DELTA)
		fAnimFreq = GetLocalFloat(oCreature, AMBIENT_ANIM_FREQ);

	if (nAnimPattern <= 0)
	{
		// No animation pattern so trigger movement if movement pattern specified.
		SetLocalInt(oCreature, AMBIENT_ANIM_STATE, AMBIENT_ANIM_NONE);
		if (GetLocalInt(oCreature, AMBIENT_MOVE_PATTERN) % 10)
		{
			Log_Trace(LOG_CHANNEL_AMBIENT_AI, "Ambient_Animate()", "No AMBIENT_ANIM_PATTERN");
			WR_AddCommand(oCreature, CommandWait(AMBIENT_SMALL_DELAY));
			return;
		}

		// Creature has no animation pattern or move pattern assigned so default
		// to playing random idle twitches (otherwise creature remains frozen in
		// last animation when returning from staged conversation).
		Log_Trace(LOG_CHANNEL_AMBIENT_AI, "Ambient_Animate()", " *** No AMBIENT_ANIM_PATTERN or AMBIENT_MOVE_PATTERN ***");

		nAnimsToDo = AMBIENT_ANIM_RESET;
		nAnimPattern = AMBIENT_ANIM_PATTERN_TWITCHES;
		fAnimFreq = AMBIENT_ANIM_FREQ_RANDOM;

		SetLocalInt(oCreature, AMBIENT_ANIM_PATTERN, nAnimPattern);
		SetLocalFloat(oCreature, AMBIENT_ANIM_FREQ, fAnimFreq);
	}

	if (fabs(fAnimFreq) < AMBIENT_SMALL_DELTA)
	{
		Log_Trace(LOG_CHANNEL_AMBIENT_AI, "Ambient_Animate()", "No AMBIENT_ANIM_FREQ");

		// Trigger movement if valid (check first to prevent endless cycling).
		SetLocalInt(oCreature, AMBIENT_ANIM_STATE, AMBIENT_ANIM_NONE);
		if (GetLocalInt(oCreature, AMBIENT_MOVE_PATTERN) % 10)
			WR_AddCommand(oCreature, CommandWait(AMBIENT_SMALL_DELAY));
		return;
	}

	if (nAnimsToDo == AMBIENT_ANIM_RESET)
	{
		// Start of animation phase - determine number to play.
		// fAnimFreq format: min.max (-1.0 to play all in sequence)
		if (fAnimFreq < 0.0f)
		{
			// -1.0 = play all animations (AMBIENT_ANIM_FREQ_ORDERED)
			nAnimsToDo = GetM2DAInt(TABLE_AMBIENT, COL_AMBIENT_ANIM_TOTAL, nAnimPattern);
		}
		else
		{
			// Choose random number of animations
			int32 nMin = FloatToInt(fAnimFreq);
			int32 nMax = FloatToInt((fAnimFreq - nMin) * 10);
			nAnimsToDo = Random(abs(nMax - nMin)) + Min(nMin, nMax);
		}
	}

	if (nAnimsToDo == 0)
	{
		// No animations - trigger movement if valid (check first to prevent endless cycling).
		SetLocalInt(oCreature, AMBIENT_ANIM_STATE, AMBIENT_ANIM_NONE);
		if (GetLocalInt(oCreature, AMBIENT_MOVE_PATTERN) % 10)
			WR_AddCommand(oCreature, CommandWait(AMBIENT_SMALL_DELAY));
		return;
	}

	if (nAnimsToDo > 0)     // Pick and play an animation from 2DA
	{
		int32 nAnims = GetM2DAInt(TABLE_AMBIENT, COL_AMBIENT_ANIM_TOTAL, nAnimPattern);
		if (nAnims == 0)
		{
			Log_Trace(LOG_CHANNEL_AMBIENT_AI, "Ambient_Animate()", "No animations for nAnimPattern: " + IntToString(nAnimPattern));

			// Trigger movement if valid (check first to prevent endless cycling).
			SetLocalInt(oCreature, AMBIENT_ANIM_STATE, AMBIENT_ANIM_NONE);
			if (GetLocalInt(oCreature, AMBIENT_MOVE_PATTERN) % 10)
				WR_AddCommand(oCreature, CommandWait(AMBIENT_SMALL_DELAY));
			return;
		}

		float fAction = 0.0f;   // Action format: animation.loops
		if (fAnimFreq < 0.0f)
		{
			// If pattern has changed, we may have to reset if it has less
			// animations than before
			if (nAnimsToDo > nAnims)
			{
				nAnimsToDo = nAnims;
			}

			// Select next action in sequence
			fAction = GetM2DAFloat(TABLE_AMBIENT, COL_AMBIENT_ANIM + IntToString(nAnims - nAnimsToDo), nAnimPattern);
		}
		else
		{
			// Select random action
			fAction = GetM2DAFloat(TABLE_AMBIENT, COL_AMBIENT_ANIM + IntToString(Random(nAnims)), nAnimPattern);
		}

		// If fAction is positive then it's an animation, otherwise it's a placeable interaction.
		if (fAction > 0.0)
		{
			// Parse animation (left of decimal) and number of loops (right of decimal) from action.
			int32 nAnimation = FloatToInt(fAction);
			int32 nLoops = FloatToInt((fAction - nAnimation) * 100);

			if (nAnimation > 0)
			{
				int32 bBlend = !(GetLocalInt(oCreature, AMBIENT_SYSTEM_STATE) & AMBIENT_SYSTEM_NOBLEND);

				// If NoPlayNext bit is set on the creature, respect it. Otherwise, use value set in 2da.
				int32 bPlayNext = !(GetLocalInt(oCreature, AMBIENT_SYSTEM_STATE) & AMBIENT_SYSTEM_NOPLAYNEXT);
				if (bPlayNext)
					bPlayNext = !(GetM2DAInt(TABLE_AMBIENT, COL_AMBIENT_ANIM_NOPLAYNEXT, nAnimPattern));
				if (nAnimation == 1)
					bPlayNext = TRUE_;

				Log_Trace(LOG_CHANNEL_AMBIENT_AI, "Ambient_Animate()", "Animation: " + IntToString(nAnimation)
					+ ((nLoops > 0) ? (", Loops: " + IntToString(nLoops)) : (""))
					+ ", PlayNext: " + IntToString(bPlayNext) + ", Blend: " + IntToString(bBlend)
					+ ", nAnimsToDo: " + IntToString(nAnimsToDo) + ", nAnimsTotal: " + IntToString(nAnims)
					+ ", Pattern: " + IntToString(nAnimPattern));

				WR_AddCommand(oCreature, CommandPlayAnimation(nAnimation, (nLoops >= AMBIENT_LOOP_FOREVER) ? -1 : nLoops, bPlayNext, bBlend));

				// Mark as busy to prevent generation of EVENT_TYPE_AMBIENT_CONTINUE events by engine.
				if (nLoops > 0)
				{
					int32 nAmbientState = GetLocalInt(oCreature, AMBIENT_SYSTEM_STATE);
					SetLocalInt(oCreature, AMBIENT_SYSTEM_STATE, nAmbientState | AMBIENT_SYSTEM_RUNNING);
				}
			}
		}
		else
		{
			// Parse interaction (left of decimal) and number of loops (right of decimal) from action.
			int32 nInteraction = FloatToInt(fabs(fAction));
			int32 nLoops = FloatToInt((fabs(fAction) - nInteraction) * 100);
			int32 nPose = 1;

			if (nInteraction > 0)
			{
				TArray<AActor*> aPlc = GetNearestObject(oCreature, OBJECT_TYPE_PLACEABLE);
				if (aPlc.Num() > 0)
				{
					AActor* oTarget = aPlc[0];
					int32 bPlayNext = !(GetLocalInt(oCreature, AMBIENT_SYSTEM_STATE) & AMBIENT_SYSTEM_NOPLAYNEXT);
					int32 bBlend = !(GetLocalInt(oCreature, AMBIENT_SYSTEM_STATE) & AMBIENT_SYSTEM_NOBLEND);

					Log_Trace(LOG_CHANNEL_AMBIENT_AI, "Ambient_Animate()", "Interaction: " + IntToString(nInteraction)
						+ ((nLoops > 0) ? (", Loops: " + IntToString(nLoops)) : (""))
						+ ", PlayNext: " + IntToString(bPlayNext) + ", Blend: " + IntToString(bBlend)
						+ ", nAnimsToDo: " + IntToString(nAnims - nAnimsToDo) + "/" + IntToString(nAnims)
						+ ", Pattern: " + IntToString(nAnimPattern));

					InteractWithObject(oCreature, oTarget, nInteraction, 1, (nLoops >= AMBIENT_LOOP_FOREVER) ? -1 : nLoops, bPlayNext, FALSE_);

					// Mark as busy to prevent generation of EVENT_TYPE_AMBIENT_CONTINUE events by engine.
					if (nLoops > 0)
					{
						int32 nAmbientState = GetLocalInt(oCreature, AMBIENT_SYSTEM_STATE);
						SetLocalInt(oCreature, AMBIENT_SYSTEM_STATE, nAmbientState | AMBIENT_SYSTEM_RUNNING);
					}
				}
			}
		}
	}

	// Decrement number of animations remaining to play.
	// A movement phase is triggered when no more animations remain.
	SetLocalInt(oCreature, AMBIENT_ANIM_STATE, --nAnimsToDo);
}

void Ambient_Move(AActor* oCreature)
{
	int32 bMoving = FALSE_;

	int32 nMovePattern = GetLocalInt(oCreature, AMBIENT_MOVE_PATTERN);

	int32 nRunChance = nMovePattern / 10;     // 2nd digit is the likelihood (0-9) of running (0 = 0%, 1 = 10%, etc).
	int32 bRun = (nRunChance > Random(9));

	nMovePattern = nMovePattern % 10;     // 1st digit is the 'real' move pattern.

	switch (nMovePattern)
	{
	case AMBIENT_MOVE_NONE:
		Log_Trace(LOG_CHANNEL_AMBIENT_AI, "Ambient_Move()", "No AMBIENT_MOVE_PATTERN");

		// Not moving so do animation phase instead.
		if ((GetLocalInt(oCreature, AMBIENT_ANIM_PATTERN) || GetLocalInt(oCreature, AMBIENT_ANIM_PATTERN_OVERRIDE)))
		{
			SetLocalInt(oCreature, AMBIENT_ANIM_STATE, AMBIENT_ANIM_RESET);
			Ambient_Animate(oCreature);
			return;
		}
		break;
	case AMBIENT_MOVE_PATROL:
	case AMBIENT_MOVE_LOOP:
	case AMBIENT_MOVE_WARP:
	case AMBIENT_MOVE_ONCE:
	case AMBIENT_MOVE_RANDOM:
		bMoving = Ambient_WalkWaypoints(oCreature, nMovePattern, bRun);
		break;
	case AMBIENT_MOVE_WANDER:
		bMoving = Ambient_MoveRandom(oCreature, AMBIENT_RANGE_NEAR, bRun);
		break;
	case AMBIENT_MOVE_WANDER_FAR:
		bMoving = Ambient_MoveRandom(oCreature, AMBIENT_RANGE_FAR, bRun);
		break;
	case AMBIENT_MOVE_PATH_PATROL:
	case AMBIENT_MOVE_PATH_LOOP:
		bMoving = Ambient_MovePath(oCreature, (nMovePattern == AMBIENT_MOVE_PATH_LOOP), bRun);
		break;
	default:
		Log_Trace(LOG_CHANNEL_AMBIENT_AI, "Ambient_Move()", "Invalid AMBIENT_MOVE_PATTERN");

		// Trigger animation phase if valid and not already doing looping animation (check first to prevent endless cycling).
		if ((GetLocalInt(oCreature, AMBIENT_ANIM_PATTERN) || GetLocalInt(oCreature, AMBIENT_ANIM_PATTERN_OVERRIDE)))
			WR_AddCommand(oCreature, CommandWait(AMBIENT_SMALL_DELAY));
	}
	/*
	if (bMoving)
	{
	// Mark as busy to prevent generation of EVENT_TYPE_AMBIENT_CONTINUE events by engine.
	int32 nAmbientState = GetLocalInt(oCreature, AMBIENT_SYSTEM_STATE);
	SetLocalInt(oCreature, AMBIENT_SYSTEM_STATE, nAmbientState | AMBIENT_SYSTEM_RUNNING);
	}
	*/
	// Movement phase done - animation phase is next.
	SetLocalInt(oCreature, AMBIENT_ANIM_STATE, AMBIENT_ANIM_RESET);
}

int32 Ambient_WalkWaypoints(AActor* oCreature, int32 nMovementPattern, int32 bRun)
{
	int32 cWP = GetLocalInt(oCreature, AMBIENT_MOVE_COUNT);   // Number of destinations available to move to.
	if (cWP == AMBIENT_MOVE_COUNT_INVALID)
	{
		// Calculate and cache destination count
		cWP = Ambient_GetDestinations(oCreature).Num();
		SetLocalInt(oCreature, AMBIENT_MOVE_COUNT, cWP);
		SetLocalInt(oCreature, AMBIENT_MOVE_STATE, AMBIENT_MOVE_NEXT);
	}

	if (cWP == 0)
	{
		Log_Trace(LOG_CHANNEL_AMBIENT_AI, "Ambient_WalkWaypoints()", "No valid destinations.");
		WR_AddCommand(oCreature, CommandWait(AMBIENT_SMALL_DELAY));
		return FALSE_;
	}

	int32    nMoveState = GetLocalInt(oCreature, AMBIENT_MOVE_STATE);
	int32    nWP = nMoveState & 0x0000FFFF;    // loword = The number of the AActor/waypoint last moved to.
	int32    nMoveDir = nMoveState & 0xFFFF0000;    // hiword = Direction of travel.
	int32    bJump = FALSE_;

	/*
	if (nWP == cWP && cWP == 1)
	{
	// There's only 1 ambient waypoint and creature is already at it.
	WR_AddCommand(oCreature, CommandWait(AMBIENT_SMALL_DELAY));
	return FALSE_;
	}
	*/

	// Figure out next destination.
	switch (nMovementPattern)
	{
	case AMBIENT_MOVE_PATROL:     // 1,2,3,2,1,2,3, etc
	{
		if (nMoveDir == AMBIENT_MOVE_NEXT)
		{
			nWP++;
			if (nWP > cWP)
			{
				nWP -= 2;
				nMoveDir = AMBIENT_MOVE_PREV;
			}
		}
		else
		{
			nWP--;
			if (nWP < 1)
			{
				nWP += 2;
				nMoveDir = AMBIENT_MOVE_NEXT;
			}
		}
		break;
	}
	case AMBIENT_MOVE_LOOP:       // 1,2,3,1,2,3, etc
	{
		nWP++;
		if (nWP > cWP)
			nWP = 1;
		break;
	}
	case AMBIENT_MOVE_WARP:       // 1,2,3,jump to 1,2,3, etc
	{
		nWP++;
		if (nWP > cWP)
		{
			nWP = 1;
			bJump = TRUE_;
		}
		break;
	}
	case AMBIENT_MOVE_ONCE:
	{
		nWP++;
		if (nWP > cWP)
		{
			// Reached last waypoint.
			SetLocalInt(oCreature, AMBIENT_MOVE_PATTERN, AMBIENT_MOVE_NONE);
			SetLocalInt(oCreature, AMBIENT_ANIM_STATE, AMBIENT_ANIM_RESET);
			return FALSE_;
		}
		break;
	}
	case AMBIENT_MOVE_RANDOM:     // any destination other than current one
	{
		int32 r = Random(cWP) + 1;
		while (cWP > 1 && r == nWP)
			r = Random(cWP) + 1;
		nWP = r;
		break;
	}
	default:
	{
		Log_Trace(LOG_CHANNEL_AMBIENT_AI, "Ambient_WalkWaypoints()", "Invalid movement pattern: " + IntToString(nMovementPattern));
	}
	}

	AActor* oWP = Ambient_GetDestination(oCreature, nWP);
	if (IsObjectValid(oWP))
	{
		SetLocalInt(oCreature, AMBIENT_MOVE_STATE, nMoveDir + nWP);

		if (bJump)
		{
			Log_Trace(LOG_CHANNEL_AMBIENT_AI, "Ambient_WalkWaypoints()", "Jump to: " + GetTag(oWP));
			WR_AddCommand(oCreature, CommandJumpToLocation(GetLocation(oWP)));
		}
		else
		{
			Log_Trace(LOG_CHANNEL_AMBIENT_AI, "Ambient_WalkWaypoints()", "Move to: " + GetTag(oWP));
			WR_AddCommand(oCreature, CommandMoveToLocation(GetLocation(oWP), bRun));
		}
		return TRUE_;

	}

	// Found no valid location to move to
	WR_AddCommand(oCreature, CommandWait(AMBIENT_SMALL_DELAY));
	SetLocalInt(oCreature, AMBIENT_MOVE_STATE, AMBIENT_MOVE_NEXT);
	SetLocalInt(oCreature, AMBIENT_MOVE_COUNT, AMBIENT_MOVE_COUNT_INVALID); // Force recount of destinations next invocation
	return FALSE_;
}

int32 Ambient_MoveRandom(AActor* oCreature, int32 nRange, int32 bRun)
{
	FVector vHome = Rubber_GetHome(oCreature);
	FVector lDest = FVector(vHome.X + RandFF(2 * nRange, -nRange), vHome.Y + RandFF(2 * nRange, -nRange), vHome.Z);
	WR_AddCommand(oCreature, CommandMoveToLocation(lDest, bRun));
	return TRUE_;
}

void Ambient_SpawnStart(AActor* oCreature)
{
	if (!GetObjectActive(oCreature) || GetObjectType(oCreature) != OBJECT_TYPE_CREATURE || IsDead(oCreature) || IsDying(oCreature) || IsPartyMember(oCreature))
		return;

	int32 nAmbientState = GetLocalInt(oCreature, AMBIENT_SYSTEM_STATE);

	// Hack: Originally AMBIENT_COMMAND didn't require the AMBIENT_SYSTEM_ENABLED bit set. Now it does, so flip the bit here.
	if (GetLocalInt(oCreature, AMBIENT_COMMAND))
	{
		nAmbientState |= AMBIENT_SYSTEM_SPAWNSTART;
	}

	if (nAmbientState & AMBIENT_SYSTEM_SPAWNSTART)
	{
		// Start ambient behavior.
		nAmbientState |= AMBIENT_SYSTEM_ENABLED;
		SetLocalInt(oCreature, AMBIENT_SYSTEM_STATE, nAmbientState);
		SetLocalInt(oCreature, AMBIENT_ANIM_STATE, AMBIENT_ANIM_RESET);

		Log_Trace(LOG_CHANNEL_AMBIENT_AI, "Ambient_SpawnStart()", "AMBIENT_SYSTEM_STATE: " + IntToHexString(nAmbientState));

		Ambient_DoSomething(oCreature);
	}
}
