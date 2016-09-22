//TODO cleanup //UE_LOG or turn it into #ifdef DEBUG
#include "DA2UE4.h"
#include "DA2UE4Creature.h"
#include "DA2UE4Item.h"
#include "DA2UE4GameInstance.h"
#include "DA2UE4Library.h"
#include "DA2UE4PlayerController.h"
#include "DA2UE4CreatureController.h"
#include "DA2UE4Party.h"
#include "DA2UE4TargetPoint.h"
#include "DA2UE4Trigger.h"

#include "Json.h"

#include "UMG.h"

#include "ldf.h"
#include "events_h.h"
#include "STypes.h"

#include "Kismet/KismetMathLibrary.h"

#include <string>

//DHK

FString GetRealName(FString instance)
{
	const TCHAR *delim;
	delim = TEXT("_");

	TArray<FString> Parsed;
	instance.ParseIntoArray(Parsed, delim);

	FString _final;

	for (int32 i = 0; i < Parsed.Num() - 1; i++)
	{
		_final += Parsed[i];
		if (i < Parsed.Num() - 2)
			_final += "_";
	}

	return _final;
}

int64 GetNewHash(FString sString, int32 bGuid)
{
	int64 _hash = 0xCBF29CE484222325;

	if (bGuid == TRUE_)
	{
		sString = sString.Replace(TEXT("-"), TEXT(""));
		sString = sString.Replace(TEXT("{"), TEXT(""));
		sString = sString.Replace(TEXT("}"), TEXT(""));
	}

	for (char cc : sString.GetCharArray())
	{
		_hash *= 0x00000100000001B3;
		_hash ^= cc;
	}

	if (_hash < 0)
		_hash = _hash + UINT64_MAX + 1;

#ifdef DEBUG
	FString hashString(std::to_string(_hash).c_str());
	LogWarning("FNV64 HASH: " + hashString + " from FString " + sString);

#endif // DEBUG

	return _hash;
}

FString GetJSON(int32 n2DA)
{
	FString _resource;

	switch (n2DA)
	{
		//case TABLE_AREA_LOAD_HINT: _resource = "LoadHints"; break;
		//case TABLE_AREA_LOAD_HINT_VLOW: _resource = "LoadHintsVLowLevel"; break;
	case TABLE_CONVERSATION_ANIMATIONS: _resource = "ANIM_conversation"; break;
	case TABLE_BASE_ANIMATIONS: _resource = "ANIM_base"; break;
	case TABLE_COMBAT_ANIMATIONS: _resource = "ANIM_combat"; break;
	case TABLE_PROJECTILES: _resource = "Projectiles"; break;
	case TABLE_ITEMPRPS: _resource = "ItemProperties"; break;
	case TABLE_UI_MESSAGES: _resource = "UIMessages"; break;
	case TABLE_PROPERTIES: _resource = "Properties"; break;
	case TABLE_PACKAGES: _resource = "Packages"; break;
	case TABLE_RANGES: _resource = "Ranges"; break;
	case TABLE_AMBIENT: _resource = "AmbientAI"; break;
		//case TABLE_RESOURCES: _resource = "Resources"; break;
	case TABLE_SHAPECHANGE: _resource = "ShapeChange"; break;
	case TABLE_TALK: _resource = "TalkTable"; break;
	case TABLE_GAME_SETTINGS: _resource = "GameSettings"; break;
	case TABLE_ABILITIES_SPELLS: _resource = "AbilitiesBase"; break;
	case TABLE_EVENTS: _resource = "EngineEvents"; break;
	case TABLE_APPEARANCE: _resource = "AppearanceBase"; break;
		//case TABLE_ABILITIES: _resource = "AbilitiesBase"; break;
		//case TABLE_AREA_DATA: _resource = "AreaData"; break;
	case TABLE_EXPERIENCE: _resource = "Experience"; break;
	case TABLE_ITEMS: _resource = "BaseItem"; break;
	case TABLE_RULES_CLASSES: _resource = "BaseClass"; break;
	case TABLE_RULES_RACES: _resource = "BaseRace"; break;
	case TABLE_AUTOSCALE: _resource = "AutoScale"; break;
		//case TABLE_AUTOSCALE_DATA: _resource = "AutoscaleData"; break;
	case TABLE_DIFFICULTY: _resource = "Difficulty"; break;
	case TABLE_COMMANDS: _resource = "Commands"; break;
	case TABLE_CREATURERANKS: _resource = "CreatureRanks"; break;
		//case TABLE_MATERIAL: _resource = "MaterialRules"; break;//?
		//case TABLE_MATERIAL_TYPES: _resource = "MaterialTypes"; break;
	case TABLE_SOUNDSETS: _resource = "SoundSetTypes"; break;
	case TABLE_TACTICS_CONDITIONS: _resource = "TacticsConditions"; break;
	case TABLE_TACTICS_BASE_CONDITIONS: _resource = "TacticsBaseConditions"; break;
	case TABLE_AI_TACTICS_TARGET_TYPE: _resource = "AITacticsTargetType"; break;
	case TABLE_AI_ABILITY_COND: _resource = "AIAbilitiesCond"; break;
	case TABLE_AI_BEHAVIORS: _resource = "AIBehaviors"; break;
	case TABLE_ITEMSTATS: _resource = "ItemStats"; break;
		//case TABLE_DAMAGETYPES: _resource = "DamageTypes"; break;
	case TABLE_EFFECTS: _resource = "Effects"; break;
		//case TABLE_EFFECT_IMMUNITIES: _resource = "EffectImmunities"; break;

		//AI Packages
	case AI_TABLE_DEFAULT: _resource = "AIP_Default"; break;
#ifdef DEBUG
	default: LogError("Unknown Table ID requested:" + n2DA); break;

#endif // DEBUG

	}

	return _resource;
}

int32 GetJsonNodeCount(FString table)
{
	FString sValue;

	FString aFullPath = FPaths::GameDevelopersDir();
	aFullPath += "Source/JSON/";
	aFullPath += *table;
	aFullPath += ".json";
	FString JsonStr;
	FFileHelper::LoadFileToString(JsonStr, *aFullPath);

	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonStr);
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());

	if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
	{
		TArray<TSharedPtr<FJsonValue>> objArray = JsonObject->GetArrayField(*table);

		return objArray.Num();
	}
	else {
#ifdef DEBUG
		LogError("Not Deserialized" + aFullPath);
#endif
	}
	return -1;
}

FString GetJsonNode(FString table, FString keyValue, FString sRow, FString keyID)
{
	FString sValue;

	FString aFullPath = FPaths::GameDevelopersDir();
	aFullPath += "Source/JSON/";
	aFullPath += *table;
	aFullPath += ".json";
	FString JsonStr;
	FFileHelper::LoadFileToString(JsonStr, *aFullPath);

	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonStr);
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());

	if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
	{
		//FString keyID = "ID";
		TArray<TSharedPtr<FJsonValue>> objArray = JsonObject->GetArrayField(*table);

		if (objArray.Num() > 0)
		{
			int32 counter = 0;
			do {
				TSharedPtr<FJsonValue> value = objArray[counter];
				TSharedPtr<FJsonObject> json = value->AsObject();

				FString id = json->GetStringField(*keyID);

				if (id == sRow)
				{
					sValue = json->GetStringField(*keyValue);
					break;
				}
				counter++;
			} while (counter < objArray.Num());
		}
	}
	else {
#ifdef DEBUG
		LogError("Not Deserialized" + aFullPath);
#endif
	}

	return sValue;
}

FString GetConvAnimString(FString sField)
{
	FString json = GetJSON(TABLE_CONVERSATION_ANIMATIONS);
	return GetJsonNode(json, "Name", sField, "AnimationList");
}

FString GetConvAnimType(FString sField)
{
	FString json = GetJSON(TABLE_CONVERSATION_ANIMATIONS);
	return GetJsonNode(json, "Type", sField, "AnimationList");
}

UAnimSequence* GetConvAnimation(FString AnimationResource)
{
	//AnimSequence'/Game/Data/Art/Anims/CNV/mh_2p_exchange_a.mh_2p_exchange_a'
	FString sPath = "/Game/Data/Art/Anims/CNV/";
	sPath += AnimationResource;
	sPath += ".";
	sPath += AnimationResource;

	UAnimSequence* animSequence = LoadAnimationSequenceFromPath(FName(*sPath));
	return animSequence;
}

UWorld* GetCurrentWorld()
{
	bool IsValid;
	return UDA2UE4Library::GetDA2UE4Data(IsValid)->DA2UE4CurrentWorld;
}

ADA2UE4PlayerController* GetCurrentPlayerController()
{
	return Cast<ADA2UE4PlayerController>(Cast<APawn>(GetActorFromName(GetModule()->PLAYER_LEADER_STORE))->GetController());
}

int32 GetGameSettings(int32 nSetting)
{
	return GetM2DAInt(TABLE_GAME_SETTINGS, "Value", nSetting);
}

AActor* GetActorFromName(FName name)
{
	if (name.IsNone()) return nullptr;

	for (TActorIterator<ADA2UE4Creature> ActorItr(GetCurrentWorld()); ActorItr; ++ActorItr)
	{
		ADA2UE4Creature* oCreature = *ActorItr;
		if (oCreature->GetName() == name.ToString())
		{
			return oCreature;
		}
	}
	return nullptr;
}

//DA
void LogTrace(int32 nChannel, FString sLogEntry, AActor* oTarget)
{
#ifdef DEBUG
	if (oTarget)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s : Channel %i: %s "), *(oTarget->GetName()), nChannel, *sLogEntry);
	}
	else UE_LOG(LogTemp, Warning, TEXT("Generic : Channel %i: %s"), nChannel, *sLogEntry);
#endif
}

FString IntToString(int32 nInteger)
{
	return FString::FromInt(nInteger);
}

FString Int64ToString(int64 nInteger)
{
	FString nString(std::to_string(nInteger).c_str());
	return nString;
}

FString IntToHexString(int32 nInteger)
{
	//TODO int32 to hex FString
	return FString::FromInt(nInteger);
}

float IntToFloat(int32 nInteger)
{
	return nInteger;
}

FString FloatToString(float fFloat, int32 nWidth, int32 nDecimals)
{
	//TODO finish FloatToString function 
	return FString::SanitizeFloat(fFloat);
}

int32 FloatToInt(float fFloat)
{
	return fFloat;
}

TArray<ADA2UE4Creature*> GetTeam(int32 nTeamId, int32 nMembersType)
{
	//TODO get team for non-creatures? is it expensive?
	TArray<ADA2UE4Creature*> _team;

	for (TActorIterator<ADA2UE4Creature> ActorItr(GetCurrentWorld()); ActorItr; ++ActorItr)
	{
		ADA2UE4Creature* oCreature = *ActorItr;
		int32 CurValue = GetTeamId(oCreature);
		if (CurValue == nTeamId)
		{
			_team.Add(oCreature);
		}
	}

	return _team;
}

void SetTeamId(AActor* aActor, int32 nTeamId)
{
	if (aActor == nullptr) return;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		oCreature->TEAM = nTeamId;
	}
#ifdef DEBUG
	else LogError("SetTeamId: unknown actor type");

#endif // DEBUG

}

int32 GetTeamId(AActor* aActor)
{
	if (aActor == nullptr) return -1;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		return oCreature->TEAM;
	}
#ifdef DEBUG
	else LogError("GetTeamId: unknown actor type");

