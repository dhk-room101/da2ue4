#include "DA2UE4.h"
#include "DA2UE4Creature.h"
#include "DA2UE4GameInstance.h"
#include "ai_main_h.h"
#include "ai_constants_h.h"
#include "ai_conditions_h.h"
#include "ai_behaviors_h.h"
#include "ldf.h"
#include "m2da_data_h.h"
#include "sys_stealth_h.h"
#include "commands_h.h"
#include "effect_ai_modifier_h.h"
#include "ui_h.h"
#include "effects_h.h"
#include "utility_h.h"
#include "STypes.h"

ECombatResult AI_DetermineCombatRound(AActor* aActor, AActor* oLastTarget, int32 nLastCommand, int32 nLastCommandStatus, int32 nLastSubCommand)
{
	ADA2UE4Creature* OBJECT_SELF = Cast<ADA2UE4Creature>(aActor);
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "AI_DetermineCombatRound", "***** START ***** , last command status: " + IntToString(nLastCommandStatus));
#endif

	// -------------------------------------------------------------------------
	// Non combatant
	// -------------------------------------------------------------------------
	if (GetCombatantType(OBJECT_SELF) == CREATURE_TYPE_NON_COMBATANT)
	{
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "AI_DetermineCombatRound", "NON COMBATANT CREATURE - running away");
#endif
		return ECombatResult::CREATURE_TYPE_NON_COMBATANT;
	}

	// make stealthed creature get out of stealth if alone and not at the start of combat
	if (!IsFollower(OBJECT_SELF) && IsStealthy(OBJECT_SELF) && GetGameMode() == EGameMode::GM_COMBAT)
	{
		TArray<AActor*> arAllies = _AI_GetAllies(OBJECT_SELF, -1, -1);
		int32 nSize = arAllies.Num();
		if (nSize == 0)
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "AI_DetermineCombatRound", "No allies left dropping out of stealth and continuing with AI");
#endif
			DropStealth(OBJECT_SELF);
		}
		else // some allies alive -> if one of them is not in stealth then we're good
		{
			int32 i;
			AActor* oCurrent;
			int32 bRemoveStealth = TRUE_;
			for (i = 0; i < nSize; i++)
			{
				oCurrent = arAllies[i];
				if (!IsStealthy(oCurrent))
				{
#ifdef DEBUG
					Log_Trace_AI(OBJECT_SELF, "AI_DetermineCombatRound", "Found at least one non-stealth ally - clear to keep stealth");
#endif
					bRemoveStealth = FALSE_;
					break;
				}

			}
			if (bRemoveStealth)
			{
#ifdef DEBUG
				Log_Trace_AI(OBJECT_SELF, "AI_DetermineCombatRound", "All allies are stealthy - dropping stealth");
#endif
				DropStealth(OBJECT_SELF);
			}
		}
	}

	if (GetGameMode() != EGameMode::GM_COMBAT && GetGameMode() != EGameMode::GM_EXPLORE)
	{
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "AI_DetermineCombatRound", "Not combat or explore game mode - WAITING");
#endif
		if (!IsControlled(OBJECT_SELF))
		{
			FCommand cWait = _AI_DoNothing(OBJECT_SELF, -1, nLastCommandStatus, FALSE_, FALSE_);
			WR_AddCommand(OBJECT_SELF, cWait);
		}
		return ECombatResult::NOT_COMBAT_OR_EXPLORE_GAMEMODE;
	}

	SetObjectInteractive(OBJECT_SELF, TRUE_);

	// Do not run any AI if the creature is doing something right now
	FCommand cCurrent = GetCurrentCommand(OBJECT_SELF);
	int32 nCurrentType = GetCommandType(cCurrent);
	int32 nQueueSize = GetCommandQueueSize(OBJECT_SELF);
	if (nCurrentType != COMMAND_TYPE_INVALID || nQueueSize > 0)
	{
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "AI_DetermineCombatRound", "Do nothing - creature running command already: " + Log_GetCommandNameById(nCurrentType), LOG_SEVERITY_WARNING);
#endif
		return ECombatResult::CREATURE_COMMAND_IN_PROGRESS;
	}

	// -------------------------------------------------------------------------
	// Ability usage disabled - Master off switch per creature
	// -------------------------------------------------------------------------
	else if (GetCreatureFlag(OBJECT_SELF, CREATURE_RULES_FLAG_AI_OFF))
	{
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "AI_DetermineCombatRound", "ABORT: CREATURE_RULES_FLAG_AI_OFF was set");
#endif
		if (IsFollower(OBJECT_SELF))
		{
#ifdef DEBUG
			LogWarning("IMPORTANT! follower has ability use by AI disabled! This should never happen without debug scripts! - please contact YARON");
#endif // DEBUG
		}
		return ECombatResult::CREATURE_RULES_FLAG_AI_OFF_ENABLED;
	}
	else if (!IsFollower(OBJECT_SELF) && OBJECT_SELF->AI_LIGHT_ACTIVE == TRUE_)
	{
		//AActor* [] oNearestFollowers = GetNearestObjectByGroup(OBJECT_SELF, GROUP_PC, OBJECT_TYPE_CREATURE, 1, TRUE_, TRUE_);
		//AActor* oNearestFollower = oNearestFollowers[0];
		//float fDistance = GetDistanceBetween(OBJECT_SELF, oNearestFollower);

		//if(!IsObjectValid(oNearestFollower) || fDistance > LIGHT_AI_MIN_DISTANCE * SCALE_MULTIPLIER)
		AI_DetermineCombatRound_Light(OBJECT_SELF, oLastTarget, nLastCommand, nLastCommandStatus, nLastSubCommand);
		return ECombatResult::AI_LIGHT_ACTIVE;

	}

	// AOE check
	int32 nCheckChance = 0; // chance to try and avoid AOE if actually inside
	if (!IsFollower(OBJECT_SELF))
	{
		int32 nAppearance = GetAppearanceType(OBJECT_SELF);
		nCheckChance = GetM2DAInt(TABLE_APPEARANCE, "AvoidAOEChance", nAppearance);
	}
	else if (!IsControlled(OBJECT_SELF))// follower - chance based on behavior
	{
		if (AI_BehaviorCheck_AvoidAOE(OBJECT_SELF))
			nCheckChance = AI_FOLLOWER_AVOID_AOE_CHANCE;
	}

	if (nCheckChance > 0 && OBJECT_SELF->AI_FLAG_STATIONARY != AI_STATIONARY_STATE_HARD)
	{
		TArray<int32> AbilityAOEs = GetAbilitiesDueToAOEs(OBJECT_SELF);
		int32 nArraySize = AbilityAOEs.Num();
		if (nArraySize > 0) // in ability AOEs
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "AI_DetermineCombatRound", "Creature is in an ability AOE");
#endif
			int32 i;
			//unused 			int32 nAOE;
			int32 nRand;
			for (i = 0; i < nArraySize; i++)
			{
				if (GetM2DAInt(TABLE_AI_ABILITY_COND, "HostileAOE", AbilityAOEs[i]) == 1)
				{
					// in hostile AOE

					// random chance to try and exit
					nRand = Random(100);
#ifdef DEBUG
					Log_Trace_AI(OBJECT_SELF, "AI_DetermineCombatRound", "Hostile AOE ability: " + IntToString(AbilityAOEs[i]) + ", escape chance: " + IntToString(nCheckChance) + ", roll: " + IntToString(nRand));
#endif
					if (nRand <= nCheckChance)
					{
						// use the regular movement cooldown
						if (_AI_CheckMoveTimer(OBJECT_SELF) == TRUE_)
						{
#ifdef DEBUG
							Log_Trace_AI(OBJECT_SELF, "AI_DetermineCombatRound", "Escaping AOE!");
#endif
							/*location lLoc = GetLocation(OBJECT_SELF);
							FVector vPos = GetPositionFromLocation(lLoc);
							FVector vPos = OBJECT_SELF->GetActorLocation();
							int32 nRandX = Random(16) - 8;
							int32 nRandY = Random(16) - 8;
							vPos.X += nRandX;
							vPos.Y += nRandY;
							//location lNewLoc = Location(GetArea(OBJECT_SELF), vPos, GetFacing(OBJECT_SELF));
							//FCommand cMove = CommandMoveToLocation(lNewLoc, TRUE_);*/
							FCommand cMove = CommandMoveAwayFromObject(OBJECT_SELF, AI_AOE_FLEE_DISTANCE * SCALE_MULTIPLIER, TRUE_);
							WR_AddCommand(OBJECT_SELF, cMove, FALSE_, FALSE_, -1, AI_COMMAND_TIMER);
							_AI_SetMoveTimer(OBJECT_SELF);
							return ECombatResult::AI_AOE_FLEE;
						}
						else
							break; // exit loop
					}
				}
			}
		}
	}



	// -------------------------------------------------------------------------
	// AI disabled (followers only)
	// -------------------------------------------------------------------------
	if (IsFollower(OBJECT_SELF) && !IsPartyAIEnabled(OBJECT_SELF))
	{
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "AI_DetermineCombatRound", "FOLLOWER AI DISABLED - will run partial AI instead");
#endif
		AI_DetermineCombatRound_Partial(OBJECT_SELF, oLastTarget, nLastCommand, nLastCommandStatus, nLastSubCommand);
		return ECombatResult::AI_PARTIAL_ACTIVE;
	}

	// We assume that whoever called AI_DetermineCombatRound has validated that the creature is valid for combat
	if (Effects_HasAIModifier(OBJECT_SELF, AI_MODIFIER_IGNORE))
	{
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "AI_DetermineCombatRound", "I have the IGNORE flag set - doing nothing", LOG_SEVERITY_WARNING);
#endif
		return ECombatResult::AI_MODIFIER_IGNORE;
	}

	if (IsFollower(OBJECT_SELF) && !IsControlled(OBJECT_SELF)) // check hidden tactics based on behavior
	{
		if (AI_BehaviorCheck_PreferRange(OBJECT_SELF) && _AI_GetWeaponSetEquipped(OBJECT_SELF) != AI_WEAPON_SET_RANGED
			&& _AI_HasWeaponSet(OBJECT_SELF, AI_WEAPON_SET_RANGED) &&
			_AI_Condition_BeingAttackedByAttackType(OBJECT_SELF, AI_TARGET_TYPE_SELF, AI_ATTACK_TYPE_MELEE, -1, -1, -1) == nullptr)
		{
			TArray<AActor*> arEnemies = GetCreaturesInMeleeRing(OBJECT_SELF, 0.0, 360.0, TRUE_);
			if (arEnemies.Num() == 0)
			{
#ifdef DEBUG
				Log_Trace_AI(OBJECT_SELF, "AI_DetermineCombatRound", "Follower preferring RANGED weapon - switching");
#endif
				FCommand cSwitch = _AI_SwitchWeaponSet(OBJECT_SELF, AI_WEAPON_SET_RANGED);
				WR_AddCommand(OBJECT_SELF, cSwitch);
				return ECombatResult::AI_WEAPON_SET_RANGED;
			}
		}
		else if (AI_BehaviorCheck_PreferMelee(OBJECT_SELF) && _AI_GetWeaponSetEquipped(OBJECT_SELF) != AI_WEAPON_SET_MELEE
			&& _AI_HasWeaponSet(OBJECT_SELF, AI_WEAPON_SET_MELEE))
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "AI_DetermineCombatRound", "Follower preferring MELEE weapon - switching");
#endif
			FCommand cSwitch = _AI_SwitchWeaponSet(OBJECT_SELF, AI_WEAPON_SET_MELEE);
			WR_AddCommand(OBJECT_SELF, cSwitch);
			return ECombatResult::AI_WEAPON_SET_MELEE;
		}
		else if (AI_BehaviorCheck_AvoidNearbyEnemies(OBJECT_SELF))
		{
			if ((nLastCommand == COMMAND_TYPE_MOVE_TO_OBJECT || nLastCommand == COMMAND_TYPE_MOVE_TO_LOCATION) &&
				nLastCommandStatus < 0)
			{
#ifdef DEBUG
				Log_Trace_AI(OBJECT_SELF, "AI_DetermineCombatRound", "Last command was movement and failed - not evaluating Avoid Enemies behavior");
#endif
			}
			else
			{
				TArray<AActor*> arEnemies = _AI_GetEnemies(OBJECT_SELF, -1, -1);
				int32 nSize = arEnemies.Num();
				if (nSize > 0)
				{
					AActor* oEnemy = arEnemies[0];
					float fDistance = GetDistanceBetween(OBJECT_SELF, oEnemy);
					if (fDistance < AI_RANGE_SHORT * SCALE_MULTIPLIER)
					{
#ifdef DEBUG
						Log_Trace_AI(OBJECT_SELF, "AI_DetermineCombatRound", "Follower avoiding nearby enemies - moving away");
#endif
						FCommand cMove = CommandMoveAwayFromObject(oEnemy, AI_MOVE_AWAY_DISTANCE_SHORT * SCALE_MULTIPLIER, TRUE_);
						WR_AddCommand(OBJECT_SELF, cMove, FALSE_, FALSE_, -1, AI_COMMAND_TIMER);
					}
				}
			}
		}
		else if (AI_BehaviorCheck_AvoidMelee(OBJECT_SELF))
		{
			if ((nLastCommand == COMMAND_TYPE_MOVE_TO_OBJECT || nLastCommand == COMMAND_TYPE_MOVE_TO_LOCATION) &&
				nLastCommandStatus < 0)
			{
#ifdef DEBUG
				Log_Trace_AI(OBJECT_SELF, "AI_DetermineCombatRound", "Last command was movement and failed - not evaluating Avoid Melee Enemies behavior");
#endif
			}
			else
			{
				TArray<AActor*> arEnemies = GetCreaturesInMeleeRing(OBJECT_SELF, 0.0, 360.0, TRUE_);
				int32 nSize = arEnemies.Num();
				int32 i;
				AActor* oCurrent;
				for (i = 0; i < nSize; i++)
				{
					oCurrent = arEnemies[i];
					if (IsUsingMeleeWeapon(oCurrent) && GetAttackTarget(oCurrent) == OBJECT_SELF)
					{
#ifdef DEBUG
						Log_Trace_AI(OBJECT_SELF, "AI_DetermineCombatRound", "Follower avoiding melee enemies - moving away");
#endif
						FCommand cMove = CommandMoveToActor(oCurrent, TRUE_, AI_MOVE_AWAY_DISTANCE_SHORT * SCALE_MULTIPLIER, TRUE_);
						WR_AddCommand(OBJECT_SELF, cMove, FALSE_, FALSE_, -1, AI_COMMAND_TIMER);
						return ECombatResult::AI_MOVE_AWAY_DISTANCE_MELEE;
					}
				}
			}
		}
	}

	// Check if the creature should bring his team to help
	int32 nTeamHelpStatus = OBJECT_SELF->AI_HELP_TEAM_STATUS;
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "AI_DetermineCombatRound", "Team Help Status: " + IntToString(nTeamHelpStatus), LOG_SEVERITY_WARNING);
#endif
	if (nTeamHelpStatus == AI_HELP_TEAM_STATUS_ACTIVE)
	{
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "AI_DetermineCombatRound", "Calling team for help", LOG_SEVERITY_WARNING);
#endif
		OBJECT_SELF->AI_HELP_TEAM_STATUS = AI_HELP_TEAM_STATUS_CALLED_FOR_HELP;
		FCommand cMove = CommandMoveToLocation(GetLocation(OBJECT_SELF));
		int32 nTeamID = GetTeamId(OBJECT_SELF);
		if (nTeamID > 0)
		{
			TArray<ADA2UE4Creature*> arTeam = GetTeam(nTeamID);
			int32 nSize = arTeam.Num();
			int32 i;
			ADA2UE4Creature* oCurrent;
			//unused 			float fHelpDistance;
			for (i = 0; i < nSize; i++)
			{
				oCurrent = arTeam[i];
				// Sending only if the creature is not in combat yet and not helping yet
				if (GetCombatState(oCurrent) == FALSE_ &&  oCurrent->AI_HELP_TEAM_STATUS == AI_HELP_TEAM_STATUS_ACTIVE)
				{
#ifdef DEBUG
					Log_Trace_AI(OBJECT_SELF, "AI_DetermineCombatRound", "Bring creature to help: " + GetTag(oCurrent));
#endif
					oCurrent->AI_HELP_TEAM_STATUS = AI_HELP_TEAM_STATUS_HELPING;
					WR_ClearAllCommands(oCurrent);
					WR_AddCommand(oCurrent, cMove, FALSE_, FALSE_, -1, 0.0); // No timeout so they won't stop too soon
				}
			}
		}
	}

	// controlled party member -> use partial AI
	if (IsControlled(OBJECT_SELF))
	{
		if (GetCombatState(OBJECT_SELF) == FALSE_)
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "AI_DetermineCombatRound", "Follower not in combat state - aborting AI", LOG_SEVERITY_WARNING);
#endif
			return ECombatResult::FOLLOWER_NOT_COMBAT_STATE;
		}
		AI_DetermineCombatRound_Partial(OBJECT_SELF, oLastTarget, nLastCommand, nLastCommandStatus, nLastSubCommand);
	}
	// enemy OR non-controlled party member -> let the AI rules table determine the round
	else
	{
		//TODO AI Ballista if needed
		/*if (OBJECT_SELF->AI_BALLISTA_SHOOTER_STATUS > 0)
		{
		if (AI_Ballista_HandleAI() == TRUE_)
		return;
		}*/

		if (!IsFollower(OBJECT_SELF) && OBJECT_SELF->AI_FLAG_STATIONARY == AI_STATIONARY_STATE_VERY_SOFT)
		{
			// clear state if there is an enemy nearby
			TArray<AActor*> arEnemies = _AI_GetEnemies(OBJECT_SELF, -1, -1);
			if (arEnemies.Num() > 0)
			{
				float fDistance = GetDistanceBetween(OBJECT_SELF, arEnemies[0]);
				if (fDistance > AI_MELEE_RANGE * SCALE_MULTIPLIER)
				{
#ifdef DEBUG
					Log_Trace_AI(OBJECT_SELF, "AI_DetermineCombatRound", "Found nearby enemy - clearing stationary flag");
#endif
					OBJECT_SELF->AI_FLAG_STATIONARY = AI_STATIONARY_STATE_DISABLED;
				}
			}
		}
		// Iterate the AI rules table, until executing a valid rule
		// First we get the first tactic ID/Priority. This can be any positive number since
		// the package 2da includes tactics which may not apply to the current level of the creature

		int32 nPackageTable = _AI_GetPackageTable(OBJECT_SELF);
		int32 nTacticsNum;
		if (_AI_UseGUITables(OBJECT_SELF))
			nTacticsNum = GetNumTactics(OBJECT_SELF);
		else
			nTacticsNum = _AI_GetTacticsNum(nPackageTable);
		if (nTacticsNum > AI_MAX_TACTICS)
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "AI_DetermineCombatRound", "Too many tactics: " + IntToString(nTacticsNum));
#endif
			return ECombatResult::TOO_MANY_TACTICS;
		}
		int32 i = 1;
		int32 nLastTactic = OBJECT_SELF->AI_LAST_TACTIC;
		int32 nTablesDisabled = GetModule()->AI_DISABLE_TABLES;
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "AI_DetermineCombatRound", "tactics num: " + IntToString(nTacticsNum));
		Log_Trace_AI(OBJECT_SELF, "AI_DetermineCombatRound", "Last Tactic ID: " + IntToString(nLastTactic));
