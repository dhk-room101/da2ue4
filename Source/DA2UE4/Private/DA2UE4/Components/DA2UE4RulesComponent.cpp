// Fill out your copyright notice in the Description page of Project Settings.

// -----------------------------------------------------------------------------
// rules_core.nss
// -----------------------------------------------------------------------------
//
// This is the core creature event scripts handling any event not handled
// in a previous script.
//
// It is the main entry point into combat AI, ability use and any event that
// is global to all creatures.
//
//
// Event Chain:
//                             / player_core (followers) \
//      Custom creature script                            > rules_core
//                             \ creature_core           /
//
//
// Note: certain events bypass the chain and process directly in here:
//
//  EFFECT_APPLIED
//  EFECT_UNAPPLIED
//  IMPACT
//
// -----------------------------------------------------------------------------
// Owner: Georg Zoeller.
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
#include "DA2UE4RulesComponent.h"
#include "DA2UE4Creature.h"
#include "ldf.h"
#include "log_h.h"
#include "core_h.h"
#include "utility_h.h"
#include "sys_stealth_h.h"
#include "combat_h.h"
#include "wrappers_h.h"
#include "ai_main_h.h"
#include "sys_soundset_h.h"
#include "sys_ambient_h.h"
#include "commands_h.h"
#include "ability_h.h"

/*#include "rules_h.h"
#include "events_h.h"
#include "effects_h.h"
#include "ui_h.h"
#include "sys_itemprops_h.h"
#include "approval_h.h"*/

// Sets default values for this component's properties
UDA2UE4RulesComponent::UDA2UE4RulesComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UDA2UE4RulesComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

}


