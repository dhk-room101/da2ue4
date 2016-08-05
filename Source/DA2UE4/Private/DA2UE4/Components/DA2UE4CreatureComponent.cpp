// Fill out your copyright notice in the Description page of Project Settings.

// -----------------------------------------------------------------------------
// creature_core
// -----------------------------------------------------------------------------
/*

Handles AI Creature (NPC) events.

Follower/Player events are handled in player_core.

Note: this script redirects all events not handled into rules_core.

*/
// -----------------------------------------------------------------------------
// Owner: Yaron Jakobs, Georg Zoeller
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//
//             L I M I T E D   E D I T   P E R M I S S I O N S
//
//      If you are not Georg or Yaron, you need permission to edit this file
//      and the changes have to be code reviewed before they are checked
//      in.
//
// -----------------------------------------------------------------------------

#include "DA2UE4.h"
#include "DA2UE4CreatureComponent.h"
#include "DA2UE4Creature.h"
#include "ldf.h"
#include "log_h.h"
#include "core_h.h"
#include "events_h.h"
#include "utility_h.h"
#include "ai_main_h.h"
#include "sys_rubberband_h.h"
#include "sys_ambient_h.h"
#include "commands_h.h"

void UDA2UE4CreatureComponent::EquipDefaultItem(AActor* OBJECT_SELF, int32 nAppType, int32 nTargetSlot, FString s2daColumnName)
{
	// Equip default creature item based on appearance (only if none equipped yet)
	AActor* oItem = GetItemInEquipSlot(nTargetSlot, OBJECT_SELF);
	FString rDefaultCreatureItem;
	AActor* oDefaultCreatureoItem;
	FString sItem;
	if (!IsObjectValid(oItem))
	{
		rDefaultCreatureItem = GetM2DAString(TABLE_APPEARANCE, s2daColumnName, nAppType);
		if (rDefaultCreatureItem == "****")
			rDefaultCreatureItem = sItem = s2daColumnName;
		else sItem = rDefaultCreatureItem;
#ifdef DEBUG
		Log_Trace(LOG_CHANNEL_EVENTS, "creature_core.EVENT_TYPE_SPAWN", "Default creature item for this appearance: [" + sItem + "] for inventory slot: " +
			IntToString(nTargetSlot));
#endif
		if (sItem != "")
		{
			oDefaultCreatureoItem = CreateItemOnObject(rDefaultCreatureItem, OBJECT_SELF);
			SetItemDroppable(oDefaultCreatureoItem, FALSE_);
			if (IsObjectValid(oDefaultCreatureoItem))
			{
#ifdef DEBUG
				Log_Trace(LOG_CHANNEL_EVENTS, "creature_core.EVENT_TYPE_SPAWN", "Creating default item on creature");
#endif
				EquipItem(OBJECT_SELF, oDefaultCreatureoItem, nTargetSlot);
			}
		}
	}
}

void UDA2UE4CreatureComponent::SignalCommandPending(ADA2UE4Creature* OBJECT_SELF, FCommand command)
{
	switch (command.nCommandType)
	{
	case COMMAND_TYPE_ATTACK:
	{
		OBJECT_SELF->currentCommand = SetCommandFloat(OBJECT_SELF->currentCommand, GetCurrentWorld()->GetTimeSeconds(), 6);

		if (OBJECT_SELF->currentCommand.nCommandType == COMMAND_TYPE_ATTACK)
		{
			OBJECT_SELF->currentCommand = SetCommandFloat(OBJECT_SELF->currentCommand, 2.63333, 7);
		}
		
		AActor* oTarget = GetCommandObject(command, 0);

		FGameEvent ev = Event(EVENT_TYPE_COMMAND_PENDING);
		ev = SetEventCreator(ev, OBJECT_SELF);
		ev = SetEventObject(ev, 0, OBJECT_SELF);
		ev = SetEventObject(ev, 1, oTarget);
		ev = SetEventInteger(ev, 0, COMMAND_TYPE_ATTACK); //command
		ev = SetEventInteger(ev, 1, COMMAND_TYPE_ATTACK); //subCommand - needed?

		SignalEvent(OBJECT_SELF, ev);

		break;
	}
	default: LogError("Unknown command type"); break;
	}
}

