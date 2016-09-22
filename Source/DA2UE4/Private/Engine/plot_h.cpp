#include "DA2UE4.h"
#include "DA2UE4GameInstance.h"
#include "DA2UE4Party.h"
#include "plot_h.h"

#include "ldf.h"
#include "events_h.h"
#include "wrappers_h.h"
#include "utility_h.h"

#include "demo_constants_h.h"

FPlot ParsePlot(int32 nPlotID)
{
	FString sValue;

	FString sPlot = FString::FromInt(nPlotID);

	FString aFullPath = FPaths::GameDevelopersDir();
	aFullPath += "Source/JSON/PLO/";
	aFullPath += *sPlot;
	aFullPath += ".json";
	FString JsonStr;
	FFileHelper::LoadFileToString(JsonStr, *aFullPath);

	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonStr);
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());

	FPlot plot;

	if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
	{
		plot.ResRefID = nPlotID;

		TMap<FString, TSharedPtr<FJsonValue>> JsonValuesMap = JsonObject->Values;
		TSharedPtr<FJsonValue> rootValues;
		JsonValuesMap.RemoveAndCopyValue("Resource", rootValues);
		TSharedPtr<FJsonObject> rootObject = rootValues->AsObject();

		TMap<FString, TSharedPtr<FJsonValue>> rootValuesMap = rootObject->Values;
		TSharedPtr<FJsonValue> agentValues;
		rootValuesMap.RemoveAndCopyValue("Agent", agentValues);
		TSharedPtr<FJsonObject> agentObject = agentValues->AsObject();

		TMap<FString, TSharedPtr<FJsonValue>> agentValuesMap = agentObject->Values;

		for (auto const& y : agentValuesMap)
		{
			if (y.Key == "ResRefName")
			{
				y.Value.Get()->TryGetString(sValue);
				plot.ResRefName = sValue;
			}
			if (y.Key == "LocalCopy")
			{
				y.Value.Get()->TryGetString(sValue);
				plot.LocalCopy = sValue == "False" ? false : true;
			}
			if (y.Key == "Name")
			{
				y.Value.Get()->TryGetString(sValue);
				plot.Name = sValue;
			}
			if (y.Key == "NameStringID")
			{
				y.Value.Get()->TryGetString(sValue);
				plot.NameStringID = FCString::Atoi(*sValue);
			}
			if (y.Key == "NameRequiresReTranslation")
			{
				y.Value.Get()->TryGetString(sValue);
				plot.NameRequiresReTranslation = sValue == "False" ? false : true;
			}
			if (y.Key == "GUID")
			{
				y.Value.Get()->TryGetString(sValue);
				plot.GUID = sValue;
			}
			if (y.Key == "ScriptURI")
			{
				y.Value.Get()->TryGetString(sValue);
				plot.ScriptURI = FCString::Atoi(*sValue);
			}
			if (y.Key == "Priority")
			{
				y.Value.Get()->TryGetString(sValue);
				plot.Priority = FCString::Atoi(*sValue);
			}
			if (y.Key == "JournalImage")
			{
				y.Value.Get()->TryGetString(sValue);
				plot.JournalImage = sValue;
			}
			if (y.Key == "ParentPlotURI")
			{
				y.Value.Get()->TryGetString(sValue);
				plot.ParentPlotURI = FCString::Atoi(*sValue);
			}
			if (y.Key == "EntryType")
			{
				y.Value.Get()->TryGetString(sValue);
				plot.EntryType = FCString::Atoi(*sValue);
			}
			if (y.Key == "AllowPausing")
			{
				y.Value.Get()->TryGetString(sValue);
				plot.AllowPausing = sValue == "False" ? false : true;
			}
		}

		TSharedPtr<FJsonValue> StatusListValues;
		agentValuesMap.RemoveAndCopyValue("StatusList", StatusListValues);
		TSharedPtr<FJsonObject> StatusListObject = StatusListValues->AsObject();

		TArray<TSharedPtr<FJsonValue>> StatusArray = StatusListObject->GetArrayField("Agent");

		FPlotNode statusNode;

		if (StatusArray.Num() > 0)
		{
			int32 counter = 0;
			do {
				TSharedPtr<FJsonValue> StatusValue = StatusArray[counter];
				TSharedPtr<FJsonObject> StatusObject = StatusValue->AsObject();

				TMap<FString, TSharedPtr<FJsonValue>> StatusValuesMap = StatusObject->Values;
				for (auto const& x : StatusValuesMap)
				{
					if (x.Key == "Flag")
					{
						x.Value.Get()->TryGetString(sValue);
						statusNode.Flag = FCString::Atoi64(*sValue);
					}
					if (x.Key == "Name")
					{
						x.Value.Get()->TryGetString(sValue);
						statusNode.Name = sValue;
					}
					if (x.Key == "Final")
					{
						x.Value.Get()->TryGetString(sValue);
						statusNode.Final = sValue == "False" ? false : true;
					}
					if (x.Key == "Repeatable")
					{
						x.Value.Get()->TryGetString(sValue);
						statusNode.Repeatable = sValue == "False" ? false : true;
					}
					if (x.Key == "JournalText")
					{
						x.Value.Get()->TryGetString(sValue);
						statusNode.JournalText = sValue;
					}
					if (x.Key == "JournalTextStringID")
					{
						x.Value.Get()->TryGetString(sValue);
						statusNode.JournalTextStringID = FCString::Atoi(*sValue);
					}
					if (x.Key == "JournalTextRequiresReTranslation")
					{
						x.Value.Get()->TryGetString(sValue);
						statusNode.JournalTextRequiresReTranslation = sValue == "False" ? false : true;
					}
					if (x.Key == "RewardID")
					{
						x.Value.Get()->TryGetString(sValue);
						statusNode.RewardID = FCString::Atoi(*sValue);
					}
					if (x.Key == "Comment")
					{
						x.Value.Get()->TryGetString(sValue);
						statusNode.Comment = sValue;
					}
					if (x.Key == "DefaultValue")
					{
						x.Value.Get()->TryGetString(sValue);
						statusNode.DefaultValue = FCString::Atoi(*sValue);
					}
					if (x.Key == "AreaLocationTag")
					{
						x.Value.Get()->TryGetString(sValue);
						statusNode.AreaLocationTag = sValue;
					}
					if (x.Key == "OfferID")
					{
						x.Value.Get()->TryGetString(sValue);
						statusNode.OfferID = FCString::Atoi(*sValue);
					}
				}

				//update Flag with pattern ResRefID+"000"+Flag
				statusNode.Flag = PlotFlagConversion(plot.ResRefID, statusNode.Flag);

				FPlotElement ePlot;
				ePlot.pNode = statusNode;
				ePlot.pValue = 0; //false by default
				plot.StatusList.Add(ePlot);

				counter++;
			} while (counter < StatusArray.Num());
		}
#ifdef DEBUG
		LogWarning("Plot " + IntToString(nPlotID) + " parsed!!");
#endif // DEBUG

		//TODO PlotAssistInfoList
		GetParty()->Plots.Add(plot);
	}
	else {
#ifdef DEBUG
		LogError("Not Deserialized" + aFullPath);
#endif
	}
	return plot;
}