#endif
		// If the last tactic failed then resume from the tactic after it
		if (nLastCommandStatus < 0 && nLastTactic >= 0)
		{
			i = nLastTactic;
			i++;
			if (IsFollower(OBJECT_SELF) && i == nTacticsNum)
			{
#ifdef DEBUG
				Log_Trace_AI(OBJECT_SELF, "AI_DetermineCombatRound", "Last tactic failed for follower and failed tactic was last at table: WAITING");
#endif
				FCommand cWait = _AI_DoNothing(OBJECT_SELF, nLastTactic, nLastCommandStatus, TRUE_);
				WR_AddCommand(OBJECT_SELF, cWait);
				return ECombatResult::LAST_TACTIC_FAILED;
			}
			else
			{
#ifdef DEBUG
				Log_Trace_AI(OBJECT_SELF, "AI_DetermineCombatRound", "Last tactic failed, resuming from the next tactic: " + IntToString(i));
#endif
			}
		}
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "AI_DetermineCombatRound", "nTablesDisabled= " + IntToString(nTablesDisabled));
		Log_Trace_AI(OBJECT_SELF, "AI_DetermineCombatRound", "i= " + IntToString(i));
#endif

		// If a default action failed - we would try the entire table again
		int32 nExecuteRet;
		int32 nUseGUITables = _AI_UseGUITables(OBJECT_SELF);

		if (!nTablesDisabled)
		{
			for (i; i <= nTacticsNum; i++)
			{
				// For every rule: evaluate condition - if valid -> execute
				nExecuteRet = _AI_ExecuteTactic(OBJECT_SELF, nPackageTable, i, nLastCommandStatus, nUseGUITables);
				// if TRUE_/1 => return (tactic executed)
				// if FALSE_/0 => continue normally (tactic not executed)
				// if -1 => jump to default action
				// if greater then 1 => jump to specific tactic
				if (nExecuteRet == TRUE_)
					return ECombatResult::TACTIC_EXECUTED;
				else if (nExecuteRet == -1)
					break;
				else if (nExecuteRet > 1 && nExecuteRet > i)
					i = nExecuteRet - 1; // it will be increased by 1 by the for loop
			}
		}

		if (GetObjectActive(OBJECT_SELF) == FALSE_)
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "AI_DetermineCombatRound", "Object inactive - exiting");
#endif
			return ECombatResult::OBJECT_SELF_INACTIVE;
		}

		// Continue from this point only if in combat state
		if (GetCombatState(OBJECT_SELF) == FALSE_)
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "AI_DetermineCombatRound", "Not in combat. Not evaluating default action (attack)");
#endif
			// IMPORTANT: can't put here any wait or move commands as they will conflict with the engine follow
			// commands, especially if the user selected the GUI option to disable party following.
			return ECombatResult::NOT_IN_COMBAT;
		}
#ifdef DEBUG
		LogWarning("AI_DetermineCombatRound: AI_ExecuteDefaultAction");

#endif // DEBUG

		return ECombatResult::DEFAULT_ACTION;
		//AI_ExecuteDefaultAction(OBJECT_SELF, oLastTarget, nLastCommand, nLastCommandStatus, nLastSubCommand);


	}  // END else (non-controlled follower OR enemy)
	return ECombatResult::UNKNOWN_COMBAT_RESULT;
}

FCommand _AI_DoNothing(AActor* OBJECT_SELF, int32 nLastTacticID, int32 nLastCommandStatus, int32 nAllowTaunts, int32 bQuick, AActor* oTarget, int32 nClearThreat)
{
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_DoNothing", "START");
#endif

	int32 nRand = 0;
	int32 nRand2 = 0;
	FCommand cRet;
	int32 nAnim = 0;

	if (IsFollower(OBJECT_SELF) || _AI_HasAIStatus(OBJECT_SELF, AI_STATUS_PARALYZE) || _AI_HasAIStatus(OBJECT_SELF, AI_STATUS_DAZE)
		|| _AI_HasAIStatus(OBJECT_SELF, AI_STATUS_STUN))
		nAllowTaunts = FALSE_;

	nRand2 = Random(3) + 1;

	if (IsControlled(OBJECT_SELF))
	{
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_DoNothing", "Controlled follower - NOT WAITING");
#endif
		return cRet;
	}

	if (nClearThreat && oTarget != nullptr && !IsFollower(OBJECT_SELF))
	{
		// Lowering threat to oTarget
		AI_Threat_UpdateCantAttackTarget(OBJECT_SELF, oTarget);
	}

	// If last command was DoNothing and it failed (possibly by trying to play a non-existing taunt animation)
	if (nLastTacticID == AI_TACTIC_ID_WAIT && nLastCommandStatus < 0)
	{
		if (bQuick)
			cRet = CommandWait(AI_DO_NOTHING_DELAY_QUICK);
		else
			cRet = CommandWait(AI_DO_NOTHING_DELAY);
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_DoNothing", "Last DoNothing command failed - trying to wait");
#endif
	}
	else if (nAllowTaunts)
	{
		nRand = Random(100) + 1;
		if (nRand <= AI_TAUNT_CHANCE && GetAppearanceType(OBJECT_SELF) != APR_TYPE_OGRE)
		{
			if (nRand2 == 1) nAnim = 144;
			else if (nRand2 == 2) nAnim = 2005;
			else if (nRand2 == 3) nAnim = 149;
			cRet = CommandPlayAnimation(nAnim);
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_DoNothing", "Playing taunt animation");
#endif

		}
		if (GetCommandType(cRet) == COMMAND_TYPE_INVALID)
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_DoNothing", "failed to add taunt animation - waiting instead");
#endif
			if (bQuick)
				cRet = CommandWait(AI_DO_NOTHING_DELAY_QUICK);
			else
				cRet = CommandWait(AI_DO_NOTHING_DELAY);
		}
	}
	else
	{
		if (bQuick)
			cRet = CommandWait(AI_DO_NOTHING_DELAY_QUICK);
		else
			cRet = CommandWait(AI_DO_NOTHING_DELAY);
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_DoNothing", "Waiting");
#endif
	}
	return cRet;
}

void AI_DetermineCombatRound_Light(AActor* OBJECT_SELF, AActor* oLastTarget, int32 nLastCommand, int32 nLastCommandStatus, int32 nLastSubCommand)
{
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "AI_DetermineCombatRound_Light", "Last target: " + GetTag(oLastTarget));
#endif
	AActor* oTarget = oLastTarget;
	if (!_AI_IsHostileTargetValid(OBJECT_SELF, oTarget))
	{
		//AActor* [] arTargets = GetNearestObjectByHostility(OBJECT_SELF, TRUE_, OBJECT_TYPE_CREATURE, 1, TRUE_, TRUE_);
		//oTarget = arTargets[0];
		//oTarget = AI_Threat_GetThreatTarget(OBJECT_SELF, TRUE_);

		// Yaron, Jan 2 2009: removed the above threat call as it grabbed sometimes targets that were not perceived
		// Instead, we'll call the engine threat target directly
		// Yaron, Jan 5, 2009: some climax armies were getting stuck on the most hated target and trying to attack
		// it even when it was surrounded by enemies already and thus continually failing. Instead, they would now
		// pick a random target from their threat array.
		int32 nThreatArraySize = GetThreatTableSize(OBJECT_SELF);
		int32 nEnemy = Random(nThreatArraySize);
		oTarget = GetThreatEnemy(OBJECT_SELF, nEnemy);
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "AI_DetermineCombatRound_Light", "Threat target: " + GetTag(oTarget));
#endif
	}

	if ((nLastCommandStatus != COMMAND_SUCCESSFUL && nLastCommandStatus != COMMAND_FAILED_TARGET_DESTROYED &&
		nLastCommandStatus != COMMAND_FAILED_TIMEOUT) || !_AI_IsHostileTargetValid(OBJECT_SELF, oTarget))
	{
		FCommand cWait = CommandWait(AI_DO_NOTHING_DELAY);
		WR_AddCommand(OBJECT_SELF, cWait);
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "AI_DetermineCombatRound_Light", "No valid target - waiting");
#endif
	}
	else
	{
		//FCommand cAttack = _AI_ExecuteAttack(oTarget, nLastCommandStatus);
		FCommand cAttack = CommandAttack(oTarget);
		WR_AddCommand(OBJECT_SELF, cAttack, FALSE_, FALSE_, -1, AI_COMMAND_TIMER_QUICK);
	}
}

int32 _AI_CheckMoveTimer(AActor* aActor)
{
	ADA2UE4Creature* OBJECT_SELF = Cast<ADA2UE4Creature>(aActor);
	int32 nMoveStart = OBJECT_SELF->AI_MOVE_TIMER;
	int32 nCurrentTime = GetTime();
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_CheckMoveTimer", "current time: " + IntToString(nCurrentTime) + ", move start: " + IntToString(nMoveStart));
	Log_Trace_AI(OBJECT_SELF, "_AI_CheckMoveTimer", "Move/Turn time dif: " + IntToString(nCurrentTime - nMoveStart));
#endif
	if (nMoveStart != 0 && nCurrentTime - nMoveStart <= AI_MOVE_MIN_TIME)
	{
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteTactic", "Last move/turn happened too soon");
#endif
		return FALSE_;
	}
	return TRUE_;
}

void _AI_SetMoveTimer(AActor* aActor)
{
	ADA2UE4Creature* OBJECT_SELF = Cast<ADA2UE4Creature>(aActor);
	OBJECT_SELF->AI_MOVE_TIMER = GetTime();
	Log_Trace_AI(OBJECT_SELF, "_AI_CheckMoveTimer", "Set move timer to: " + IntToString(GetTime()));
}

int32 IsPartyAIEnabled(AActor* aActor)
{
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		return oCreature->bTactics;
	}
	else LogError("IsPartyAIEnabled: unknown actor type");
	return FALSE_;
}

void AI_DetermineCombatRound_Partial(AActor* OBJECT_SELF, AActor* oLastTarget, int32 nLastCommand, int32 nLastCommandStatus, int32 nLastSubCommand)
{
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "AI_DetermineCombatRound_Partial", "START, last target: " + GetTag(oLastTarget));
#endif
	if (GetCombatState(OBJECT_SELF) == FALSE_)
	{
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "AI_DetermineCombatRound_Partial", "Follower not in combat - aborting partial AI");
#endif
		return;
	}

	FCommand cCommand;

	AActor* oSelectedTarget = GetAttackTarget(OBJECT_SELF);
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "AI_DetermineCombatRound_Partial", "Selected target: " + GetTag(oSelectedTarget));
#endif

	AActor* oTarget = oSelectedTarget;
	if (!_AI_IsHostileTargetValid(OBJECT_SELF, oTarget))
		oTarget = oLastTarget;

#ifdef DEBUG
	if (!_AI_IsHostileTargetValid(OBJECT_SELF, oTarget))
		Log_Trace_AI(OBJECT_SELF, "AI_DetermineCombatRound_Partial", "COULD NOT FIND VALID TARGET");
	else
		Log_Trace_AI(OBJECT_SELF, "AI_DetermineCombatRound_Partial", "FINAL target: " + GetTag(oTarget));
#endif

	//unused 	AActor* oCurrent;
	//unused 	int32 i;
	float fRangeToSelected = GetDistanceBetween(OBJECT_SELF, oTarget);

	// If enemy is in melee range and has melee weapon -> attack
	if (_AI_IsHostileTargetValid(OBJECT_SELF, oTarget) && _AI_IsTargetInMeleeRange(OBJECT_SELF, oTarget) && IsUsingMeleeWeapon(OBJECT_SELF))
	{
		cCommand = CommandAttack(oTarget);
	}
	// If has ranged weapon and has ammo and is within the range of my ranged weapon -> attack
	else if (_AI_IsHostileTargetValid(OBJECT_SELF, oTarget) && IsUsingRangedWeapon(OBJECT_SELF)
		&& fRangeToSelected <= _AI_GetEquippedWeaponRange(OBJECT_SELF) &&
		(nLastCommand == COMMAND_TYPE_ATTACK || nLastCommand == COMMAND_TYPE_USE_ABILITY))
	{
		if (AI_BehaviorCheck_AttackBack(OBJECT_SELF) == FALSE_)
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "AI_DetermineCombatRound_Partial", "Creature behavior set to NOT attack back - aborting");
#endif
		}
		else
			cCommand = CommandAttack(oTarget);
	}

	// NOTE: there is another part of the player attack logic that does not go here, but to the attack event in rules_core
	// This deals with any creatures that attack me while I don't have any target

	if (GetCommandType(cCommand) == COMMAND_TYPE_INVALID)
	{
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "AI_DetermineCombatRound_Partial", "NO VALID ACTION - DOING NOTHING (waiting)", LOG_SEVERITY_CRITICAL);
#endif
		if (IsControlled(OBJECT_SELF))
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "AI_DetermineCombatRound_Partial", "Controlled follower - aborting wait - keeping queue empty", LOG_SEVERITY_CRITICAL);
#endif
			return;
		}
		cCommand = _AI_DoNothing(OBJECT_SELF, -1, nLastCommandStatus, FALSE_, TRUE_);
	}

	if (IsObjectValid(oTarget) && IsObjectHostile(OBJECT_SELF, oTarget))
	{
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "AI_DetermineCombatRound_Partial", "Attacking hostile target: allowing rest of party to attack");
#endif
		AI_SetPartyAllowedToAttack(OBJECT_SELF, TRUE_);
	}

	WR_AddCommand(OBJECT_SELF, cCommand);
}

int32 _AI_IsTargetInMeleeRange(AActor* OBJECT_SELF, AActor* oTarget)
{
	int32 nApp = GetAppearanceType(oTarget);
	if (!IsObjectValid(oTarget))
		return FALSE_;

	float fMaxRange = AI_MELEE_RANGE * SCALE_MULTIPLIER;
	float fPerSpace = GetM2DAFloat(TABLE_APPEARANCE, "PERSPACE", nApp) * SCALE_MULTIPLIER;
	if (fPerSpace > 1.0 * SCALE_MULTIPLIER && fPerSpace <= 2.0 * SCALE_MULTIPLIER)
		fMaxRange += (fPerSpace / 2);
	else if (fPerSpace > 2.0 * SCALE_MULTIPLIER)
		fMaxRange = AI_RANGE_MEDIUM * SCALE_MULTIPLIER;
	float fDistance = GetDistanceBetween(OBJECT_SELF, oTarget);
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_IsTargetInMeleeRange", "Target: " + GetTag(oTarget) + ", Distance: " + FloatToString(fDistance));
#endif

	if (fDistance <= fMaxRange)
		return TRUE_;

	return FALSE_;

	/*AActor* [] arEnemiesInMelee = GetCreaturesInMeleeRing(OBJECT_SELF, 0.0, 360.0, TRUE_);
	AActor* oCurrent;
	int32 nSize = GetArraySize(arEnemiesInMelee);
	int32 i;
	for(i = 0; i < nSize; i++)
	{
	oCurrent = arEnemiesInMelee[i];
	if(oCurrent == oTarget)
	return TRUE_;
	}

	return FALSE_;*/
}

float _AI_GetEquippedWeaponRange(AActor* OBJECT_SELF)
{
	AActor* oWeapon = GetItemInEquipSlot(INVENTORY_SLOT_MAIN, OBJECT_SELF);
	float fRange = GetItemRange(oWeapon);
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_GetEquippedWeaponRange", "Weapon: " + GetTag(oWeapon) + ", Weapon Range: " + FloatToString(fRange));
#endif

	return fRange;
}

void AI_SetPartyAllowedToAttack(AActor* OBJECT_SELF, int32 nStatus)
{
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "AI_SetPartyAllowedToAttack", "Setting party allowed-to-attack status to: " + IntToString(nStatus));
#endif
	GetModule()->AI_PARTY_CLEAR_TO_ATTACK = nStatus;
	// giving a wait command if the follower is doing nothing (to enable combat commands again)
	TArray<AActor*> arParty = GetPartyList();
	int32 nSize = arParty.Num();
	int32 i;
	AActor* oCurrent;
	FCommand cCurrentCommand;
	FCommand cWait = CommandWait(AI_DO_NOTHING_DELAY_QUICK);
	for (i = 0; i < nSize; i++)
	{
		oCurrent = arParty[i];
		cCurrentCommand = GetCurrentCommand(oCurrent);
		if (oCurrent != OBJECT_SELF && GetCommandQueueSize(oCurrent) == 0 && GetCommandType(cCurrentCommand) == COMMAND_TYPE_INVALID)
			WR_AddCommand(oCurrent, cWait);
	}
}

int32 _AI_GetWeaponSetEquipped(AActor* oTarget)
{
	int32 nRet = AI_WEAPON_SET_INVALID;
	int32 nActiveWeaponSet = GetActiveWeaponSet(oTarget);
	AActor* oWeapon = GetItemInEquipSlot(INVENTORY_SLOT_MAIN, oTarget, nActiveWeaponSet);

	if (GetItemType(oWeapon) == ITEM_TYPE_WEAPON_WAND)
		nRet = AI_WEAPON_SET_RANGED;
	else if (GetItemType(oWeapon) == ITEM_TYPE_WEAPON_RANGED)
		nRet = AI_WEAPON_SET_RANGED;
	else if (GetItemType(oWeapon) == ITEM_TYPE_WEAPON_MELEE)
		nRet = AI_WEAPON_SET_MELEE;
	else
		nRet = AI_WEAPON_SET_MELEE;

	//int32 nCurrentSet = GetActiveWeaponSet(OBJECT_SELF);
	//if(nCurrentSet == Items_GetRangedWeaponSet(OBJECT_SELF))
	//    nRet = AI_WEAPON_SET_RANGED;
	//else if(nCurrentSet == Items_GetMeleeWeaponSet(OBJECT_SELF))
	//    nRet = AI_WEAPON_SET_MELEE;

	return nRet;
}

