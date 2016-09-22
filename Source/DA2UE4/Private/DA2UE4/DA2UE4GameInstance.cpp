// Fill out your copyright notice in the Description page of Project Settings.

#include "DA2UE4.h"
#include "DA2UE4GameInstance.h"

#include "Runtime/MoviePlayer/Public/MoviePlayer.h"

#include "DA2UE4Library.h"
#include "DA2UE4Party.h"
#include "DA2UE4SaveGame.h"
#include "DA2UE4Creature.h"

#include "STypes.h"

#include "ldf.h"
#include "core_h.h"
#include "events_h.h"
#include "STypes.h"
#include "ai_main_h.h"
#include "ability_h.h"
#include "effect_resurrection_h.h"
#include "sys_soundset_h.h"

#include <string>

void UDA2UE4GameInstance::Init()
{
	UGameInstance::Init();

	LogWarning("Game Instance ON");

	GAME_INIT_TYPE = 0;//NEW

	PARTY = NewObject<ADA2UE4Party>();

	//set game mode explore if currently invalid
	if (GAME_MODE == EGameMode::GM_INVALID)
	{
		GAME_MODE = EGameMode::GM_EXPLORE;
#ifdef DEBUG
		LogWarning("setting game mode to explore");
#endif
	}

	if (WAYPOINT.IsNone())
		WAYPOINT = "demo100wp_start";

	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UDA2UE4GameInstance::BeginLoadingScreen);
	FCoreUObjectDelegates::PostLoadMap.AddUObject(this, &UDA2UE4GameInstance::EndLoadingScreen);
}

void UDA2UE4GameInstance::BeginLoadingScreen(const FString& MovieName)
{
	bool save = SaveGameState();

	FLoadingScreenAttributes LoadingScreen;
	LoadingScreen.bAutoCompleteWhenLoadingCompletes = false;
	//LoadingScreen.MinimumLoadingScreenDisplayTime = 10;
	LoadingScreen.WidgetLoadingScreen = FLoadingScreenAttributes::NewTestLoadingScreenWidget();

	GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
	GetMoviePlayer()->PlayMovie();
}

void UDA2UE4GameInstance::EndLoadingScreen()
{

}