int64 PlotFlagConversion(int32 ResRefID, int64 Flag)
{
	return FCString::Atoi64(*(IntToString(ResRefID) + TEXT("000") + IntToString(Flag)));
}

int32 SetPlotFlag(int32 plotIndex, int64 nPlotFlag, int32 nPlotValue)
{
	//update the actual Plot table in Party
	for (int32 j = 0; j < GetParty()->Plots[plotIndex].StatusList.Num(); j++)
	{
		if (GetParty()->Plots[plotIndex].StatusList[j].pNode.Flag == nPlotFlag)
		{
			GetParty()->Plots[plotIndex].StatusList[j].pValue = nPlotValue;
			return TRUE_;
		}
	}
#ifdef DEBUG
	LogError("SetPlotFlag: plot not found with flag " + IntToString(nPlotFlag));
#endif // DEBUG

	return FALSE_;
}

//nPlotFlag comes from CONV file and is 0-512 range, so needs conversion when looking for plot
int32 HandlePlotFlag(int32 nPlotEventType /*get/set*/, FString nPlotGUID, int64 nPlotFlag, int32 nPlotValue)
{
	int32 handled = FALSE_;
	FPlot plot;

	int32 plotIndex = -1; //position in plot array, so we keep track in one place and don't search multiple times

	for (int32 i = 0; i < GetParty()->Plots.Num(); i++)
	{
		if (GetParty()->Plots[i].GUID == nPlotGUID)
		{
			plot = GetParty()->Plots[i];
			plotIndex = i;
			break;
		}
	}

	if (plot.ResRefID == 0) return FALSE_;//not found

	//int32 convertedFlag = PlotFlagConversion(plot.ResRefID, nPlotFlag);
	nPlotFlag = PlotFlagConversion(plot.ResRefID, nPlotFlag); //not needed

	int64 _hash = GetNewHash(nPlotGUID, TRUE_);

	switch (_hash)
	{
	case PLT_DEMO000PL_MAIN:
	{
		switch (nPlotEventType)
		{
		case EVENT_TYPE_GET_PLOT://only defined flags
		{
			//plot less than but bigger than
			int64 lessThan = FCString::Atoi64(*(IntToString(plot.ResRefID) + TEXT("000") + IntToString(255)));
			int64 biggerThan = FCString::Atoi64(*(IntToString(plot.ResRefID) + TEXT("000") + IntToString(0)));
			if (nPlotFlag <= lessThan && nPlotFlag >= biggerThan) //starting main flag in our plot range
			{
#ifdef DEBUG
				LogError("Get Flags need to be > 255 in their respective plot, got " + IntToString(nPlotFlag));
#endif
				return FALSE_;
			}

			switch (nPlotFlag)
			{
			case DEMO_DECLINED_QUEST:
			{
				//This is a "defined" plot flag. When the plot is checked
				//to see whether the flag is true or false, its status is
				//determined using the following code.
				if (WR_GetPlotFlag(PLT_DEMO000PL_MAIN, DEMO_TALKED_TO_BARKEEP) == TRUE_ &&
					WR_GetPlotFlag(PLT_DEMO000PL_MAIN, DEMO_QUEST_ACCEPTED) == FALSE_)
				{
					return TRUE_;
				}
				else
				{
					return FALSE_;
				}
				break;
			}
			default:
			{
#ifdef DEBUG
				LogError(Int64ToString(_hash) + " GET Unknown flag: " + IntToString(nPlotFlag));

#endif // DEBUG

				break;
			}
			}

			break;
		}
		case EVENT_TYPE_SET_PLOT:
		{
			switch (nPlotFlag)
			{
			case DEMO_TALKED_TO_BARKEEP:
			{
				if (nPlotValue == -1) return FALSE_; //error, needs to be 0/1
				return SetPlotFlag(plotIndex, nPlotFlag, TRUE_);
			}
			case DEMO_QUEST_ACCEPTED:
			{
				//TODO UT_AddItemToInventory(DEMO_INKEEPER_KEY_R);
				LogWarning("DEMO_QUEST_ACCEPTED: UT_AddItemToInventory(DEMO_INKEEPER_KEY_R)");

				if (nPlotValue == -1) return FALSE_; //error, needs to be 0/1
				return SetPlotFlag(plotIndex, nPlotFlag, TRUE_);
			}
			case DEMO_BANDIT_HOSTILE:
			{
				//This causes all members of the bandit's "team" (the bandit and
				//the other bar patrons) to turn hostile. Since they're in the presence
				//of the player already, they'll immediately perceive him and initiate
				//combat.

				UT_TeamGoesHostile(BANDIT_TEAM);

				//During debug manually add the player to the bandits threat target
// 				for (int32 nIndex = 0; nIndex < GetArraySize(arTeam); nIndex++)
// 				{
// 					GameObject _member = arTeam[nIndex];
// 					SetEnemy(_member, GetHero());
// 				}

				WR_SetGameMode(static_cast<uint8>(EGameMode::GM_COMBAT));

				break;
			}


			default:
			{
#ifdef DEBUG
				LogError(Int64ToString(_hash) + " SET Unknown flag: " + IntToString(nPlotFlag));

#endif // DEBUG
				break;
			}
			}

			break;
		}
		default:
		{
#ifdef DEBUG
			LogError("Unknown plot event type!!!");
#endif
			break;
		}
		}
		break;
	}
	default:
	{
		LogError("Plot not found, needed " + Int64ToString(_hash));
		break;
	}

	}

	return handled;
}