int32 _AI_HasWeaponSet(AActor* OBJECT_SELF, int32 nWeaponSetType)
{
	int32 nRet = FALSE_;
	switch (nWeaponSetType)
	{
	case AI_WEAPON_SET_MELEE:
	{
		return Items_GetMeleeWeaponSet(OBJECT_SELF) != -1;
		break;
	}
	case AI_WEAPON_SET_RANGED:
	{
		return Items_GetRangedWeaponSet(OBJECT_SELF, TRUE_) != -1; // Also checks for ammo
		break;
	}
	default: LogError("_AI_HasWeaponSet: how did I get to default switch?"); break;
	}

	return -1;
}

FCommand _AI_SwitchWeaponSet(AActor* OBJECT_SELF, int32 nWeaponSetType)
{
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_SwitchWeaponSet", "Switching weapons to set type: " + IntToString(nWeaponSetType));
#endif

	int32 nSet = -1;
	switch (nWeaponSetType)
	{
	case AI_WEAPON_SET_MELEE:
	{
		nSet = Items_GetMeleeWeaponSet(OBJECT_SELF);
		break;
	}
	case AI_WEAPON_SET_RANGED:
	{
		nSet = Items_GetRangedWeaponSet(OBJECT_SELF, TRUE_);
		break;
	}
	}
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_SwitchWeaponSet", "Switching weapons to set: " + IntToString(nSet));
#endif

	FCommand cmd;

#ifdef DEBUG
	if (nSet == -1)
		Log_Trace_AI(OBJECT_SELF, "_AI_SwitchWeaponSet", "ERROR: FAILED TO FIND A SET TO SWITCH TO");
#endif

	if (nSet != -1)
	{
		cmd = CommandSwitchWeaponSet(nSet);
	}

	return cmd;
}

int32 _AI_GetPackageTable(AActor* OBJECT_SELF)
{
	// For now, returning a table number set on the creature

	int32 nTable = -1;
	//TODO Polymorph
	if (_AI_HasAIStatus(OBJECT_SELF, AI_STATUS_POLYMORPH))
	{
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_GetPackageTable", "POLYMORPH ON!");
#endif
		int32 nNewTable = -1;
		if (IsModalAbilityActive(OBJECT_SELF, ABILITY_SPELL_BEAR))
			nNewTable = GetM2DAInt(TABLE_SHAPECHANGE, "TacticsTable", ABILITY_SPELL_BEAR);
		else if (IsModalAbilityActive(OBJECT_SELF, ABILITY_SPELL_SPIDER_SHAPE))
			nNewTable = GetM2DAInt(TABLE_SHAPECHANGE, "TacticsTable", ABILITY_SPELL_SPIDER_SHAPE);
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_GetPackageTable", "polymorph new table: " + IntToString(nNewTable));
#endif
		if (nNewTable != -1)
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_GetPackageTable", "SHAPECHANGED - switching to new table: " + IntToString(nNewTable));
#endif
			nTable = nNewTable;
		}
	}
	else if (_AI_UseGUITables(OBJECT_SELF))
	{
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_GetPackageTable", "Creature is FOLLOWER - using GUI table");
#endif
	}

	if (nTable == -1)
	{
		nTable = GetPackageAI(OBJECT_SELF);

		if (nTable <= AI_TABLE_DEFAULT)
			nTable = AI_TABLE_DEFAULT;
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_GetPackageTable", "Using table: " + IntToString(nTable));
#endif
	}

	return nTable;
}

int32 _AI_UseGUITables(AActor* OBJECT_SELF)
{
	int32 nUseGUI = GetModule()->AI_USE_GUI_TABLES_FOR_FOLLOWERS;
	if (!nUseGUI)
		return FALSE_;

	return (GetFollowerState(OBJECT_SELF) != FOLLOWER_STATE_INVALID && !_AI_HasAIStatus(OBJECT_SELF, AI_STATUS_POLYMORPH));
}

int32 _AI_GetTacticsNum(int32 nPackageTable)
{
#ifdef DEBUG
	LogWarning("_AI_GetTacticsNum: requested nPackageTable is " + IntToString(nPackageTable));
#endif

	return GetM2DARows(nPackageTable);
}

int32 _AI_ExecuteTactic(AActor* aActor, int32 nPackageTable, int32 nTacticID, int32 nLastCommandStatus, int32 nUseGUITables)
{
	ADA2UE4Creature* OBJECT_SELF = Cast<ADA2UE4Creature>(aActor);
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteTactic", "START [Package Table: " + IntToString(nPackageTable) + "], TacticID: [" + IntToString(nTacticID) + "]");
#endif // DEBUG

	// read the package and retrieve the condition, target type and action

	// First checking if the tactic is enabled (followers only)
	if (nUseGUITables != 0 && !IsTacticEnabled(OBJECT_SELF, nTacticID))
	{
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteTactic", "Tactic not enabled - moving to next tactic");
#endif
		return FALSE_;
	}

	// MGB - March 9, 2009
	// Only evaluate the random trigger chance if we are using an AI Package table.
	// This should be done before retrieving the other numbers to trivially reject
	// rules that have extremely low chances of being fired.
	if (nUseGUITables == 0)
	{
		//int32 nTacticTriggerChance = GetM2DAInt(nPackageTable, HASH_USECHANCE, nTacticID);
		//TODO to hash or not to hash :D ?
		int32 nTacticTriggerChance = GetM2DAInt(nPackageTable, "UseChance", nTacticID);

		int32 nDifficulty = GetGameDifficulty();
		if (nDifficulty == GAME_DIFFICULTY_CASUAL)
		{
			// should not affect 100% tactics
			if (nTacticTriggerChance < 100 && nTacticTriggerChance >= 80)
				nTacticTriggerChance = 50;
			else if (nTacticTriggerChance >= 50 && nTacticTriggerChance < 80)
				nTacticTriggerChance = 25;
			else if (nTacticTriggerChance >= 20 && nTacticTriggerChance < 50)
				nTacticTriggerChance = 10;
			else if (nTacticTriggerChance >= 10 && nTacticTriggerChance < 20)
				nTacticTriggerChance = 5;
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteTactic", "Adjusted trigger chance (casual difficulty): " + IntToString(nTacticTriggerChance));
#endif
		}

		// Verifying random chance
		int32 nRandom = Random(100) + 1;
		//Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteTactic", "RANDOM: " + IntToString(nRandom) + ", Tactic Trigger Chance:" + IntToString(nTacticTriggerChance));
		if (nRandom > nTacticTriggerChance)
		{
#ifdef DEBUG
			//Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteTactic", "Tactic did not pass random check");
#endif
			return FALSE_;
		}
	}

	if (GetObjectActive(OBJECT_SELF) == FALSE_)
	{
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteTactic", "Object Inactive - exiting");
#endif
		return FALSE_;
	}

	//unused 	int32 nRet;
	int32 nTacticTargetType;
	int32 nTacticTargetBitField;
	int32 nTacticCondition;
	int32 nTacticCommand;
	int32 nTacticSubCommand;
	int32 nLastTacticID = OBJECT_SELF->AI_LAST_TACTIC;
	FString sTacticItemTag = GetTacticCommandItemTag(OBJECT_SELF, nTacticID);

	if (nUseGUITables != 0)
	{
		nTacticTargetType = GetTacticTargetType(OBJECT_SELF, nTacticID);
		nTacticCondition = GetTacticCondition(OBJECT_SELF, nTacticID);
		nTacticCommand = GetTacticCommand(OBJECT_SELF, nTacticID);
		nTacticSubCommand = GetTacticCommandParam(OBJECT_SELF, nTacticID);
	}
	else
	{
		nTacticTargetType = GetM2DAInt(nPackageTable, "TargetType", nTacticID);
		nTacticCondition = GetM2DAInt(nPackageTable, "Condition", nTacticID);
		nTacticCommand = GetM2DAInt(nPackageTable, "Command", nTacticID);
		nTacticSubCommand = GetM2DAInt(nPackageTable, "SubCommand", nTacticID);
	}

	nTacticTargetBitField = GetM2DAInt(TABLE_AI_TACTICS_TARGET_TYPE, "Type", nTacticTargetType);

#ifdef DEBUG
	FString sTacticSubCommand = IntToString(nTacticSubCommand);
	FString sTacticCommand = _AI_GetCommandString(nTacticCommand);

	if (nTacticCommand == AI_COMMAND_USE_ABILITY || nTacticCommand == AI_COMMAND_ACTIVATE_MODE || nTacticCommand == AI_COMMAND_DEACTIVATE_MODE)
		sTacticSubCommand = Ability_GetAbilityNameById(nTacticSubCommand);

	Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteTactic", "[" + IntToString(nTacticID) + "]" +
		"[Target: " + IntToString(nTacticTargetType) + "] " +
		"[Condition: " + IntToString(nTacticCondition) + "] " +
		"[" + sTacticCommand + "] " +
		"[" + sTacticSubCommand + "] ");

	if (sTacticItemTag != "")
		Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteTactic", "[Item Tag]: " + sTacticItemTag);
#endif

	int32 nTacticCondition_Base = GetM2DAInt(TABLE_TACTICS_CONDITIONS, "ConditionBase", nTacticCondition);

	// Retrieve condition details
	int32 nTacticCondition_ValidTarget = GetM2DAInt(TABLE_TACTICS_BASE_CONDITIONS, "ValidForTarget", nTacticCondition_Base);
	int32 nTacticCondition_Parameter = GetM2DAInt(TABLE_TACTICS_CONDITIONS, "ConditionParameter", nTacticCondition);
	int32 nTacticCondition_Parameter2 = GetM2DAInt(TABLE_TACTICS_CONDITIONS, "ConditionParameter2", nTacticCondition);

#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteTactic", "[Condition Valid for Target: " + IntToString(nTacticCondition_ValidTarget) + "] " +
		"[Condition Base: " + IntToString(nTacticCondition_Base) + "] " +
		"[Condition Parameter: " + IntToString(nTacticCondition_Parameter) + "] ");
#endif // DEBUG

	// Verify that the target is valid
	if ((nTacticTargetBitField & nTacticCondition_ValidTarget) == 0)
	{
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteTactic", "Tactic target type is not valid for the specified condition!", LOG_SEVERITY_CRITICAL);
#endif
		return FALSE_;
	}

	// Verify that the ability can be executed (ignoring any possible target)

	if (!_AI_IsCommandValid(OBJECT_SELF, nTacticCommand, nTacticSubCommand, nTacticTargetType))
	{
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteTactic", "Tactic command can not be executed");
#endif
		return FALSE_;
	}

	// Handling the tactic based on the base condition

	AActor* oTarget = nullptr; // Any tactic action will be applied to this AActor
	FVector lTarget;
	AActor* oFollowerSelectedTarget = nullptr; // for tracking follower targets
	if (IsFollower(OBJECT_SELF))
		oFollowerSelectedTarget = GetAttackTarget(OBJECT_SELF); // last target - hostile or not

	if (GetHasEffects(OBJECT_SELF, EFFECT_TYPE_CONFUSION) && nTacticTargetType == AI_TARGET_TYPE_SELF)
	{
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteTactic", "Confused creature trying to target SELF - aborting tactic");
#endif
		return FALSE_;
	}

	switch (nTacticCondition_Base)
	{
	case AI_BASE_CONDITION_HAS_EFFECT_APPLIED:
	{
		oTarget = _AI_Condition_GetCreatureWithAIStatus(OBJECT_SELF, nTacticCondition_Parameter, nTacticTargetType, nTacticCommand, nTacticSubCommand, nTacticID);
		break;
	}
	case AI_BASE_CONDITION_HP_LEVEL:
	{
		oTarget = _AI_Condition_GetCreatureWithHPLevel(OBJECT_SELF, nTacticCondition_Parameter, nTacticTargetType, nTacticCommand, nTacticSubCommand, nTacticID);
		break;
	}
	case AI_BASE_CONDITION_MANA_OR_STAMINA_LEVEL:
	{
		oTarget = _AI_Condition_GetCreatureWithManaOrStaminaLevel(OBJECT_SELF, nTacticCondition_Parameter, nTacticTargetType, nTacticCommand, nTacticSubCommand, nTacticID);
		break;
	}
	case AI_BASE_CONDITION_MOST_DAMAGED_IN_PARTY:
	{
		oTarget = _AI_Condition_GetNthMostDamagedCreatureInGroup(OBJECT_SELF, nTacticCondition_Parameter, nTacticTargetType, nTacticCommand, nTacticSubCommand, nTacticID);
		break;
	}
	case AI_BASE_CONDITION_CLUSTERED_WITH_SAME_GROUP:
	{
		lTarget = _AI_Condition_GetEnemyClusteredWithSameGroup(OBJECT_SELF, nTacticCondition_Parameter, nTacticCommand, nTacticSubCommand, nTacticID);
		break;
	}
	case AI_BASE_CONDITION_MOST_HATED_ENEMY:
	{
		oTarget = _AI_Condition_GetMostHatedEnemy(OBJECT_SELF, nTacticCondition_Parameter, nTacticCommand, nTacticSubCommand, nTacticID);
		break;
	}
	case AI_BASE_CONDITION_NEAREST_VISIBLE:
	{
		oTarget = _AI_Condition_GetNearestVisibleCreature(OBJECT_SELF, nTacticTargetType, nTacticCondition_Parameter, nTacticCommand, nTacticSubCommand, nTacticID);
		break;
	}
	case AI_BASE_CONDITION_NEAREST_RACE:
	{
		oTarget = _AI_Condition_GetNearestVisibleCreatureByRace(OBJECT_SELF, nTacticTargetType, nTacticCondition_Parameter, nTacticCommand, nTacticSubCommand, nTacticID);
		break;
	}
	case AI_BASE_CONDITION_NEAREST_CLASS:
	{
		oTarget = _AI_Condition_GetNearestVisibleCreatureByClass(OBJECT_SELF, nTacticTargetType, nTacticCondition_Parameter, nTacticCommand, nTacticSubCommand, nTacticID);
		break;
	}
	case AI_BASE_CONDITION_NEAREST_GENDER:
	{
		oTarget = _AI_Condition_GetNearestVisibleCreatureByGender(OBJECT_SELF, nTacticTargetType, nTacticCondition_Parameter, nTacticCommand, nTacticSubCommand, nTacticID);
		break;
	}
	case AI_BASE_CONDITION_ATTACKING_PARTY_MEMBER:
	{
		oTarget = _AI_Condition_GetNearestEnemyAttackingPartyMember(OBJECT_SELF, nTacticCommand, nTacticSubCommand, nTacticCondition_Parameter, nTacticID);
		break;
	}
	case AI_BASE_CONDITION_HAS_ANY_BUFF_EFFECT:
	{
		oTarget = _AI_Condition_GetNearestEnemyWithAnyBuffEffect(OBJECT_SELF, nTacticCommand, nTacticSubCommand, nTacticCondition_Parameter, nTacticID, nTacticTargetType);
		break;
	}
	case AI_BASE_CONDITION_FLIP_COVER_STATE:
	{
		oTarget = _AI_Condition_GetNearestFlipCoverByState(OBJECT_SELF, nTacticCondition_Parameter, nTacticID);
		break;
	}
	case AI_BASE_CONDITION_VULNERABLE_TO_DAMAGE:
	{
		oTarget = _AI_Condition_GetEnemyVulnerableToDamage(OBJECT_SELF, nTacticCondition_Parameter, nTacticCommand, nTacticSubCommand, nTacticID);
		break;
	}
	case AI_BASE_CONDITION_ANY:
	{
		oTarget = _AI_Condition_GetAnyTarget(OBJECT_SELF, nTacticTargetType, nTacticCommand, nTacticSubCommand, nTacticID);
		break;
	}
	case AI_BASE_CONDITION_HAS_AMMO_LEVEL:
	{
		oTarget = _AI_Condition_SelfHasAmmoLevel(OBJECT_SELF, nTacticCondition_Parameter);
		break;
	}
	case AI_BASE_CONDITION_HAS_ARMOR_TYPE:
	{
		oTarget = _AI_Condition_HasArmorType(OBJECT_SELF, nTacticTargetType, nTacticCondition_Parameter, nTacticCommand, nTacticSubCommand);
		break;
	}
	case AI_BASE_CONDITION_MOST_ENEMIES_HAVE_ARMOR_TYPE:
	{
		oTarget = _AI_Condition_MostEnemiesHaveArmorType(OBJECT_SELF, nTacticCondition_Parameter);
		break;
	}
	case AI_BASE_CONDITION_ALL_ENEMIES_HAVE_ARMOR_TYPE:
	{
		oTarget = _AI_Condition_AllEnemiesHaveArmorType(OBJECT_SELF, nTacticCondition_Parameter);
		break;
	}
	case AI_BASE_CONDITION_TARGET_HAS_RANK:
	{
		oTarget = _AI_Condition_TargetHasRank(OBJECT_SELF, nTacticTargetType, nTacticCondition_Parameter, nTacticID, nTacticCommand, nTacticSubCommand);
		break;
	}
	case AI_BASE_CONDITION_BEING_ATTACKED_BY_ATTACK_TYPE:
	{
		oTarget = _AI_Condition_BeingAttackedByAttackType(OBJECT_SELF, nTacticTargetType, nTacticCondition_Parameter, nTacticCommand, nTacticSubCommand, nTacticID);
		break;
	}
	case AI_BASE_CONDITION_USING_ATTACK_TYPE:
	case AI_BASE_CONDITION_TARGET_USING_ATTACK_TYPE_FOLLOWER:
	{
		oTarget = _AI_Condition_UsingAttackType(OBJECT_SELF, nTacticTargetType, nTacticCondition_Parameter, nTacticCommand, nTacticSubCommand);
		break;
	}
	case AI_BASE_CONDITION_MOST_ENEMIES_USING_ATTACK_TYPE:
	{
		oTarget = _AI_Condition_MostEnemiesUsingAttackType(OBJECT_SELF, nTacticCondition_Parameter);
		break;
	}
	case AI_BASE_CONDITION_ALL_ENEMIES_USING_ATTACK_TYPE:
	{
		oTarget = _AI_Condition_AllEnemiesUsingAttackType(OBJECT_SELF, nTacticCondition_Parameter);
		break;
	}
	case AI_BASE_CONDITION_AT_LEAST_X_ENEMIES_ARE_ALIVE:
	{
		oTarget = _AI_Condition_AtLeastXEnemiesAreAlive(OBJECT_SELF, nTacticTargetType, nTacticCondition_Parameter);
		break;
	}
	case AI_BASE_CONDITION_AT_LEAST_X_CREATURES_ARE_DEAD:
	{
		oTarget = _AI_Condition_AtLeastXCreaturesAreDead(OBJECT_SELF, nTacticTargetType, nTacticCondition_Parameter, nTacticCommand, nTacticSubCommand);
		break;
	}
	case AI_BASE_CONDITION_AT_LEAST_X_ALLIES_ARE_ALIVE:
	{
		oTarget = _AI_Condition_AtLeastXAlliesAreAlive(OBJECT_SELF, nTacticTargetType, nTacticCondition_Parameter, nTacticCondition_Parameter2);
		break;
	}
	case AI_BASE_CONDITION_ENEMY_AI_TARGET_AT_RANGE:
	{
		oTarget = _AI_Condition_GetTargetAtRange(OBJECT_SELF, nTacticTargetType, nTacticCondition_Parameter, nTacticCommand, nTacticSubCommand);
		break;
	}
	case AI_BASE_CONDITION_TARGET_AT_FLANK_LOCATION:
	{
		oTarget = _AI_Condition_GetTargetAtFlankLocation(OBJECT_SELF, nTacticCondition_Parameter, nTacticTargetType);
		break;
	}
	case AI_BASE_CONDITION_SURROUNDED_BY_TARGETS:
	{
		oTarget = _AI_Condition_SurroundedByAtLeastXEnemies(OBJECT_SELF, nTacticCommand, nTacticSubCommand, nTacticCondition_Parameter, nTacticID);
		break;
	}
	case AI_BASE_CONDITION_USING_RANGED_ATTACKS_AT_RANGE:
	{
		oTarget = _AI_Condition_GetTargetUsingRangedWeaponsAtRange(OBJECT_SELF, nTacticTargetType, nTacticCondition_Parameter, nTacticCommand, nTacticSubCommand);
		break;
	}
	case AI_BASE_CONDITION_PARTY_MEMBERS_TARGET:
	{
		oTarget = _AI_Condition_GetPartyMemberTarget(OBJECT_SELF, nTacticCommand, nTacticSubCommand, nTacticCondition_Parameter, nTacticID);
		break;
	}
	case AI_BASE_CONDITION_SELF_HP_LEVEL:
	{
		oTarget = _AI_Condition_SelfHPLevel(OBJECT_SELF, nTacticCondition_Parameter, nTacticTargetType, nTacticCommand, nTacticSubCommand, nTacticID);
		break;
	}
	case AI_BASE_CONDITION_SELF_MANA_STAMINA_LEVEL:
	{
		oTarget = _AI_Condition_SelfManaStaminaLevel(OBJECT_SELF, nTacticCondition_Parameter, nTacticTargetType, nTacticCommand, nTacticSubCommand, nTacticID);
		break;
	}
	case AI_BASE_CONDITION_FOLLOWER_AI_TARGET_AT_RANGE:
	{
		oTarget = _AI_Condition_GetTargetAtRange(OBJECT_SELF, nTacticTargetType, nTacticCondition_Parameter, nTacticCommand, nTacticSubCommand);
		break;
	}


	}

	// If target type is SELF: fail in case was trying to trigger an ability that tries to cure something SELF doesn't have
	// For example: casting 'remove poison' while SELF does not have any poison effect
	// NOTE: this is handled for allies in _AIGetAllies
	if (nTacticTargetType == AI_TARGET_TYPE_SELF && nTacticCommand == AI_COMMAND_USE_ABILITY)
	{
		if (!_AI_IsTargetValidForBeneficialAbility(OBJECT_SELF, nTacticSubCommand))
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteTactic", "Trying to apply a beneficial ability to SELF, but SELF does not need it");
#endif
			oTarget = nullptr;
		}
	}

	if (!IsLocationValid(lTarget) && !IsObjectValid(oTarget))
	{
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteTactic", "No valid target for condition");
#endif
		return FALSE_;
	}
	if (IsLocationValid(lTarget))
	{
		//FVector vDebug = GetPositionFromLocation(lTarget);
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteTactic", "VALID LOCATION: " + lTarget.ToString());
#endif
	}