#endif // DEBUG

	return -1;
}

AActor* GetPartyLeader()
{
	return GetActorFromName(GetModule()->PLAYER_LEADER_STORE);
}

ADA2UE4Party* GetParty(AActor* aActor)
{
	//TODO party based on creature, not nullptr
	return GetModule()->PARTY;
}

UDA2UE4GameInstance* GetModule()
{
	return GetCurrentWorld() ? Cast<UDA2UE4GameInstance>(GetCurrentWorld()->GetGameInstance()) : nullptr;
}

void SetGroupId(AActor* aActor, int32 nGroupId)
{
	if (aActor == nullptr) return;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		oCreature->GROUP = nGroupId;
	}
#ifdef DEBUG
	else LogError("SetGroupId: unknown actor type");

#endif // DEBUG

}

int32 GetGroupId(AActor* aActor)
{
	if (aActor == nullptr) return -1;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		return oCreature->GROUP;
	}
#ifdef DEBUG
	else LogError("GetGroupId: unknown actor type");

#endif // DEBUG

	return -1;
}

FString GetTag(AActor* aActor)
{
	//if (aActor == nullptr) return "";//TODO uncomment
	return aActor->GetName();
}

void RemoveEffectsByParameters(AActor* aActor, int32 nEffectType, int32 nAbilityId, AActor* oCreator, int32 bIncludeInnate)
{
	if (aActor == nullptr) return;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oTarget = Cast<ADA2UE4Creature>(aActor);
		//TODO complete RemoveEffectsByParameters function, bIncludeInnate
		for (FEffect effect : oTarget->Effects)
		{
			if (effect.nEffectType == nEffectType)
			{
				oTarget->Effects.Remove(effect);
			}
		}
	}
#ifdef DEBUG
	else LogError("RemoveEffectsByParameters: unknown actor type");

#endif // DEBUG

}

int32 GetGroupHostility(int32 nGroupA, int32 nGroupB)
{
	//TODO Dynamic GetGroupHostility, currently static
	if ((nGroupA == GROUP_PC || nGroupA == GROUP_FRIENDLY) &&
		nGroupB == GROUP_HOSTILE || nGroupB == GROUP_HOSTILE_ON_GROUND)
		return TRUE_;
	if ((nGroupA == GROUP_HOSTILE || nGroupA == GROUP_HOSTILE_ON_GROUND) &&
		(nGroupB == GROUP_PC || nGroupB == GROUP_FRIENDLY))
		return TRUE_;
	return FALSE_;
}

void SetGroupHostility(int32 nGroupA, int32 nGroupB, int32 bHostile)
{
	TArray<ADA2UE4Creature*> arGroupA = GetTeam(nGroupA, OBJECT_TYPE_CREATURE);
	TArray<ADA2UE4Creature*> arGroupB = GetTeam(nGroupB, OBJECT_TYPE_CREATURE);

	if (bHostile)
	{
		for (ADA2UE4Creature* creatureA : arGroupA)
		{
			if (!creatureA->HostilityTargetGroup.Contains(nGroupB))
				creatureA->HostilityTargetGroup.Add(nGroupB);
		}

		for (ADA2UE4Creature* creatureB : arGroupB)
		{
			if (creatureB->HostilityTargetGroup.Contains(nGroupA))
				creatureB->HostilityTargetGroup.Add(nGroupA);
		}
	}
	else
	{
		for (ADA2UE4Creature* creatureA : arGroupA)
		{
			if (creatureA->HostilityTargetGroup.Contains(nGroupB))
				creatureA->HostilityTargetGroup.Remove(nGroupB);
		}

		for (ADA2UE4Creature* creatureB : arGroupB)
		{
			if (creatureB->HostilityTargetGroup.Contains(nGroupA))
				creatureB->HostilityTargetGroup.Remove(nGroupA);
		}
	}
}

int32 IsObjectHostile(AActor* oSource, AActor* oTarget)
{
	if (oSource == nullptr || oTarget == nullptr) return FALSE_;
	if (oSource->IsA(ADA2UE4Creature::StaticClass()) && (oTarget->IsA(ADA2UE4Creature::StaticClass())))
	{
		return GetGroupHostility(Cast<ADA2UE4Creature>(oSource)->GROUP, Cast<ADA2UE4Creature>(oTarget)->GROUP);
	}
#ifdef DEBUG
	else LogError("IsObjectHostile: unknown actor type");

#endif // DEBUG

	return FALSE_;
}

AActor* GetEventObject(FGameEvent evEvent, int32 nIndex)
{
	return (evEvent.oList.Num() > nIndex) ? evEvent.oList[nIndex] : nullptr;
}

int32 GetEventInteger(FGameEvent evEvent, int32 nIndex)
{
	return (evEvent.nList.Num() > nIndex) ? evEvent.nList[nIndex] : -1;
}

AActor* GetEventCreator(FGameEvent evEvent)
{
	return evEvent.oCreator;
}

FGameEvent Event(int32 nEventType)
{
	return FGameEvent(nEventType);
}

FGameEvent SetEventCreator(FGameEvent evEvent, AActor* oCreator)
{
	FGameEvent ev;
	evEvent.oCreator = oCreator;
	ev = evEvent;
	return ev;
}

FCommand Command(int32 nCommandType)
{
	return FCommand(nCommandType);
}

float GetCommandFloat(FCommand cCommand, int32 nIndex)
{
	return (cCommand.fList.Num() > nIndex) ? cCommand.fList[nIndex] : -1.f;
}

int32 GetCommandInt(FCommand cCommand, int32 nIndex)
{
	return (cCommand.nList.Num() > nIndex) ? cCommand.nList[nIndex] : -1;
}

AActor* GetCommandObject(FCommand cCommand, int32 nIndex)
{
	return (cCommand.oList.Num() > nIndex) ? cCommand.oList[nIndex] : nullptr;
}

FVector GetCommandVector(FCommand cCommand, int32 nIndex)
{
	return (cCommand.vList.Num() > nIndex) ? cCommand.vList[nIndex] : FVector(0.f);
}

FString GetCommandString(FCommand cCommand, int32 nIndex)
{
	return (cCommand.sList.Num() > nIndex) ? cCommand.sList[nIndex] : "";
}

FCommand SetCommandFloat(FCommand cCommand, float nCommandFloat)
{
	FCommand c;
	cCommand.fList.Add(nCommandFloat);
	c = cCommand;
	return c;
}

FCommand SetCommandInt(FCommand cCommand, int32 nCommandInt)
{
	FCommand c;
	cCommand.nList.Add(nCommandInt);
	c = cCommand;
	return c;
}

FCommand SetCommandObject(FCommand cCommand, AActor* nCommandObject)
{
	FCommand c;
	cCommand.oList.Add(nCommandObject);
	c = cCommand;
	return c;
}

FCommand SetCommandVector(FCommand cCommand, FVector vVector)
{
	FCommand c;
	cCommand.vList.Add(vVector);
	c = cCommand;
	return c;
}

FCommand SetCommandString(FCommand cCommand, FString sString)
{
	FCommand c;
	cCommand.sList.Add(sString);
	c = cCommand;
	return c;
}

FEffect Effect(int32 nEffectType)
{
	return FEffect(nEffectType);
}

AActor* GetEffectCreator(FEffect eEffect)
{
	return eEffect.oCreator;
}

int32 GetEffectDurationType(FEffect eEffect)
{
	return eEffect.nDurationType;
}

void Engine_ApplyEffectOnObject(int32 nDurationType, FEffect eEffect, AActor* aTarget, float fDuration, int32 nAbilityId)
{
	if (aTarget == nullptr) return;
	if ((nDurationType == EFFECT_DURATION_TYPE_PERMANENT ||
		nDurationType == EFFECT_DURATION_TYPE_INSTANT) && fDuration > 0.f)
	{
#ifdef DEBUG
		LogError("Engine_ApplyEffectOnObject: permanent/instant with timer ?!?");

#endif // DEBUG

		return;
	}

	if (aTarget->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oTarget = Cast<ADA2UE4Creature>(aTarget);
		eEffect.nDurationType = nDurationType;
		oTarget->Effects.Add(eEffect);
		FGameEvent ev = Event(EVENT_TYPE_APPLY_EFFECT);
		ev.eEffect = eEffect;
		SignalEvent(oTarget, ev);

	}
#ifdef DEBUG
	else LogError("Engine_ApplyEffectOnObject: unknown actor type");

#endif // DEBUG

}

int32 GetEffectType(FEffect eEffect)
{
	return eEffect.nEffectType;
}

int32 GetEffectInteger(FEffect efEffect, int32 nIndex)
{
	return (efEffect.nList.Num() > nIndex) ? efEffect.nList[nIndex] : -1;
}

FEffect SetEffectInteger(FEffect efEffect, int32 nValue)
{
	FEffect ef;
	efEffect.nList.Add(nValue);
	ef = efEffect;
	return ef;
}

FEffect SetEffectCreator(FEffect efEffect, AActor* oCreator)
{
	FEffect ef;
	efEffect.oCreator = oCreator;
	ef = efEffect;
	return ef;
}

FEffect SetEffectFloat(FEffect efEffect, float fValue)
{
	FEffect ef;
	efEffect.fList.Add(fValue);
	ef = efEffect;
	return ef;
}

FEffect SetEffectObject(FEffect efEffect, AActor* aActor)
{
	FEffect ef;
	efEffect.oList.Add(aActor);
	ef = efEffect;
	return ef;
}

AActor* GetHero()
{
	//TODO return proper hero, not party member controlled
	return GetActorFromName(GetModule()->PLAYER_LEADER_STORE);
}

AActor* GetMainControlled()
{
	return GetActorFromName(GetModule()->PLAYER_LEADER_STORE);
}

FGameEvent SetEventObject(FGameEvent evEvent, AActor* oValue)
{
	FGameEvent ev;
	evEvent.oList.Add(oValue);
	ev = evEvent;
	return ev;
}

FGameEvent SetEventInteger(FGameEvent evEvent, int32 nValue)
{
	FGameEvent ev;
	evEvent.nList.Add(nValue);
	ev = evEvent;
	return ev;
}

FGameEvent SetEventFloat(FGameEvent evEvent, float fValue)
{
	FGameEvent ev;
	evEvent.fList.Add(fValue);
	ev = evEvent;
	return ev;
}

FGameEvent SetEventVector(FGameEvent evEvent, FVector vValue)
{
	FGameEvent ev;
	evEvent.vList.Add(vValue);
	ev = evEvent;
	return ev;
}

FGameEvent SetEventString(FGameEvent evEvent, FString sValue)
{
	FGameEvent ev;
	evEvent.sList.Add(sValue);
	ev = evEvent;
	return ev;
}

void SignalEvent(UObject* oObject, FGameEvent evEvent, int32 bProcessImmediate)
{
	if (oObject == nullptr) return;
	if (oObject->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(oObject);
		if (bProcessImmediate)
			oCreature->qEvents.Insert(evEvent, 0);
		else
			oCreature->qEvents.Add(evEvent);
	}
	else if (oObject->IsA(UDA2UE4GameInstance::StaticClass()))
	{
		//UDA2UE4GameInstance* module = Cast<UDA2UE4GameInstance>(aActor);
		if (bProcessImmediate)
			GetModule()->qEvents.Insert(evEvent, 0);
		else
			GetModule()->qEvents.Add(evEvent);
	}
#ifdef DEBUG
	else LogError("SignalEvent: unknown actor type");

#endif // DEBUG

}