FString GetPlotResRef(int64 nPlotHash)
{
	for (FPlot plot : GetParty()->Plots)
	{
		int64 _hash = GetNewHash(plot.GUID, TRUE_);
		if (nPlotHash == _hash)
		{
			return plot.ResRefName;
		}
	}
	return "";
}

int32 GetPartyPlotFlag(int64 nPlotHash, int32 nFlag)
{
	for (FPlot plot : GetParty()->Plots)
	{
		int64 _hash = GetNewHash(plot.GUID, TRUE_);
		if (nPlotHash == _hash)
		{
			for (FPlotElement e : plot.StatusList)
			{
				if (e.pNode.Flag == nFlag)
				{
					return e.pValue;
				}
			}
		}
	}

#ifdef DEBUG
	LogError("plot_h: didn't find plot flag " + IntToString(nFlag) + " current value from hash " + IntToString(nPlotHash));
#endif // DEBUG
	return FALSE_;//error
}

FString GetPlotFlagName(int64 nPlotHash, int32 nFlag)
{
	for (FPlot plot : GetParty()->Plots)
	{
		int64 _hash = GetNewHash(plot.GUID, TRUE_);
		if (nPlotHash == _hash)
		{
			for (FPlotElement e : plot.StatusList)
			{
				if (e.pNode.Flag == nFlag)
				{
					return e.pNode.Name;
				}
			}
		}
	}
	return "";//error
}