void UDA2UE4GameInstance::HandleModuleEvents()
{
	FGameEvent ev = GetCurrentEvent(this);
	if (ev.nGameEventType == EVENT_TYPE_INVALID) return;

#ifdef DEBUG
	ev = SetEventString(ev, *(this->GetName()));
	Log_Events("", ev);
#endif

	int32 nEventType = GetEventType(ev);

	switch (nEventType)
	{
	case EVENT_TYPE_SET_GAME_MODE:
	{
		int32 nGameMode = GetEventInteger(ev, 0);
#ifdef DEBUG
		Log_Trace(LOG_CHANNEL_EVENTS, GetCurrentScriptName(ev), "Setting game mode to: " + IntToString(nGameMode));

#endif // DEBUG


		//TODO complete EVENT_TYPE_SET_GAME_MODE
		// -----------------------------------------------------------------
		// Signal Pause event to the game when hitting combat
		// -----------------------------------------------------------------
		// 		if (nGameMode == EGameMode::GM_COMBAT && GetGameMode() == EGameMode::GM_EXPLORE)
		// 		{
		// 			if (ConfigIsAutoPauseEnabled())
		// 			{
		// 				DelayEvent(0.5f, this, Event(EVENT_TYPE_AUTOPAUSE));
		// 			}
		// 		}

		// 		if (nGameMode == EGameMode::GM_EXPLORE && IsNoExploreArea())
		// 		{
		// #ifdef DEBUG
		// 			Log_Trace(LOG_CHANNEL_EVENTS, GetCurrentScriptName(ev), "COMBAT-ONLY AREA!!! ABORTING CHANGE TO EXPLORE MODE!!!");

		// #endif // DEBUG
		// 			break;
		// 		}

		SetGameMode(nGameMode);

		break;
	}
	// ---------------------------------------------------------------------
	// Game Mode Switch
	//      int32(0) - New Game Mode (GM_* constant)
	//      int32(1) - Old Game Mode (GM_* constant)
	// ---------------------------------------------------------------------
	case EVENT_TYPE_GAMEMODE_CHANGE:
	{
		int32 nNewGameMode = GetEventInteger(ev, 0);
		int32 nOldGameMode = GetEventInteger(ev, 1);

		// -----------------------------------------------------------------
		// Georg: I'm tracking game mode switches for aggregated
		//        'time spent in mode x' analysis
		// -----------------------------------------------------------------
		//TrackModuleEvent(nEvent, OBJECT_SELF, nullptr, nNewGameMode, nOldGameMode);
#ifdef DEBUG
		Log_Trace(LOG_CHANNEL_SYSTEMS, "Game mode changed from " + IntToString(nOldGameMode) + " to " + IntToString(nNewGameMode));

#endif // DEBUG

		//------------------------------------------------------------------
		// 2008-Feb-29 - EV 89889 by Georg
		// We don't care about the new game mode being GUI and other
		// non gameplay related modes
		//------------------------------------------------------------------
		if (nNewGameMode == GM_GUI || nNewGameMode == GM_FLYCAM || nNewGameMode == GM_INVALID || nNewGameMode == GM_PREGAME)
		{
			return;
		}

		if (nNewGameMode != GM_COMBAT)
		{
			// prevent party members from attacking until allowed to do so
			AI_SetPartyAllowedToAttack(GetHero(), FALSE_);

			//TODO read INI entries
			//Check for levelup tutorial if in Explore mode
			//don't show if chargen has been skipped
			// 			if (ReadIniEntry("DebugOptions", "SkipCharGen") == "1")
			// 			{
			// 				//do nothing
			// 			}
			// 			else
			// 			{
			// 				if (nNewGameMode == GM_EXPLORE)
			// 				{
			// 					AActor* oModule = GetModule();
			// 					//1 means show the tutorial, 2 means it has been seen
			// 					if (GetLocalInt(oModule, "TUTORIAL_HAVE_SEEN_LEVEL_UP") == 1)
			// 					{
			// 						SetLocalInt(oModule, "TUTORIAL_HAVE_SEEN_LEVEL_UP", 2);
			// 						BeginTrainingMode(TRAINING_SESSION_LEVEL_UP);
			// 					}
			// 				}
			// 			}
		}

		if (nNewGameMode == GM_COMBAT || nNewGameMode == GM_EXPLORE)
		{
			Ability_OnGameModeChange(nNewGameMode);
		}

		// ----------------------------------------------------------------
		// Remove party soundset restrictions
		// -----------------------------------------------------------------
		SSPartyResetSoundsetRestrictions();

		// -----------------------------------------------------------------
		// If new game mode is combat, set CombatState on each Party Member
		// -----------------------------------------------------------------
		SetCombatStateParty(nNewGameMode == GM_COMBAT);

		if (nNewGameMode == GM_COMBAT)
		{
			//TODO EVENT_TYPE_GAMEMODE_CHANGE Tutorial Combat
			// Tutorial message
			// 			if (GetLocalInt(GetModule(), TUTORIAL_ENABLED))
			// 			{
			// 				if (GetCreatureCoreClass(GetHero()) == CLASS_WIZARD)
			// 				{
			// 					WR_SetPlotFlag(PLT_TUT_COMBAT_BASIC_MAGIC, TUT_COMBAT_BASIC_MAGIC_1, TRUE_);
			// 				}
			// 				else
			// 					WR_SetPlotFlag(PLT_TUT_COMBAT_BASIC, TUT_COMBAT_BASIC_1, TRUE_);
			// 			}
		}
		// else below

		// <rant>
		// Georg Sept 10, 2008:
		//        If we are going into dialog mode, we always revive the party.
		//        Note: I am not happy about this. The core problem is that we have to
		//              requirements fighting with each other.
		//
		//              a) Don't have people jump up immediately after combat
		//              b) Before a dialog starts, people need to be revived and it has to happen
		//                 inline in the currently executing context because once it ends, we're
		//                 no longer in control of what happens, the dialog/cutscene engine takes over.
		//
		//              This lends itself to a host of issues, including race conditions.
		//
		//              The real solution is to have the writers stop writing 'combat ends,
		//              force dialog situations' but that's not gonna happen for DA.
		//
		//              The following 3 lines will force all party members alive - immediately
		//              in-line. Implicitly, this will cause termination of the delayed gamemode
		//              change event loop in the event queue next time it gets processed.
		//
		//
		else if (nNewGameMode == GM_CONVERSATION)
		{
			ResurrectPartyMembers();
		}
		// </rant>



		break;
	}
	default:
	{
#ifdef DEBUG
		LogWarning("module event not found");
#endif
		break;
	}
	}
}

bool UDA2UE4GameInstance::SaveGameState()
{
	UDA2UE4SaveGame* SaveGameInstance = Cast<UDA2UE4SaveGame>(UGameplayStatics::CreateSaveGameObject(UDA2UE4SaveGame::StaticClass()));

	SaveGameInstance->PLAYER_LEADER_STORE = PLAYER_LEADER_STORE;

	FPartyRecord PartyRecord;
	PartyRecord.Henchman0 = PARTY->Henchman0;
	PartyRecord.Henchman1 = PARTY->Henchman1;
	PartyRecord.Henchman2 = PARTY->Henchman2;
	PartyRecord.Henchman3 = PARTY->Henchman3;

	SaveGameInstance->PartyRecord = PartyRecord;

	for (TActorIterator<ADA2UE4Creature> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		ADA2UE4Creature* oCreature = *ActorItr;
		FCreatureRecord CreatureRecord;
		CreatureRecord.Class = oCreature->GetClass();
		CreatureRecord.Name = FName(*oCreature->GetName());

		SaveGameInstance->CreatureRecords.Add(CreatureRecord);
	}

	UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->SaveSlotName, SaveGameInstance->UserIndex);

	bool l = LoadGameState();

	LogWarning("Game State Saved!");

	return true;
}

bool UDA2UE4GameInstance::LoadGameState()
{
	UDA2UE4SaveGame* LoadGameInstance = Cast<UDA2UE4SaveGame>(UGameplayStatics::CreateSaveGameObject(UDA2UE4SaveGame::StaticClass()));
	LoadGameInstance = Cast<UDA2UE4SaveGame>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->SaveSlotName, LoadGameInstance->UserIndex));
	//FString PlayerNameToDisplay = LoadGameInstance->PlayerName;

	for (int32 i = 0; i < LoadGameInstance->CreatureRecords.Num(); i++)
	{
		if (LoadGameInstance->CreatureRecords[i].Class->IsChildOf(ADA2UE4Creature::StaticClass()))
		{
			LogWarning("");
		}
	}
	return true;
}