void DelayEvent(float fSeconds, UObject* oObject, FGameEvent evEvent, FString scriptname)
{
	if (oObject == nullptr) return;
	if (oObject->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(oObject);
		evEvent.signalTime = GetCurrentWorld()->GetTimeSeconds() + fSeconds;
		oCreature->qEvents.Add(evEvent);
	}
	else if (oObject->IsA(UDA2UE4GameInstance::StaticClass()))
	{
		//UDA2UE4GameInstance* module = Cast<UDA2UE4GameInstance>(aActor);
		evEvent.signalTime = GetCurrentWorld()->GetTimeSeconds() + fSeconds;
		GetModule()->qEvents.Add(evEvent);
	}
#ifdef DEBUG
	else LogError("DelayEvent: unknown actor type");

#endif // DEBUG

}

FGameEvent GetCurrentEvent(UObject* oObject)
{
	FGameEvent ev;
	if (oObject == nullptr) return ev;

	if (oObject->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(oObject);
		if (oCreature)
		{
			if (oCreature->qEvents.Num() > 0)
			{
				for (int32 i = 0; i < oCreature->qEvents.Num(); i++)
				{
					ev = oCreature->qEvents[i];
					if (ev.signalTime == 0 ||
						GetCurrentWorld()->GetTimeSeconds() - ev.signalTime > 0)
					{
						oCreature->qEvents.RemoveAt(i);
						return ev;
					}
				}
			}
		}
	}
	else if (oObject->IsA(UDA2UE4GameInstance::StaticClass()))
	{
		//UDA2UE4GameInstance* module = Cast<UDA2UE4GameInstance>(aActor);
		if (GetModule())
		{
			if (GetModule()->qEvents.Num() > 0)
			{
				for (int32 i = 0; i < GetModule()->qEvents.Num(); i++)
				{
					ev = GetModule()->qEvents[i];
					if (ev.signalTime == 0 ||
						GetCurrentWorld()->GetTimeSeconds() - ev.signalTime > 0)
					{
						GetModule()->qEvents.RemoveAt(i);
						return ev;
					}
				}
			}
		}
	}
#ifdef DEBUG
	else LogError("GetCurrentEvent: unknown actor type");

#endif // DEBUG


	return ev;
}

int32 GetEventType(FGameEvent evEvent)
{
	return evEvent.nGameEventType;
}

FString GetEventString(FGameEvent evEvent, int32 nIndex)
{
	return (evEvent.sList.Num() > nIndex) ? evEvent.sList[nIndex] : "";
}

int32 IsFollower(AActor* aActor)
{
	if (aActor == nullptr) return FALSE_;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		return (GetModule()->PARTY->Henchman0 == FName(*aActor->GetName()) ||
			GetModule()->PARTY->Henchman1 == FName(*aActor->GetName()) ||
			GetModule()->PARTY->Henchman2 == FName(*aActor->GetName()) ||
			GetModule()->PARTY->Henchman3 == FName(*aActor->GetName())) ? TRUE_ : FALSE_;
	}
	if (aActor->IsA(ADA2UE4Trigger::StaticClass()))
	{
		return FALSE_;
	}
#ifdef DEBUG
	else LogError("IsFollower: unknown actor type");

#endif // DEBUG

	return FALSE_;
}

void LogWarning(FString sWarning)
{
	if (GetCurrentWorld())
	{
		FString sTime = FloatToString(GetCurrentWorld()->GetTimeSeconds()) + ": ";
		sWarning = sTime + sWarning;
	}
	UE_LOG(LogTemp, Warning, TEXT("%s"), *sWarning);
}

void LogError(FString sError)
{
	if (GetCurrentWorld())
	{
		FString sTime = FloatToString(GetCurrentWorld()->GetTimeSeconds()) + ": ";
		sError = sTime + sError;
	}
	UE_LOG(LogTemp, Error, TEXT("%s"), *sError);
}

FString GetCurrentScriptName(FGameEvent ev)
{
	return ev.sList[0];
}

FString GetM2DAString(int32 n2DA, FString sColumn, int32 nRow)
{
	FString json = GetJSON(n2DA);
	return GetJsonNode(json, sColumn, FString::FromInt(nRow));
}

int32 GetM2DAInt(int32 n2DA, FString sColumn, int32 nRow)
{
	FString json = GetJSON(n2DA);
	FString sInt = GetJsonNode(json, sColumn, FString::FromInt(nRow));
	return FCString::Atoi(*sInt);
}

float GetM2DAFloat(int32 n2DA, FString sColumn, int32 nRow)
{
	FString json = GetJSON(n2DA);
	FString sFloat = GetJsonNode(json, sColumn, FString::FromInt(nRow));
	return FCString::Atof(*sFloat);
}

int32 IsModalAbilityActive(AActor* aActor, int32 nAbilityId)
{
	if (aActor == nullptr) return FALSE_;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		for (FAbility a : oCreature->Abilities)
		{
			if (a.nAbilityID == nAbilityId && IsModalAbility(nAbilityId) && IsAbilityActive(aActor, nAbilityId))
				return TRUE_;
		}
		return FALSE_;
	}
#ifdef DEBUG
	else LogError("IsModalAbilityActive: unknown actor type");

#endif // DEBUG

	return FALSE_;
}

int32 IsModalAbility(int32 nAbility)
{
	return GetM2DAInt(TABLE_ABILITIES_SPELLS, "usetype", nAbility) == 2 ? TRUE_ : FALSE_;
}

int32 IsAbilityActive(AActor* aActor, int32 nAbilityID)
{
	if (aActor == nullptr) return FALSE_;
	int32 nActive = FALSE_;
	TArray<FEffect> thisEffects = GetEffects(aActor, EFFECT_TYPE_INVALID, nAbilityID);
	int32 nSize = thisEffects.Num();

#ifdef DEBUG
	LogWarning("START, abilityID: " + FString::FromInt(nAbilityID) + ", number of effects for this ability: " + FString::FromInt(nSize) + " : " + aActor->GetName());
#endif

	return (nSize > 0 ? TRUE_ : FALSE_);
}

TArray<FEffect> GetEffects(AActor* aActor, int32 nEffectType, int32 nAbilityId, AActor* oCreator, int32 nDurationType, int32 nEffectId)
{
	TArray<FEffect> effects;
	if (aActor == nullptr) return effects;

	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		for (FEffect effect : oCreature->Effects)
		{
			if (nEffectType != EFFECT_TYPE_INVALID)
			{
				if (effect.nEffectType == nEffectType)
					if (!effects.Contains(effect))
						effects.Add(effect);
			}
			else if (!effects.Contains(effect))
				effects.Add(effect);
			if (nAbilityId != 0)
			{
				if (effect.nAbilityID == nAbilityId)
					if (!effects.Contains(effect))
						effects.Add(effect);
			}
			else if (!effects.Contains(effect))
				effects.Add(effect);
			if (nDurationType != EFFECT_DURATION_TYPE_INVALID)
			{
				if (effect.nDurationType == nDurationType)
					if (!effects.Contains(effect))
						effects.Add(effect);
			}
			else if (!effects.Contains(effect))
				effects.Add(effect);
			if (nEffectId != 0)
			{
				if (effect.nEffectID == nEffectId)
					if (!effects.Contains(effect))
						effects.Add(effect);
			}
			else if (!effects.Contains(effect))
				effects.Add(effect);
		}
		return effects;
	}
#ifdef DEBUG
	else LogError("GetEffects: unknown actor type");

#endif // DEBUG

	return effects;
}

int32 GetHasEffects(AActor* aActor, int32 nEffectType, int32 nAbilityId)
{
	if (aActor == nullptr) return FALSE_;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		//TODO double check haseffect with abilityID
		for (FEffect effect : oCreature->Effects)
		{
			if (nEffectType != EFFECT_TYPE_INVALID)
			{
				if (effect.nEffectType == nEffectType)
					return TRUE_;
			}
			if (nAbilityId != -1)
			{
				if (effect.nAbilityID == nAbilityId)
					return TRUE_;
			}
		}
		return FALSE_;
	}
#ifdef DEBUG
	else LogError("GetHasEffect: unknown actor type");

#endif // DEBUG

	return FALSE_;
}

void RemoveEffect(AActor* aActor, FEffect eEffect)
{
	if (aActor == nullptr) return;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oTarget = Cast<ADA2UE4Creature>(aActor);
		if (oTarget->Effects.Contains(eEffect))
			oTarget->Effects.Remove(eEffect);
#ifdef DEBUG
		else
		{
			LogError("Trying to remove non existing effect of %s" + oTarget->GetName());
		}
#endif
	}
#ifdef DEBUG
	else LogError("RemoveEffect: unknown actor type");

#endif // DEBUG

}

void SetCombatState(AActor* aActor, int32 nCombatState, int32 nInstantEquipWeapon)
{
	//TODO SetCombatState nInstantEquipWeapon
	if (aActor == nullptr) return;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		oCreature->COMBAT_STATE = nCombatState;
	}
#ifdef DEBUG
	else LogError("SetCombatState: unknown actor type");

#endif // DEBUG

}

int32 GetCombatState(AActor* aActor)
{
	if (aActor == nullptr) return -1;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		return oCreature->COMBAT_STATE;
	}
#ifdef DEBUG
	else LogError("GetCombatState: unknown actor type");

#endif // DEBUG

	return -1;
}

int32 GetStealthEnabled(AActor* aActor)
{
	if (aActor == nullptr) return -1;
	return GetHasEffects(aActor, EFFECT_TYPE_STEALTH);
}

int32 GetAttributeBool(FString sAttribute)
{
	//TODO implement GetAttributeBool
	return TRUE_;
}

int32 GetDebugHelpersEnabled()
{
	//TODO implement GetDebugHelpersEnabled
	return TRUE_;
}

FString GetStringByStringId(int32 nId)
{
	return GetM2DAString(TABLE_TALK, "String", nId);
}

void DisplayFloatyMessage(AActor* aActor, FString sMessage, int32 nStyle, int32 nColor, float fDuration)
{
	if (aActor == nullptr) return;
	GetCurrentPlayerController()->CreateWidgetFloaty(aActor, sMessage);
}

int32 HasAbility(AActor* aActor, int32 nAbility)
{
	if (aActor == nullptr) return FALSE_;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		for (FAbility a : oCreature->Abilities)
		{
			if (a.nAbilityID == nAbility) return TRUE_;
		}
		return FALSE_;
	}
#ifdef DEBUG
	else LogError("HasAbility: unknown actor type");

#endif // DEBUG

	return FALSE_;
}

int32 GetCreatureRank(AActor* aActor)
{
	if (aActor == nullptr) return -1;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		return oCreature->RANK;
	}
#ifdef DEBUG
	else LogError("GetCreatureRank: unknown actor type");

#endif // DEBUG

	return -1;
}

int32 IsControlled(AActor* aActor)
{
	if (aActor == nullptr) return FALSE_;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		//TODO return party members controlled
		return (aActor == GetHero() ? TRUE_ : FALSE_);
	}
#ifdef DEBUG
	else LogError("IsControlled: unknown actor type");

#endif // DEBUG

	return FALSE_;
}

int32 GetObjectType(AActor* aActor)
{
	if (aActor == nullptr) return OBJECT_TYPE_INVALID;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		return OBJECT_TYPE_CREATURE;
	}