#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteTactic", "GOT TARGET: " + GetTag(oTarget));
#endif


	// Check if the command is valid on the target and execute the command
	FCommand cTacticCommand;
	int32 nAbilityTargetType;
	int32 nStationary = OBJECT_SELF->AI_FLAG_STATIONARY;
	if (IsFollower(OBJECT_SELF)) nStationary = FALSE_;

	//Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteTactic", "Creature stationary state: " + IntToString(nStationary));

	switch (nTacticCommand)
	{
	case AI_COMMAND_USE_HEALTH_POTION_MOST:
	{
		float fCurrentHealth = GetCurrentHealth(OBJECT_SELF);
		float fMaxHealth = GetMaxHealth(OBJECT_SELF);
		if (fCurrentHealth == fMaxHealth)
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteTactic", "health full, not using health potion");
#endif
			return FALSE_;
		}
		AActor* oItem = _AI_GetPotionByFilter(OBJECT_SELF, AI_POTION_TYPE_HEALTH, AI_POTION_LEVEL_MOST_POWERFUL);
		cTacticCommand = _AI_GetPotionUseCommand(OBJECT_SELF, oItem);

		if (GetCommandType(cTacticCommand) == COMMAND_TYPE_INVALID)
			return FALSE_;

		break;
	}
	case AI_COMMAND_USE_HEALTH_POTION_LEAST:
	{
		float fCurrentHealth = GetCurrentHealth(OBJECT_SELF);
		float fMaxHealth = GetMaxHealth(OBJECT_SELF);
		if (fCurrentHealth == fMaxHealth)
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteTactic", "health full, not using health potion");
#endif
			return FALSE_;
		}
		AActor* oItem = _AI_GetPotionByFilter(OBJECT_SELF, AI_POTION_TYPE_HEALTH, AI_POTION_LEVEL_LEAST_POWERFUL);
		cTacticCommand = _AI_GetPotionUseCommand(OBJECT_SELF, oItem);

		if (GetCommandType(cTacticCommand) == COMMAND_TYPE_INVALID)
			return FALSE_;
		break;
	}
	case AI_COMMAND_USE_LYRIUM_POTION_MOST:
	{
		float fCurrentMana = GetCurrentManaStamina(OBJECT_SELF);
		float fMaxMana = IntToFloat(GetMaxManaStamina(OBJECT_SELF));
		if (fCurrentMana == fMaxMana)
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteTactic", "mana full, not using mana potion");
#endif
			return FALSE_;
		}
		AActor* oItem = _AI_GetPotionByFilter(OBJECT_SELF, AI_POTION_TYPE_MANA, AI_POTION_LEVEL_MOST_POWERFUL);
		cTacticCommand = _AI_GetPotionUseCommand(OBJECT_SELF, oItem);

		if (GetCommandType(cTacticCommand) == COMMAND_TYPE_INVALID)
			return FALSE_;

		break;
	}
	case AI_COMMAND_USE_LYRIUM_POTION_LEAST:
	{
		float fCurrentMana = GetCurrentManaStamina(OBJECT_SELF);
		float fMaxMana = IntToFloat(GetMaxManaStamina(OBJECT_SELF));
		if (fCurrentMana == fMaxMana)
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteTactic", "mana full, not using mana potion");
#endif
			return FALSE_;
		}
		AActor* oItem = _AI_GetPotionByFilter(OBJECT_SELF, AI_POTION_TYPE_MANA, AI_POTION_LEVEL_LEAST_POWERFUL);
		cTacticCommand = _AI_GetPotionUseCommand(OBJECT_SELF, oItem);

		if (GetCommandType(cTacticCommand) == COMMAND_TYPE_INVALID)
			return FALSE_;
		break;
	}
	case AI_COMMAND_RUN_SCRIPT:
	{
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteTactic", "Running custom script (custom AI event)");
#endif
		FGameEvent evCustomAI = Event(EVENT_TYPE_HANDLE_CUSTOM_AI);
		FCommand cLast = GetPreviousCommand(OBJECT_SELF);
		int32 nLastCommand = GetCommandType(cLast);
		SendEventHandleCustomAI(OBJECT_SELF, nullptr, nLastCommand, nLastCommandStatus,
			-1, nTacticTargetType, nTacticSubCommand, nTacticID);

		return TRUE_;
	}
	case AI_COMMAND_SWITCH_TO_MELEE:
	{
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteTactic", "Switching to melee weapon set");
#endif
		if (_AI_GetWeaponSetEquipped(OBJECT_SELF) == AI_WEAPON_SET_MELEE)
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteTactic", "Creature already has melee weapons equipped - aborting command");
#endif
			return FALSE_;
		}
		cTacticCommand = _AI_SwitchWeaponSet(OBJECT_SELF, AI_WEAPON_SET_MELEE);
		break;
	}
	case AI_COMMAND_SWITCH_TO_RANGED:
	{
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteTactic", "Switching to ranged weapon set");
#endif
		if (_AI_GetWeaponSetEquipped(OBJECT_SELF) == AI_WEAPON_SET_RANGED)
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteTactic", "Creature already has ranged weapons equipped - aborting command");
#endif
			return FALSE_;
		}
		cTacticCommand = _AI_SwitchWeaponSet(OBJECT_SELF, AI_WEAPON_SET_RANGED);
		break;
	}
	case AI_COMMAND_JUMP_TO_LATER_TACTIC:
	{
		if (nTacticSubCommand != -1 && nTacticSubCommand <= nTacticID)
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteTactic", "Jump to later tactic: invalid value (must be greater then current tactic id)");
#endif
			return FALSE_;
		}
		return nTacticSubCommand;
	}
	case AI_COMMAND_FLY:
	{
		AActor* oTurnTo = nullptr;

		if (_AI_CheckMoveTimer(OBJECT_SELF) == FALSE_)
			return FALSE_;

		switch (nTacticSubCommand)
		{
		case AI_FLY_TURN_MOST_HATED:
		{
			oTurnTo = AI_Threat_GetThreatTarget(OBJECT_SELF);
			cTacticCommand = _AI_GetFlyCommand(OBJECT_SELF, oTurnTo);
			OBJECT_SELF->AI_LAST_TACTIC = AI_TACTIC_ID_MOVE; // the last tactic being used
			break;
		}
		case AI_FLY_TURN_NEAREST_AI_WP:
		{
			oTurnTo = UT_GetNearestObjectByTag(OBJECT_SELF, AI_WP_MOVE);
			cTacticCommand = _AI_GetFlyCommand(OBJECT_SELF, oTurnTo);
			OBJECT_SELF->AI_LAST_TACTIC = AI_TACTIC_ID_MOVE; // the last tactic being used
			break;
		}
		case AI_FLY_TURN_NEAREST_ALLY:
		{
			TArray<AActor*> arAllies = _AI_GetAllies(OBJECT_SELF, AI_COMMAND_MOVE, nTacticSubCommand);
			oTurnTo = arAllies[0];
			cTacticCommand = _AI_GetFlyCommand(OBJECT_SELF, oTurnTo);
			OBJECT_SELF->AI_LAST_TACTIC = AI_TACTIC_ID_MOVE; // the last tactic being used
			break;
		}
		case AI_FLY_TURN_NEAREST_ENEMY:
		{
			TArray<AActor*> arEnemies = _AI_GetEnemies(OBJECT_SELF, AI_COMMAND_MOVE, nTacticSubCommand);
			oTurnTo = arEnemies[0];
			cTacticCommand = _AI_GetFlyCommand(OBJECT_SELF, oTurnTo);
			OBJECT_SELF->AI_LAST_TACTIC = AI_TACTIC_ID_MOVE; // the last tactic being used
			break;
		}
		case AI_FLY_APPROACH_MOST_HATED:
		{
			oTurnTo = AI_Threat_GetThreatTarget(OBJECT_SELF);
			cTacticCommand = _AI_GetFlyCommand(OBJECT_SELF, oTurnTo, TRUE_);
			OBJECT_SELF->AI_LAST_TACTIC = AI_TACTIC_ID_MOVE; // the last tactic being used
			break;
		}
		case AI_FLY_APPROACH_NEAREST_ENEMY:
		{
			TArray<AActor*> arEnemies = _AI_GetEnemies(OBJECT_SELF, AI_COMMAND_MOVE, nTacticSubCommand);
			oTurnTo = arEnemies[0];
			cTacticCommand = _AI_GetFlyCommand(OBJECT_SELF, oTurnTo, TRUE_);
			OBJECT_SELF->AI_LAST_TACTIC = AI_TACTIC_ID_MOVE; // the last tactic being used
			break;
		}
		case AI_FLY_APPROACH_AI_WP_NEAREST_TO_MOST_HATED:
		{
			AActor* oEnemy = AI_Threat_GetThreatTarget(OBJECT_SELF);
			oTurnTo = UT_GetNearestObjectByTag(oEnemy, AI_WP_MOVE);
			cTacticCommand = _AI_GetFlyCommand(OBJECT_SELF, oTurnTo, TRUE_);
			OBJECT_SELF->AI_LAST_TACTIC = AI_TACTIC_ID_MOVE; // the last tactic being used
			break;
		}
		case AI_FLY_APPROACH_AI_WP_NEAREST_TO_NEAREST_ENEMY:
		{
			TArray<AActor*> arEnemies = _AI_GetEnemies(OBJECT_SELF, AI_COMMAND_MOVE, nTacticSubCommand);
			AActor* oEnemy = arEnemies[0];
			oTurnTo = UT_GetNearestObjectByTag(oEnemy, AI_WP_MOVE);
			cTacticCommand = _AI_GetFlyCommand(OBJECT_SELF, oTurnTo, TRUE_);
			OBJECT_SELF->AI_LAST_TACTIC = AI_TACTIC_ID_MOVE; // the last tactic being used
			break;
		}
		}
		if (GetCommandType(cTacticCommand) == COMMAND_TYPE_WAIT)
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteTactic", "Wait command for fly/turn AI action - aborting AI instead");
#endif
			return FALSE_;
		}

		if (!IsObjectValid(oTurnTo))
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteTactic", "Invalid turn target");
#endif
			return FALSE_;
		}
		if (GetCommandType(cTacticCommand) == COMMAND_TYPE_INVALID)
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteTactic", "Invalid fly/turn command");
#endif
			return FALSE_;
		}
		_AI_SetMoveTimer(OBJECT_SELF);
		break;
	}
	case AI_COMMAND_MOVE:
	{
		AActor* oMoveTo = nullptr;
		if (nStationary > 0)
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteTactic", "Creature stationary - can't execute move command");
#endif
			return FALSE_;
		}
		if (nLastCommandStatus == COMMAND_FAILED_TIMEOUT)
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteTactic", "Last command failed on timeout - can't execute move command");
#endif
			return FALSE_;
		}
		if (_AI_CheckMoveTimer(OBJECT_SELF) == FALSE_)
			return FALSE_;

		switch (nTacticSubCommand)
		{
		case AI_MOVE_HATED_ENEMY:
		{
			oMoveTo = AI_Threat_GetThreatTarget(OBJECT_SELF);
			cTacticCommand = CommandMoveToActor(oMoveTo, TRUE_, 0.0, TRUE_);
			OBJECT_SELF->AI_LAST_TACTIC = AI_TACTIC_ID_MOVE; // the last tactic being used
			break;
		}
		case AI_MOVE_NEAREST_AI_WP:
		{
			oMoveTo = UT_GetNearestObjectByTag(OBJECT_SELF, AI_WP_MOVE);
			cTacticCommand = CommandMoveToActor(oMoveTo, TRUE_, 0.0, TRUE_);
			OBJECT_SELF->AI_LAST_TACTIC = AI_TACTIC_ID_MOVE; // the last tactic being used
			break;
		}
		case AI_MOVE_NEAREST_ALLY:
		{
			TArray<AActor*> arAllies = _AI_GetAllies(OBJECT_SELF, AI_COMMAND_MOVE, nTacticSubCommand);
			oMoveTo = arAllies[0];
			cTacticCommand = CommandMoveToActor(oMoveTo, TRUE_, 0.0, TRUE_);
			OBJECT_SELF->AI_LAST_TACTIC = AI_TACTIC_ID_MOVE; // the last tactic being used
			break;
		}
		case AI_MOVE_NEAREST_ENEMY:
		{
			TArray<AActor*> arEnemies = _AI_GetEnemies(OBJECT_SELF, AI_COMMAND_MOVE, nTacticSubCommand);
			oMoveTo = arEnemies[0];
			cTacticCommand = CommandMoveToActor(oMoveTo, TRUE_, 0.0, TRUE_);
			OBJECT_SELF->AI_LAST_TACTIC = AI_TACTIC_ID_MOVE; // the last tactic being used
			break;
		}
		case AI_MOVE_RANDOM_AI_WP:
		{
			int32 nRand = Random(3);
			TArray<AActor*> arWPs = GetNearestObjectByTag(OBJECT_SELF, AI_WP_MOVE, OBJECT_TYPE_WAYPOINT, 3);
			oMoveTo = arWPs[nRand];
			if (!IsObjectValid(oMoveTo)) // in case there are not enough AI waypoints
				oMoveTo = arWPs[0];
			cTacticCommand = CommandMoveToActor(oMoveTo, TRUE_, 0.0, TRUE_);
			OBJECT_SELF->AI_LAST_TACTIC = AI_TACTIC_ID_MOVE; // the last tactic being used
			break;
		}
		case AI_MOVE_AWAY_FROM_ENEMY_MEDIUM:
		{
			TArray<AActor*> arEnemies = _AI_GetEnemies(OBJECT_SELF, AI_COMMAND_MOVE, nTacticSubCommand);
			oMoveTo = arEnemies[0]; // Move away from
			float fDistance = GetDistanceBetween(OBJECT_SELF, oMoveTo);
			if (fDistance >= AI_MOVE_AWAY_DISTANCE_MEDIUM * SCALE_MULTIPLIER)
			{
#ifdef DEBUG
				Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteTactic", "NOT MOVING AWAY - already far away from target");
#endif
				return FALSE_;
			}
			cTacticCommand = CommandMoveAwayFromObject(oMoveTo, AI_MOVE_AWAY_DISTANCE_MEDIUM * SCALE_MULTIPLIER, TRUE_);
			OBJECT_SELF->AI_LAST_TACTIC = AI_TACTIC_ID_MOVE; // the last tactic being used
			break;
		}
		case AI_MOVE_AWAY_FROM_ENEMY_SHORT:
		{
			TArray<AActor*> arEnemies = _AI_GetEnemies(OBJECT_SELF, AI_COMMAND_MOVE, nTacticSubCommand);
			oMoveTo = arEnemies[0]; // Move away from
			float fDistance = GetDistanceBetween(OBJECT_SELF, oMoveTo);
			if (fDistance >= AI_MOVE_AWAY_DISTANCE_SHORT * SCALE_MULTIPLIER)
			{
#ifdef DEBUG
				Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteTactic", "NOT MOVING AWAY - already far away from target");
#endif
				return FALSE_;
			}
			cTacticCommand = CommandMoveAwayFromObject(oMoveTo, AI_MOVE_AWAY_DISTANCE_SHORT * SCALE_MULTIPLIER, TRUE_);
			OBJECT_SELF->AI_LAST_TACTIC = AI_TACTIC_ID_MOVE; // the last tactic being used
			break;
		}
		case AI_MOVE_AWAY_FROM_ENEMY_RANDOM:
		{
			TArray<AActor*> arEnemies = _AI_GetEnemies(OBJECT_SELF, AI_COMMAND_MOVE, nTacticSubCommand);
			oMoveTo = arEnemies[0]; // Move away from
			float fDistanceToMoveAway = FMath::RandRange(FloatToInt(AI_MOVE_AWAY_DISTANCE_MEDIUM * SCALE_MULTIPLIER - AI_MOVE_AWAY_DISTANCE_SHORT * SCALE_MULTIPLIER), FloatToInt(AI_MOVE_AWAY_DISTANCE_SHORT * SCALE_MULTIPLIER));

			float fDistance = GetDistanceBetween(OBJECT_SELF, oMoveTo);
			if (fDistance >= fDistanceToMoveAway)
			{
#ifdef DEBUG
				Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteTactic", "NOT MOVING AWAY - already far away from target");
#endif
				return FALSE_;
			}
			cTacticCommand = CommandMoveAwayFromObject(oMoveTo, fDistanceToMoveAway, TRUE_);
			OBJECT_SELF->AI_LAST_TACTIC = AI_TACTIC_ID_MOVE; // the last tactic being used
			break;
		}
		case AI_MOVE_AWAY_FROM_ENEMY_COWARD:
		{
			TArray<AActor*> arEnemies = _AI_GetEnemies(OBJECT_SELF, AI_COMMAND_MOVE, nTacticSubCommand);
			oMoveTo = arEnemies[0]; // Move away from
			float fDistance = GetDistanceBetween(OBJECT_SELF, oMoveTo);
			if (fDistance < AI_MOVE_AWAY_DISTANCE_MEDIUM * SCALE_MULTIPLIER)
				// run away
				cTacticCommand = CommandMoveToActor(oMoveTo, TRUE_, AI_MOVE_AWAY_DISTANCE_MEDIUM * SCALE_MULTIPLIER, FALSE_);
			else
				// cower in fear
				cTacticCommand = CommandPlayAnimation(602);

			break;
		}
		}

		if (!IsObjectValid(oMoveTo))
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteTactic", "Invalid movement target");
#endif
			return FALSE_;
		}
		if (GetCommandType(cTacticCommand) == COMMAND_TYPE_INVALID)
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteTactic", "Invalid move command");
#endif
			return FALSE_;
		}
		_AI_SetMoveTimer(OBJECT_SELF);
		break;
	}
	case AI_COMMAND_USE_ITEM:
	{
		// ASSUMING ITEMS CAN BE USED ONLY ON SELF

		if (Ability_CheckUseConditions(OBJECT_SELF, OBJECT_SELF, nTacticSubCommand) == FALSE_)
			return FALSE_; // failed tactic

		if (_AI_CanUseAbility(OBJECT_SELF, nTacticSubCommand, OBJECT_SELF) == FALSE_)
			return FALSE_; // can't use specific ability

						   // Command is valid to be executed on the target
		FVector vNul;
		cTacticCommand = CommandUseAbility(nTacticSubCommand, OBJECT_SELF, vNul, -1.0, sTacticItemTag);
		break;
	}
	case AI_COMMAND_USE_PLACEABLE:
	{
		//TODO AI_COMMAND_USE_PLACEABLE ai_main_h.cpp
		// Target should be valid now
		// At this moment the user should register an action on the placeable
		/*if (nStationary > 0)
		{
		float fDistance = GetDistanceBetween(OBJECT_SELF, oTarget);
		if (fDistance > AI_STATIONARY_RANGE * SCALE_MULTIPLIER)
		{
		#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteTactic", "Creature stationary - placeable too far away to execute command");
		#endif
		return FALSE_;
		}
		}

		int32 nCount = oTarget->PLC_FLIP_COVER_USE_COUNT;
		nCount++;
		oTarget->PLC_FLIP_COVER_USE_COUNT=nCount;

		// Check that I'm not already using a flip cover
		AActor* oPlaceable = OBJECT_SELF->AI_PLACEABLE_BEING_USED;
		if (IsObjectValid(oPlaceable) && !IsDead(oPlaceable))
		{
		#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteTactic", "I'm already using a placeable!", LOG_SEVERITY_WARNING);
		#endif
		return FALSE_;
		}
		OBJECT_SELF->AI_PLACEABLE_BEING_USED=oTarget);

		cTacticCommand = CommandUseObject(oTarget, PLACEABLE_ACTION_USE);*/
		break;
	}
	case AI_COMMAND_ATTACK:
	{
		// If target is not a hostile creature then fail the attack
		if (!IsObjectHostile(OBJECT_SELF, oTarget))
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteTactic", "Non-hostile target for ATTACK action!", LOG_SEVERITY_WARNING);
#endif
			return FALSE_;
		}
		cTacticCommand = _AI_ExecuteAttack(OBJECT_SELF, oTarget, nLastCommandStatus);

		break;
	}
	case AI_COMMAND_ACTIVATE_MODE:
	{
		if (Ability_CheckUseConditions(OBJECT_SELF, oTarget, nTacticSubCommand) == FALSE_)
			return FALSE_; // failed tactic

		cTacticCommand = CommandUseAbility(nTacticSubCommand, OBJECT_SELF);
		break;
	}
	case AI_COMMAND_DEACTIVATE_MODE:
	{
		// No need to check use conditions for the ability since we are trying to deactivate it

		cTacticCommand = CommandUseAbility(nTacticSubCommand, OBJECT_SELF);
		break;
	}
	case AI_COMMAND_WAIT:
	{
		int32 bQuick = FALSE_;
		if (nTacticSubCommand == 1) // this is a wait with cooldown (used for rogues for example)
		{
			int32 nMoveStart = OBJECT_SELF->AI_WAIT_TIMER;
			int32 nCurrentTime = GetTime();
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteTactic", "Wait time dif: " + IntToString(nCurrentTime - nMoveStart));
#endif
			if (nMoveStart != 0 && nCurrentTime - nMoveStart <= AI_WAIT_MIN_TIME)
			{
#ifdef DEBUG
				Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteTactic", "Last wait happened too soon");
#endif
				return FALSE_;
			}
			OBJECT_SELF->AI_WAIT_TIMER = nCurrentTime;
			bQuick = TRUE_;
		}
		cTacticCommand = _AI_DoNothing(OBJECT_SELF, nLastTacticID, nLastCommandStatus, FALSE_, TRUE_);
		break;
	}
	case AI_COMMAND_USE_ABILITY:
	{

		// -----------------------------------------------------------------
		// Ability usage disabled
		// -----------------------------------------------------------------
		if (GetCreatureFlag(OBJECT_SELF, CREATURE_RULES_FLAG_AI_NO_ABILITIES))
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteTactic", "ABORT: AI_COMMAND_USE_ABILITY - CREATURE_RULES_FLAG_AI_NO_ABILITIES was set.", LOG_SEVERITY_WARNING);
			LogWarning("ERROR! ability use disabled by a debug flag - call Yaron if you weren't using debug scripts!!!");
#endif
			return FALSE_;
		}

		// Checking target types
		nAbilityTargetType = Ability_GetAbilityTargetType(nTacticSubCommand, Ability_GetAbilityType(nTacticSubCommand));
		// NOTICE: tactic target types are not exactly the same as ability target types

		// Make sure the target type specified for the ability matches a target type that is valid for the ability
		switch (nTacticTargetType)
		{
		case AI_TARGET_TYPE_ENEMY:
		case AI_TARGET_TYPE_MOST_HATED:
		{
			if (!(nAbilityTargetType & TARGET_TYPE_HOSTILE_CREATURE))
			{
				// Trying to find a target anyways
#ifdef DEBUG
				Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteTactic", "Hostile target for an ability that does not support hostile targets - trying to find a new target!", LOG_SEVERITY_WARNING);
#endif
				if (nAbilityTargetType & TARGET_TYPE_SELF)
				{
#ifdef DEBUG
					Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteTactic", "Target will now be OBJECT_SELF");
#endif
					oTarget = OBJECT_SELF;
				}
				else if (nAbilityTargetType & TARGET_TYPE_GROUND)
				{
#ifdef DEBUG
					Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteTactic", "Target will now be location of target");
#endif
					lTarget = GetLocation(oTarget);
				}
				else
					return FALSE_;
			}
			break;
		}
		case AI_TARGET_TYPE_ALLY:
		{
			if (!(nAbilityTargetType & TARGET_TYPE_FRIENDLY_CREATURE))
			{
				if (nAbilityTargetType & TARGET_TYPE_SELF)
				{
#ifdef DEBUG
					Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteTactic", "Ally target for an ability that does not support friendly targets - Target will now be SELF!", LOG_SEVERITY_WARNING);
#endif
					oTarget = OBJECT_SELF;
				}
				else
				{
#ifdef DEBUG
					Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteTactic", "Ally target for an ability that does not support friendly targets - FAILING TACTIC!", LOG_SEVERITY_WARNING);
#endif
					return FALSE_;
				}
			}
			break;
		}
		case AI_TARGET_TYPE_PLACEABLE:
		{
			if (!(nAbilityTargetType & TARGET_TYPE_PLACEABLE))
			{
#ifdef DEBUG
				Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteTactic", "Placeable target for an ability that does not support placeable targets - FAILING TACTIC!", LOG_SEVERITY_WARNING);
#endif
				return FALSE_;
			}
			break;
		}
		case AI_TARGET_TYPE_SELF:
		{
			if (!(nAbilityTargetType & TARGET_TYPE_SELF))
			{
#ifdef DEBUG
				Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteTactic", "Self target for an ability that does not support self targets - trying to find a different target!", LOG_SEVERITY_WARNING);
#endif
				if (nAbilityTargetType & TARGET_TYPE_HOSTILE_CREATURE)
				{
					if (IsFollower(OBJECT_SELF))
					{
						if (_AI_IsHostileTargetValid(OBJECT_SELF, oFollowerSelectedTarget))
							oTarget = oFollowerSelectedTarget;
						else
							oTarget = _AI_Condition_GetNearestVisibleCreature(OBJECT_SELF, AI_TARGET_TYPE_ENEMY, 1, nTacticCommand, nTacticSubCommand, nTacticID);
					}
					else
						oTarget = _AI_Condition_GetMostHatedEnemy(OBJECT_SELF, 1, nTacticCommand, nTacticSubCommand, nTacticID);
				}
				else if (nAbilityTargetType & TARGET_TYPE_FRIENDLY_CREATURE)
				{
					oTarget = _AI_Condition_GetAnyTarget(OBJECT_SELF, AI_TARGET_TYPE_ALLY, nTacticCommand, nTacticSubCommand, nTacticID);
				}
				else
				{
#ifdef DEBUG
					Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteTactic", "Can not find a different target for this ability target type!", LOG_SEVERITY_WARNING);
#endif
					return FALSE_;
				}

				if (!IsObjectValid(oTarget))
				{
#ifdef DEBUG
					Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteTactic", "Failed to find a secondary target for this ability!", LOG_SEVERITY_WARNING);
#endif
					return FALSE_;
				}

			}
			break;
		}
		}

		if (nLastCommandStatus == COMMAND_FAILED_TIMEOUT && IsObjectHostile(OBJECT_SELF, oTarget))
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteTactic", "Last command failed on timeout and this ability targets hostiles - aborting ability use");
#endif
			return FALSE_;
		}

		if (Ability_CheckUseConditions(OBJECT_SELF, oTarget, nTacticSubCommand) == FALSE_)
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteTactic", "Ability_CheckUseConditions is FALSE for " + IntToString(nTacticSubCommand), LOG_SEVERITY_CRITICAL);
#endif
			return FALSE_; // failed tactic
		}

		if (_AI_CanUseAbility(OBJECT_SELF, nTacticSubCommand, oTarget) == FALSE_)
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteTactic", "_AI_CanUseAbility is FALSE for " + IntToString(nTacticSubCommand), LOG_SEVERITY_CRITICAL);
#endif
			return FALSE_; // can't use specific ability
		}

		FVector vTarget;

		if (IsLocationValid(lTarget))
		{
			vTarget = lTarget;
			oTarget = nullptr;
		}
		// Command is valid to be executed on the target
		cTacticCommand = CommandUseAbility(nTacticSubCommand, oTarget, vTarget);
		break;
	}
	}

	if (GetCommandType(cTacticCommand) == COMMAND_TYPE_INVALID)
	{
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteTactic", "INVALID TACTIC COMMAND - FAILING TACTIC", LOG_SEVERITY_CRITICAL);
#endif
		return FALSE_;;
	}

	if (GetHasEffects(OBJECT_SELF, EFFECT_TYPE_CONFUSION) && oTarget == OBJECT_SELF)
	{
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteTactic", "Confused creature trying to target SELF (second check) - aborting tactic");
#endif
		return FALSE_;
	}

	if (IsFollower(OBJECT_SELF) && IsObjectValid(oTarget) && IsObjectHostile(OBJECT_SELF, oTarget)
		&& !AI_GetPartyAllowedToAttack(OBJECT_SELF))
	{

		if (IsControlled(OBJECT_SELF))
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteTactic", "Controlled follower attacking - clearing rest of party to target enemies");
#endif
			AI_SetPartyAllowedToAttack(OBJECT_SELF, TRUE_);
		}
		else if (AI_BehaviorCheck_AttackOnCombatStart(OBJECT_SELF) == FALSE_)
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteTactic", "Non-controlled follower trying to attack a hostile before being allowed - trying to move closer to leader");
#endif
			cTacticCommand = _AI_MoveToControlled(OBJECT_SELF, nLastCommandStatus);
		}
	}

	// Flagging last tactic used
	OBJECT_SELF->AI_LAST_TACTIC = nTacticID;
	float fTimer = AI_COMMAND_TIMER;
	if (oTarget == OBJECT_SELF)
		fTimer = 0.0;
	WR_AddCommand(OBJECT_SELF, cTacticCommand, FALSE_, FALSE_, -1, fTimer);

