// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/SaveGame.h"
#include "DA2UE4SaveGame.generated.h"

USTRUCT()
struct FPlotAssistInfoListRecord
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(SaveGame)
		FName PlotAdvancerTag;
	UPROPERTY(SaveGame)
		bool AdvancesPlot;
};

USTRUCT()
struct FPlotNodeRecord
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(SaveGame)
		int64 Flag = 0;
	UPROPERTY(SaveGame)
		FString Name = "";
	UPROPERTY(SaveGame)
		bool Final = false;
	UPROPERTY(SaveGame)
		bool Repeatable = false;
	UPROPERTY(SaveGame)
		FString JournalText = "";
	UPROPERTY(SaveGame)
		int32 JournalTextStringID = 0;
	UPROPERTY(SaveGame)
		bool JournalTextRequiresReTranslation = false;
	UPROPERTY(SaveGame)
		int32 RewardID = 0;
	UPROPERTY(SaveGame)
		FString Comment = "";
	UPROPERTY(SaveGame)
		int32 DefaultValue = 0;
	UPROPERTY(SaveGame)
		FString AreaLocationTag = "";
	UPROPERTY(SaveGame)
		int32 OfferID = 0;
	UPROPERTY(SaveGame)
		TArray<FPlotAssistInfoListRecord> PlotAssistInfoList;
};

USTRUCT()
struct FPlotElementRecord
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(SaveGame)
		FPlotNodeRecord pNode;
	UPROPERTY(SaveGame)
		int32 pValue = 0;//true/false
};

USTRUCT()
struct FPlotRecord
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(SaveGame)
		int32 ResRefID = 0;
	UPROPERTY(SaveGame)
		FString ResRefName = "";
	UPROPERTY(SaveGame)
		bool LocalCopy = false;
	UPROPERTY(SaveGame)
		FString Name = "";
	UPROPERTY(SaveGame)
		int32 NameStringID = 0;
	UPROPERTY(SaveGame)
		bool NameRequiresReTranslation = false;
	UPROPERTY(SaveGame)
		FString GUID = "";
	UPROPERTY(SaveGame)
		int32 ScriptURI = 0;
	UPROPERTY(SaveGame)
		int32 Priority = 0;
	UPROPERTY(SaveGame)
		FString JournalImage = "";
	UPROPERTY(SaveGame)
		int32 ParentPlotURI = 0;
	UPROPERTY(SaveGame)
		int32 EntryType = 0;
	UPROPERTY(SaveGame)
		bool AllowPausing = false;
	UPROPERTY(SaveGame)
		TArray<FPlotElementRecord> StatusList;
};


USTRUCT()
struct FPartyRecord
{
	GENERATED_USTRUCT_BODY()

		//henchmen
		UPROPERTY(SaveGame)
		FName Henchman0;
	UPROPERTY(SaveGame)
		FName Henchman1;
	UPROPERTY(SaveGame)
		FName Henchman2;
	UPROPERTY(SaveGame)
		FName Henchman3;

	//TArray<FPlotRecord> Plots;
};

USTRUCT()
struct FCreatureRecord
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(SaveGame)
		UClass* Class = AActor::StaticClass();

	UPROPERTY(SaveGame)
		FTransform Transform;

	UPROPERTY(SaveGame)
		FName Name;
};

/**
 *
 */
UCLASS()
class DA2UE4_API UDA2UE4SaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(SaveGame)
		FName PLAYER_LEADER_STORE;

	UPROPERTY(SaveGame)
		FPartyRecord PartyRecord;

	UPROPERTY(SaveGame)
		TArray<FCreatureRecord> CreatureRecords;

	UPROPERTY(SaveGame)
		FString SaveSlotName;

	UPROPERTY(SaveGame)
		uint32 UserIndex;

	UDA2UE4SaveGame();
};