#ifdef DEBUG
	else LogError("GetObjectType: unknown actor type");

#endif // DEBUG

	return OBJECT_TYPE_INVALID;
}

void DisplayPortraitMessage(AActor* oPlayerCreature, FString sMessage, int32 nColor)
{
	//TODO implement DisplayPortraitMessage
	LogWarning("NOT IMPLEMENTED: DisplayPortraitMessage");
}

int32 GetThreatTableSize(AActor* aActor)
{
	if (aActor == nullptr) return 0;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		return oCreature->Threats.Num();
	}
#ifdef DEBUG
	else LogError("GetThreatTableSize: unknown actor type");

#endif // DEBUG

	return 0;
}

AActor* GetThreatEnemy(AActor* aActor, int32 i)
{
	if (aActor == nullptr) return nullptr;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		return (i < oCreature->Threats.Num()) ? oCreature->Threats[i].oThreat : nullptr;
	}
#ifdef DEBUG
	else LogError("GetThreatEnemy: unknown actor type");

#endif // DEBUG

	return nullptr;
}

float GetThreatValueByIndex(AActor* aActor, int32 i)
{
	if (aActor == nullptr) return 0.f;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		return (i < oCreature->Threats.Num()) ? oCreature->Threats[i].fThreat : 0.f;
	}
#ifdef DEBUG
	else LogError("GetThreatValueByIndex: unknown actor type");

#endif // DEBUG

	return 0.f;
}

float GetThreatValueByObjectID(AActor* aActor, AActor* aEnemy)
{
	if (aActor == nullptr || aEnemy == nullptr) return 0.f;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()) &&
		aEnemy->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		ADA2UE4Creature* oEnemy = Cast<ADA2UE4Creature>(aEnemy);
		for (FThreat threat : oCreature->Threats)
		{
			if (threat.oThreat == oEnemy)
			{
				return threat.fThreat;
			}
		}
	}
#ifdef DEBUG
	else LogError("GetThreatValueByObjectID: unknown actor type");

#endif // DEBUG

	return 0.0f;
}

void UpdateThreatTable(AActor* aActor, AActor* aEnemy, float fThreatChange)
{
	if (aActor == nullptr || aEnemy == nullptr) return;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()) &&
		aEnemy->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		ADA2UE4Creature* oEnemy = Cast<ADA2UE4Creature>(aEnemy);
		for (FThreat threat : oCreature->Threats)
		{
			if (threat.oThreat == oEnemy)
			{
				threat.fThreat += fThreatChange;
			}
		}
	}
#ifdef DEBUG
	else LogError("UpdateThreatTable: unknown actor type");

#endif // DEBUG

}

void ClearEnemyThreat(AActor* aActor, AActor* aEnemy)
{
	if (aActor == nullptr || aEnemy == nullptr) return;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()) &&
		aEnemy->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		ADA2UE4Creature* oEnemy = Cast<ADA2UE4Creature>(aEnemy);
		for (FThreat threat : oCreature->Threats)
		{
			if (threat.oThreat == oEnemy)
			{
				oCreature->Threats.Remove(threat);
				break;
			}
		}
	}
#ifdef DEBUG
	else LogError("ClearEnemyThreat: unknown actor type");

#endif // DEBUG

}

int32 IsDead(AActor* aActor)
{
	if (aActor == nullptr) return -1;
	//TODO dead flag on creature?
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		//ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		if (GetCreatureProperty(aActor, PROPERTY_DEPLETABLE_HEALTH) <= 0)
		{
			return TRUE_;
		}
	}
#ifdef DEBUG
	else LogError("IsDead: unknown actor type");

#endif // DEBUG

	return -1;
}

int32 HasDeathEffect(AActor* aActor, int32 bCheckForDeathEvent)
{
	if (aActor == nullptr) return -1;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		for (FEffect effect : oCreature->Effects)
		{
			if (effect.nEffectType == EVENT_TYPE_DEATH)
			{
				return TRUE_;
			}
		}
	}
#ifdef DEBUG
	else LogError("HasDeathEffect: unknown actor type");

#endif // DEBUG

	return -1;
}

int32 IsPerceiving(AActor* oidA, AActor* oidB)
{
	if (oidA == nullptr || oidB == nullptr) return FALSE_;
	if (oidA->IsA(ADA2UE4Creature::StaticClass()) &&
		oidB->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(oidA);
		APawn* perceived = Cast<APawn>(oidB);
		for (APawn* pawn : oCreature->SensedPawns)
		{
			if (pawn == perceived)
			{
				return TRUE_;
			}
		}
	}
#ifdef DEBUG
	else LogError("IsPerceiving: unknown actor type");

#endif // DEBUG

	return FALSE_;
}

int32 GetCreatureGender(AActor* aActor)
{
	if (aActor == nullptr) return -1;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		return oCreature->GENDER;
	}
#ifdef DEBUG
	else LogError("GetCreatureGender: unknown actor type");

#endif // DEBUG

	return -1;
}

int32 GetCreatureRacialType(AActor* aActor)
{
	if (aActor == nullptr) return -1;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		return oCreature->RACE;
	}
#ifdef DEBUG
	else LogError("GetCreatureRacialType: unknown actor type");

#endif // DEBUG

	return -1;
}

float GetCreatureProperty(AActor* aActor, int32 nProperty, int32 nValueType)
{
	if (aActor == nullptr) return 0.f;
	//TODO attribute vs depletable vs simple vs derived?
	FActorProperty property = GetProperty(aActor, nProperty);

	if (property.nPropertyID != PROPERTY_INVALID)
	{
		if (nValueType == PROPERTY_VALUE_TOTAL) return property.fValueTotal;
		else if (nValueType == PROPERTY_VALUE_BASE) return property.fValueBase;
		else if (nValueType == PROPERTY_VALUE_MODIFIER) return property.fValueModifier;
		else if (nValueType == PROPERTY_VALUE_CURRENT) return property.fValueCurrent;
#ifdef DEBUG
		else LogError("Unknown property value type!!!");

#endif // DEBUG

	}
#ifdef DEBUG
	LogError("Unknown property!!!");

#endif // DEBUG

	return 0.f;
}

void SetCreatureProperty(AActor* aActor, int32 nProperty, float fNewValue, int32 nValueType)
{
	if (aActor == nullptr) return;
	//FString sProperty = GetM2DAString(TABLE_PROPERTIES, "Stat", nProperty);
	FActorProperty property = GetProperty(aActor, nProperty);

	if (nValueType == PROPERTY_VALUE_TOTAL) property.fValueTotal = fNewValue;
	else if (nValueType == PROPERTY_VALUE_BASE) property.fValueBase = fNewValue;
	else if (nValueType == PROPERTY_VALUE_MODIFIER) property.fValueModifier = fNewValue;
	else if (nValueType == PROPERTY_VALUE_CURRENT) property.fValueCurrent = fNewValue;
#ifdef DEBUG
	else LogError("Unknown property value type!!!");

#endif // DEBUG

}

FActorProperty GetProperty(AActor* aActor, int32 nProperty)
{
	if (aActor == nullptr) return FActorProperty(PROPERTY_INVALID);
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);

		switch (nProperty)
		{
		case PROPERTY_DEPLETABLE_HEALTH: return oCreature->Health;
		case PROPERTY_DEPLETABLE_MANA_STAMINA: return oCreature->Mana_Stamina;
		case PROPERTY_ATTRIBUTE_FATIGUE: return oCreature->Fatigue;
		case PROPERTY_SIMPLE_LEVEL: return oCreature->Level;
		case PROPERTY_SIMPLE_CURRENT_CLASS: return oCreature->CurrentClass;
		default:
#ifdef DEBUG
			LogError("Unknown property " + FString::FromInt(nProperty));

#endif // DEBUG

			return FActorProperty(PROPERTY_INVALID);
		}
	}
	return FActorProperty(PROPERTY_INVALID);
}

int32 IsObjectValid(AActor* aActor)
{
	if (aActor == nullptr) return FALSE_;
	return aActor->IsValidLowLevel() ? TRUE_ : FALSE_;
}

int32 GetTime()
{
	return GetCurrentWorld()->GetTimeSeconds();
}

int32 Random(int32 nMaxInteger)
{
	return FMath::RandRange(0, nMaxInteger);
}

float RandomFloat()
{
	return FMath::FRand();
}

AActor* GetItemInEquipSlot(int32 nSlot, AActor* aActor, int32 nWeaponSet)
{
	AActor* item = nullptr;
	if (aActor == nullptr) return item;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		int32 wSet = 0; //main
		if (nWeaponSet == INVALID_WEAPON_SET) //the current/active weapon set is queried
		{
			wSet = oCreature->nActiveWeaponSet;
		}
		else wSet = nWeaponSet;
		if (wSet == 0) //main
		{
			if (oCreature->INVENTORY_SLOTS.Contains(nSlot))
				item = *oCreature->INVENTORY_SLOTS.Find(nSlot);
		}
		else if (wSet == 1)//secondary
		{
			//offset main, offhand, ammo slots with 100
			if (nSlot == INVENTORY_SLOT_MAIN ||
				nSlot == INVENTORY_SLOT_OFFHAND ||
				nSlot == INVENTORY_SLOT_RANGEDAMMO)
				nSlot += 100;
			if (oCreature->INVENTORY_SLOTS.Contains(nSlot))
				item = *oCreature->INVENTORY_SLOTS.Find(nSlot);
		}
		else
		{
#ifdef DEBUG
			LogError("Inventory slots can be only 0 and 1, got " + wSet);

#endif // DEBUG

			return nullptr;
		}
		return item;
	}
#ifdef DEBUG
	else LogError("GetItemInEquipSlot: unknown actor type");

#endif // DEBUG

	return item; //nullptr
}

int32 GetBaseItemType(AActor* aActor)
{
	if (aActor == nullptr) return FALSE_;
	//ID aka BaseType ?!?
	if (aActor->IsA(ADA2UE4Item::StaticClass()))
	{
		ADA2UE4Item* oItem = Cast<ADA2UE4Item>(aActor);
		return GetM2DAInt(TABLE_ITEMS, "ID", oItem->BaseItemType);
	}
#ifdef DEBUG
	else LogError("GetBaseItemType: unknown actor type");

#endif // DEBUG

	return FALSE_;
}

int32 IsPerceivingHostiles(AActor* aActor)
{
	if (aActor == nullptr) return FALSE_;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		for (APawn* pawn : oCreature->SensedPawns)
		{
			if (IsObjectHostile(oCreature, Cast<AActor>(pawn)))
				return TRUE_;
		}
	}
#ifdef DEBUG
	else LogError("IsPerceivingHostiles: unknown actor type");

#endif // DEBUG

	return FALSE_;
}

int32 IsPartyPerceivingHostiles(AActor* aActor)
{
	if (aActor == nullptr) return FALSE_;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		if (oCreature->TEAM <= 0)
		{
#ifdef DEBUG
			Log_Trace(LOG_CHANNEL_SYSTEMS, "ldf.h : IsPartyPerceivingHostiles", "Invalid Team ID");

#endif // DEBUG

			return FALSE_;
		}

		TArray<ADA2UE4Creature*> arNewList;

		arNewList = GetTeam(oCreature->TEAM, OBJECT_TYPE_CREATURE);

		for (ADA2UE4Creature* teamMember : arNewList)
		{
			if (IsPerceivingHostiles(teamMember)) return TRUE_;
		}
	}