#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteTactic", "***** TACTIC EXECUTED! *****");
#endif
	return TRUE_;

}

FString _AI_GetCommandString(int32 nAICommand)
{
	FString sRet = "INVALID";

	switch (nAICommand)
	{
	case AI_COMMAND_ACTIVATE_MODE: sRet = "***** ACTIVATE MODE *****"; break;
	case AI_COMMAND_ATTACK: sRet = "***** ATTACK *****"; break;
	case AI_COMMAND_DEACTIVATE_MODE: sRet = "***** DEACTIVATE MODE *****"; break;
	case AI_COMMAND_USE_ABILITY: sRet = "USE ABILITY"; break;
	case AI_COMMAND_USE_ITEM: sRet = "***** USE ITEM *****"; break;
	case AI_COMMAND_USE_PLACEABLE: sRet = "***** USE PLACEABLE *****"; break;
	case AI_COMMAND_WAIT: sRet = "***** WAIT *****"; break;
	case AI_COMMAND_MOVE: sRet = "***** MOVE *****"; break;
	case AI_COMMAND_JUMP_TO_LATER_TACTIC: sRet = "***** JUMP TO LATER TACTIC *****"; break;
	case AI_COMMAND_SWITCH_TO_MELEE: sRet = "***** SWTICH TO MELEE *****"; break;
	case AI_COMMAND_SWITCH_TO_RANGED: sRet = "***** SWITCH TO RANGED *****"; break;
	case AI_COMMAND_FLY: sRet = "***** FLY *****"; break;
	case AI_COMMAND_USE_HEALTH_POTION_LEAST: sRet = "**** USE LEAST POWERFUL HEALING POTION"; break;
	case AI_COMMAND_USE_HEALTH_POTION_MOST: sRet = "**** USE MOST POWERFUL HEALING POTION"; break;
	case AI_COMMAND_USE_LYRIUM_POTION_LEAST: sRet = "**** USE LEAST POWERFUL MANA POTION"; break;
	case AI_COMMAND_USE_LYRIUM_POTION_MOST: sRet = "**** USE MOST POWERFUL MANA POTION"; break;
	}
	return sRet;
}

int32 _AI_IsCommandValid(AActor* OBJECT_SELF, int32 nCommand, int32 nSubCommand, int32 nTacticTargetType)
{
	int32 nResult = TRUE_;
	//unused 	int32 nAbilityType;

	switch (nCommand)
	{
	case AI_COMMAND_ATTACK:
	{
		if (GetEffectsFlags(OBJECT_SELF) & EFFECT_FLAG_DISABLE_COMBAT)
			nResult = FALSE_;
		// Otherwise, this can not fail. The attacker can always attack with fists.
		// NOTE: this FCommand can still be valid if the attacker does not have enough ammo for a ranged weapon
		// -> the 'attack' routine will try to switch a weapon set.

		break;
	}
	case AI_COMMAND_ACTIVATE_MODE:
	{
		nResult = _AI_IsAbilityValid(OBJECT_SELF, nSubCommand);
		if (nResult == TRUE_)
		{
			if (IsModalAbilityActive(OBJECT_SELF, nSubCommand))
				nResult = FALSE_; // Ability is already active - can't activate again
		}
		if (Ability_GetAbilityType(nSubCommand) == ABILITY_TYPE_SPELL && (GetEffectsFlags(OBJECT_SELF) & EFFECT_FLAG_DISABLE_SPELLS))
			nResult = FALSE_;
		else if (Ability_GetAbilityType(nSubCommand) == ABILITY_TYPE_TALENT && (GetEffectsFlags(OBJECT_SELF) & EFFECT_FLAG_DISABLE_TALENTS))
			nResult = FALSE_;
		break;
	}
	case AI_COMMAND_DEACTIVATE_MODE:
	{
		if (!IsModalAbilityActive(OBJECT_SELF, nSubCommand))
			nResult = FALSE_; // Ability is already inactive - can't deactivate again
		break;
	}
	case AI_COMMAND_USE_ABILITY:
	{
		if (Ability_IsModalAbility(nSubCommand))
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_IsCommandValid", "ERROR: Trying to execute a modal ability as a normal ability: " + IntToString(nSubCommand), LOG_SEVERITY_WARNING);
#endif
			nResult = FALSE_;
		}

		nResult = _AI_IsAbilityValid(OBJECT_SELF, nSubCommand);

		if (Ability_GetAbilityType(nSubCommand) == ABILITY_TYPE_SPELL && (GetEffectsFlags(OBJECT_SELF) & EFFECT_FLAG_DISABLE_SPELLS))
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_IsCommandValid", "can't trigger spell - spells disabled for this creature: " + IntToString(nSubCommand), LOG_SEVERITY_WARNING);
#endif
			nResult = FALSE_;
		}
		else if (Ability_GetAbilityType(nSubCommand) == ABILITY_TYPE_TALENT && (GetEffectsFlags(OBJECT_SELF) & EFFECT_FLAG_DISABLE_TALENTS))
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_IsCommandValid", "can't trigger talent - talents disabled for this creature: " + IntToString(nSubCommand), LOG_SEVERITY_WARNING);
#endif
			nResult = FALSE_;
		}
		else if (Ability_GetAbilityType(nSubCommand) == ABILITY_TYPE_TALENT &&
			GetHasEffects(OBJECT_SELF, EFFECT_TYPE_MISDIRECTION_HEX) &&
			nTacticTargetType == AI_TARGET_TYPE_ENEMY)
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_IsCommandValid", "can't trigger talent - having misdirection hex for a hostile target talent: " + IntToString(nSubCommand), LOG_SEVERITY_WARNING);
#endif
			nResult = FALSE_;

		}


		// TEMP - staff hack
		if (nSubCommand == 11130) // STAFF
		{
			if (GetCreatureCoreClass(OBJECT_SELF) != CLASS_WIZARD)
				nResult = FALSE_;
		}
		// END TEMP
		break;
	}
	case AI_COMMAND_USE_PLACEABLE:
	{
		// can only work if the creature is in combat
		if (GetCombatState(OBJECT_SELF) == FALSE_)
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_IsCommandValid", "Can't use placeable outside of combat");
#endif
			nResult = FALSE_;
		}
		break;
	}
	case AI_COMMAND_USE_ITEM:
	{
		nResult = _AI_IsAbilityValid(OBJECT_SELF, nSubCommand);

		if (GetEffectsFlags(OBJECT_SELF) & EFFECT_FLAG_DISABLE_ITEMS)
			nResult = FALSE_;
		else if (Ability_GetAbilityType(nSubCommand) != ABILITY_TYPE_ITEM)
			nResult = FALSE_;
		break;
	}
	case AI_COMMAND_MOVE:
	{
		if (GetEffectsFlags(OBJECT_SELF) & EFFECT_FLAG_DISABLE_MOVEMENT)
			nResult = FALSE_;
		break;
	}
	case AI_COMMAND_SWITCH_TO_MELEE:
	{
		if (!_AI_HasWeaponSet(OBJECT_SELF, AI_WEAPON_SET_MELEE))
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_IsCommandValid", "No melee weapon set for switch-to-melee action - aborting");
#endif
			return FALSE_;
		}
		break;
	}
	case AI_COMMAND_SWITCH_TO_RANGED:
	{
		if (!_AI_HasWeaponSet(OBJECT_SELF, AI_WEAPON_SET_RANGED))
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_IsCommandValid", "No ranged weapon set for switch-to-melee action - aborting");
#endif
			return FALSE_;
		}
		break;
	}
	}
	return nResult;
}

