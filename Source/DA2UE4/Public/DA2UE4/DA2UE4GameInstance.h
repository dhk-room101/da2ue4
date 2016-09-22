// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "STypes.h"

#include "Engine/GameInstance.h"
#include "DA2UE4GameInstance.generated.h"

class ADA2UE4Party;
class ADA2UE4Creature;
class ADA2UE4Item;

/**
 *
 */
UCLASS()
class DA2UE4_API UDA2UE4GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;

	UFUNCTION()
		virtual void BeginLoadingScreen(const FString& MapName);
	UFUNCTION()
		virtual void EndLoadingScreen();

	void HandleModuleEvents();

	bool SaveGameState();
	bool LoadGameState();

	UPROPERTY()
		int32 GAME_INIT_TYPE = 0; //0-New/1-Load
	UPROPERTY()
		FName PLAYER_LEADER_STORE;
	UPROPERTY()
		EGameMode GAME_MODE = EGameMode::GM_INVALID;
	UPROPERTY()
		ADA2UE4Party* PARTY = nullptr;
	UPROPERTY()
		FName WAYPOINT;

	UPROPERTY()
		int32 APP_RANGE_VALUE_CRISIS = 0;
	UPROPERTY()
		int32 APP_RANGE_VALUE_HOSTILE = 0;
	UPROPERTY()
		int32 APP_RANGE_VALUE_NEUTRAL = 0;
	UPROPERTY()
		int32 APP_RANGE_VALUE_WARM = 0;
	UPROPERTY()
		int32 APP_RANGE_VALUE_FRIENDLY = 0;
	UPROPERTY()
		int32 APP_ROMANCE_RANGE_VALUE_INTERESTED = 0;
	UPROPERTY()
		int32 APP_ROMANCE_RANGE_VALUE_CARE = 0;
	UPROPERTY()
		int32 APP_ROMANCE_RANGE_VALUE_ADORE = 0;
	UPROPERTY()
		int32 APP_ROMANCE_RANGE_VALUE_LOVE = 0;

	//active conversation, if not 0 the conv engine should kick in
	UPROPERTY()
		int32 CONVERSATION = 0;
	UPROPERTY()
		int32 DISABLE_FOLLOWER_DIALOG = 0;
	UPROPERTY()
		FName PARTY_OVERRIDE_DIALOG;
	UPROPERTY()
		int32 PARTY_OVERRIDE_DIALOG_ACTIVE = 0;
	UPROPERTY()
		int32 AMB_SYSTEM_DIALOG = 0;
	UPROPERTY()
		int32 AI_PARTY_CLEAR_TO_ATTACK = 0;
	UPROPERTY()
		int32 AI_DISABLE_TABLES = 0;
	//yes
	UPROPERTY()
		int32 AI_USE_GUI_TABLES_FOR_FOLLOWERS = 1;

	//queue events
	UPROPERTY()
	TArray<FGameEvent> qEvents;

protected:
	UPROPERTY()
	TArray<ADA2UE4Creature*> Creatures;

	UPROPERTY()
	TArray<ADA2UE4Item*> Items;
};