#ifdef DEBUG
	else LogError("IsPartyPerceivingHostiles: unknown actor type");

#endif // DEBUG

	return FALSE_;
}

TArray<AActor*> GetPartyList(AActor* aActor)
{
	TArray<AActor*> party;

	if (aActor == nullptr) return party;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		if (!GetModule()->PARTY->Henchman0.IsNone())
			party.Add(GetActorFromName(GetModule()->PARTY->Henchman0));
		if (!GetModule()->PARTY->Henchman1.IsNone())
			party.Add(GetActorFromName(GetModule()->PARTY->Henchman1));
		if (!GetModule()->PARTY->Henchman2.IsNone())
			party.Add(GetActorFromName(GetModule()->PARTY->Henchman2));
		if (!GetModule()->PARTY->Henchman3.IsNone())
			party.Add(GetActorFromName(GetModule()->PARTY->Henchman3));
		return party;
	}
#ifdef DEBUG
	else LogError("GetPartyList: unknown actor type");

#endif // DEBUG

	return party;
}

int32 GetObjectActive(AActor* aActor)
{
	if (aActor == nullptr) return 0;
	//TODO get object active custom in creature class
	/*SetActorHiddenInGame(!active);
	SetActorEnableCollision(active);
	SetActorTickEnabled(active);*/
#ifdef DEBUG
	bool b = (!(aActor->bHidden) && aActor->GetActorEnableCollision() && aActor->IsActorTickEnabled());
	FString s;
	if (b) s = "true"; else s = "false";
	LogWarning("actor " + aActor->GetName() + " is active: " + s);
#endif // DEBUG

	return !(aActor->bHidden) && aActor->GetActorEnableCollision() && aActor->IsActorTickEnabled();
}

void SetObjectActive(AActor* aActor, int32 nActive, int32 nAnimation, int32 nVFX, int32 nNextLine)
{
	if (aActor == nullptr) return;
	bool bActive;
	if (nActive == TRUE_)
		bActive = true;
	else bActive = false;

	//TODO SetObjectActive nAnimation, nVFX, nNextLine
	aActor->SetActorHiddenInGame(!bActive);
	aActor->SetActorEnableCollision(bActive);
	aActor->SetActorTickEnabled(bActive);
}

EGameMode GetGameMode()
{
	return GetModule()->GAME_MODE;
}

void SetGameMode(int32 nMode)
{
	FGameEvent ev = Event(EVENT_TYPE_GAMEMODE_CHANGE);
	ev = SetEventInteger(ev, nMode);//New desired game mode
	ev = SetEventInteger(ev, static_cast<uint8>(GetGameMode()));//Current-old Game mode
	SignalEvent(GetModule(), ev);

	switch (nMode)
	{
	case GM_EXPLORE:
	{
		GetModule()->GAME_MODE = EGameMode::GM_EXPLORE;
		break;
	}
	case GM_COMBAT:
	{
		GetModule()->GAME_MODE = EGameMode::GM_COMBAT;
		break;
	}
	case GM_CONVERSATION:
	{
		GetModule()->GAME_MODE = EGameMode::GM_CONVERSATION;
		break;
	}
	default:
#ifdef DEBUG
		LogError("SetGameMode: unknown mode!");

#endif
		break;
	}
}

int32 IsImmortal(AActor* aActor)
{
	if (aActor == nullptr) return FALSE_;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		return oCreature->bImmortal;
	}
	if (aActor->IsA(ADA2UE4Trigger::StaticClass()))
	{
		return FALSE_;
	}
#ifdef DEBUG
	else LogError("IsImmortal: unknown actor type");

#endif // DEBUG

	return FALSE_;
}

int32 IsPlot(AActor* aActor)
{
	if (aActor == nullptr) return FALSE_;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		return oCreature->bPlot;
	}
	if (aActor->IsA(ADA2UE4Trigger::StaticClass()))
	{
		return FALSE_;
	}
#ifdef DEBUG
	else LogError("IsPlot: unknown actor type");

#endif // DEBUG
	return FALSE_;
}

void PlaySoundSet(AActor* oTarget, int32 nSoundSetEntry, float fProbabilityOverride)
{
	if (oTarget == nullptr) return;
	FString sound = GetM2DAString(TABLE_SOUNDSETS, "NODENAME", nSoundSetEntry);
#ifdef DEBUG
	LogWarning("Playing SoundSet: " + sound);
#endif // DEBUG
}

int32 GetAppearanceType(AActor* aActor)
{
	if (aActor == nullptr) return FALSE_;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		int32 nRace = oCreature->RACE;
		return GetM2DAInt(TABLE_RULES_RACES, "Appearance", nRace);
	}
#ifdef DEBUG
	else LogError("GetAppearanceType: unknown actor type");

#endif // DEBUG
	return FALSE_;
}

int32 GetFollowerApproval(AActor* aActor)
{
	if (aActor == nullptr) return FALSE_;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		if (oCreature->bApproval)
		{
			return oCreature->APPROVAL;
		}
		else return 0; //not permanent follower plot/story related
	}
#ifdef DEBUG
	else LogError("GetFollowerApproval: unknown actor type");

#endif // DEBUG
	return FALSE_;
}

TArray<AActor*> GetPerceivedCreatureList(AActor* aActor, int32 bHostile)
{
	TArray<AActor*> aArray;
	if (aActor == nullptr) return aArray;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		for (APawn* pawn : oCreature->SensedPawns)
		{
			if (bHostile)
			{
				if (IsObjectHostile(oCreature, pawn))	aArray.Add(pawn);
			}
			else aArray.Add(pawn);
		}
	}
#ifdef DEBUG
	else LogError("GetPerceivedCreatureList: unknown actor type");

#endif // DEBUG
	return aArray;
}

int32 GetFollowerState(AActor* aActor)
{
	if (aActor == nullptr) return -1;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		return oCreature->FOLLOWER_STATE;
	}
#ifdef DEBUG
	else LogError("GetFollowerState: unknown actor type");

#endif // DEBUG
	return -1;
}

void SetPlaceableHealth(AActor* oPlc, int32 nHealth)
{
	//TODO implement SetPlaceableHealth
	LogError("SetPlaceableHealth to be implemented");
}

void SetAILevel(AActor* aActor, int32 nAILevel)
{
	if (aActor == nullptr) return;
	//TODO implement SetAILevel
	LogError("SetAILevel to be implemented");
}

int32 ClearAmbientDialogs(AActor* aActor)
{
	//TODO implement ClearAmbientDialogs fully
	if (aActor == nullptr)
	{
#ifdef DEBUG
		LogError("ClearAmbientDialogs to be implemented for all creatures if nullptr");

#endif // DEBUG
		return 0;
	}
	else if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		if (oCreature->AMB_SYSTEM_DIALOG != 0)
		{
			//nResult = TRUE_;
			oCreature->AMB_SYSTEM_DIALOG = 0;
		}
	}
	else if (aActor->IsA(UDA2UE4GameInstance::StaticClass()))
	{
		//TODO this makes sure an already running ambient dialog triggers its plot flag action and look at gen00pt_ambient_ai
		UDA2UE4GameInstance* module = Cast<UDA2UE4GameInstance>(aActor);
		if (module->AMB_SYSTEM_DIALOG != 0)
		{
			module->AMB_SYSTEM_DIALOG = 0;
		}
	}
#ifdef DEBUG
	else LogError("ClearAmbientDialogs: unknown actor type");

#endif // DEBUG
	return 0;
}

int32 GetGameDifficulty()
{
	return GetGameSettings(SETTING_GAME_DIFFICULTY);
}

int32 GetPackage(AActor* aActor)
{
	if (aActor == nullptr) return -1;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		return oCreature->PACKAGE;
	}
#ifdef DEBUG
	else LogError("GetPackage: unknown actor type");

#endif // DEBUG
	return -1;
}

int32 GetPackageAI(AActor* aActor)
{
	if (aActor == nullptr) return -1;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		return oCreature->PACKAGEAI;
	}
#ifdef DEBUG
	else LogError("GetPackageAI: unknown actor type");

#endif // DEBUG
	return -1;
}

int32 ClearAllCommands(AActor* aActor, int32 nHardClear)
{
	if (aActor == nullptr) return FALSE_;
	if (!nHardClear)
	{
#ifdef DEBUG
		LogError("ClearAllCommands: nHardClear not implemented");

#endif // DEBUG
		return 0;
	}
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		oCreature->qCommands.Empty();
		oCreature->currentCommand = FCommand(COMMAND_TYPE_INVALID);
	}
#ifdef DEBUG
	else LogError("ClearAllCommands: unknown actor type");

#endif // DEBUG
	return 0;
}

float GetDistanceBetween(AActor* aActorA, AActor* aActorB, int32 bSubtractPersonalSpace)
{
	//TODO implement GetDistanceBetween with bSubtractPersonalSpace
	if (aActorA == nullptr || aActorB == nullptr) return -1.0f;
	return (aActorA->GetActorLocation() - aActorB->GetActorLocation()).Size();
}

float GetDistanceBetweenLocations(FVector vVectorA, FVector vVectorB)
{
	return (vVectorA - vVectorB).Size();
}

int32 GetCombatantType(AActor* aActor)
{
	if (aActor == nullptr) return -1;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		return oCreature->COMBATANT_TYPE;
	}
#ifdef DEBUG
	else LogError("GetCombatantType: unknown actor type");

#endif // DEBUG
	return -1;
}

TArray<AActor*> GetNearestObjectByGroup(AActor* aActor, int32 nGroupId, int32 nObjectType, int32 nNumberOfObjects, int32 nCheckLiving, int32 nCheckPerceived, int32 nIncludeSelf)
{
	//TODO GetNearestObjectByGroup incorrect sorting, but assumes few actors in the sensing radius anyway
	TArray<AActor*> aArray;
	if (aActor == nullptr) return aArray;
	if (nObjectType != OBJECT_TYPE_CREATURE)
	{
#ifdef DEBUG
		LogError("GetNearestObjectByGroup: only creatures are considered");

#endif // DEBUG
		return aArray;
	}
	if (nCheckLiving != TRUE_)
	{
#ifdef DEBUG
		LogError("GetNearestObjectByGroup: why looking for dead people?");

#endif // DEBUG
		return aArray;
	}
	if (nCheckPerceived != TRUE_)
	{
#ifdef DEBUG
		LogError("GetNearestObjectByGroup: why looking for unsensed pawns?");

#endif // DEBUG
		return aArray;
	}
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		if (nIncludeSelf == TRUE_) aArray.Add(Cast<APawn>(aActor));
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);

		TArray<APawn*> sensedArray;

		for (APawn* pawn : oCreature->SensedPawns)
		{
			if (GetGroupId(pawn) == nGroupId)
				sensedArray.Add(pawn);
		}

		struct FCompareSensedPawnsLocation
		{
			FORCEINLINE bool operator()(const AActor& A, const AActor& B) const
			{
				//TODO doublecheck that FCompareSensedPawnsLocation returns the closest sorted
				return (A.GetActorLocation() - B.GetActorLocation()).Size() < 0;
			}
		};

		sensedArray.Sort(FCompareSensedPawnsLocation());

		nNumberOfObjects = nNumberOfObjects < sensedArray.Num() ? nNumberOfObjects : sensedArray.Num();

		for (int32 i = 0; i < nNumberOfObjects; i++)
		{
			aArray.Add(sensedArray[i]);
		}

		return aArray;
	}