int32 _AI_IsAbilityValid(AActor* OBJECT_SELF, int32 nAbilityID)
{
	int32 nAbilityType = Ability_GetAbilityType(nAbilityID);
	int32 nResult = TRUE_;
	if (!HasAbility(OBJECT_SELF, nAbilityID))
	{
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_IsAbilityValid", "ERROR: Creature does not have ability: " + IntToString(nAbilityID), LOG_SEVERITY_WARNING);
#endif
		nResult = FALSE_;
	}
	else if (Ability_CostCheck(OBJECT_SELF, nAbilityID, nAbilityType) == FALSE_)
	{
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_IsAbilityValid", "Not enough resources to trigger ability: " + IntToString(nAbilityID), LOG_SEVERITY_WARNING);
#endif
		nResult = FALSE_;
	}
	else if (GetRemainingCooldown(OBJECT_SELF, nAbilityID) > 0.0)
	{
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_IsAbilityValid", "Can't trigger ability (Cooldown running) - time left: " + FloatToString(GetRemainingCooldown(OBJECT_SELF, nAbilityID)));
#endif
		nResult = FALSE_;
	}
	else if (nAbilityID == ABILITY_TALENT_STEALTH && GetCombatState(OBJECT_SELF) == TRUE_ &&
		!HasAbility(OBJECT_SELF, ABILITY_TALENT_COMBAT_STEALTH))
	{
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_IsAbilityValid", "Can't execute stealth in combat");
#endif
		nResult = FALSE_;
	}

	// Check special ability conditions for triggering tactic
	int32 nFollower_TrigOutComb = GetM2DAInt(TABLE_AI_ABILITY_COND, "Follower_TrigOutComb", nAbilityID);;
	int32 nTrigOutCombat = GetM2DAInt(TABLE_AI_ABILITY_COND, "TrigOutComb", nAbilityID);;
	if (nResult == TRUE_)
	{
		if (IsFollower(OBJECT_SELF) && GetCombatState(OBJECT_SELF) == FALSE_ && !nFollower_TrigOutComb)
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_IsAbilityValid", "Ability not allowed to trigger outside combat for followers");
#endif
			nResult = FALSE_;
		}
		else if (!IsFollower(OBJECT_SELF) && GetCombatState(OBJECT_SELF) == FALSE_ && !nTrigOutCombat)
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_IsAbilityValid", "Ability not allowed to trigger outside combat for non-followers");
#endif
			nResult = FALSE_;
		}
	}

	// special case: stealth
	// Do not allow stealth if no ally is left alive or non stealthed
	if (nAbilityID == ABILITY_TALENT_STEALTH)
	{
		TArray<AActor*> arAllies = _AI_GetAllies(OBJECT_SELF, -1, -1);
		int32 nSize = arAllies.Num();
		if (nSize == 0)
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_IsAbilityValid", "Trying to use stealth with no allies around - aborting");
#endif
			nResult = FALSE_;
		}
		else // some allies alive
		{
			int32 i;
			AActor* oCurrent;
			for (i = 0; i < nSize; i++)
			{
				oCurrent = arAllies[i];
				if (IsStealthy(oCurrent))
				{
#ifdef DEBUG
					Log_Trace_AI(OBJECT_SELF, "_AI_IsAbilityValid", "Trying to use stealth while at least one ally is stealthy - aborting");
#endif
					nResult = FALSE_;
					break;
				}
			}
		}
	}

	// HACK START
	// Allow followers to trigger only specific abilities outside of combat
	//TODO doublecheck Allow followers to trigger only specific abilities outside of combat
	if (IsFollower(OBJECT_SELF) && GetCombatState(OBJECT_SELF) == FALSE_ && GetAbilityType(nAbilityID) != ABILITY_TYPE_ITEM)
	{
		// only heal can work
		// Joshua@23/01/08: I allowed for Modal Abilities as well.
		if (nAbilityID == ABILITY_SPELL_HEAL || Ability_IsModalAbility(nAbilityID))
			nResult = TRUE_;
		else
			nResult = FALSE_;
	}
	// HACK END

	return nResult;
}

AActor* _AI_GetPotionByFilter(AActor* OBJECT_SELF, int32 nPotionType, int32 nPotionPower)
{
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_GetPotionByFilter", "START, potion type: " + IntToString(nPotionType) + ", potion power type: " + IntToString(nPotionPower));
#endif


	TArray<AActor*> arItems;

	if (IsFollower(OBJECT_SELF))
		arItems = GetItemsInInventory(GetPartyLeader());
	else
		arItems = GetItemsInInventory(OBJECT_SELF);
	int32 nSize = arItems.Num();
	int32 i = 0;
	AActor* oCurrent = nullptr;
	AActor* oSelectedItem = nullptr;
	int32 nCurrentCost = 0.f;
	int32 nStoredCost = 0.f;
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_GetPotionByFilter", "Items #: " + IntToString(nSize));
#endif
	for (i = 0; i < nSize; i++)
	{
		// this can be made faster if we add a special item property just to flag specific types of potions
		oCurrent = arItems[i];
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_GetPotionByFilter", "GOT ITEM: " + GetTag(oCurrent));
#endif

		if (nPotionType == AI_POTION_TYPE_HEALTH)
		{
			if (GetItemPropertyPower(oCurrent, ITEM_PROPERTY_IS_HEALING_POTION, FALSE_) > 0)
			{
#ifdef DEBUG
				Log_Trace_AI(OBJECT_SELF, "_AI_GetPotionByFilter", "HEALTH POTION!");
#endif
				nCurrentCost = GetItemValue(oCurrent);
				if (nPotionPower == AI_POTION_LEVEL_MOST_POWERFUL && nCurrentCost > nStoredCost)
				{
					nStoredCost = nCurrentCost;
					oSelectedItem = oCurrent;
				}
				else if (nPotionPower == AI_POTION_LEVEL_LEAST_POWERFUL && (nCurrentCost <= nStoredCost || nStoredCost == 0))
				{
					nStoredCost = nCurrentCost;
					oSelectedItem = oCurrent;
				}
			}
		}
		else if (nPotionType == AI_POTION_TYPE_MANA)
		{
			if (GetItemPropertyPower(oCurrent, ITEM_PROPERTY_IS_MANA_POTION, FALSE_) > 0)
			{
#ifdef DEBUG
				Log_Trace_AI(OBJECT_SELF, "_AI_GetPotionByFilter", "MANA POTION!");
#endif
				nCurrentCost = GetItemValue(oCurrent);
				if (nPotionPower == AI_POTION_LEVEL_MOST_POWERFUL && nCurrentCost > nStoredCost)
				{
					nStoredCost = nCurrentCost;
					oSelectedItem = oCurrent;
				}
				else if (nPotionPower == AI_POTION_LEVEL_LEAST_POWERFUL && (nCurrentCost <= nStoredCost || nStoredCost == 0))
				{
					nStoredCost = nCurrentCost;
					oSelectedItem = oCurrent;
				}
			}
		}
	}

	return oSelectedItem;
}

FCommand _AI_GetPotionUseCommand(AActor* OBJECT_SELF, AActor* oItem)
{
	FCommand cRet;
	// WORKS ONLY IN COMBAT!!!
	if (IsObjectValid(oItem) && GetCombatState(OBJECT_SELF) == TRUE_)
	{
		int32 nAbility = GetItemAbilityId(oItem);
		if (Ability_CheckUseConditions(OBJECT_SELF, OBJECT_SELF, nAbility) == FALSE_)
			return cRet; // failed tactic

		if (_AI_CanUseAbility(OBJECT_SELF, nAbility, OBJECT_SELF) == FALSE_)
			return cRet; // can't use specific ability

						 // Command is valid to be executed on the target
		FVector vNul;
		cRet = CommandUseAbility(nAbility, OBJECT_SELF, vNul, -1.0, GetTag(oItem));

	}
	else
		return cRet;

	return cRet;
}

int32 _AI_CanUseAbility(AActor* aActor, int32 nAbility, AActor* oTarget)
{
	ADA2UE4Creature* OBJECT_SELF = Cast<ADA2UE4Creature>(aActor);

	int32 nRet = TRUE_;
	float fDistance;

	switch (nAbility)
	{
	case ABILITY_SPELL_CHAIN_LIGHTNING:
	{
		// for followers - valid only if there are at least 3+ enemies
		if (IsFollower(OBJECT_SELF))
		{
			TArray<AActor*> arEnemies = GetNearestObjectByGroup(OBJECT_SELF, GROUP_HOSTILE, OBJECT_TYPE_CREATURE, 3, TRUE_, TRUE_, FALSE_);
			int32 nSize = arEnemies.Num();
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_CanUseAbility", "number of hostiles found: " + IntToString(nSize));
#endif
			if (nSize < 3)
				nRet = FALSE_;
		}
		break;
	}
	case ABILITY_TALENT_SHIELD_BASH:
	case ABILITY_TALENT_OVERPOWER:
	case ABILITY_TALENT_OVERRUN: // pommel strike
	case ABILITY_TALENT_MONSTER_DOG_CHARGE:
	case ABILITY_TALENT_PINNING_SHOT:
	case ABILITY_TALENT_FRIGHTENING:
	case ABILITY_TALENT_DIRTY_FIGHTING:
	{
		if (_AI_HasAIStatus(oTarget, AI_STATUS_CANT_ATTACK))
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_CanUseAbility", "target already can't fight - aborting use of stun/knockdown ability");
#endif
			nRet = FALSE_;
		}
		break;
	}
	case ABILITY_TALENT_HOLY_SMITE:
	case ABILITY_SPELL_WYNNES_SEAL_PORTAL: // mana drain
	case ABILITY_SPELL_MANA_CLASH:
	{
		// only on mages and applies only for non-followers (followers have the 'nearest by class' to filter)
		if (!IsFollower(OBJECT_SELF))
		{
			int32 nClass = GetCreatureCoreClass(oTarget);
			if (nClass != CLASS_WIZARD)
			{
#ifdef DEBUG
				Log_Trace_AI(OBJECT_SELF, "_AI_CanUseAbility", "mage-only spell targeted at non mages");
#endif
				nRet = FALSE_;
			}
		}
		break;
	}
	case MONSTER_HIGH_DRAGON_FIRE_SPIT:
	case ARCHDEMON_DETONATE_DARKSPAWN:
	case ARCHDEMON_CORRUPTION_BLAST:
	{
		float fAngle = GetAngleBetweenObjects(OBJECT_SELF, oTarget);
		if (fAngle > 60.0 && fAngle < 300.0)
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_CanUseAbility", "Too large angle to trigger ability");
#endif
			nRet = FALSE_;
		}
		break;
	}
	case ARCHDEMON_VORTEX:
	case ARCHDEMON_SMITE:
	{
		fDistance = GetDistanceBetween(OBJECT_SELF, oTarget);
		if (fDistance < AI_RANGE_MEDIUM * SCALE_MULTIPLIER)
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_CanUseAbility", "Archdemon abilities can't trigger too close");
#endif
			nRet = FALSE_;

		}

		break;
	}
	case ABILITY_SPELL_WALKING_BOMB:
	{
		// can't use if virulent is on
		if (Ability_IsAbilityActive(oTarget, 12011))
			nRet = FALSE_;
		break;
	}
	case 12011: // virulent walking bomb
	{
		// can't use if virulent is on
		if (Ability_IsAbilityActive(oTarget, ABILITY_SPELL_WALKING_BOMB))
			nRet = FALSE_;
		break;
	}
	case ABILITY_TALENT_MONSTER_DOG_OVERWHELM:
	case ABILITY_TALENT_MONSTER_SHRIEK_OVERWHLEM:
	case MONSTER_BEAR_OVERWHELM:
	case MONSTER_SPIDER_OVERWHELM:
	case MONSTER_STALKER_OVERWHLEM:
	case MONSTER_DRAGON_OVERWHELM:
	{
		if (!IsHumanoid(oTarget))
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_CanUseAbility", "Overwhelm target not humanoid - can't execute ability");
#endif
			nRet = FALSE_;
		}
		else if (GetHasEffects(oTarget, EFFECT_TYPE_OVERWHELMED) || GetHasEffects(oTarget, EFFECT_TYPE_GRABBED))
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_CanUseAbility", "Target is already being overwhelmed or grabbed - can't execute another overwhelm");
#endif
			nRet = FALSE_;
		}

		break;
	}
	case ABILITY_TALENT_MONSTER_OGRE_GRAB:
	case ABILITY_TALENT_BROODMOTHER_GRAB_LEFT:
	case ABILITY_TALENT_BROODMOTHER_GRAB_RIGHT:
	case MONSTER_HIGH_DRAGON_GRAB_LEFT:
	case MONSTER_HIGH_DRAGON_GRAB_RIGHT:
	{

		if (!IsHumanoid(oTarget))
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_CanUseAbility", "Grabbed target not humanoid - can't execute ability");
#endif
			nRet = FALSE_;
		}
		else if (GetHasEffects(oTarget, EFFECT_TYPE_OVERWHELMED) || GetHasEffects(oTarget, EFFECT_TYPE_GRABBED))
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_CanUseAbility", "Target is already being overwhelmed or grabbed - can't execute another grab");
#endif
			nRet = FALSE_;
		}

		// Archdemon only: not trigger it on party members
		if (nAbility == MONSTER_HIGH_DRAGON_GRAB_LEFT || nAbility == MONSTER_HIGH_DRAGON_GRAB_RIGHT)
		{
			if (GetAppearanceType(OBJECT_SELF) == APP_TYPE_ARCHDEMON && IsFollower(oTarget))
			{
#ifdef DEBUG
				Log_Trace_AI(OBJECT_SELF, "_AI_CanUseAbility", "Archdemon can't grab party members");
#endif
				nRet = FALSE_;
			}
		}
		break;
	}
	case ABILITY_SPELL_MONSTER_OGRE_HURL:
	{
		// Ogre Hurl ability can be used only with a minimum distance
		fDistance = GetDistanceBetween(OBJECT_SELF, oTarget);
		if (fDistance < (AI_RANGE_SHORT * SCALE_MULTIPLIER * 2))
			nRet = FALSE_;
		break;
	}
	case ABILITY_TALENT_SHIELD_DEFENSE:
	{
		// Do not run it if I have Shield Wall
		if (HasAbility(OBJECT_SELF, ABILITY_TALENT_SHIELD_WALL))
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_CanUseAbility", "I have a better ability (Shield Wall) - not trying to run this one");
#endif
			nRet = FALSE_;
		}
		break;
	}
	case 200010: // healing potions
	case 200011:
	case 200012:
	case 200013:
	{
		float fCurrentStat = GetCurrentHealth(OBJECT_SELF);
		float fMaxStat = GetMaxHealth(OBJECT_SELF);
		if (fCurrentStat == fMaxStat)
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_CanUseAbility", "full health - not using health potion");
#endif
			nRet = FALSE_;
		}
		break;
	}
	case 200030: // mana potions
	case 200031:
	case 200032:
	case 200033:
	{
		float fCurrentStat = GetCurrentManaStamina(OBJECT_SELF);
		float fMaxStat = IntToFloat(GetMaxManaStamina(OBJECT_SELF));
		if (fCurrentStat == fMaxStat)
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_CanUseAbility", "full mana - not using mana potion");
#endif
			nRet = FALSE_;
		}
		break;
	}

	}

	// Stationary non-follower check
	if (!IsFollower(OBJECT_SELF) && OBJECT_SELF->AI_FLAG_STATIONARY > 0 && oTarget != OBJECT_SELF)
	{
		fDistance = GetDistanceBetween(OBJECT_SELF, oTarget);
		int32 nAbilityRangeID = GetM2DAInt(TABLE_ABILITIES_TALENTS, "range", nAbility);
		float fAbilityRange = GetM2DAFloat(TABLE_RANGES, "PrimaryRange", nAbilityRangeID) * SCALE_MULTIPLIER;

#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_CanUseAbility", "Stationary creature ability range: " + FloatToString(fAbilityRange) + ", distance to target: " +
			FloatToString(fDistance));
#endif

		if (fDistance > fAbilityRange)
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_CanUseAbility", "Stationary creature: target too far for ability range");
#endif
			nRet = FALSE_;
		}
		else
		{
			if (CheckLineOfSightObject(OBJECT_SELF, oTarget) == FALSE_)
			{
#ifdef DEBUG
				Log_Trace_AI(OBJECT_SELF, "_AI_CanUseAbility", "Stationary creature: no line of sight for target");
#endif
				nRet = FALSE_;
			}
			else
			{
#ifdef DEBUG
				Log_Trace_AI(OBJECT_SELF, "_AI_CanUseAbility", "Stationary creature: Clear to execute ability without moving");
#endif
			}
		}
	}

	return nRet;
}