void UDA2UE4CreatureComponent::HandleCreatureEvent(ADA2UE4Creature* OBJECT_SELF)
{
	//event
	if (!OBJECT_SELF->bScriptHasCustom || OBJECT_SELF->bScriptCoreRedirected)
	{
		//if no event, return
		if (OBJECT_SELF->qEvents.Num() == 0) { return; }

#ifdef DEBUG
		LogWarning("creature component");
#endif // DEBUG

		FGameEvent ev = GetCurrentEvent(this->GetOwner());
		if (ev.nGameEventType == EVENT_TYPE_INVALID) return; //possible delayed events in queue
		int32 nEventType = GetEventType(ev);

#ifdef DEBUG
		ev = SetEventString(ev, 0, *(this->GetName()));
		Log_Events("", ev);
#endif

		if (IsPartyMember(OBJECT_SELF))
		{
			LogError("Party member " + OBJECT_SELF->GetName() + " firing events into creature_core. This is a critical bug. Please inform Yaron");
			//TODO Is HandleEventRef really needed?
			//HandleEventRef(ev, R"rules_core.ncs"); // fix it
			return;
		}

		switch (nEventType)
		{
		case EVENT_TYPE_SPAWN:
		{
			//TODO Implement EVENT_TYPE_SPAWN
#ifdef DEBUG
			LogWarning("I'm alive " + this->GetOwner()->GetName());
#endif
			// Creatures with a ranged weapon in their default slot, prefer
			// ranged weapon
			if (_AI_GetWeaponSetEquipped(OBJECT_SELF) == AI_WEAPON_SET_RANGED)
				_AI_SetFlag(OBJECT_SELF, AI_FLAG_PREFERS_RANGED, TRUE_);

			// -----------------------------------------------------------------
			// Force default item equip from the 2da
			// -----------------------------------------------------------------
			int32 nAppType = GetAppearanceType(OBJECT_SELF);
			EquipDefaultItem(OBJECT_SELF, nAppType, INVENTORY_SLOT_MAIN, "DefaultWeapon");
			EquipDefaultItem(OBJECT_SELF, nAppType, INVENTORY_SLOT_CHEST, "DefaultArmor");

			// Store start location for homing "rubberband" system
			Rubber_SetHome(OBJECT_SELF);

			// Check whether ambient behavior should start on spawn.
			Ambient_SpawnStart(OBJECT_SELF);

			// If has stealth and in combat and hostile to the player then trigger stealth
			if (HasAbility(OBJECT_SELF, ABILITY_TALENT_STEALTH) && GetCombatState(OBJECT_SELF) == FALSE_
				&& GetGroupHostility(GROUP_PC, GetGroupId(OBJECT_SELF)) == TRUE_)
			{
#ifdef DEBUG
				Log_Trace_AI(OBJECT_SELF, "creature_core", "TRIGGERING STEALTH");
#endif
				Ambient_Stop(OBJECT_SELF);
				FCommand cStealth = CommandUseAbility(ABILITY_TALENT_STEALTH, OBJECT_SELF);
				WR_AddCommand(OBJECT_SELF, cStealth);

			}

			// -----------------------------------------------------------------
			// Creature with roaming enabled.
			// -----------------------------------------------------------------           //
			float fRoamDistance = GetLocalFloat(OBJECT_SELF, ROAM_DISTANCE);
			if (fRoamDistance > 25.0f)
			{
				SetRoamRadius(OBJECT_SELF, fRoamDistance * SCALE_MULTIPLIER);
			}


			break;
		}
		case EVENT_TYPE_PERCEPTION_DISAPPEAR:
		{
			LogWarning("Creature Disappear " + Cast<ADA2UE4Creature>(GetEventObject(ev, 0))->PlayerState->PlayerName);
			break;
		}
		case EVENT_TYPE_DELAYED_SHOUT:
		{
#ifdef DEBUG
			Log_Trace(LOG_CHANNEL_EVENTS, "creature_core", "DELAYED SHOUT EVENT START", OBJECT_SELF, LOG_LEVEL_CRITICAL);
#endif

			// Triggering a shouts dialog with a delay
			int32 nShoutsActive = GetLocalInt(OBJECT_SELF, SHOUTS_ACTIVE);
			FString rDialogOverride = GetLocalResource(OBJECT_SELF, SHOUTS_DIALOG_OVERRIDE);
			float fDelay = GetLocalFloat(OBJECT_SELF, SHOUTS_DELAY);
			if (fDelay < 3.0)
				fDelay = 3.0; // so it doesn't happen too often

			if (!nShoutsActive)
				break;

			FString rDialog = "";
			if (rDialogOverride != "NONE")
				rDialog = rDialogOverride;

#ifdef DEBUG
			Log_Trace(LOG_CHANNEL_SYSTEMS, "creature_core_h.EVENT_TYPE_DELAYED_SHOUT", "delay= " + FloatToString(fDelay) + ", dialog override: " + rDialogOverride);
#endif


			// Check perception list: if no party members are in it - stop shouting
			TArray<AActor*> arPerceived = GetPerceivedCreatureList(OBJECT_SELF);
			int32 nSize = arPerceived.Num();
			int32 i;
			AActor* oCurrent;
			int32 bPerceingPartyMembers = FALSE_;

			for (i = 0; i < nSize; i++)
			{
				oCurrent = arPerceived[i];
				if (IsFollower(oCurrent))
				{
					bPerceingPartyMembers = TRUE_;
					break;
				}
			}

			if (!bPerceingPartyMembers)
			{
#ifdef DEBUG
				Log_Trace(LOG_CHANNEL_SYSTEMS, "creature_core_h.EVENT_TYPE_DELAYED_SHOUT", "Not perceiving any party members - stopping shouts");
#endif
				UT_SetShoutsFlag(OBJECT_SELF, FALSE_); // This will break the delayed event loop
			}
			else
			{
				UT_Talk(OBJECT_SELF, OBJECT_SELF, rDialog);
				DelayEvent(fDelay, OBJECT_SELF, ev);
			}

			// -------------------------------------------------------------
			// Event handled, do not fall through to rules_core
			// -------------------------------------------------------------
			//bEventHandled = TRUE_;
			break;
		}
		default:
		{
#ifdef DEBUG
			LogWarning("creature event not found, redirecting to rules");
#endif
			OBJECT_SELF->qEvents.Insert(ev, 0);//reinsert
			OBJECT_SELF->bScriptRulesRedirected = TRUE_;
			break;
		}
		}

		if (OBJECT_SELF->bScriptHasCustom)
			OBJECT_SELF->bScriptCoreRedirected = FALSE_;
	}

}