#ifdef DEBUG
	else LogError("GetNearestObjectByGroup: unknown actor type");

#endif // DEBUG
	return aArray;
}

TArray<AActor*> GetNearestObjectByHostility(AActor* aActor, int32 nHostility, int32 nObjectType, int32 nNumberOfObjects, int32 nCheckLiving, int32 nCheckPerceived, int32 nIncludeSelf)
{
	TArray<AActor*> aArray;
	if (aActor == nullptr) return aArray;
	if (nObjectType != OBJECT_TYPE_CREATURE)
	{
#ifdef DEBUG
		LogError("GetNearestObjectByHostility: only creatures are considered");

#endif // DEBUG
		return aArray;
	}
	if (nCheckLiving != TRUE_)
	{
#ifdef DEBUG
		LogError("GetNearestObjectByHostility: why looking for dead people?");

#endif // DEBUG
		return aArray;
	}
	if (nCheckPerceived != TRUE_)
	{
#ifdef DEBUG
		LogError("GetNearestObjectByHostility: why looking for unsensed pawns?");

#endif // DEBUG
		return aArray;
	}
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		if (nIncludeSelf == TRUE_) aArray.Add(Cast<APawn>(aActor));
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);

		TArray<APawn*> sensedArray;

		for (APawn* pawn : oCreature->SensedPawns)
		{
			if (IsObjectHostile(oCreature, pawn))
				sensedArray.Add(pawn);
		}

		struct FCompareSensedPawnsLocation
		{
			FORCEINLINE bool operator()(const AActor& A, const AActor& B) const
			{
				return (A.GetActorLocation() - B.GetActorLocation()).Size() < 0;
			}
		};

		sensedArray.Sort(FCompareSensedPawnsLocation());

		nNumberOfObjects = nNumberOfObjects < sensedArray.Num() ? nNumberOfObjects : sensedArray.Num();

		for (int32 i = 0; i < nNumberOfObjects; i++)
		{
			aArray.Add(sensedArray[i]);
		}

		return aArray;
	}
#ifdef DEBUG
	else LogError("GetNearestObjectByHostility: unknown actor type");

#endif // DEBUG
	return aArray;
}

TArray<AActor*> GetNearestObjectToLocation(FVector vVector, int32 nObjectType, int32 nNumberOfObjects)
{
	//TODO GetNearestObjectToLocation return more than 1 object. Also may be slow and expensive
	TArray<AActor*> aArray;
	TArray<AActor*> tempArray;
	if (nObjectType != OBJECT_TYPE_CREATURE)
	{
#ifdef DEBUG
		LogError("GetNearestObjectToLocation: only creatures are considered");

#endif // DEBUG
		return aArray;
	}
	if (nNumberOfObjects != 1)
	{
#ifdef DEBUG
		LogError("GetNearestObjectToLocation: why more than 1?");

#endif // DEBUG
		return aArray;
	}
	for (TActorIterator<ADA2UE4Creature> ActorItr(GetCurrentWorld()); ActorItr; ++ActorItr)
	{
		tempArray.Add(*ActorItr);
	}

	AActor* tmp = NewObject<AActor>();
	tmp->SetActorLocation(vVector);
	tempArray.Add(tmp);

	for (int32 i = 0; i < tempArray.Num(); i++)
	{
		for (int32 j = 0; j < tempArray.Num() - i; j++)
		{
			if ((tempArray[j]->GetActorLocation() - tempArray[j + 1]->GetActorLocation()).Size() > 0)
			{
				tmp = tempArray[j];
				tempArray[j] = tempArray[j + 1];
				tempArray[j + 1] = tmp;
			}
		}
	}

	for (int32 i = 0; i < tempArray.Num(); i++)
	{
		if (tempArray[i]->GetActorLocation() == vVector)
		{
			if (i > 0 /*it's not the first one*/)
			{
				aArray.Add(tempArray[i - 1]);
				break;
			}
			else if (tempArray.Num() > 1 /*has at least one more actor in array*/)
			{
				aArray.Add(tempArray[i + 1]);
				break;
			}
#ifdef DEBUG
			else LogError("a world with no actors?");

#endif // DEBUG
		}
	}

	tempArray.Empty();
	tmp->Destroy();

	return aArray;
}

TArray<AActor*> GetNearestObject(AActor* aActor, int32 nObjectType, int32 nNumberOfObjects, int32 nCheckLiving, int32 nCheckPerceived, int32 nIncludeSelf)
{
	TArray<AActor*> aArray;
	if (aActor == nullptr) return aArray;
	if (nObjectType != OBJECT_TYPE_PLACEABLE)
	{
#ifdef DEBUG
		LogError("GetNearestObject: only Placeables are considered");

#endif // DEBUG
		return aArray;
	}
	//TODO GetNearestObject to be implemented
#ifdef DEBUG
	LogError("GetNearestObject: to be implemented!!");

#endif // DEBUG
	return aArray;
}

TArray<AActor*> GetNearestObjectByTag(AActor* aActor, FString sTag, int32 nObjectType, int32 nNumberOfObjects, int32 nCheckLiving, int32 nCheckPerceived, int32 nIncludeSelf)
{
	TArray<AActor*> aArray;
	if (aActor == nullptr) return aArray;
	if (nObjectType != OBJECT_TYPE_WAYPOINT)
	{
#ifdef DEBUG
		LogError("GetNearestObjectByTag: only waypoints are considered for now");

#endif // DEBUG
		return aArray;
	}
	if (nNumberOfObjects != 1)
	{
#ifdef DEBUG
		LogError("GetNearestObjectByTag: why more than 1?");

#endif // DEBUG
		return aArray;
	}
	for (TActorIterator<ADA2UE4TargetPoint> ActorItr(GetCurrentWorld()); ActorItr; ++ActorItr)
	{
		ADA2UE4TargetPoint* waypoint = *ActorItr;
		if (waypoint->GetName() == sTag)
			aArray.Add(*ActorItr);
	}

	return aArray;
}

int32 GetEffectAbilityID(FEffect efEffect)
{
	return efEffect.nAbilityID;
}

int32 GetEffectsFlags(AActor* aActor)
{
	if (aActor == nullptr) return FALSE_;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		return oCreature->EffectFlags;
	}
#ifdef DEBUG
	else LogError("GetEffectsFlags: unknown actor type");

#endif // DEBUG
	return 0; //error
}

int32 GetObjectInteractive(AActor* aActor)
{
	if (aActor == nullptr) return FALSE_;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		return oCreature->bInteractive;
	}
#ifdef DEBUG
	else LogError("GetObjectInteractive: unknown actor type");

#endif // DEBUG
	return FALSE_;
}

void SetObjectInteractive(AActor* aActor, int32 nValue)
{
	if (aActor == nullptr) return;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		oCreature->bInteractive = nValue;
	}
#ifdef DEBUG
	else LogError("SetObjectInteractive: unknown actor type");

#endif // DEBUG
}

TArray<int32> GetAbilitiesDueToAOEs(AActor* aActor)
{
	TArray<int32> aArray;
	if (aActor == nullptr) return aArray;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		for (FEffect effect : oCreature->Effects)
		{
			if (GetM2DAInt(TABLE_ABILITIES_SPELLS, "aoe_type", effect.nAbilityID) != 0)
			{
				aArray.Add(effect.nEffectType);
			}
		}
	}
#ifdef DEBUG
	else LogError("GetEffectsFlags: unknown actor type");

#endif // DEBUG
	return aArray;
}

UWorld* GetArea(AActor* aActor)
{
	if (aActor == nullptr) return nullptr;
	return aActor->GetWorld();
}

AActor* GetAttackTarget(AActor* aActor)
{
	if (aActor == nullptr) return nullptr;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		return Cast<ADA2UE4CreatureController>(oCreature->GetController())->GetActionTarget();
	}
#ifdef DEBUG
	else LogError("GetAttackTarget: unknown actor type");

#endif // DEBUG
	return nullptr;
}

void SetAttackTarget(AActor* aActor, AActor* aTarget)
{
	//TODO doublecheck SetAttackTarget null target
	if (aActor == nullptr) return;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		if (aTarget == nullptr)
			Cast<ADA2UE4CreatureController>(oCreature->GetController())->SetActionTarget(nullptr);
		else
			Cast<ADA2UE4CreatureController>(oCreature->GetController())->SetActionTarget(Cast<APawn>(aTarget));
	}
#ifdef DEBUG
	else LogError("SetAttackTarget: unknown actor type");

#endif // DEBUG
}

int32 GetActiveWeaponSet(AActor* aActor)
{
	if (aActor == nullptr) return FALSE_;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		return oCreature->nActiveWeaponSet;
	}
#ifdef DEBUG
	else LogError("GetActiveWeaponSet: unknown actor type");

#endif // DEBUG
	return 0; //error
}

AActor* GetTacticTargetObject(AActor* aActor, int32 nIndex)
{
	if (aActor == nullptr) return nullptr;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		if (oCreature->Tactics.Num() < nIndex) return nullptr;
		else return (oCreature->Tactics[nIndex].oTacticTarget);
	}
#ifdef DEBUG
	else LogError("GetTacticTargetObject: unknown actor type");

#endif // DEBUG
	return nullptr; //error
}

AActor* GetTacticConditionObject(AActor* aActor, int32 nIndex)
{
	if (aActor == nullptr) return nullptr;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		if (oCreature->Tactics.Num() < nIndex) return nullptr;
		else return (oCreature->Tactics[nIndex].oTacticCondition);
	}
#ifdef DEBUG
	else LogError("GetTacticConditionObject: unknown actor type");

#endif // DEBUG
	return nullptr; //error
}

TArray<AActor*> GetCreaturesInMeleeRing(AActor* aActor, float fStartAngle, float fEndAngle, int32 bOnlyHostiles, int32 nRingIndex)
{
	TArray<AActor*> aArray;
	if (aActor == nullptr) return aArray;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);

		for (APawn* pawn : oCreature->SensedPawns)
		{
			if (GetDistanceBetween(oCreature, pawn) < AI_MELEE_RANGE*SCALE_MULTIPLIER)
			{
				if (bOnlyHostiles)
				{
					if (IsObjectHostile(oCreature, pawn))
						aArray.Add(pawn);
				}
				else aArray.Add(pawn);
			}
		}
		return aArray;
	}
#ifdef DEBUG
	else LogError("GetCreaturesInMeleeRing: unknown actor type");

#endif // DEBUG
	return aArray;
}

TArray<AActor*> GetCreaturesInCone(AActor* aActor, int32 bOnlyHostiles)
{
	//TODO GetCreaturesInCone currently uses Sensed Pawns so it may be too big
	TArray<AActor*> aArray;
	if (aActor == nullptr) return aArray;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);

		for (APawn* pawn : oCreature->SensedPawns)
		{
			if (bOnlyHostiles)
			{
				if (IsObjectHostile(oCreature, pawn))
					aArray.Add(pawn);
			}
			else aArray.Add(pawn);
		}
		return aArray;
	}
#ifdef DEBUG
	else LogError("GetCreaturesInCone: unknown actor type");

#endif // DEBUG
	return aArray;
}

FVector GetLocation(AActor* aActor)
{
	return (aActor != nullptr) ? aActor->GetActorLocation() : FVector(0.f);
}