FCommand _AI_GetFlyCommand(AActor* aActor, AActor* oTurnTo, int32 bMoveTo)
{
	ADA2UE4Creature* OBJECT_SELF = Cast<ADA2UE4Creature>(aActor);

	float fAngle = GetAngleBetweenObjects(OBJECT_SELF, oTurnTo);
	float fMyFacing = GetFacing(OBJECT_SELF);
	float fEnemyFacing = GetFacing(oTurnTo);
	float fTurnAngle = 360.0 - fAngle + fMyFacing;
	if (fTurnAngle >= 180.0)
		fTurnAngle = fMyFacing - fAngle;
	float fDif;
	if (fTurnAngle > fMyFacing) fDif = fTurnAngle - fMyFacing;
	else fDif = fMyFacing - fTurnAngle;
	float fDistance = GetDistanceBetween(OBJECT_SELF, oTurnTo);
	FCommand cFly;
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_GetFlyCommand", "Object: " + GetTag(oTurnTo) + ", turn angle: " + FloatToString(fTurnAngle) + ", distance: " + FloatToString(fDistance)
		+ ", angle dif: " + FloatToString(fDif));
#endif


	if (fDif < AI_TURN_MIN_ANGLE && bMoveTo == FALSE_)
	{
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_GetFlyCommand", "Too small angle to turn - avoiding turn and returning invalid command");
#endif
		return cFly;
	}
	else if (fDistance < AI_FLY_MIN_DISTANCE && bMoveTo == TRUE_)
	{
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_GetFlyCommand", "Too small distance to fly - avoiding fly and returning invalid command. Distance: " + FloatToString(fDistance));
#endif
		return cFly;
	}


	FVector lLoc;
	if (bMoveTo)
	{
		if (fDistance > AI_FLY_MAX_DISTANCE &&
			OBJECT_SELF->CREATURE_COUNTER_3 == 0) // CREATURE_COUNTER_3 used to enable/disable stomp. 1 is for disabled
		{
			WR_ClearAllCommands(OBJECT_SELF, TRUE_);
			WR_SetObjectActive(OBJECT_SELF, FALSE_);
			FGameEvent eFlyDown = Event(EVENT_TYPE_SET_OBJECT_ACTIVE);
			float fFacing = GetFacing(oTurnTo);
			eFlyDown = SetEventFloat(eFlyDown, fFacing);
			eFlyDown = SetEventVector(eFlyDown, GetLocation(oTurnTo));
			eFlyDown = SetEventInteger(eFlyDown, TRUE_); // tells it to call an AI function
			DelayEvent(2.5f, OBJECT_SELF, eFlyDown);

			// putting wait command as a flag to abort AI
			cFly = CommandWait(2.5);

		}
		else if (fDistance < AI_FLY_MAX_DISTANCE)
		{
			//lLoc = Location(GetArea(OBJECT_SELF), GetPosition(oTurnTo), fTurnAngle);
			cFly = CommandFly(oTurnTo->GetActorLocation());
		}
	}
	else if (fDistance > AI_TURN_MIN_DISTANCE)
	{
		//lLoc = Location(GetArea(OBJECT_SELF), GetPosition(OBJECT_SELF), fTurnAngle);
		cFly = CommandFly(OBJECT_SELF->GetActorLocation());
	}



	return cFly;
}

FCommand _AI_ExecuteAttack(AActor* aActor, AActor* oTarget, int32 nLastCommandStatus)
{
	ADA2UE4Creature* OBJECT_SELF = Cast<ADA2UE4Creature>(aActor);
	// This can include a weapon switch condition as well:
	// If current creature equips a ranged weapon and the target is within melee range -> switch to melee
	// If current creature equips a melee weapon and the target is not within melee range AND
	// the creature prefers ranged weapons (flag) AND the ranged weapon set has enough ammo -> switch to ranged weapon
	// All of the conditions above assume the creature has the appropriate weapon sets
	// If the creature decides to switch weapons then we will NOT add another melee command this round

	FCommand cTacticCommand;
	int32 nTacticID = 0; // used to store the tactic that was executed, if it there is no tactic ID from a table
	int32 nLastTacticID = OBJECT_SELF->AI_LAST_TACTIC; // the last tactic being used
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteAttack", "*** START ***, Target: " + GetTag(oTarget) + ", Last Command Status: "
		+ IntToString(nLastCommandStatus) + ", last tactic id: " + IntToString(nLastTacticID));
#endif

	float fDistance = 0.f;
	//unused 	int32 nRand;
	//unused 	float fAvoidDistance;
	//unused 	AActor* oCurrentWP;
	TArray<AActor*> arWPs;
	//unused 	float fWPDistance;
	//unused 	int32 nSize;
	//unused 	int32 i;
	int32 bWPFound = FALSE_;
	TArray<AActor*> oCreaturesNearWP;
	//unused 	float fNearestCreatureToWPDistance;
	TArray<AActor*> arPerceivedCreatures;
	//unused 	AActor* oCreatureNearWP;
	//unused 	AActor* oOldWP;
	int32 nFailMessage = 0;
	switch (nLastCommandStatus)
	{
	case COMMAND_FAILED_COMMAND_CLEARED: nFailMessage = UI_DEBUG_COMMAND_FAILED; break;
	case COMMAND_FAILED_INVALID_DATA: nFailMessage = UI_DEBUG_INVALID_DATA; break;
	case COMMAND_FAILED_INVALID_PATH: nFailMessage = UI_DEBUG_INVALID_PATH; break;
	case COMMAND_FAILED_NO_LINE_OF_SIGHT: nFailMessage = UI_DEBUG_NO_LOS; break;
		//case COMMAND_FAILED_NO_SPACE_IN_MELEE_RING: nFailMessage = UI_DEBUG_NO_SPACE_IN_MELEE_RING; break;
	case COMMAND_FAILED_TARGET_DESTROYED: nFailMessage = UI_DEBUG_TARGET_DESTROYED; break;
	case COMMAND_FAILED_DISABLED: nFailMessage = UI_DEBUG_MOVEMENT_DISABLED; break;
	case COMMAND_FAILED_TIMEOUT: nFailMessage = UI_DEBUG_COMMAND_TIMED_OUT; break;
	}
#ifdef DEBUG
	if (nFailMessage > 0)
		UI_DisplayMessage(OBJECT_SELF, nFailMessage);
#endif


	if (GetEffectsFlags(OBJECT_SELF) & EFFECT_FLAG_DISABLE_COMBAT)
	{
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteAttack", "Can't add any combat commands: combat is disabled by effect - waiting instead");
#endif
		cTacticCommand = _AI_DoNothing(OBJECT_SELF, nLastTacticID, nLastCommandStatus, TRUE_);
		nTacticID = AI_TACTIC_ID_WAIT;
	}

	// Special handling:
	// If the last command failed AND the last command was a part of this generic 'attack' sequence
	// dec 17, 2008 -- yaron: adding a condition that allows this part to run if the command before was not a normal attack and the failure is 'movement disable'
	// feb 24, 2009 -- yaron: same as above but also for timeout failures
	else if (nLastCommandStatus < 0 &&
		(nLastTacticID < 0 || (nLastTacticID >= 0 && (nLastCommandStatus == COMMAND_FAILED_DISABLED || nLastCommandStatus == COMMAND_FAILED_TIMEOUT))) &&
		nLastCommandStatus != COMMAND_FAILED_TARGET_DESTROYED)
	{
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteAttack", "Last tactic failed AND was an Attack tactic - trying something else, error: " + IntToString(nLastCommandStatus));
#endif
		if (IsFollower(OBJECT_SELF))
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteAttack", "Command failed for follower - trying only to WAIT");
#endif
			// Nothing much we can do in these cases except wait
			cTacticCommand = _AI_DoNothing(OBJECT_SELF, nLastTacticID, nLastCommandStatus, TRUE_);
			nTacticID = AI_TACTIC_ID_WAIT;
		}
		else
		{
			fDistance = GetDistanceBetween(OBJECT_SELF, oTarget);

			// timeout failure - special case
			// (regardless of what the last action was)
			if (nLastCommandStatus == COMMAND_FAILED_TIMEOUT)
			{
#ifdef DEBUG
				Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteAttack", "Handling attack timeout");
#endif
				if (!IsFollower(OBJECT_SELF) &&
					OBJECT_SELF->CREATURE_HAS_TIMER_ATTACK == 1) // first failure -> try to attack again
				{
#ifdef DEBUG
					Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteAttack", "First timer failure - try to attack again");
#endif
					OBJECT_SELF->CREATURE_HAS_TIMER_ATTACK = 2;
					cTacticCommand = CommandAttack(oTarget);
					nTacticID = AI_TACTIC_ID_ATTACK;

				}
				else // not first failure -> switch weapon or try to attack again
				{
#ifdef DEBUG
					Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteAttack", "Not first timer failure - trying to switch to ranged");
#endif
					if (_AI_GetWeaponSetEquipped(OBJECT_SELF) == AI_WEAPON_SET_MELEE &&
						_AI_HasWeaponSet(OBJECT_SELF, AI_WEAPON_SET_RANGED))
					{
#ifdef DEBUG
						Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteAttack", "switching to ranged because of timer failure");
#endif
						cTacticCommand = _AI_SwitchWeaponSet(OBJECT_SELF, AI_WEAPON_SET_RANGED);
						nTacticID = AI_TACTIC_ID_SWITCH_MELEE_TO_RANGED;
					}
					else // can't switch -> try to attack again (no need for wait as this is a timeout failure
					{
#ifdef DEBUG
						Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteAttack", "timer failure - can't switch - trying to attack again");
#endif
						_AI_ApplyTimerDifficultyEffects(OBJECT_SELF, oTarget);
						// adding speed boost for normal/hard difficulty
						cTacticCommand = CommandAttack(oTarget);
						nTacticID = AI_TACTIC_ID_ATTACK;
					}

				}
			}
			else // not a timer failure
			{

				switch (nLastTacticID)
				{
				case AI_TACTIC_ID_ATTACK:
				{
					// if a follower has the movement disabled GUI activated and he fails the last action
					if (nLastCommandStatus == COMMAND_FAILED_DISABLED &&
						IsFollower(OBJECT_SELF) &&
						_AI_GetWeaponSetEquipped(OBJECT_SELF) == AI_WEAPON_SET_MELEE &&
						_AI_HasWeaponSet(OBJECT_SELF, AI_WEAPON_SET_RANGED) &&
						!_AI_IsTargetInMeleeRange(OBJECT_SELF, oTarget))
					{
						cTacticCommand = _AI_SwitchWeaponSet(OBJECT_SELF, AI_WEAPON_SET_RANGED);
						nTacticID = AI_TACTIC_ID_SWITCH_MELEE_TO_RANGED;
					}
					else if (!IsFollower(OBJECT_SELF) &&
						_AI_GetWeaponSetEquipped(OBJECT_SELF) == AI_WEAPON_SET_MELEE &&
						_AI_HasWeaponSet(OBJECT_SELF, AI_WEAPON_SET_RANGED) && !_AI_IsTargetInMeleeRange(OBJECT_SELF, oTarget))
					{
						cTacticCommand = _AI_SwitchWeaponSet(OBJECT_SELF, AI_WEAPON_SET_RANGED);
						nTacticID = AI_TACTIC_ID_SWITCH_MELEE_TO_RANGED;
					}
					// if has melee but no ranged: move closer
					else if (_AI_GetWeaponSetEquipped(OBJECT_SELF) == AI_WEAPON_SET_MELEE &&
						!_AI_HasWeaponSet(OBJECT_SELF, AI_WEAPON_SET_RANGED) &&
						fDistance > (AI_MINIMAL_MELEE_DISTANCE * SCALE_MULTIPLIER + 1.0 * SCALE_MULTIPLIER) &&
						nLastCommandStatus != COMMAND_FAILED_DISABLED &&
						nLastCommandStatus != COMMAND_FAILED_PATH_ACTION_REQUIRED &&
						nLastCommandStatus != COMMAND_FAILED_INVALID_PATH)
					{
#ifdef DEBUG
						Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteAttack", "Trying to move closer to target");
#endif
						cTacticCommand = CommandMoveToActor(oTarget, TRUE_, AI_MINIMAL_MELEE_DISTANCE * SCALE_MULTIPLIER);
						nTacticID = AI_TACTIC_ID_MOVE;
					}
					// if has ranged: change to melee
					else if (_AI_GetWeaponSetEquipped(OBJECT_SELF) == AI_WEAPON_SET_RANGED &&
						_AI_HasWeaponSet(OBJECT_SELF, AI_WEAPON_SET_MELEE) && _AI_IsTargetInMeleeRange(OBJECT_SELF, oTarget))
					{
						cTacticCommand = _AI_SwitchWeaponSet(OBJECT_SELF, AI_WEAPON_SET_MELEE);
						nTacticID = AI_TACTIC_ID_SWITCH_RANGED_TO_MELEE;
					}
					// if too close and not on ring, move a little ways away from target
					else if (nLastCommandStatus == COMMAND_FAILED_NO_SPACE_IN_MELEE_RING &&
						fDistance < AI_MELEE_RANGE * SCALE_MULTIPLIER)
					{
#ifdef DEBUG
						Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteAttack", "Trying to move away from target");
#endif
						cTacticCommand = CommandMoveAwayFromObject(oTarget, 2.0, FALSE_);
						nTacticID = AI_TACTIC_ID_MOVE;
					}
					else
					{
						cTacticCommand = _AI_DoNothing(OBJECT_SELF, nLastTacticID, nLastCommandStatus, TRUE_, FALSE_, oTarget);
						nTacticID = AI_TACTIC_ID_WAIT;
					}
					break;
				}
				case AI_TACTIC_ID_SWITCH_MELEE_TO_RANGED:
				case AI_TACTIC_ID_SWITCH_RANGED_TO_MELEE:
				case AI_TACTIC_ID_WAIT:
				case AI_TACTIC_ID_MOVE:
				default:
				{
					// Nothing much we can do in these cases except wait
					cTacticCommand = _AI_DoNothing(OBJECT_SELF, nLastTacticID, nLastCommandStatus, TRUE_, FALSE_, oTarget);
					nTacticID = AI_TACTIC_ID_WAIT;
					break;
				}
				} // end of tactic ID failure switch
			} // end of else (not a timer failure)
		} // end of 'not follower' if-else
	}
	else if (_AI_GetWeaponSetEquipped(OBJECT_SELF) == AI_WEAPON_SET_RANGED &&
		(_AI_IsTargetInMeleeRange(OBJECT_SELF, oTarget) || !_AI_GetFlag(OBJECT_SELF, AI_FLAG_PREFERS_RANGED)) &&
		_AI_HasWeaponSet(OBJECT_SELF, AI_WEAPON_SET_MELEE) &&
		nLastTacticID != AI_TACTIC_ID_SWITCH_MELEE_TO_RANGED) // so it won't try to switch to melee right after changing to ranged
	{
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteAttack", "Trying to switch into a melee weapon set");
#endif

		if (IsFollower(OBJECT_SELF) && AI_BehaviorCheck_PreferRange(OBJECT_SELF))
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteAttack", "Follower prefers range - switch to melee aborted");
#endif
		}
		else
		{
			int32 nRandBackToMelee = Random(100) + 1;
			if (!IsFollower(OBJECT_SELF) && !_AI_GetFlag(OBJECT_SELF, AI_FLAG_PREFERS_RANGED) && nRandBackToMelee > 33)
			{
#ifdef DEBUG
				Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteAttack", "Not preferring range, but random chance failed to allow changing back to melee");
#endif
			}
			else if (_AI_IsTargetInMeleeRange(OBJECT_SELF, oTarget))
			{
				cTacticCommand = _AI_SwitchWeaponSet(OBJECT_SELF, AI_WEAPON_SET_MELEE);
				nTacticID = AI_TACTIC_ID_SWITCH_RANGED_TO_MELEE;

				if (GetCommandType(cTacticCommand) == COMMAND_TYPE_INVALID) // failed to switch
				{
					cTacticCommand = CommandAttack(oTarget); // Continue attacking with ranged weapon
					nTacticID = AI_TACTIC_ID_ATTACK;
				}
			}
		}
	}
	else if (_AI_GetWeaponSetEquipped(OBJECT_SELF) == AI_WEAPON_SET_MELEE &&
		!_AI_IsTargetInMeleeRange(OBJECT_SELF, oTarget) &&
		(IsFollower(OBJECT_SELF) || _AI_GetFlag(OBJECT_SELF, AI_FLAG_PREFERS_RANGED)) &&
		_AI_HasWeaponSet(OBJECT_SELF, AI_WEAPON_SET_RANGED) &&     // Ranged weapon set check includes ammo check
		nLastTacticID != AI_TACTIC_ID_SWITCH_RANGED_TO_MELEE) // Did not try
	{
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteAttack", "Trying to switch into a ranged weapon set");
#endif
		if (IsFollower(OBJECT_SELF) && AI_BehaviorCheck_PreferMelee(OBJECT_SELF))
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteAttack", "Follower prefers range - switch to range aborted");
#endif
		}
		else if (IsFollower(OBJECT_SELF) && !AI_BehaviorCheck_PreferRange(OBJECT_SELF))
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteAttack", "Follower does NOT prefer range - switch to range aborted");
#endif
		}
		else
		{
			cTacticCommand = _AI_SwitchWeaponSet(OBJECT_SELF, AI_WEAPON_SET_RANGED);
			//cTacticCommand = _AI_SwitchWeaponSet(0);
			nTacticID = AI_TACTIC_ID_SWITCH_MELEE_TO_RANGED;

			if (GetCommandType(cTacticCommand) == COMMAND_TYPE_INVALID) // failed to switch
			{
				cTacticCommand = CommandAttack(oTarget); // Continue attacking with melee weapon
				nTacticID = AI_TACTIC_ID_ATTACK;
			}
		}

	}
	else// Did not switch any weapon set -> continue attacking with current.
	{
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteAttack", "NORMAL ATTACK");
#endif

		cTacticCommand = CommandAttack(oTarget);
		nTacticID = AI_TACTIC_ID_ATTACK;
	}
	// ...one last ammo check!


	// One last check - in case we failed to switch a weapon:
	if (GetCommandType(cTacticCommand) == COMMAND_TYPE_INVALID)
	{
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteAttack", "FAILED TO FIND A VALID COMMAND - TRYING COMMAND ATTACK", LOG_SEVERITY_CRITICAL);
#endif
		cTacticCommand = CommandAttack(oTarget);
		nTacticID = AI_TACTIC_ID_ATTACK;
	}

	// Last check - making sure selected attack matches stationary flag
	if (!IsFollower(OBJECT_SELF) && OBJECT_SELF->AI_FLAG_STATIONARY > 0 && GetCommandType(cTacticCommand) == COMMAND_TYPE_ATTACK)
	{
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteAttack", "Creature stationary - checking if he can execute selected attack");
#endif
		if (_AI_GetWeaponSetEquipped(OBJECT_SELF) == AI_WEAPON_SET_MELEE)
		{
			// if melee weapon -> attack only if target is in stationaty range
			fDistance = GetDistanceBetween(OBJECT_SELF, oTarget);
			if (fDistance > AI_STATIONARY_RANGE * SCALE_MULTIPLIER)
			{
#ifdef DEBUG
				Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteAttack", "Creature stationary - too far from melee target to execute attack - WAITING");
#endif
				cTacticCommand = _AI_DoNothing(OBJECT_SELF, nLastTacticID, nLastCommandStatus, TRUE_, TRUE_);
				nTacticID = AI_TACTIC_ID_WAIT;
			}
		}
		else if (_AI_GetWeaponSetEquipped(OBJECT_SELF) == AI_WEAPON_SET_RANGED)
		{
			// if ranged weapon -> attack only if target is in weapon range
			AActor* oWeapon = GetItemInEquipSlot(INVENTORY_SLOT_MAIN, OBJECT_SELF);
			fDistance = GetDistanceBetween(OBJECT_SELF, oTarget);
			float fWeaponRange = GetItemRange(oWeapon);

#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteAttack", "Weapon range check for stationary creature - weapon range: " + FloatToString(fWeaponRange));
#endif
			if (fDistance > fWeaponRange)
			{
#ifdef DEBUG
				Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteAttack", "Creature stationary - too far from ranged target to execute attack - WAITING");
#endif
				cTacticCommand = _AI_DoNothing(OBJECT_SELF, nLastTacticID, nLastCommandStatus, TRUE_, TRUE_);
				nTacticID = AI_TACTIC_ID_WAIT;
			}
			else // creature within range - check line of sight
			{
				if (CheckLineOfSightObject(OBJECT_SELF, oTarget) == FALSE_)
				{
#ifdef DEBUG
					Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteAttack", "Creature stationary - no line of sight to target - WAITING");
#endif
					cTacticCommand = _AI_DoNothing(OBJECT_SELF, nLastTacticID, nLastCommandStatus, TRUE_, TRUE_);
					nTacticID = AI_TACTIC_ID_WAIT;
				}
			}

		}
	}