// Called every frame
void UDA2UE4RulesComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ADA2UE4Creature* OBJECT_SELF = Cast<ADA2UE4Creature>(this->GetOwner());

	if (OBJECT_SELF->bScriptRulesRedirected)
	{
		if (OBJECT_SELF->qEvents.Num() == 0) { return; }

#ifdef DEBUG
		LogWarning("rules component");
#endif // DEBUG

		FGameEvent ev = GetCurrentEvent(this->GetOwner());
		if (ev.nGameEventType == EVENT_TYPE_INVALID) return; //possible delayed events in queue
		int32 nEventType = GetEventType(ev);

#ifdef DEBUG
		FString sMessage = FloatToString(GetCurrentWorld()->GetTimeSeconds()) + " DA2UE4RulesComponent";
		ev = SetEventString(ev, 0, sMessage);
		Log_Events("", ev);
#endif

		switch (nEventType)
		{
		case EVENT_TYPE_PERCEPTION_APPEAR:
		{
			ADA2UE4Creature* oAppear = Cast<ADA2UE4Creature>(GetEventObject(ev, 0));
			int32 nHostile = GetEventInteger(ev, 0); // if hostile or not (TRUE_/FALSE_)
			int32 bStealthed = GetEventInteger(ev, 1); // if stealthed or not (TRUE_/FALSE_)
			int32 nHostilityChanged = GetEventInteger(ev, 2); // if the creature was already perceived but just changed hostility (TRUE_/FALSE_)
			int32 nSize;
			int32 i;
			AActor* oAlly;
			float fCombatDelay; // before starting combat;
			int32 nRand;

			// If it's a hostile creature lying on the ground and it perceives a party member
			// then it spawn as non-hostile. Need to turn him hostile now
			if (GetLocalInt(OBJECT_SELF, SPAWN_HOSTILE_LYING_ON_GROUND) == 1 && GetGroupId(OBJECT_SELF) == GROUP_HOSTILE_ON_GROUND)
			{
				if (GetGroupId(oAppear) == GROUP_PC)
				{
					UT_CombatStart(OBJECT_SELF, oAppear);
					SetLocalInt(OBJECT_SELF, SPAWN_HOSTILE_LYING_ON_GROUND, 0); // disable it for next perception
				}
			}

			//if it is a creature that has its GO_HOSTILE_ON_PERCEIVE_PC variable set - go hostile and attack
			if (GetLocalInt(OBJECT_SELF, GO_HOSTILE_ON_PERCEIVE_PC) == 1)
			{
				if (GetGroupId(oAppear) == GROUP_PC)
				{
#ifdef DEBUG
					Log_Trace(LOG_CHANNEL_EVENTS_PERCEPTION, "rules_core", "PC Group Perceived - Changing from Neutral to Hostile");
#endif
					//Do Once Only
					SetLocalInt(OBJECT_SELF, GO_HOSTILE_ON_PERCEIVE_PC, 0);
					//Go hostile
					UT_CombatStart(OBJECT_SELF, oAppear);
				}

			}


			if (bStealthed)
			{
				// -------------------------------------------------------------
				// Stealth Check
				// -------------------------------------------------------------
				if (IsFollower(oAppear) && !Stealth_CheckSuccess(oAppear, OBJECT_SELF))
				{
					//WR_TriggerPerception(OBJECT_SELF,oAppear);
					DropStealth(oAppear);
#ifdef DEBUG
					Log_Trace(LOG_CHANNEL_EVENTS_PERCEPTION, "rules_core", "Stealth Check failed!");
#endif
				}
				else
				{
#ifdef DEBUG
					Log_Trace(LOG_CHANNEL_EVENTS_PERCEPTION, "rules_core", "Stealth Check Success!");
#endif
				}

				//return; //TODO revisit Rules_core stealth check return
			}

			if (nHostilityChanged)
			{
				//re-check hostility between self and creature.
				//The nHostile flag will not be correct in this case since the engine set the flag before the
				//event was set. By the time we get the event creature hostility towards each other will be different
				if (IsObjectHostile(OBJECT_SELF, oAppear))
					nHostile = TRUE_;
				else
					nHostile = FALSE_;
			}

			// If the hostility changed, and the AActor* is not hostile, it means the AActor* was hostile and turned
			// non-hostile. This means that we need to run some combat-stopping logic that is usually run on a
			// perception disappear event, so we could remove this creature from combat state if needed.
			if (nHostilityChanged && !nHostile)
			{
				Combat_HandleCreatureDisappear(OBJECT_SELF, oAppear);
			}

			// Handle shouts
			// Triggering when perceiving a follower
			// NOTE: this needs to be triggered when triggering a follower since if the check was for "controlled"
			// the player might switch control and confuse the system
			if (IsFollower(oAppear) && UT_GetShoutsFlag(OBJECT_SELF) == TRUE_)
			{
#ifdef DEBUG
				Log_Trace(LOG_CHANNEL_EVENTS, GetCurrentScriptName(ev), "Perceived party member: Activating shouts");
#endif
				SendEventOnDelayedShout(OBJECT_SELF); // This will start the delayed event loop
			}

			if (nHostile)
			{
#ifdef DEBUG
				Log_Trace(LOG_CHANNEL_EVENTS, GetCurrentScriptName(ev), "Perceived hostile: " + GetTag(oAppear) + ", my combat state= " + IntToString(GetCombatState(OBJECT_SELF)));
#endif

				if (!IsFollower(OBJECT_SELF))
				{
					AI_Threat_UpdateEnemyAppeared(OBJECT_SELF, oAppear);
				}

				if (GetCombatState(OBJECT_SELF) == FALSE_)
				{
#ifdef DEBUG
					Log_Trace_AI(OBJECT_SELF, "rules_core.EVENT_TYPE_PERCEPTION_APPEAR", "COMBAT ACTIVE!");
#endif

					if (!IsFollower(OBJECT_SELF))
					{
						SetCombatState(OBJECT_SELF, TRUE_);
						WR_ClearAllCommands(OBJECT_SELF, TRUE_);
						if (IsFollower(oAppear))
						{
							SSPlaySituationalSound(OBJECT_SELF, SOUND_SITUATION_ENEMY_SIGHTED, oAppear);
						}
					}

					// ---------------------------------------------------------
					// If we are not the controlled creature, engage combat AI.
					// ---------------------------------------------------------
					if (!IsControlled(OBJECT_SELF))
					{
						if (!IsFollower(OBJECT_SELF) && GetLocalInt(OBJECT_SELF, AI_CUSTOM_AI_ACTIVE)) // custom AI active
						{
							Ambient_Stop(OBJECT_SELF);

#ifdef DEBUG
							Log_Trace_AI(OBJECT_SELF, "rules_core", "Executing CUSTOM AI");
#endif
							SendEventHandleCustomAI(OBJECT_SELF, nullptr, -1, -1, -1); //
						}
						else if (!IsFollower(OBJECT_SELF))
						{
							// Initial call - all others should be when commands are completed
							Ambient_Stop(OBJECT_SELF);

							float fDistance = GetDistanceBetween(OBJECT_SELF, oAppear);
							if (fDistance > 25.0 && !nHostilityChanged && !IsUsingMeleeWeapon(OBJECT_SELF))
							{
								nRand = Random(AI_TRIGGER_DELAY_RANDOM_RANGE) + 1; // randomizing 10 int32 values to determine a delay between 0.0 and AI_TRIGGER_DELAY_RANDOM_RANGE
								fCombatDelay = IntToFloat(nRand) / IntToFloat(AI_TRIGGER_DELAY_RANDOM_RANGE) * AI_TRIGGER_DELAY_RANDOM_MAX;

#ifdef DEBUG
								Log_Trace(LOG_CHANNEL_EVENTS, GetCurrentScriptName(ev), "Triggering combat with a delay of: " + FloatToString(fCombatDelay));
#endif
								FCommand cWait = CommandWait(fCombatDelay);
								WR_AddCommand(OBJECT_SELF, cWait);
							}
							else
							{
#ifdef DEBUG
								Log_Trace(LOG_CHANNEL_EVENTS, GetCurrentScriptName(ev), "Enemy perceived too close - not delaying start of combat");
#endif
								AI_DetermineCombatRound(OBJECT_SELF);
							}
							SetFacingObject(OBJECT_SELF, oAppear);
						}
						else if (IsFollower(OBJECT_SELF)) // non-followers get wait combat instead, above
							AI_DetermineCombatRound(OBJECT_SELF);

					}

					if (GetLocalInt(OBJECT_SELF, AI_LIGHT_ACTIVE) == 1)
					{
#ifdef DEBUG
						Log_Trace_AI(OBJECT_SELF, "rules_core.EVENT_TYPE_PERCEPTION_APPEAR", "LIGHT AI CREATURE: aborting rest of perception event");
#endif
						break;
					}


					// ---------------------------------------------------------
					// Force all allies into combat
					// ---------------------------------------------------------
					TArray<AActor*>  arAllies = _AI_GetAllies(OBJECT_SELF, COMMAND_TYPE_INVALID, -1);
					nSize = arAllies.Num();
					FCommand cMove;
					int32 bIsStealthy = IsStealthy(OBJECT_SELF);
#ifdef DEBUG
					Log_Trace_AI(OBJECT_SELF, "rules_core.EVENT_TYPE_PERCEPTION_APPEAR", "BOOM stealthy= " + IntToString(bIsStealthy));
#endif

					for (i = 0; i < nSize; i++)
					{
						oAlly = arAllies[i];
						if (GetCombatState(oAlly) == FALSE_)
						{
							// Yaron, Feb 25 2008 - changing to move toward ally instead of perceiving enemy
							// Followers still trigger perception
							if (IsFollower(oAlly) && !IsStealthy(OBJECT_SELF))
							{
								if (WR_TriggerPerception(oAlly, oAppear) == FALSE_ && !IsControlled(oAlly)
									&& GetMainControlled() == OBJECT_SELF)
								{
									// TriggerPerception failed for an ally while I'm the main controlled
									// - Move to party leader
									FVector lLoc = GetMainControlled()->GetActorLocation();//GetFollowerWouldBeLocation(oAlly);
									FCommand cMove = CommandMoveToLocation(lLoc, TRUE_);
									WR_AddCommand(oAlly, cMove);

								}
								else if (!IsControlled(oAlly))
									// combat time
									// NOTE: this used to be delayed, but was removed because it causes followers
									// to linger behind the player sometimes when charging into combat
								{
									AI_DetermineCombatRound(OBJECT_SELF);

								}
							}
							else if (!IsFollower(oAlly)) // non party members
							{
								if (GetLocalInt(oAlly, AI_FLAG_STATIONARY) == 0)// && GetLocalInt(oAlly, CLIMAX_ARMY_ID) == 0)
								{
#ifdef DEBUG
									Log_Trace(LOG_CHANNEL_AI, GetCurrentScriptName(ev), "Bringing over ally: " + GetTag(oAlly));
#endif

									//int32 nRand = Random(10) + 1;
									//float fDistanceToMove = IntToFloat(nRand);
									if (GetLocalInt(OBJECT_SELF, AI_HELP_TEAM_STATUS) <= 1) // allow for 0 (not active) or 1 (active for special system)
									{
										SetLocalInt(OBJECT_SELF, AI_HELP_TEAM_STATUS, AI_HELP_TEAM_STATUS_NORMAL_ALLY_HELP_ACTIVE);
										cMove = CommandMoveToActor(oAppear, TRUE_);
										SetCreatureIsStatue(oAlly, FALSE_);
										WR_ClearAllCommands(oAlly, TRUE_);
										Ambient_Stop(oAlly);
										WR_AddCommand(oAlly, cMove, TRUE_, FALSE_);
									}
								}
							}

						}
					}


				}
				else // IN combat, perceiving hostile
				// (can be when a stealth player goes out of stealth and drags enemies back to his party)
				{
					FCommand cCurrent = GetCurrentCommand(OBJECT_SELF);
					int32 nQSize = GetCommandQueueSize(OBJECT_SELF);
					if (IsFollower(OBJECT_SELF) && !IsControlled(OBJECT_SELF))
					{
						// a non-controlled follower perceiving hostile while doing nothing
						if (nQSize == 0 && GetCommandType(cCurrent) == COMMAND_TYPE_INVALID)
						{
#ifdef DEBUG
							Log_Trace_AI(OBJECT_SELF, "rules_core.EVENT_TYPE_PERCEPTION_APPEAR", "Non-controlled follower perceiving hostile while in combat state and not running any action");
#endif
							AI_DetermineCombatRound(OBJECT_SELF);
						}
					}
				}


				// -----------------------------------------------------------------
				// Set the game into combat camera if we perceive a hostile creature
				// and are not in combat mode
				// -----------------------------------------------------------------
#ifdef DEBUG
				LogError("add events on player!!!");
#endif // DEBUG

				if (IsPartyMember(OBJECT_SELF))
				{
					int32 nMode = GetGameMode();
#ifdef DEBUG
					Log_Trace(LOG_CHANNEL_EVENTS, GetCurrentScriptName(ev), "Party game mode: " + IntToString(nMode));
#endif

					if (nMode == GM_EXPLORE || nMode == GM_DEAD)
					{
						if (IsObjectHostile(OBJECT_SELF, oAppear))
						{
#ifdef DEBUG
							Log_Trace(LOG_CHANNEL_EVENTS, GetCurrentScriptName(ev), "PARTY perceived hostiles and not in combat yet - ENTERING COMBAT MODE");
#endif

							// -------------------------------------------------
							// Play Enemy Sighted if controlled character...
							// -------------------------------------------------
							if (IsControlled(OBJECT_SELF))
							{
								SSPlaySituationalSound(OBJECT_SELF, SOUND_SITUATION_ENEMY_SIGHTED, oAppear);
							}
							WR_SetGameMode(GM_COMBAT);
						}
					}
					// This handles resurrection while the party is already in combat mode and the rejoining
					// follower combat state does not match the party's
					else if (nMode == GM_COMBAT && !GetCombatState(OBJECT_SELF))
					{
#ifdef DEBUG
						Log_Trace(LOG_CHANNEL_EVENTS, GetCurrentScriptName(ev), "RESSURECTED PARTY MEMBER: setting combat state ACTIVE");
#endif
						SetCombatState(OBJECT_SELF, TRUE_);
					}

				}

			}
#ifdef DEBUG
			Log_Trace(LOG_CHANNEL_EVENTS, GetCurrentScriptName(ev), "tactic table: " + IntToString(GetPackageAI(OBJECT_SELF)));
#endif
			// handle creatures that should run away from hostiles (for example: city elf servant escorting the player)
			if (GetPackageAI(OBJECT_SELF) == 10130) // coward package
				AI_HandleCowardFollower(oAppear);

			break;
		}

		case EVENT_TYPE_COMMAND_PENDING:
		{
			// ---------------------------------------------------------------------
			// Georg: relevant eclipse engine code
			/*
			pAttackerOnHitEvent->SetCreator(nAttackerId);
			pAttackerOnHitEvent->SetObjectId(0, nAttackerId);
			pAttackerOnHitEvent->SetInteger(0, pAttackerItem->GetOnHitEffectId());
			pAttackerOnHitEvent->SetInteger(1, pAttackerItem->GetOnHitPower());

			pAttackerOnHitEvent->SetObjectId(1, pAttackerItem->GetId());
			pAttackerOnHitEvent->SetTarget(a_pEventData->m_nTargetId);
			*/

			AActor* aCommandOwner = GetEventObject(ev, 0);
			ADA2UE4Creature* oCommandOwner = Cast<ADA2UE4Creature>(aCommandOwner);
			AActor* aTarget = GetEventObject(ev, 1);
			ADA2UE4Creature* oTarget = Cast<ADA2UE4Creature>(aTarget);
			int32 nCommandId = GetEventInteger(ev, 0);
			int32 nCommandSubType = GetEventInteger(ev, 1);


			// -----------------------------------------------------------------
			// Validate the event (target ok? user alive, etc.)
			// -----------------------------------------------------------------
			if (Events_ValidateCommandPending(oCommandOwner, oTarget, nCommandId, nCommandSubType) == FALSE_)
			{
#ifdef DEBUG
				Log_Trace(LOG_CHANNEL_EVENTS, GetCurrentScriptName(ev), "Discarding EVENT_TYPE_COMMAND_PENDING (" + IntToString(nCommandId) + ")");
#endif
				return;
			}

			// -----------------------------------------------------------------
			// Melee Attack...
			// -----------------------------------------------------------------
			if (nCommandId == COMMAND_TYPE_ATTACK)
			{

				if (GetCombatantType(oTarget) == CREATURE_TYPE_NON_COMBATANT)
				{
					LogWarning("Noncombatant Creature " + GetTag(oTarget) + " is target of an attack command! Pause and get Georg or Yaron");
				}
				// Flagging party as clear to attack (if controlled follower)
				if (IsControlled(OBJECT_SELF) && IsObjectValid(oTarget) && IsObjectHostile(OBJECT_SELF, oTarget))
				{
					AI_SetPartyAllowedToAttack(OBJECT_SELF, TRUE_);
#ifdef DEBUG
					Log_Trace_AI(OBJECT_SELF, "rules_core.EVENT_TYPE_COMMAND_PENDING", "Controlled follower attacking - clearing party to attack");
#endif
				}

				int32 nCommandResult = Combat_HandleCommandAttack(oCommandOwner, oTarget, nCommandSubType);

				// -------------------------------------------------------------
				// Trigger a battle cry
				// -------------------------------------------------------------
				if (nCommandResult == COMMAND_RESULT_SUCCESS)
				{
					SSPlaySituationalSound(OBJECT_SELF, SOUND_SITUATION_COMBAT_BATTLECRY);
				}


				// -------------------------------------------------------------
				// Send the command result to the engine, this starts the
				// command execution.
				// -------------------------------------------------------------
				SetCommandResult(oCommandOwner, nCommandResult);
			}
			//TODO command pending abilities/spells
			// -----------------------------------------------------------------
			// Abilities, Spells
			// -----------------------------------------------------------------
			/*else if (nCommandId = COMMAND_TYPE_USE_ABILITY)
			{
				// Flagging party as clear to attack (if controlled follower)
				if(IsControlled(OBJECT_SELF) && IsObjectValid(oTarget) && IsObjectHostile(OBJECT_SELF, oTarget))
				{
					#ifdef DEBUG
					Log_Trace_AI(OBJECT_SELF, "rules_core.EVENT_TYPE_COMMAND_PENDING", "Controlled follower using hostile ability - clearing party to attack");
					#endif
					AI_SetPartyAllowedToAttack(OBJECT_SELF, TRUE_);
				}

				// -------------------------------------------------------------
				// Redirect into ability_core
				// -------------------------------------------------------------
				HandleEvent(ev, RESOURCE_SCRIPT_ABILITY_CORE);
			}*/
			else
			{
				#ifdef DEBUG
				Log_Trace(LOG_CHANNEL_GENERAL, "rules_core.EVENT_TYPE_COMMAND_PENDING ",
											   "Unknown COMMAND_TYPE " + IntToString(nCommandId) + " received, ignoring",oTarget);
				#endif
			}

			break;
		}

		// ---------------------------------------------------------------------
		// EVENT_TYPE_COMMAND_COMPLETE:
		// Called after a command has finished executing.
		//
		// Params:
		//      int32(0) - Type of the last command (e.g. COMMAND_TYPE_ATTACKED)
		//      int32(1) - The status of the execution (COMMAND_SUCCESSFUL, etc)
		//      obj(0) - The target that command was applied to
		// ---------------------------------------------------------------------
		case EVENT_TYPE_COMMAND_COMPLETE:
		{
			int32 nLastCommandType = GetEventInteger(ev, 0);
			int32 nCommandStatus = GetEventInteger(ev, 1);
			int32 nLastSubCommand = GetEventInteger(ev, 2);
			AActor* oLastTarget = nullptr;
			AActor* oBlockingObject = GetEventObject(ev, 2);

			if (nLastCommandType == 0)
			{
#ifdef DEBUG
				Log_Trace_Scripting_Error("rules_core", "Invalid FCommand complete received from engine. Contact Yaron");
#endif
				return;
			}

			if (nLastCommandType == COMMAND_TYPE_USE_ABILITY)
			{
				EnableWeaponTrail(OBJECT_SELF, FALSE_);
			}

			if (IsDisabled(OBJECT_SELF) || IsDead(OBJECT_SELF) || IsDying(OBJECT_SELF))
			{
#ifdef DEBUG
				Log_Trace(LOG_CHANNEL_EVENTS, GetCurrentScriptName(ev), " discarding EVENT_TYPE_COMMAND_COMPLETE (disabled, dead, or dying)");
#endif
				return;
			}

			// If this AActor* has a roam distance defined check if we
			// are close enough to the roam point to remove the speed
			// up and clear this creature's perception list.
			float fRoamDistance = GetLocalFloat(OBJECT_SELF, ROAM_DISTANCE);
			if (fRoamDistance > 25.0f * SCALE_MULTIPLIER)
			{
				if (nLastCommandType == COMMAND_TYPE_MOVE_TO_LOCATION)
				{
					if (GetHasEffects(OBJECT_SELF, EFFECT_TYPE_MOVEMENT_RATE, 0))
					{
						if (GetDistanceBetweenLocations(GetRoamLocation(OBJECT_SELF), GetLocation(OBJECT_SELF)) < 3.0f * SCALE_MULTIPLIER)
						{
							DisplayFloatyMessage(OBJECT_SELF,"BACK HOME!YAY!");
							RemoveEffectsByParameters(OBJECT_SELF, EFFECT_TYPE_MOVEMENT_RATE, 0, OBJECT_SELF);
							SetCombatState(OBJECT_SELF, FALSE_);
							ClearPerceptionList(OBJECT_SELF);
							return;
						}
					}
				}
			}

			SSPlaySituationalSound(OBJECT_SELF, SOUND_SITUATION_COMMAND_COMPLETE);

			// In case something interactive (e.g. a door) was blocking the path
			// for the command, a solution is found here and immediately queued up.
			// After completing this intermediate action, the AI can resume normal execution.
			if (nCommandStatus == COMMAND_FAILED_PATH_ACTION_REQUIRED)
			{
				int32 bAllowPathAction = IsControlled(OBJECT_SELF) || !IsFollower(OBJECT_SELF);
				if (bAllowPathAction && !GetLocalInt(OBJECT_SELF, AI_DISABLE_PATH_BLOCKED_ACTION))
				{
					if (AI_DeterminePathBlockedAction(OBJECT_SELF, oBlockingObject))
						return;
				}
				else
				{
#ifdef DEBUG
					Log_Trace(LOG_CHANNEL_AI, GetCurrentScriptName(ev), "Not handling Path-blocked event (follower or has flag to disable this AI)");
#endif
				}

			}

			// Determine what to do for next round.
			// Behavior depends on AActor* type and can execute outside combat.
			// For party members this should run every time a command ends.
			// For normal creatures this should run only during combat.
			if (IsFollower(OBJECT_SELF) || (!IsFollower(OBJECT_SELF) && GetCombatState(OBJECT_SELF)))
			{
				if (nLastCommandType == COMMAND_TYPE_ATTACK || nLastCommandType == COMMAND_TYPE_USE_ABILITY)
					oLastTarget = GetEventObject(ev, 1);

#ifdef DEBUG
				if (GetCreatureRank(OBJECT_SELF) == CREATURE_RANK_INVALID)
					Log_Trace_Scripting_Error(GetCurrentScriptName(ev), GetTag(OBJECT_SELF) + " has no RANK");
#endif

				if (GetLocalInt(OBJECT_SELF, AI_CUSTOM_AI_ACTIVE)) // custom AI active
				{
#ifdef DEBUG
					Log_Trace_AI(OBJECT_SELF, "rules_core", "Executing CUSTOM AI");
#endif
					SendEventHandleCustomAI(OBJECT_SELF, oLastTarget, nLastCommandType, nCommandStatus, nLastSubCommand);
				}
				else
				{
					AI_DetermineCombatRound(OBJECT_SELF, oLastTarget, nLastCommandType, nCommandStatus, nLastSubCommand);
					break;
				}
			}

			// Ambient behavior
			SetLocalEvent(OBJECT_SELF, AMBIENT_EVENT, ev); //DHK
			Ambient_CommandComplete(nLastCommandType, nCommandStatus, OBJECT_SELF);
			
			// Adding this so people can have a SAFE substitute for command_complete when nothing else works
			if (GetCombatState(OBJECT_SELF) == FALSE_ && GetLocalInt(OBJECT_SELF, AI_CUSTOM_AI_ACTIVE))
			{
#ifdef DEBUG
				Log_Trace_AI(OBJECT_SELF, "rules_core", "Executing CUSTOM COMMAND COMPLETE");
#endif
				FGameEvent evCustomCommandComplete = Event(EVENT_TYPE_CUSTOM_COMMAND_COMPLETE);
				evCustomCommandComplete = SetEventInteger(evCustomCommandComplete, 0, nLastCommandType);
				evCustomCommandComplete = SetEventInteger(evCustomCommandComplete, 1, nCommandStatus);
				evCustomCommandComplete = SetEventInteger(evCustomCommandComplete, 2, nLastSubCommand);
				evCustomCommandComplete = SetEventObject(evCustomCommandComplete, 2, oBlockingObject);

				SignalEvent(OBJECT_SELF, evCustomCommandComplete);
			}

			// handle creatures that should run away from hostiles (for example: city elf servant escorting the player)
			if (GetPackageAI(OBJECT_SELF) == AI_TABLE_COWARD) // coward package
				AI_HandleCowardFollower(OBJECT_SELF);

			break;
		}
		default:
		{
#ifdef DEBUG
			LogError("rules event not found: " + ev.nGameEventType);
#endif
			break;
		}
		}
	}

	OBJECT_SELF->bScriptRulesRedirected = FALSE_;
}