int32 GetNumTactics(AActor* aActor)
{
	if (aActor == nullptr) return FALSE_;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		//TODO make sure current num of tactics doesn't exceed max allowed by current creature level
		//int32 level = GetCreatureProperty(oCreature, PROPERTY_SIMPLE_LEVEL);
		//int32 maxTactics = GetM2DAInt(TABLE_EXPERIENCE, "Tactics", level);
		//int32 populatedTactics = oCreature->Tactics.Num();
		return oCreature->Tactics.Num();
	}
#ifdef DEBUG
	else LogError("GetNumTactics: unknown actor type");

#endif // DEBUG
	return 0; //error
}

int32 GetM2DARows(int32 n2DA)
{
	FString json = GetJSON(n2DA);
	return GetJsonNodeCount(json);
}

int32 IsTacticEnabled(AActor* aActor, int32 nIndex)
{
	if (aActor == nullptr) return FALSE_;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		return (oCreature->Tactics.Num() > nIndex) ? oCreature->Tactics[nIndex].bEnabled : FALSE_;
	}
#ifdef DEBUG
	else LogError("IsTacticEnabled: unknown actor type");

#endif // DEBUG
	return FALSE_; //error
}

FString GetTacticCommandItemTag(AActor* aActor, int32 nIndex)
{
	if (aActor == nullptr) return "";
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		return (oCreature->Tactics.Num() > nIndex) ? oCreature->Tactics[nIndex].sTacticTag : "";
	}
#ifdef DEBUG
	else LogError("GetTacticCommandItemTag: unknown actor type");

#endif // DEBUG
	return ""; //error
}

int32 GetTacticTargetType(AActor* aActor, int32 nIndex)
{
	if (aActor == nullptr) return FALSE_;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		return (oCreature->Tactics.Num() > nIndex) ? oCreature->Tactics[nIndex].nTacticTargetType : 0;
	}
#ifdef DEBUG
	else LogError("GetTacticTargetType: unknown actor type");

#endif // DEBUG
	return 0; //error
}

int32 GetTacticCondition(AActor* aActor, int32 nIndex)
{
	if (aActor == nullptr) return FALSE_;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		return (oCreature->Tactics.Num() > nIndex) ? oCreature->Tactics[nIndex].nTacticCondition : 0;
	}
#ifdef DEBUG
	else LogError("GetTacticCondition: unknown actor type");

#endif // DEBUG
	return 0; //error
}

int32 GetTacticCommand(AActor* aActor, int32 nIndex)
{
	if (aActor == nullptr) return FALSE_;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		return (oCreature->Tactics.Num() > nIndex) ? oCreature->Tactics[nIndex].nTacticCommand : 0;
	}
#ifdef DEBUG
	else LogError("GetTacticCommand: unknown actor type");

#endif // DEBUG
	return 0; //error
}

int32 GetTacticCommandParam(AActor* aActor, int32 nIndex)
{
	if (aActor == nullptr) return FALSE_;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		return (oCreature->Tactics.Num() > nIndex) ? oCreature->Tactics[nIndex].nTacticSubCommand : 0;
	}
#ifdef DEBUG
	else LogError("GetTacticCommandParam: unknown actor type");

#endif // DEBUG
	return 0; //error
}

//TODO Items reflection?
int32 GetItemStackSize(AActor* aActor)
{
	if (aActor == nullptr) return FALSE_;
	if (aActor->IsA(ADA2UE4Item::StaticClass()))
	{
		ADA2UE4Item* oItem = Cast<ADA2UE4Item>(aActor);
		if (oItem->Stackable == 0) return 1;
		else return oItem->StackSize;
	}
#ifdef DEBUG
	else LogError("GetItemStackSize: unknown actor type");

#endif // DEBUG
	return 0; //error
}

float GetRemainingCooldown(AActor* aActor, int32 nAbilityId, FString sSourceItemTag)
{
	if (aActor == nullptr) return 0.f;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		float fCooldown = GetM2DAFloat(TABLE_ABILITIES_SPELLS, "cooldown", nAbilityId);
		for (FAbility abi : oCreature->Abilities)
		{
			if (abi.nAbilityID == nAbilityId)
			{
				if (abi.fLastUsed == 0.f) //never used
				{
					return 0.f;
				}
				else
				{
					float currentTime = GetCurrentWorld()->GetTimeSeconds();
					return (currentTime - abi.fLastUsed < fCooldown) ? currentTime - abi.fLastUsed : 0.f;
				}
			}
		}
	}
#ifdef DEBUG
	else LogError("GetRemainingCooldown: unknown actor type");

#endif // DEBUG
	return 1.f; //error: > 0

}

FVector GetClusterCenter(AActor* oCreator, int32 nAbilityId, int32 nClusterSize, int32 nAllyFailChance, int32 bReturnFirstMatch)
{
	//TODO GetClusterCenter to be implemented!!
	LogError("GetClusterCenter to be implemented!!");
	return FVector(0.f);
}

int32 GetPlaceableBaseType(AActor* oPlaceable)
{
	//TODO GetPlaceableBaseType to be implemented
	LogError("GetPlaceableBaseType to be implemented");
	return int32();
}

int32 GetPlaceableState(AActor* oPlaceable)
{
	//TODO GetPlaceableState to be implemented
	LogError("GetPlaceableState to be implemented");
	return int32();
}

float GetFacing(AActor* aActor)
{
	if (aActor == nullptr) return -1.f;
#ifdef DEBUG
	LogWarning("get facing, ensure that right is correct, instead of down, or something else");

#endif // DEBUG
	return FMath::RadiansToDegrees(FMath::Atan2(aActor->GetActorLocation().Z, aActor->GetActorLocation().X));
}

int32 IsLocationValid(FVector vVector)
{
	//TODO IsLocationValid raycast on an object on location
	return vVector != FVector(0.f) ? TRUE_ : FALSE_;
}

TArray<AActor*> GetItemsInInventory(AActor* aActor, int32 nGetItemsOptions, int32 nBaseItemType, FString sTagFilter, int32 bIgnorePlotItems)
{
	TArray<AActor*> aArray;
	if (aActor == nullptr) return aArray;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		if (nGetItemsOptions == GET_ITEMS_OPTION_EQUIPPED)
		{
			for (auto const& x : oCreature->INVENTORY_SLOTS)
			{
				aArray.Add(x.Value);
			}
			return aArray;
		}
		else if (nGetItemsOptions == GET_ITEMS_OPTION_BACKPACK) return oCreature->Inventory;
		else //GET_ITEMS_OPTION_ALL // slowest
		{
			aArray = oCreature->Inventory;
			for (auto const& x : oCreature->INVENTORY_SLOTS)
			{
				aArray.Add(x.Value);
			}
			return aArray;
		}
	}
#ifdef DEBUG
	else LogError("GetItemsInInventory: unknown actor type");

#endif // DEBUG
	return aArray; //error
}

float GetItemPropertyPower(AActor* aActor, int32 nPropertyId, int32 bIncludeSubItems, int32 bScalePower)
{
	if (aActor == nullptr) return 0.f;
	//TODO add health and mana potions to inventory
	if (aActor->IsA(ADA2UE4Item::StaticClass()))
	{
		ADA2UE4Item* oItem = Cast<ADA2UE4Item>(aActor);
		for (FItemProperty itemprp : oItem->ItemProperties)
		{
			if (itemprp.nItemPropertyID == nPropertyId)
			{
#ifdef DEBUG
				LogWarning("GetItemPropertyPower found " + IntToString(nPropertyId) + " with power: " + IntToString(itemprp.nPower));
#endif // DEBUG

				return itemprp.nPower;
			}
		}
		return 0;
	}
#ifdef DEBUG
	else LogError("GetItemPropertyPower: unknown actor type");

#endif // DEBUG
	return 0; //error
}

int32 GetItemValue(AActor* aActor)
{
	if (aActor == nullptr) return FALSE_;
	if (aActor->IsA(ADA2UE4Item::StaticClass()))
	{
		ADA2UE4Item* oItem = Cast<ADA2UE4Item>(aActor);
		int32 cost = 0;
		for (FItemProperty itemprp : oItem->ItemProperties)
		{
			cost += GetM2DAInt(TABLE_ITEMPRPS, "BaseCost", itemprp.nItemPropertyID);
		}
#ifdef DEBUG
		LogWarning("GetItemValue found " + GetTag(aActor) + " with cost value: " + IntToString(cost));
#endif // DEBUG
		return cost;
	}
#ifdef DEBUG
	else LogError("GetItemValue: unknown actor type");

#endif // DEBUG
	return 0; //error
}

int32 GetItemAbilityId(AActor* aItem)
{
	if (aItem == nullptr) return -1;
	if (aItem->IsA(ADA2UE4Item::StaticClass()))
	{
		ADA2UE4Item* oItem = Cast<ADA2UE4Item>(aItem);
		return oItem->ABILITY;
	}
#ifdef DEBUG
	else LogError("GetItemAbilityId: unknown actor type");

#endif // DEBUG
	return -1;
}

float GetAngleBetweenObjects(AActor* aActorA, AActor* aActorB)
{
	if (aActorA == nullptr || aActorB == nullptr) return -1.f;
	//TODO GetAngleBetweenObjects doublecheck
	//return FMath::Acos(FMath::Clamp(FVector::DotProduct(aActorA->GetActorLocation()->Normalize(),aActorB->GetActorLocation()->Normalize()),-1.f,1.f)* 57.29578f)
	return FMath::Atan2(aActorA->GetActorLocation().Z - aActorB->GetActorLocation().Z, aActorA->GetActorLocation().X - aActorB->GetActorLocation().X);
}

int32 IsHumanoid(AActor* aActor)
{
	if (aActor == nullptr) return -1;
	//TODO IsHumanoid implement 
	return TRUE_;
}

int32 GetWeaponStyle(AActor* aActor)
{
	if (aActor == nullptr) return 0;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);

		AActor* main = nullptr;
		AActor* off = nullptr;
		if (oCreature->INVENTORY_SLOTS.Contains(INVENTORY_SLOT_MAIN))
			main = *oCreature->INVENTORY_SLOTS.Find(INVENTORY_SLOT_MAIN);

		if (oCreature->INVENTORY_SLOTS.Contains(INVENTORY_SLOT_OFFHAND))
			off = *oCreature->INVENTORY_SLOTS.Find(INVENTORY_SLOT_OFFHAND);

		int32 t0 = GetBaseItemType(main);
		int32 t1 = GetBaseItemType(off);

		switch (t0)
		{
		case 40://battle axe
		case 5://great swords
		case 36://mauls
		case 19://short bow
		case 20://Longbow
		case 21://crossbow
		case 16://mage staff
			return WEAPONSTYLE_TWOHANDED;
		case 3://daggers
		case 49://war axe
		case 2://long swords
		case 4://Maces
		{
			if (t1 == 3 || t1 == 49 || t1 == 2 || t1 == 4)
				return WEAPONSTYLE_DUAL;
			else return WEAPONSTYLE_SINGLE;
		}
		default:
		{
#ifdef DEBUG
			LogError("GetWeaponStyle passed out :D");

#endif // DEBUG
			break;
		}
		}
	}
#ifdef DEBUG
	else LogError("GetWeaponStyle: unknown actor type");

#endif // DEBUG
	return 0; //error
}