#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteAttack", "Setting last tactic ID to: " + IntToString(nTacticID));
#endif
	OBJECT_SELF->AI_LAST_TACTIC = nTacticID;
	return cTacticCommand;
}

void _AI_ApplyTimerDifficultyEffects(AActor* OBJECT_SELF, AActor* oTarget)
{
	if (GetGameDifficulty() > GAME_DIFFICULTY_NORMAL)
	{
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "_AI_ExecuteAttack", "timer failure - increasing movement speed");
#endif
		FEffect eSpeed = EffectModifyMovementSpeed(1.5);
		eSpeed.oCreator = OBJECT_SELF;
		ApplyEffectOnObject(EFFECT_DURATION_TYPE_TEMPORARY, eSpeed, OBJECT_SELF, AI_COMMAND_TIMER / 2);
	}
}

void _AI_SetFlag(AActor* OBJECT_SELF, int32 nFlag, int32 nValue)
{
	ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(OBJECT_SELF);

#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_SetFlag", "flag: " + IntToString(nFlag) + ", value= " + IntToString(nValue));
#endif
	switch (nFlag)
	{
	case AI_FLAG_PREFERS_RANGED:
		oCreature->AI_FLAG_PREFERS_RANGED = nValue;
		break;
	case AI_WAIT_TIMER:
		oCreature->AI_WAIT_TIMER = nValue;
		break;
	default:
		LogError("_AI_SetFlag: unknown flag!");
		break;
	}
}

int32 _AI_GetFlag(AActor* OBJECT_SELF, int32 nFlag)
{
	ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(OBJECT_SELF);

	int32 nValue = 0;// = GetLocalInt(OBJECT_SELF, sFlag);

	switch (nFlag)
	{
	case AI_FLAG_PREFERS_RANGED:
		nValue = oCreature->AI_FLAG_PREFERS_RANGED;
		break;
	case AI_WAIT_TIMER:
		nValue = oCreature->AI_WAIT_TIMER;
		break;
	default:
		LogError("_AI_GetFlag: unknown flag!");
		break;
	}

#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "_AI_GetFlag", "flag: " + IntToString(nFlag) + ", value= " + IntToString(nValue));
#endif // DEBUG
	return nValue;
}

FCommand _AI_MoveToControlled(AActor* OBJECT_SELF, int32 nLastCommandStatus)
{
	FCommand cTacticCommand;
	AActor* oMainControlled = GetMainControlled();
	FVector lLoc = oMainControlled->GetActorLocation();//GetFollowerWouldBeLocation(OBJECT_SELF);
	float fDistance = GetDistanceBetween(OBJECT_SELF, oMainControlled);
	// NOTE: there used to be a distance check but I removed it since it cause followers to linger behind
	// when combat starts
	// yaron nov 28, 2008
	//-----
	// NOTE II: putting the distance check back, now with a very short distance
	// Without a distance check the move commands are evaluated constantly, flooding the AI.
	// This is just to slow it down a bit
	// yaron dec 8, 2008
	if (!IsStealthy(oMainControlled) && nLastCommandStatus == COMMAND_SUCCESSFUL && fDistance > AI_RANGE_SHORT * SCALE_MULTIPLIER)
	{
		if (AI_BehaviorCheck_AvoidNearbyEnemies(OBJECT_SELF))
		{
			// move to player only if no enemies nearby
			TArray<AActor*>arEnemies = GetNearestObjectByHostility(oMainControlled, TRUE_, OBJECT_TYPE_CREATURE, 1);
			fDistance = GetDistanceBetween(oMainControlled, arEnemies[0]);
			if (fDistance <= AI_RANGE_SHORT * SCALE_MULTIPLIER)
				cTacticCommand = _AI_DoNothing(OBJECT_SELF, -1, nLastCommandStatus, FALSE_, TRUE_);
			else
				cTacticCommand = CommandMoveToLocation(lLoc, TRUE_);
		}
		else
			cTacticCommand = CommandMoveToLocation(lLoc, TRUE_);
	}
	else
		cTacticCommand = _AI_DoNothing(OBJECT_SELF, -1, nLastCommandStatus, FALSE_, TRUE_);

	return cTacticCommand;

}

int32 AI_GetPartyAllowedToAttack(AActor* OBJECT_SELF)
{
	int32 nAllowed = GetModule()->AI_PARTY_CLEAR_TO_ATTACK;
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "AI_GetPartyAllowedToAttack", "Party allowed-to-attack status: " + IntToString(nAllowed));
#endif
	return nAllowed;
}

void AI_ExecuteDefaultAction(AActor* OBJECT_SELF, AActor* oLastTarget, int32 nLastCommand, int32 nLastCommandStatus, int32 nLastSubCommand)
{
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "AI_ExecuteDefaultAction", "START");
#endif
	AActor* oNewTarget = nullptr;

	// Finished checking all rules and nothing got executed
	// -> Execute default attack
	AActor* oSelectedTarget = GetAttackTarget(OBJECT_SELF);

#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "AI_ExecuteDefaultAction", "Could not assign any AI tactic - creature will try to attack normally");
#endif

	if (IsFollower(OBJECT_SELF))
	{
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "AI_ExecuteDefaultAction", "Follower selected target: " + GetTag(oSelectedTarget));
#endif
		float fDistanceToPlayer = GetDistanceBetween(OBJECT_SELF, GetMainControlled());

		if (nLastCommand == COMMAND_TYPE_ATTACK)
		{
			AActor* oTargetOverride = _AI_GetTargetOverride(OBJECT_SELF);
			if (_AI_IsHostileTargetValid(OBJECT_SELF, oTargetOverride))
			{
#ifdef DEBUG
				Log_Trace_AI(OBJECT_SELF, "AI_ExecuteDefaultAction", "Follower picking override target (probably a summon): " + GetTag(oTargetOverride));
#endif
				oNewTarget = oTargetOverride;
			}
			// A follower can NOT switch target by himself
			else if (_AI_IsHostileTargetValid(OBJECT_SELF, oSelectedTarget))
				oNewTarget = oSelectedTarget;
			else // no valid target -> try to find a new target
			{
				oNewTarget = _AI_Condition_GetNearestVisibleCreature(OBJECT_SELF, AI_TARGET_TYPE_ENEMY, 1, -1, -1, -1);
				if (!_AI_IsHostileTargetValid(OBJECT_SELF, oNewTarget))
				{
#ifdef DEBUG
					Log_Trace_AI(OBJECT_SELF, "AI_ExecuteDefaultAction", "Follower: could not find a new target - moving to player or waiting");
#endif
					if (fDistanceToPlayer > AI_FOLLOWER_PLAYER_DISTANCE * SCALE_MULTIPLIER && nLastCommandStatus == COMMAND_SUCCESSFUL)
					{
						//TODO location lLoc = GetFollowerWouldBeLocation(OBJECT_SELF);
						FVector lLoc = GetMainControlled()->GetActorLocation();
						FCommand cMove = CommandMoveToLocation(lLoc, TRUE_);
						WR_AddCommand(OBJECT_SELF, cMove, FALSE_, FALSE_, -1, AI_COMMAND_TIMER);
						return;
					}
					else
					{
						FCommand cWait = _AI_DoNothing(OBJECT_SELF, -1, nLastCommandStatus, FALSE_, TRUE_);
						WR_AddCommand(OBJECT_SELF, cWait);
						return;
					}
				}
#ifdef DEBUG
				Log_Trace_AI(OBJECT_SELF, "AI_ExecuteDefaultAction", "Follower: acquired new target: " + GetTag(oNewTarget));
#endif
			}
			// Follower allowed to continue attacking
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "AI_ExecuteDefaultAction", "Follower: continue attacking current target");
#endif
			FCommand cFollowerAttack = _AI_ExecuteAttack(OBJECT_SELF, oNewTarget, nLastCommandStatus);
			WR_AddCommand(OBJECT_SELF, cFollowerAttack);
			return;
		}
		else if (AI_BehaviorCheck_DefaultAttack(OBJECT_SELF) == FALSE_) // last command not attack
		{
			if (_AI_IsHostileTargetValid(OBJECT_SELF, oSelectedTarget))
			{
				// continue attacking if already has a target
				FCommand cFollowerAttack = _AI_ExecuteAttack(OBJECT_SELF, oNewTarget, nLastCommandStatus);
				WR_AddCommand(OBJECT_SELF, cFollowerAttack);
				return;
			}
			else
			{
#ifdef DEBUG
				Log_Trace_AI(OBJECT_SELF, "AI_ExecuteDefaultAction", "Follower: last command wasn't attack - moving or doing nothing (follower not allowed to pick a target)");
#endif
				FCommand cMoveOrWait = _AI_MoveToControlled(OBJECT_SELF, nLastCommandStatus);
				WR_AddCommand(OBJECT_SELF, cMoveOrWait, FALSE_, FALSE_, -1, AI_COMMAND_TIMER);
				return;
			}
		}
		else if (!AI_GetPartyAllowedToAttack(OBJECT_SELF) && AI_BehaviorCheck_AttackOnCombatStart(OBJECT_SELF) == FALSE_)
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "AI_ExecuteDefaultAction", "Follower: not allowed to attack - moving or doing nothing (follower not allowed to pick a target)");
#endif
			FCommand cMoveOrWait = _AI_MoveToControlled(OBJECT_SELF, nLastCommandStatus);
			WR_AddCommand(OBJECT_SELF, cMoveOrWait, FALSE_, FALSE_, -1, AI_COMMAND_TIMER);
			return;
		}

	}
	else // not a follower, can switch a target
	{
		oNewTarget = _AI_Condition_GetMostHatedEnemy(OBJECT_SELF, 1, COMMAND_TYPE_ATTACK, -1, -1);
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "AI_ExecuteDefaultAction", "Not a follower picking new target (most hated): " + GetTag(oNewTarget));
#endif
	}

	if (IsFollower(OBJECT_SELF))
	{
		// A follower that is allowed to pick a target by himself
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "AI_ExecuteDefaultAction", "Follower: behavior allows picking new target - attacking nearest visible");
#endif
		oNewTarget = _AI_Condition_GetNearestVisibleCreature(OBJECT_SELF, AI_TARGET_TYPE_ENEMY, 1, -1, -1, -1);
		if (!IsObjectValid(oNewTarget) && GetGameMode() == EGameMode::GM_COMBAT)
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "AI_ExecuteDefaultAction", "Follower: could not find valid target (room connection error OR enemy too far away) - trying to move to leader");
#endif
			FCommand cMoveOrWait = _AI_MoveToControlled(OBJECT_SELF, nLastCommandStatus);
			WR_AddCommand(OBJECT_SELF, cMoveOrWait, FALSE_, FALSE_, -1, AI_COMMAND_TIMER);
			return;
		}
	}

	FCommand cAttack;

	if (_AI_IsHostileTargetValid(OBJECT_SELF, oNewTarget))
	{
		cAttack = _AI_ExecuteAttack(OBJECT_SELF, oNewTarget, nLastCommandStatus);
		if (GetCommandType(cAttack) == COMMAND_TYPE_INVALID)
		{
#ifdef DEBUG
			Log_Trace_AI(OBJECT_SELF, "AI_ExecuteDefaultAction", "INVALID TACTIC COMMAND - WAITING", LOG_SEVERITY_CRITICAL);
#endif
			cAttack = _AI_DoNothing(OBJECT_SELF, -1, -1, TRUE_);
		}
	}
	else // new target
	{
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "AI_ExecuteDefaultAction", "COULD NOT FIND A NEW TARGET - WAITING", LOG_SEVERITY_CRITICAL);
#endif
		cAttack = _AI_DoNothing(OBJECT_SELF, -1, -1, TRUE_);
	}

	WR_AddCommand(OBJECT_SELF, cAttack, FALSE_, FALSE_, -1, AI_COMMAND_TIMER);

}

void AI_HandleCowardFollower(AActor* OBJECT_SELF, AActor* oAppear)
{
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "AI_HandleCowardFollower", "START, appear: " + GetTag(oAppear));
#endif

	if (GetGameMode() != EGameMode::GM_COMBAT)
		return;

	if (IsObjectValid(oAppear) && GetGroupHostility(GROUP_PC, GetGroupId(oAppear) == TRUE_)) // perceived a creature that hates the player
	{
		// run away from this creature
		FCommand cWalk = CommandMoveAwayFromObject(oAppear, AI_RANGE_MEDIUM * SCALE_MULTIPLIER, TRUE_);
		PlaySoundSet(OBJECT_SELF, SS_EXPLORE_ENEMIES_SIGHTED_OTHER);
		WR_ClearAllCommands(OBJECT_SELF);
		WR_AddCommand(OBJECT_SELF, cWalk);
	}
	else // FCommand complete. look for someone to run away from
	{
		AActor* oNearestHostile = UT_GetNearestCreatureByGroup(OBJECT_SELF, GROUP_HOSTILE);
		float fDistance = GetDistanceBetween(OBJECT_SELF, oNearestHostile);
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "AI_HandleCowardFollower", "nearest visible hostile: " + GetTag(oNearestHostile) + ", distance: " + FloatToString(fDistance));
#endif

		if (fDistance < (AI_RANGE_MEDIUM * SCALE_MULTIPLIER / 2))
		{
			FCommand cWalk = CommandMoveAwayFromObject(oNearestHostile, AI_RANGE_MEDIUM * SCALE_MULTIPLIER, TRUE_);
			WR_AddCommand(OBJECT_SELF, cWalk);
		}
		else if (IsObjectValid(oNearestHostile) && fDistance <= AI_RANGE_MEDIUM * SCALE_MULTIPLIER)// cower
		{
			FCommand cPlayAnimation = CommandPlayAnimation(3009, 1, TRUE_, TRUE_);
			WR_AddCommand(OBJECT_SELF, cPlayAnimation);
		}
	}
}

int32 AI_DeterminePathBlockedAction(AActor* OBJECT_SELF, AActor* oBlockingObject)
{
	//TODO complete AI_DeterminePathBlockedAction
	// Check that we have a valid AActor
	if (!IsObjectValid(oBlockingObject))
	{
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "AI_DeterminePathBlockedAction", "INVALID BLOCKING OBJECT");
#endif
		return FALSE_;
	}

	// Attempt to find an action to unblock the path
#ifdef DEBUG
	Log_Trace_AI(OBJECT_SELF, "AI_DeterminePathBlockedAction", "***** START ***** , blocking AActor: " + GetTag(oBlockingObject));
#endif
	FCommand cPathAction;
	FCommand cPathActionContinue;

	// DOOR - Locked
	if (GetPlaceableState(oBlockingObject) == PLC_STATE_DOOR_LOCKED)
	{
		if (!IsControlled(OBJECT_SELF))
		{
			cPathAction = _AI_DoNothing(OBJECT_SELF, -1, -1, FALSE_, TRUE_);
		}
		// Other interesting cases to consider are:
		//   if rogue has lockpicking
		//   if ranged combatant has shattering shot
		//   if mage has spell for breaking a placeable
		//   ... etc
	}
	// DOOR - Unlocked
	else if (GetPlaceableState(oBlockingObject) == PLC_STATE_DOOR_UNLOCKED)
	{
		cPathAction = CommandUseObject(oBlockingObject, PLACEABLE_ACTION_OPEN);

		// Other interesting cases to consider are:
		//   hostile creatures might still decide to break the door to make this more aggressive?
		//   I don't see hurlocks or ogres gently pushing the door...
	}
	// Add more cases here as necessary
	// ...

	// If we found a valid way to unblock the path, add it to the AI queue
	if (GetCommandType(cPathAction) != COMMAND_TYPE_INVALID)
	{
		// This action should unblock the path
		WR_AddCommand(OBJECT_SELF, cPathAction, FALSE_, FALSE_, -1, AI_COMMAND_TIMER);

		// After that, the character can resume the previous action
		FCommand cPreviousCommand = GetPreviousCommand(OBJECT_SELF);
		WR_AddCommand(OBJECT_SELF, cPreviousCommand, FALSE_, FALSE_, COMMAND_ADDBEHAVIOR_DONTCLEAR, AI_COMMAND_TIMER);

		return TRUE_;
	}
	// If not, just report a warning and return failure
	else
	{
#ifdef DEBUG
		Log_Trace_AI(OBJECT_SELF, "AI_DeterminePathBlockedAction", "Couldn't find a solution for dealing with the blocking AActor", LOG_SEVERITY_WARNING);
#endif
		return FALSE_;
	}
}