void UDA2UE4CreatureComponent::HandleCreatureCommand(ADA2UE4Creature* OBJECT_SELF)
{
	//command
	if (OBJECT_SELF->qCommands.Num() == 0)
	{
		if (OBJECT_SELF->currentCommand.nCommandType == COMMAND_TYPE_INVALID) return;
		else
		{
			if (OBJECT_SELF->currentCommand.nCommandStatus == COMMAND_IN_PROGRESS)
			{
				if (OBJECT_SELF->currentCommand.nCommandResult == COMMAND_RESULT_INVALID)
					return; // still in progress, no result yet
				else HandleCommandResult(OBJECT_SELF);
			}
			else
			{
				if (OBJECT_SELF->previousCommand.nCommandType == COMMAND_TYPE_ATTACK)
				{
					//add a wait
					FCommand cWait = CommandWait(2.63333);
					WR_AddCommand(OBJECT_SELF, cWait);
				}
				else LogError("no command queue, not invalid and not in progress: success/fail?");
			}
		}
	}
	else
	{
		if (OBJECT_SELF->currentCommand.nCommandType != COMMAND_TYPE_INVALID)
		{
			if (OBJECT_SELF->currentCommand.nCommandStatus == COMMAND_IN_PROGRESS)
			{
				if (OBJECT_SELF->currentCommand.nCommandResult == COMMAND_RESULT_INVALID)
					return; // still in progress, no result yet
				else HandleCommandResult(OBJECT_SELF);
			}
			//return;
			else LogError("command queue, not invalid and not in progress: success/fail?");
		}
		else
		{
			//time to move up the queue
			OBJECT_SELF->currentCommand = OBJECT_SELF->qCommands[0];
			OBJECT_SELF->currentCommand.nCommandStatus = COMMAND_IN_PROGRESS;
			OBJECT_SELF->qCommands.RemoveAt(0, 1);

#ifdef DEBUG
			LogWarning("Current command " + IntToString(OBJECT_SELF->currentCommand.nCommandType));
#endif // DEBUG

			//signal event
			SignalCommandPending(OBJECT_SELF, OBJECT_SELF->currentCommand);
		}
	}
}