int32 CheckLineOfSightObject(AActor* aSource, AActor* aTarget)
{
	if (aSource == nullptr || aTarget == nullptr) return FALSE_;
	if (aSource->IsA(ADA2UE4Creature::StaticClass())
		&& aTarget->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aSource);
		return oCreature->SensedPawns.Contains(Cast<APawn>(aTarget));
	}
#ifdef DEBUG
	else LogError("CheckLineOfSightObject: unknown actor type");

#endif // DEBUG
	return 0; //error
}

void SetFacingObject(AActor* aActorA, AActor* aActorB, int32 bInvert, int32 bInstant)
{
	//TODO SetFacingObject bInvert/bInstant
	if (aActorA == nullptr || aActorB == nullptr) return;
	FRotator r = UKismetMathLibrary::FindLookAtRotation(aActorA->GetActorLocation(), aActorB->GetActorLocation());
	aActorA->SetActorRotation(r);
}

int32 TriggerPerception(AActor* oPerceivingCreature, AActor* oPerceivedCreature)
{
	//TODO TriggerPerception handle dead creatures, etc
	return TRUE_;
}

void SetCreatureIsStatue(AActor* aActor, int32 value)
{
	if (aActor == nullptr) return;
	aActor->FindComponentByClass<class USkeletalMeshComponent>()->bNoSkeletonUpdate = value;
}

AActor* CreateItemOnObject(FString rItemFileName, AActor* aTarget, int32 nStackSize, FString sTag, int32 bSuppressNotification, int32 bDroppable)
{
	//TODO CreateItemOnObject dynamic based on item type, and other settings
	if (aTarget == nullptr) return nullptr;
	if (aTarget->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aTarget);

		FString sPath = "/Game/Data/Blueprints/GameObjects/Items";

		UBlueprintGeneratedClass* bpItem = LoadBlueprintFromPath<UBlueprintGeneratedClass>(FName(*sPath), FName(*rItemFileName));
		ADA2UE4Item* aItem = oCreature->GetWorld()->SpawnActor<ADA2UE4Item>(bpItem);

		ADA2UE4Item* oItem = NewObject<ADA2UE4Item>();
		oItem = aItem;

		aItem->Destroy();

		//add item to backpack
		if (oItem)	oCreature->Inventory.Add(oItem);

		return oItem;
	}
#ifdef DEBUG
	else LogError("CreateItemOnObject: unknown actor type");

#endif // DEBUG
	return nullptr;
}

void SetItemDroppable(AActor* aItem, int32 bDroppable)
{
	if (aItem == nullptr) return;
	if (aItem->IsA(ADA2UE4Item::StaticClass()))
	{
		ADA2UE4Item* oItem = Cast<ADA2UE4Item>(aItem);
		oItem->Droppable = bDroppable;
	}
#ifdef DEBUG
	else LogError("SetItemDroppable: unknown actor type");

#endif // DEBUG
	return;
}

int32 EquipItem(AActor* aActor, AActor* aItem, int32 nEquipSlot, int32 nWeaponSet)
{
	if (aActor == nullptr || aItem == nullptr) return FALSE_;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()) &&
		aItem->IsA(ADA2UE4Item::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		ADA2UE4Item* oItem = Cast<ADA2UE4Item>(aItem);

		int32 wSet = 0; //main
		if (nWeaponSet == INVALID_WEAPON_SET) //the current/active weapon set is queried
		{
			wSet = oCreature->nActiveWeaponSet;
		}
		else wSet = nWeaponSet;
		if (wSet == 0) //main
		{
			oItem->Slot = nEquipSlot;
			oCreature->INVENTORY_SLOTS.Add(nEquipSlot, oItem);
			//remove from backpack
			oCreature->Inventory.Remove(oItem);
		}
		else if (wSet == 1)//secondary
		{
			//offset main, offhand, ammo slots with 100
			if (nEquipSlot == INVENTORY_SLOT_MAIN ||
				nEquipSlot == INVENTORY_SLOT_OFFHAND ||
				nEquipSlot == INVENTORY_SLOT_RANGEDAMMO)
				nEquipSlot += 100;
			{
				oItem->Slot = nEquipSlot;
				oCreature->INVENTORY_SLOTS.Add(nEquipSlot, oItem);
				//remove from backpack
				oCreature->Inventory.Remove(oItem);
			}
		}
		else
		{
#ifdef DEBUG
			LogError("Inventory weapon sets can be only 0 and 1, got " + wSet);

#endif // DEBUG
			return FALSE_;
		}
	}
#ifdef DEBUG
	else LogError("EquipItem: unknown actor type");

#endif // DEBUG
	return FALSE_;//error
}

float GetItemStat(AActor* aItem, int32  nStatType)
{
	//TODO GetItemStat finish
	if (aItem == nullptr) return 0.f;
	if (aItem->IsA(ADA2UE4Item::StaticClass()))
	{
		ADA2UE4Item* oItem = Cast<ADA2UE4Item>(aItem);
		int32 _Type = GetBaseItemType(oItem);
		int32 _Stat = 0;
		switch (nStatType)
		{
		case ITEM_STAT_ATTACK:
		{
			_Stat = GetM2DAInt(TABLE_ITEMSTATS, "Attack", _Type);
			break;
		}
		case ITEM_STAT_ARMOR_PENETRATION:
		{
			_Stat = GetM2DAInt(TABLE_ITEMSTATS, "ArmorPenetration", _Type);
			break;
		}
		case ITEM_STAT_CRIT_CHANCE_MODIFIER:
		{
			_Stat = GetM2DAInt(TABLE_ITEMSTATS, "CritChanceModifier", _Type);
			break;
		}
		case ITEM_STAT_OPTIMUM_RANGE:
		{
			_Stat = GetM2DAInt(TABLE_ITEMSTATS, "OptimumRange", _Type) * SCALE_MULTIPLIER;
			break;
		}
		case ITEM_STAT_ATTRIBUTE_MOD:
		{
			_Stat = GetM2DAInt(TABLE_ITEMSTATS, "AttributeMod", _Type);
			break;
		}
		case ITEM_STAT_DAMAGE:
		{
			_Stat = GetM2DAInt(TABLE_ITEMSTATS, "Damage", _Type);
			break;
		}
		default:
#ifdef DEBUG
			LogError("GetItemStat: unknown stat");

#endif // DEBUG
			break;
		}
		return _Stat;
	}
#ifdef DEBUG
	else LogError("GetItemStat: unknown actor type");

#endif // DEBUG
	return 0.f;
}

int32 GetItemEquipSlot(AActor* aItem)
{
	if (aItem == nullptr) return INVENTORY_SLOT_INVALID;
	if (aItem->IsA(ADA2UE4Item::StaticClass()))
	{
		ADA2UE4Item* oItem = Cast<ADA2UE4Item>(aItem);

		return oItem->Slot;
	}
#ifdef DEBUG
	else LogError("GetItemEquipSlot: unknown actor type");

#endif // DEBUG
	return INVENTORY_SLOT_INVALID;
}

int32 GetCanDiePermanently(AActor* aActor)
{
	if (aActor == nullptr) return FALSE_;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		if (IsFollower(oCreature)) return FALSE_;

		return oCreature->NoPermDeath;
	}
#ifdef DEBUG
	else LogError("GetCanDiePermanently: unknown actor type");

#endif // DEBUG
	return FALSE_;
}

FEffect EffectImpact(float fDamage, AActor* oWeapon, int32 nVfx, int32 nAbi, int32 nDamageType)
{
	FEffect ef = Effect(EFFECT_TYPE_IMPACT);
	ef = SetEffectObject(ef, oWeapon);
	ef = SetEffectFloat(ef, fDamage);
	ef = SetEffectInteger(ef, nVfx);
	ef = SetEffectInteger(ef, nAbi);
	ef = SetEffectInteger(ef, nDamageType);

	return ef;
}

void SetAttackResult(AActor* aAttacker, int32 nResult1, FEffect eDamageEffect1, int32 nResult2, FEffect eDamageEffect2)
{
	if (aAttacker == nullptr) return;
	if (aAttacker->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oAttacker = Cast<ADA2UE4Creature>(aAttacker);
		oAttacker->AttackResults.Empty(); //reset it if not already

		FCombatAttackResultStruct main;
		main.nAttackResult = nResult1;
		main.eImpactEffect = eDamageEffect1;

		oAttacker->AttackResults.Add(main);

		FCombatAttackResultStruct off;//the other hand
		off.nAttackResult = nResult2;
		off.eImpactEffect = eDamageEffect2;

		oAttacker->AttackResults.Add(off);
	}

#ifdef DEBUG
	else LogError("SetAttackResult: unknown actor type");

#endif // DEBUG
}

void SetAimLoopDuration(AActor* aActor, float fDuration)
{
	if (aActor == nullptr) return;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		SetAttackDuration(oCreature, fDuration);
	}
#ifdef DEBUG
	else LogError("SetAimLoopDuration: unknown actor type");

#endif // DEBUG
}

void SetAttackDuration(AActor* aActor, float fDuration)
{
	if (aActor == nullptr) return;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		oCreature->AttackDuration += fDuration;
	}
#ifdef DEBUG
	else LogError("SetAttackDuration: unknown actor type");

#endif // DEBUG
}

void EnableWeaponTrail(AActor* aActor, int32 bEnable)
{
	if (aActor == nullptr) return;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		//TODO EnableWeaponTrail to be implemented
#ifdef DEBUG
		LogError("EnableWeaponTrail to be implemented!!");

#endif // DEBUG
		return;
	}
#ifdef DEBUG
	else LogError("EnableWeaponTrail: unknown actor type");

#endif // DEBUG
}

FVector GetRoamLocation(AActor* aActor)
{
	if (aActor == nullptr) return FVector(0.f);
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		return oCreature->HOME_LOCATION;
	}
#ifdef DEBUG
	else LogError("GetRoamLocation: unknown actor type");

#endif // DEBUG
	return FVector(0.f);
}

void ClearPerceptionList(AActor* aActor)
{
	if (aActor == nullptr) return;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		oCreature->SensedPawns.Empty();
		//oCreature->MeleeRingPawns.Empty();
	}
#ifdef DEBUG
	else LogError("ClearPerceptionList: unknown actor type");

#endif // DEBUG
}

AActor* GetObjectByTag(FString sTag, int32 nNth, int32 nObjectType)
{
	if (nObjectType == OBJECT_TYPE_INVALID) return nullptr;
	switch (nObjectType)
	{
	case OBJECT_TYPE_WAYPOINT:
	{
		for (TActorIterator<ADA2UE4TargetPoint> ActorItr(GetCurrentWorld()); ActorItr; ++ActorItr)
		{
			ADA2UE4TargetPoint* waypoint = *ActorItr;
			if (GetTag(waypoint) == sTag) return waypoint;
		}
		return nullptr;
	}
	default:
	{
#ifdef DEBUG
		LogError("GetObjectByTag unknown object type!!");

#endif // DEBUG
		return nullptr;
	}
	}
}

void InteractWithObject(AActor* oCreature, AActor* oTarget, int32 nInteractionId, int32 nPose, int32 nLoops, int32 bPlayExit, int32 bSkipReposition)
{
	//TODO InteractWithObject
	LogError("InteractWithObject to be implemented!!");
}

void DestroyObject(AActor* aActor, int32 nDelay)
{
#ifdef DEBUG
	if (nDelay != 0)
	{
		LogError("Why destroy with delay?");
	}
#endif // DEBUG

	aActor->Destroy();
}