void UDA2UE4CreatureComponent::HandleCommandResult(ADA2UE4Creature* OBJECT_SELF)
{
	if (OBJECT_SELF->currentCommand.nCommandType == COMMAND_TYPE_ATTACK)
	{
		float CommandStartTimestamp = GetCommandFloat(OBJECT_SELF->currentCommand, 6);
		float CommandDuration = GetCommandFloat(OBJECT_SELF->currentCommand, 7);
		if (GetCurrentWorld()->GetTimeSeconds() - CommandStartTimestamp < CommandDuration)
		{
			return; // not ready yet
		}
	}
	int32 nLastCommandType = OBJECT_SELF->currentCommand.nCommandType;
	int32 nCommandStatus = OBJECT_SELF->currentCommand.nCommandStatus;
	int32 nLastSubCommand = -1; //TODO seems Ability ID related
	AActor* oTarget = GetCommandObject(OBJECT_SELF->currentCommand, 0);
	AActor* oBlockingObject = nullptr; //TODO command oBlockingObject

    //do the switcheroo
	OBJECT_SELF->previousCommand = OBJECT_SELF->currentCommand;
	if (OBJECT_SELF->qCommands.Num() > 0)
	{
		OBJECT_SELF->currentCommand = OBJECT_SELF->qCommands[0];
		OBJECT_SELF->qCommands.RemoveAt(0, 1);
	}
	else OBJECT_SELF->currentCommand = FCommand(COMMAND_TYPE_INVALID);

	FGameEvent evCommandComplete = Event(EVENT_TYPE_COMMAND_COMPLETE);
	evCommandComplete = SetEventInteger(evCommandComplete, 0, nLastCommandType);
	evCommandComplete = SetEventInteger(evCommandComplete, 1, nCommandStatus);
	evCommandComplete = SetEventInteger(evCommandComplete, 2, nLastSubCommand);
	evCommandComplete = SetEventObject(evCommandComplete, 0, OBJECT_SELF); //oAttacker
	evCommandComplete = SetEventObject(evCommandComplete, 1, oTarget);
	evCommandComplete = SetEventObject(evCommandComplete, 2, oBlockingObject);

	SignalEvent(OBJECT_SELF, evCommandComplete);

}

// Called every frame
void UDA2UE4CreatureComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ADA2UE4Creature* OBJECT_SELF = Cast<ADA2UE4Creature>(this->GetOwner());

	//if player, return
	if (OBJECT_SELF->RANK == CREATURE_RANK_PLAYER) { return; }

	HandleCreatureEvent(OBJECT_SELF);
	HandleCreatureCommand(OBJECT_SELF);
}



