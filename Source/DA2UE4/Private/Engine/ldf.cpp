//TODO cleanup //UE_LOG or turn it into #ifdef DEBUG
#include "DA2UE4.h"
#include "DA2UE4Creature.h"
#include "DA2UE4Item.h"
#include "DA2UE4Module.h"
#include "DA2UE4Library.h"
#include "DA2UE4PlayerController.h"
#include "DA2UE4TargetPoint.h"

#include "Json.h"

#include "ldf.h"
#include "events_h.h"

#include "Kismet/KismetMathLibrary.h"

/*obsolete
//#include "Engine.h"
//#include "Engine/DataTable.h"
//#include "TableCommands.h"
*/

//DHK

//HandleTable();
/*FString table = "Commands";
FString key = "Label";
HandleJson(&table, &key);*/

FString GetJSON(int32 n2DA)
{
	FString _resource;

	switch (n2DA)
	{
		//case TABLE_AREA_LOAD_HINT: _resource = "LoadHints"; break;
		//case TABLE_AREA_LOAD_HINT_VLOW: _resource = "LoadHintsVLowLevel"; break;
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

	default: LogError("Unknown Table ID requested:" + n2DA); break;
	}

	return _resource;
}

/*void ADA2UE4GameMode::HandleTable()
{
UDataTable* GameObjectLookupTable;
static ConstructorHelpers::FObjectFinder<UDataTable>
GameObjectLookupDataTable_BP(TEXT("DataTable'/Game/Data/M2da/TableCommands.TableCommands'"));
GameObjectLookupTable = GameObjectLookupDataTable_BP.Object;
FTableCommands* row;
for (auto it : GameObjectLookupTable->RowMap)
{
#ifdef DEBUG
if (it.Key == "89")
{
row = (FTableCommands*)(it.Value);
FString rLabel = row->Label;
#ifdef DEBUG
UE_LOG(LogTemp, Warning, TEXT("%s and %s"), *it.Key.ToString(), *rLabel);
#endif
break;
}
#endif
}
}*/

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
		FString keyID = "ID";
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

FString GetJsonNode(FString table, FString key, FString sRow)
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
		FString keyID = "ID";
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
					sValue = json->GetStringField(*key);
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

UWorld* GetCurrentWorld()
{
	bool IsValid;
	return UDA2UE4Library::GetDA2UE4Data(IsValid)->DA2UE4CurrentWorld;
}

ADA2UE4PlayerController* GetCurrentPlayerController()
{
	UWorld* world = GetCurrentWorld();
	ADA2UE4PlayerController* playerController = Cast<ADA2UE4PlayerController>(GetModule()->PLAYER_LEADER_STORE->GetController());
	return playerController;
}

int32 GetGameSettings(int32 nSetting)
{
	return GetM2DAInt(TABLE_GAME_SETTINGS, "Value", nSetting);
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

FString IntToHexString(int32 nInteger)
{
	//TODO int to hex string
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

FGameEvent GetLocalEvent(AActor* aActor, FName sVarName)
{
	if (aActor == nullptr) return FGameEvent(EVENT_TYPE_INVALID);
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* aCreature = Cast<ADA2UE4Creature>(aActor);

		UProperty* PropIt = aCreature->GetClass()->FindPropertyByName(sVarName);
		FGameEvent* ResolvedProperty = PropIt->ContainerPtrToValuePtr<FGameEvent>(aCreature);

		return (*ResolvedProperty);
	}
	return FGameEvent(PROPERTY_INVALID);
}

void SetLocalEvent(AActor* aActor, FName sVarName, FGameEvent evEvent)
{
	if (aActor == nullptr) return;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* aCreature = Cast<ADA2UE4Creature>(aActor);

		UProperty* PropIt = aCreature->GetClass()->FindPropertyByName(sVarName);
		FGameEvent* ResolvedProperty = PropIt->ContainerPtrToValuePtr<FGameEvent>(aCreature);

		*ResolvedProperty = evEvent;
	}
}

int32 GetLocalInt(AActor* aActor, FName sVarName)
{
	if (aActor == nullptr) return 0;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* aCreature = Cast<ADA2UE4Creature>(aActor);

		UProperty* PropIt = aCreature->GetClass()->FindPropertyByName(sVarName);
		UIntProperty *NumericProp = Cast<UIntProperty>(PropIt);

		int32 CurValue = NumericProp->GetPropertyValue_InContainer(aCreature);

		return CurValue;
	}
	else if (aActor->IsA(ADA2UE4Module::StaticClass()))
	{
		ADA2UE4Module* module = Cast<ADA2UE4Module>(aActor);

		UProperty* PropIt = module->GetClass()->FindPropertyByName(sVarName);
		UIntProperty *NumericProp = Cast<UIntProperty>(PropIt);

		int32 CurValue = NumericProp->GetPropertyValue_InContainer(module);

		return CurValue;
	}
	else if (aActor->IsA(ADA2UE4Item::StaticClass()))
	{
		ADA2UE4Item* oItem = Cast<ADA2UE4Item>(aActor);

		UProperty* PropIt = oItem->GetClass()->FindPropertyByName(sVarName);
		UIntProperty *NumericProp = Cast<UIntProperty>(PropIt);

		int32 CurValue = NumericProp->GetPropertyValue_InContainer(oItem);

		return CurValue;
	}
	else LogError("GetLocalInt: unknown actor type");
	return 0; //error
}

void SetLocalInt(AActor* aActor, FName sVarName, int32 nValue)
{
	if (aActor == nullptr) return;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* aCreature = Cast<ADA2UE4Creature>(aActor);

		UProperty* PropIt = aCreature->GetClass()->FindPropertyByName(sVarName);
		UIntProperty *NumericProp = Cast<UIntProperty>(PropIt);

		void* ValuePtr = NumericProp->ContainerPtrToValuePtr<void>(aActor);
		int64 temp64 = nValue;
		NumericProp->SetIntPropertyValue(ValuePtr, temp64);
	}
	else if (aActor->IsA(ADA2UE4Module::StaticClass()))
	{
		ADA2UE4Module* module = Cast<ADA2UE4Module>(aActor);

		UProperty* PropIt = module->GetClass()->FindPropertyByName(sVarName);
		UIntProperty *NumericProp = Cast<UIntProperty>(PropIt);

		void* ValuePtr = NumericProp->ContainerPtrToValuePtr<void>(aActor);
		int64 temp64 = nValue;
		NumericProp->SetIntPropertyValue(ValuePtr, temp64);
	}
	else LogError("SetLocalInt: unknown actor type");
}

float GetLocalFloat(AActor* aActor, FName sVarName)
{
	if (aActor == nullptr) return 0.f;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* aCreature = Cast<ADA2UE4Creature>(aActor);

		UProperty* PropIt = aCreature->GetClass()->FindPropertyByName(sVarName);
		UFloatProperty *NumericProp = Cast<UFloatProperty>(PropIt);

		float CurValue = NumericProp->GetPropertyValue_InContainer(aCreature);

		return CurValue;
	}
	else LogError("GetLocalFloat: unknown actor type");
	return 0.f; //error
}

void SetLocalFloat(AActor* aActor, FName sVarName, float fValue)
{
	if (aActor == nullptr) return;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* aCreature = Cast<ADA2UE4Creature>(aActor);

		UProperty* PropIt = aCreature->GetClass()->FindPropertyByName(sVarName);
		UFloatProperty *NumericProp = Cast<UFloatProperty>(PropIt);

		void* ValuePtr = NumericProp->ContainerPtrToValuePtr<void>(aActor);
		NumericProp->SetFloatingPointPropertyValue(ValuePtr, fValue);
	}
	else if (aActor->IsA(ADA2UE4Module::StaticClass()))
	{
		ADA2UE4Module* module = Cast<ADA2UE4Module>(aActor);

		UProperty* PropIt = module->GetClass()->FindPropertyByName(sVarName);
		UFloatProperty *NumericProp = Cast<UFloatProperty>(PropIt);

		void* ValuePtr = NumericProp->ContainerPtrToValuePtr<void>(aActor);
		NumericProp->SetFloatingPointPropertyValue(ValuePtr, fValue);
	}
	else LogError("SetLocalFloat: unknown actor type");
}

TArray<ADA2UE4Creature*> GetTeam(int32 nTeamId, int32 nMembersType)
{
	//TODO get team for non-creatures? is it expensive?
	TArray<ADA2UE4Creature*> _team;

	for (TActorIterator<ADA2UE4Creature> ActorItr(GetCurrentWorld()); ActorItr; ++ActorItr)
	{
		ADA2UE4Creature* aCreature = *ActorItr;
		int32 CurValue = GetTeamId(aCreature);
		if (CurValue == nTeamId)
		{
			_team.Add(aCreature);
		}
	}

	return _team;
}

void SetTeamId(AActor* aActor, int32 nTeamId)
{
	SetLocalInt(aActor, TEAM, nTeamId);
}

int32 GetTeamId(AActor* aActor)
{
	return GetLocalInt(aActor, TEAM);
}

AActor* GetPartyLeader()
{
	return GetModule()->PLAYER_LEADER_STORE;
}

ADA2UE4Module* GetModule()
{
	bool IsValid;
	return UDA2UE4Library::GetDA2UE4Data(IsValid)->DA2UE4CurrentModule;
}

void SetGroupId(AActor* aActor, int32 nGroupId)
{
	SetLocalInt(aActor, GROUP, nGroupId);
}

int32 GetGroupId(AActor* aActor)
{
	return GetLocalInt(aActor, GROUP);
}

FString GetTag(AActor* aActor)
{
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
	else LogError("RemoveEffectsByParameters: unknown actor type");
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
			creatureA->HostilityTargetGroup.Add(nGroupB);
		}

		for (ADA2UE4Creature* creatureB : arGroupB)
		{
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
	else LogError("IsObjectHostile: unknown actor type");
	return FALSE_;
}

void SetLocalObject(AActor* aActor, FName sVarName, AActor* oValue)
{
	if (aActor == nullptr) return;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* aCreature = Cast<ADA2UE4Creature>(aActor);

		UProperty* PropIt = aCreature->GetClass()->FindPropertyByName(sVarName);
		UObject** ResolvedObject = PropIt->ContainerPtrToValuePtr<UObject*>(aCreature);
		*ResolvedObject = oValue;
	}
	else LogError("SetLocalObject: unknown actor type");
}

AActor* GetLocalObject(AActor* aActor, FName sVarName)
{
	if (aActor == nullptr) return nullptr;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* aCreature = Cast<ADA2UE4Creature>(aActor);

		UProperty* PropIt = aCreature->GetClass()->FindPropertyByName(sVarName);
		AActor** ResolvedObject = PropIt->ContainerPtrToValuePtr<AActor*>(aCreature);

#ifdef DEBUG
		LogWarning("Got local actor " + (IsObjectValid(*ResolvedObject) ? Cast<AActor>(*ResolvedObject)->GetName() : "null") + " from actor " + aCreature->GetName());
#endif // DEBUG

		return IsObjectValid(*ResolvedObject) ? Cast<ADA2UE4Creature>(*ResolvedObject) : nullptr;
	}
	else LogError("GetLocalObject: unknown actor type");
	return nullptr;
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

FString GetLocalString(AActor* aActor, FName sVarName)
{
	if (aActor == nullptr) return "";
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* aCreature = Cast<ADA2UE4Creature>(aActor);

		UProperty* PropIt = aCreature->GetClass()->FindPropertyByName(sVarName);
		UStrProperty *NumericProp = Cast<UStrProperty>(PropIt);

		FString CurValue = NumericProp->GetPropertyValue_InContainer(aCreature);

		return CurValue;
	}
	else if (aActor->IsA(ADA2UE4Module::StaticClass()))
	{
		ADA2UE4Module* module = Cast<ADA2UE4Module>(aActor);

		UProperty* PropIt = module->GetClass()->FindPropertyByName(sVarName);
		UStrProperty *NumericProp = Cast<UStrProperty>(PropIt);

		FString CurValue = NumericProp->GetPropertyValue_InContainer(module);

		return CurValue;
	}
	else LogError("GetLocalString: unknown actor type");
	return ""; //error
}

FString GetLocalResource(AActor* aActor, FName sVarName)
{
	FString resource = GetLocalString(aActor, sVarName);
#ifdef DEBUG
	LogWarning("Resource " + resource + " was requested for " + aActor->GetName());
#endif // DEBUG
	return resource;
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

FCommand SetCommandFloat(FCommand cCommand, float nCommandFloat, int32 nIndex)
{
	FCommand c;
	cCommand.fList.Insert(nCommandFloat, nIndex);
	c = cCommand;
	return c;
}

FCommand SetCommandInt(FCommand cCommand, int32 nCommandInt, int32 nIndex)
{
	FCommand c;
	cCommand.nList.Insert(nCommandInt, nIndex);
	c = cCommand;
	return c;
}

FCommand SetCommandObject(FCommand cCommand, AActor* nCommandObject, int32 nIndex)
{
	FCommand c;
	cCommand.oList.Insert(nCommandObject, nIndex);
	c = cCommand;
	return c;
}

FCommand SetCommandVector(FCommand cCommand, FVector vVector, int32 nIndex)
{
	FCommand c;
	cCommand.vList.Insert(vVector, nIndex);
	c = cCommand;
	return c;
}

FCommand SetCommandString(FCommand cCommand, FString sString, int32 nIndex)
{
	FCommand c;
	cCommand.sList.Insert(sString, nIndex);
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
		LogError("Engine_ApplyEffectOnObject: permanent/instant with timer ?!?");
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
	else LogError("Engine_ApplyEffectOnObject: unknown actor type");
}

int32 GetEffectType(FEffect eEffect)
{
	return eEffect.nEffectType;
}

int32 GetEffectInteger(FEffect efEffect, int32 nIndex)
{
	return (efEffect.nList.Num() > nIndex) ? efEffect.nList[nIndex] : -1;
}

FEffect SetEffectInteger(FEffect efEffect, int32 nIndex, int32 nValue)
{
	FEffect ef;
	efEffect.nList.Insert(nValue, nIndex);
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

FEffect SetEffectFloat(FEffect efEffect, int32 nIndex, float fValue)
{
	FEffect ef;
	efEffect.fList.Insert(fValue, nIndex);
	ef = efEffect;
	return ef;
}

FEffect SetEffectObject(FEffect efEffect, int32 nIndex, AActor* aActor)
{
	FEffect ef;
	efEffect.oList.Insert(aActor, nIndex);
	ef = efEffect;
	return ef;
}

AActor* GetHero()
{
	//TODO return proper hero, not party member controlled
	return GetModule()->PLAYER_LEADER_STORE;
}

AActor* GetMainControlled()
{
	return GetModule()->PLAYER_LEADER_STORE;
}

FGameEvent SetEventObject(FGameEvent evEvent, int32 nIndex, AActor* oValue)
{
	FGameEvent ev;
	evEvent.oList.Insert(oValue, nIndex);
	ev = evEvent;
	return ev;
}

FGameEvent SetEventInteger(FGameEvent evEvent, int32 nIndex, int32 nValue)
{
	FGameEvent ev;
	evEvent.nList.Insert(nValue, nIndex);
	ev = evEvent;
	return ev;
}

FGameEvent SetEventFloat(FGameEvent evEvent, int32 nIndex, float fValue)
{
	FGameEvent ev;
	evEvent.fList.Insert(fValue, nIndex);
	ev = evEvent;
	return ev;
}

FGameEvent SetEventVector(FGameEvent evEvent, int32 nIndex, FVector vValue)
{
	FGameEvent ev;
	evEvent.vList.Insert(vValue, nIndex);
	ev = evEvent;
	return ev;
}

FGameEvent SetEventString(FGameEvent evEvent, int32 nIndex, FString sValue)
{
	FGameEvent ev;
	evEvent.sList.Insert(sValue, nIndex);
	ev = evEvent;
	return ev;
}

void SignalEvent(AActor* aActor, FGameEvent evEvent, int32 bProcessImmediate)
{
	if (aActor == nullptr) return;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* aCreature = Cast<ADA2UE4Creature>(aActor);
		if (bProcessImmediate)
			aCreature->qEvents.Insert(evEvent, 0);
		else
			aCreature->qEvents.Add(evEvent);
	}
	else if (aActor->IsA(ADA2UE4Module::StaticClass()))
	{
		ADA2UE4Module* module = Cast<ADA2UE4Module>(aActor);
		if (bProcessImmediate)
			module->qEvents.Insert(evEvent, 0);
		else
			module->qEvents.Add(evEvent);
	}
	else LogError("SignalEvent: unknown actor type");
}

void DelayEvent(float fSeconds, AActor* aActor, FGameEvent evEvent, FString scriptname)
{
	if (aActor == nullptr) return;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* aCreature = Cast<ADA2UE4Creature>(aActor);
		evEvent.signalTime = GetCurrentWorld()->GetTimeSeconds() + fSeconds;
		aCreature->qEvents.Add(evEvent);
	}
	else if (aActor->IsA(ADA2UE4Module::StaticClass()))
	{
		ADA2UE4Module* module = Cast<ADA2UE4Module>(aActor);
		evEvent.signalTime = GetCurrentWorld()->GetTimeSeconds() + fSeconds;
		module->qEvents.Add(evEvent);
	}
	else LogError("DelayEvent: unknown actor type");
}

FGameEvent GetCurrentEvent(AActor* aActor)
{
	FGameEvent ev;
	if (aActor == nullptr) return ev;

	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* aCreature = Cast<ADA2UE4Creature>(aActor);
		if (aCreature)
		{
			if (aCreature->qEvents.Num() > 0)
			{
				for (int32 i = 0; i < aCreature->qEvents.Num(); i++)
				{
					ev = aCreature->qEvents[i];
					if (ev.signalTime == 0 ||
						GetCurrentWorld()->GetTimeSeconds() - ev.signalTime > 0)
					{
						aCreature->qEvents.RemoveAt(i);
						return ev;
					}
				}
			}
		}
	}
	else if (aActor->IsA(ADA2UE4Module::StaticClass()))
	{
		ADA2UE4Module* module = Cast<ADA2UE4Module>(aActor);
		if (module)
		{
			if (module->qEvents.Num() > 0)
			{
				for (int32 i = 0; i < module->qEvents.Num(); i++)
				{
					ev = module->qEvents[i];
					if (ev.signalTime == 0 ||
						GetCurrentWorld()->GetTimeSeconds() - ev.signalTime > 0)
					{
						module->qEvents.RemoveAt(i);
						return ev;
					}
				}
			}
		}
	}
	else LogError("GetCurrentEvent: unknown actor type");

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

int32 IsFollower(AActor* oCreature)
{
	if (oCreature == nullptr) return FALSE_;
	if (oCreature->IsA(ADA2UE4Creature::StaticClass()))
	{
		return GetModule()->Party.Contains(Cast<ADA2UE4Creature>(oCreature)) ? TRUE_ : FALSE_;
	}
	else LogError("IsFollower: unknown actor type");
	return FALSE_;
}

void LogWarning(FString sWarning)
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *sWarning);
}

void LogError(FString sError)
{
	UE_LOG(LogTemp, Error, TEXT("%s"), *sError);
}

FString GetCurrentScriptName(FGameEvent ev)
{
	return ev.sList[0];
}

FString GetM2DAString(int32 n2DA, FString sColumn, int32 nRow, FString s2DA)
{
	FString json = GetJSON(n2DA);
	return GetJsonNode(json, sColumn, FString::FromInt(nRow));
}

int32 GetM2DAInt(int32 n2DA, FString sColumn, int32 nRow, FString s2DA)
{
	FString json = GetJSON(n2DA);
	FString sInt = GetJsonNode(json, sColumn, FString::FromInt(nRow));
	return FCString::Atoi(*sInt);
}

float GetM2DAFloat(int32 n2DA, FString sColumn, int32 nRow, FString s2DA)
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
			if (a.nAbilityID == nAbilityId && a.bSustained == TRUE_)
				return TRUE_;
		}
		return FALSE_;
	}
	else LogError("IsModalAbilityActive: unknown actor type");
	return FALSE_;
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
	else LogError("GetEffects: unknown actor type");
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
	else LogError("GetHasEffect: unknown actor type");
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
	else LogError("RemoveEffect: unknown actor type");
}


void SetCombatState(AActor* oCreature, int32 nCombatState, int32 nInstantEquipWeapon)
{
	SetLocalInt(oCreature, COMBAT_STATE, TRUE_);
}

int32 GetCombatState(AActor* oCreature)
{
	return GetLocalInt(oCreature, COMBAT_STATE);
}

int32 GetStealthEnabled(AActor* oCreature)
{
	return GetHasEffects(oCreature, EFFECT_TYPE_STEALTH);
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
	return GetM2DAString(TABLE_TALK, "tlkID", nId);
}

void DisplayFloatyMessage(AActor* oCreature, FString sMessage, int32 nStyle, int32 nColor, float fDuration)
{
	GetCurrentPlayerController()->CreateWidgetFloaty(oCreature, sMessage);
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
	else LogError("HasAbility: unknown actor type");
	return FALSE_;
}

int32 GetCreatureRank(AActor* aActor)
{
	return GetLocalInt(aActor, RANK);
}

int32 IsControlled(AActor* aActor)
{
	if (aActor == nullptr) return FALSE_;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		//TODO return party members controlled
		return (aActor == GetHero() ? TRUE_ : FALSE_);
	}
	else LogError("IsControlled: unknown actor type");
	return FALSE_;
}

int32 GetObjectType(AActor* aActor)
{
	if (aActor == nullptr) return OBJECT_TYPE_INVALID;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		return OBJECT_TYPE_CREATURE;
	}
	else LogError("GetObjectType: unknown actor type");
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
	else LogError("GetThreatTableSize: unknown actor type");
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
	else LogError("GetThreatEnemy: unknown actor type");
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
	else LogError("GetThreatValueByIndex: unknown actor type");
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
	else LogError("GetThreatValueByObjectID: unknown actor type");
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
	else LogError("UpdateThreatTable: unknown actor type");
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
	else LogError("ClearEnemyThreat: unknown actor type");
}

int32 IsDead(AActor* aActor)
{
	if (aActor == nullptr) return FALSE_;
	//TODO dead flag on creature?
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		//ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		if (GetCreatureProperty(aActor, PROPERTY_DEPLETABLE_HEALTH) <= 0)
		{
			return TRUE_;
		}
	}
	else LogError("IsDead: unknown actor type");
	return FALSE_;
}

int32 HasDeathEffect(AActor* aActor, int32 bCheckForDeathEvent)
{
	if (aActor == nullptr) return FALSE_;
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
	else LogError("HasDeathEffect: unknown actor type");
	return FALSE_;
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
	else LogError("IsPerceiving: unknown actor type");
	return FALSE_;
}

int32 GetCreatureGender(AActor* aActor)
{
	return GetLocalInt(aActor, GENDER);
}

int32 GetCreatureRacialType(AActor* aActor)
{
	return GetLocalInt(aActor, RACE);
}

float GetCreatureProperty(AActor* oCreature, int32 nProperty, int32 nValueType)
{
	//TODO attribute vs depletable vs simple vs derived?
	FString sProperty = GetM2DAString(TABLE_PROPERTIES, "Stat", nProperty);

	FActorProperty property = GetProperty(oCreature, FName(*sProperty));

	if (nValueType == PROPERTY_VALUE_TOTAL) return property.fValueTotal;
	else if (nValueType == PROPERTY_VALUE_BASE) return property.fValueBase;
	else if (nValueType == PROPERTY_VALUE_MODIFIER) return property.fValueModifier;
	else if (nValueType == PROPERTY_VALUE_CURRENT) return property.fValueCurrent;
	else LogError("Unknown property value type!!!");

	return 0.f;
}

void SetCreatureProperty(AActor* oCreature, int32 nProperty, float fNewValue, int32 nValueType)
{
	FString sProperty = GetM2DAString(TABLE_PROPERTIES, "Stat", nProperty);
	FActorProperty property = GetProperty(oCreature, FName(*sProperty));

	if (nValueType == PROPERTY_VALUE_TOTAL) property.fValueTotal = fNewValue;
	else if (nValueType == PROPERTY_VALUE_BASE) property.fValueBase = fNewValue;
	else if (nValueType == PROPERTY_VALUE_MODIFIER) property.fValueModifier = fNewValue;
	else if (nValueType == PROPERTY_VALUE_CURRENT) property.fValueCurrent = fNewValue;
	else LogError("Unknown property value type!!!");
}

FActorProperty GetProperty(AActor* aActor, FName sVarName)
{
	if (aActor == nullptr) return FActorProperty(PROPERTY_INVALID);
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* aCreature = Cast<ADA2UE4Creature>(aActor);

		UProperty* PropIt = aCreature->GetClass()->FindPropertyByName(sVarName);
		FActorProperty* ResolvedProperty = PropIt->ContainerPtrToValuePtr<FActorProperty>(aCreature);

#ifdef DEBUG
		FString s = FString::FromInt((ResolvedProperty)->nPropertyID);
		LogWarning("Getting property with ID: " + s + " and value " + (FString::FromInt((ResolvedProperty)->fValueTotal) + " from actor " + aActor->GetName()));
#endif // DEBUG

		return (*ResolvedProperty);
	}
	return FActorProperty(PROPERTY_INVALID);
}

int32 IsObjectValid(AActor* aActor)
{
	return aActor->IsValidLowLevel() ? TRUE_ : FALSE_;
	/*if (aActor == nullptr) return FALSE_;
	//split it into object_types
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		for (TActorIterator<ADA2UE4Creature> ActorItr(GetCurrentWorld()); ActorItr; ++ActorItr)
		{
			ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
			if (oCreature == *ActorItr)
			{
				return TRUE_;
			}
		}
		LogError("IsObjectValid: creature not found!!!");
	}
	else LogError("IsObjectValid: unknown actor type");
	return FALSE_;*/
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
			LogError("Inventory slots can be only 0 and 1, got " + wSet);
			return nullptr;
		}
		return item;
	}
	else LogError("GetItemInEquipSlot: unknown actor type");
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
	else LogError("GetBaseItemType: unknown actor type");
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
			if (IsObjectHostile(aActor, Cast<AActor>(pawn)))
				return TRUE_;
		}
	}
	else LogError("IsPerceivingHostiles: unknown actor type");
	return FALSE_;
}

int32 IsPartyPerceivingHostiles(AActor* aActor)
{
	if (aActor == nullptr) return FALSE_;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		if (GetLocalInt(aActor, TEAM) <= 0)
		{
			Log_Trace(LOG_CHANNEL_SYSTEMS, "ldf.h : IsPartyPerceivingHostiles", "Invalid Team ID");
			return FALSE_;
		}

		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		TArray<ADA2UE4Creature*> arNewList;

		arNewList = GetTeam(GetLocalInt(oCreature, TEAM), OBJECT_TYPE_CREATURE);

		for (ADA2UE4Creature* teamMember : arNewList)
		{
			if (IsPerceivingHostiles(teamMember)) return TRUE_;
		}
	}
	else LogError("IsPartyPerceivingHostiles: unknown actor type");
	return FALSE_;
}

TArray<AActor*> GetPartyList(AActor* oCreature)
{
	return GetModule()->Party;
}

int32 GetObjectActive(AActor* oActor)
{
	//TODO get object active custom in creature class
	/*SetActorHiddenInGame(!active);
	SetActorEnableCollision(active);
	SetActorTickEnabled(active);*/
#ifdef DEBUG
	bool b = (!(oActor->bHidden) && oActor->GetActorEnableCollision() && oActor->IsActorTickEnabled());
	FString s;
	if (b) s = "true"; else s = "false";
	LogWarning("actor " + oActor->GetName() + " is active: " + s);
#endif // DEBUG

	return !(oActor->bHidden) && oActor->GetActorEnableCollision() && oActor->IsActorTickEnabled();
}

void SetObjectActive(AActor* aActor, int32 nActive, int32 nAnimation, int32 nVFX, int32 nNextLine)
{
	bool bActive;
	if (nActive == TRUE_)
		bActive = true;
	else bActive = false;

	//TODO SetObjectActive nAnimation, nVFX, nNextLine
	aActor->SetActorHiddenInGame(!bActive);
	aActor->SetActorEnableCollision(bActive);
	aActor->SetActorTickEnabled(bActive);
}

int32 GetGameMode()
{
	return GetModule()->GAME_MODE;
}

int32 IsImmortal(AActor* aActor)
{
	if (aActor == nullptr) return FALSE_;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		return oCreature->bImmortal;
	}
	else LogError("IsImmortal: unknown actor type");
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
	else LogError("IsPlot: unknown actor type");
	return FALSE_;
}

void PlaySoundSet(AActor* oTarget, int32 nSoundSetEntry, float fProbabilityOverride)
{
	FString sound = GetM2DAString(TABLE_SOUNDSETS, SS_NODENAME, nSoundSetEntry);
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
		int32 nRace = GetLocalInt(oCreature, RACE);
		return GetM2DAInt(TABLE_RULES_RACES, "Appearance", nRace);
	}
	else LogError("GetAppearanceType: unknown actor type");
	return FALSE_;
}

int32 GetFollowerApproval(AActor* aActor)
{
	if (aActor == nullptr) return FALSE_;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		if (oCreature->bApproval) { return GetLocalInt(oCreature, APPROVAL); }
		else return 0; //not permanent follower plot/story related
	}
	else LogError("GetFollowerApproval: unknown actor type");
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
				if (IsObjectHostile(aActor, pawn))	aArray.Add(pawn);
			}
			else aArray.Add(pawn);
		}
	}
	else LogError("GetPerceivedCreatureList: unknown actor type");
	return aArray;
}

int32 GetFollowerState(AActor* aActor)
{
	return GetLocalInt(aActor, FOLLOWER_STATE);
}

void SetPlaceableHealth(AActor* oPlc, int32 nHealth)
{
	//TODO implement SetPlaceableHealth
	LogError("SetPlaceableHealth to be implemented");
}

void SetAILevel(AActor* oActor, int32 nAILevel)
{
	//TODO implement SetAILevel
	LogError("SetAILevel to be implemented");
}

int32 ClearAmbientDialogs(AActor* aActor)
{
	//TODO implement ClearAmbientDialogs fully
	if (aActor == nullptr)
	{
		LogError("ClearAmbientDialogs to be implemented for all creatures if nullptr");
		return 0;
	}
	else if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		if (GetLocalInt(oCreature, AMB_SYSTEM_DIALOG) != 0)
		{
			//nResult = TRUE_;
			SetLocalInt(oCreature, AMB_SYSTEM_DIALOG, 0);
		}
	}
	else if (aActor->IsA(ADA2UE4Module::StaticClass()))
	{
		//TODO this makes sure an already running ambient dialog triggers its plot flag action and look at gen00pt_ambient_ai
		ADA2UE4Module* module = Cast<ADA2UE4Module>(aActor);
		if (GetLocalInt(module, AMB_SYSTEM_DIALOG) != 0)
		{
			SetLocalInt(module, AMB_SYSTEM_DIALOG, 0);
		}
	}
	else LogError("ClearAmbientDialogs: unknown actor type");
	return 0;
}

int32 BeginConversation(AActor* oTarget, FString rConversationFile)
{
	//TODO implement BeginConversation
	LogError("BeginConversation to be implemented");
	return FALSE_;
}

int32 GetGameDifficulty()
{
	return GetGameSettings(SETTING_GAME_DIFFICULTY);
}

int32 GetPackage(AActor* aActor)
{
	return GetLocalInt(aActor, PACKAGE);
}

int32 GetPackageAI(AActor* aActor)
{
	return GetLocalInt(aActor, PACKAGEAI);
}

int32 ClearAllCommands(AActor* aActor, int32 nHardClear)
{
	if (aActor == nullptr) return FALSE_;
	if (!nHardClear) { LogError("ClearAllCommands: nHardClear not implemented"); return 0; }
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		oCreature->qCommands.Empty();
		oCreature->currentCommand = FCommand(COMMAND_TYPE_INVALID);
	}
	else LogError("ClearAllCommands: unknown actor type");
	return 0;
}

float GetDistanceBetween(AActor* oActorA, AActor* oActorB, int32 bSubtractPersonalSpace)
{
	//TODO implement GetDistanceBetween with bSubtractPersonalSpace
	if (oActorA == nullptr || oActorB == nullptr) return -1.0f;
	float distanceBetween = (oActorA->GetActorLocation() - oActorB->GetActorLocation()).Size();
	return distanceBetween;
}

float GetDistanceBetweenLocations(FVector vVectorA, FVector vVectorB)
{
	return (vVectorA - vVectorB).Size();
}

int32 GetCombatantType(AActor* aActor)
{
	return GetLocalInt(aActor, COMBATANT_TYPE);
}

TArray<AActor*> GetNearestObjectByGroup(AActor* aActor, int32 nGroupId, int32 nObjectType, int32 nNumberOfObjects, int32 nCheckLiving, int32 nCheckPerceived, int32 nIncludeSelf)
{
	//TODO GetNearestObjectByGroup incorrect sorting, but assumes few actors in the sensing radius anyway
	TArray<AActor*> aArray;
	if (aActor == nullptr) return aArray;
	if (nObjectType != OBJECT_TYPE_CREATURE)
	{
		LogError("GetNearestObjectByGroup: only creatures are considered");
		return aArray;
	}
	if (nCheckLiving != TRUE_)
	{
		LogError("GetNearestObjectByGroup: why looking for dead people?");
		return aArray;
	}
	if (nCheckPerceived != TRUE_)
	{
		LogError("GetNearestObjectByGroup: why looking for unsensed pawns?");
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
	else LogError("GetNearestObjectByGroup: unknown actor type");
	return aArray;
}

TArray<AActor*> GetNearestObjectByHostility(AActor* aActor, int32 nHostility, int32 nObjectType, int32 nNumberOfObjects, int32 nCheckLiving, int32 nCheckPerceived, int32 nIncludeSelf)
{
	TArray<AActor*> aArray;
	if (aActor == nullptr) return aArray;
	if (nObjectType != OBJECT_TYPE_CREATURE)
	{
		LogError("GetNearestObjectByHostility: only creatures are considered");
		return aArray;
	}
	if (nCheckLiving != TRUE_)
	{
		LogError("GetNearestObjectByHostility: why looking for dead people?");
		return aArray;
	}
	if (nCheckPerceived != TRUE_)
	{
		LogError("GetNearestObjectByHostility: why looking for unsensed pawns?");
		return aArray;
	}
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		if (nIncludeSelf == TRUE_) aArray.Add(Cast<APawn>(aActor));
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);

		TArray<APawn*> sensedArray;

		for (APawn* pawn : oCreature->SensedPawns)
		{
			if (IsObjectHostile(aActor, pawn))
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
	else LogError("GetNearestObjectByHostility: unknown actor type");
	return aArray;
}

TArray<AActor*> GetNearestObjectToLocation(FVector vVector, int32 nObjectType, int32 nNumberOfObjects)
{
	//TODO GetNearestObjectToLocation return more than 1 object. Also may be slow and expensive
	TArray<AActor*> aArray;
	TArray<AActor*> tempArray;
	if (nObjectType != OBJECT_TYPE_CREATURE)
	{
		LogError("GetNearestObjectToLocation: only creatures are considered");
		return aArray;
	}
	if (nNumberOfObjects != 1)
	{
		LogError("GetNearestObjectToLocation: why more than 1?");
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
			else LogError("a world with no actors?");
		}
	}

	tempArray.Empty();
	tmp->Destroy();

	return aArray;
}

TArray<AActor*> GetNearestObject(AActor* aActor, int32 nObjectType, int32 nNumberOfObjects, int32 nCheckLiving, int32 nCheckPerceived, int32 nIncludeSelf)
{
	TArray<AActor*> aArray;
	if (nObjectType != OBJECT_TYPE_PLACEABLE)
	{
		LogError("GetNearestObject: only Placeables are considered");
		return aArray;
	}
	//TODO GetNearestObject to be implemented
	LogError("GetNearestObject: to be implemented!!");
	return aArray;
}

TArray<AActor*> GetNearestObjectByTag(AActor* aActor, FString sTag, int32 nObjectType, int32 nNumberOfObjects, int32 nCheckLiving, int32 nCheckPerceived, int32 nIncludeSelf)
{
	TArray<AActor*> aArray;
	if (nObjectType != OBJECT_TYPE_WAYPOINT)
	{
		LogError("GetNearestObjectByTag: only waypoints are considered for now");
		return aArray;
	}
	if (nNumberOfObjects != 1)
	{
		LogError("GetNearestObjectByTag: why more than 1?");
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
	else LogError("GetEffectsFlags: unknown actor type");
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
	else LogError("GetObjectInteractive: unknown actor type");
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
	else LogError("SetObjectInteractive: unknown actor type");
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
	else LogError("GetEffectsFlags: unknown actor type");
	return aArray;
}

UWorld* GetArea(AActor* oActor)
{
	return oActor->GetWorld();
}

AActor* GetAttackTarget(AActor* oCreature)
{
	return GetLocalObject(oCreature, AI_THREAT_TARGET);
}

int32 GetActiveWeaponSet(AActor* aActor)
{
	if (aActor == nullptr) return FALSE_;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		return oCreature->nActiveWeaponSet;
	}
	else LogError("GetActiveWeaponSet: unknown actor type");
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
	else LogError("GetTacticTargetObject: unknown actor type");
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
	else LogError("GetTacticConditionObject: unknown actor type");
	return nullptr; //error
}

TArray<AActor*> GetCreaturesInMeleeRing(AActor* aActor, float fStartAngle, float fEndAngle, int32 bOnlyHostiles, int32 nRingIndex)
{
	TArray<AActor*> aArray;
	if (aActor == nullptr) return aArray;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);

		for (APawn* pawn : oCreature->MeleeRingPawns)
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
	else LogError("GetCreaturesInMeleeRing: unknown actor type");
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
	else LogError("GetCreaturesInCone: unknown actor type");
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
		//int32 level = GetCreatureProperty(aActor, PROPERTY_SIMPLE_LEVEL);
		//int32 maxTactics = GetM2DAInt(TABLE_EXPERIENCE, "Tactics", level);
		//int32 populatedTactics = oCreature->Tactics.Num();
		return oCreature->Tactics.Num();
	}
	else LogError("GetNumTactics: unknown actor type");
	return 0; //error
}

int32 GetM2DARows(int32 n2DA, FString s2DA)
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
	else LogError("IsTacticEnabled: unknown actor type");
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
	else LogError("GetTacticCommandItemTag: unknown actor type");
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
	else LogError("GetTacticTargetType: unknown actor type");
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
	else LogError("GetTacticCondition: unknown actor type");
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
	else LogError("GetTacticCommand: unknown actor type");
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
	else LogError("GetTacticCommandParam: unknown actor type");
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
	else LogError("GetItemStackSize: unknown actor type");
	return 0; //error
}

float GetRemainingCooldown(AActor* aActor, int32 nAbilityId, FString sSourceItemTag)
{
	if (aActor == nullptr) return 0.f;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		float fCooldown = GetM2DAFloat(TABLE_ABILITIES_SPELLS, "cooldown", nAbilityId);
		float fLastUsed = 0.f;
		for (FAbility abi : oCreature->Abilities)
		{
			if (abi.nAbilityID == nAbilityId)
			{
				fLastUsed = abi.fLastUsed;
				break;
			}
		}
		float currentTime = GetCurrentWorld()->GetTimeSeconds();
		return (currentTime - fLastUsed < fCooldown) ? currentTime - fLastUsed : 0.f;

	}
	else LogError("GetRemainingCooldown: unknown actor type");
	return 1.f; //error

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

float GetFacing(AActor* oTarget)
{
	LogWarning("get facing, ensure that right is correct, instead of down, or something else");
	return FMath::RadiansToDegrees(FMath::Atan2(oTarget->GetActorLocation().Z, oTarget->GetActorLocation().X));
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
	else LogError("GetItemsInInventory: unknown actor type");
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
	else LogError("GetItemPropertyPower: unknown actor type");
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
	else LogError("GetItemValue: unknown actor type");
	return 0; //error
}

int32 GetItemAbilityId(AActor* oItem)
{
	return GetLocalInt(oItem, ABILITY);
}

float GetAngleBetweenObjects(AActor* aActorA, AActor* aActorB)
{
	//TODO GetAngleBetweenObjects doublecheck
	//return FMath::Acos(FMath::Clamp(FVector::DotProduct(aActorA->GetActorLocation()->Normalize(),aActorB->GetActorLocation()->Normalize()),-1.f,1.f)* 57.29578f)
	return FMath::Atan2(aActorA->GetActorLocation().Z - aActorB->GetActorLocation().Z, aActorA->GetActorLocation().X - aActorB->GetActorLocation().X);
}

int32 IsHumanoid(AActor* aActor)
{
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

		int t0 = GetBaseItemType(main);
		int t1 = GetBaseItemType(off);
		
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
		default: LogError("GetWeaponStyle passed out :D"); break;
		}
	}
	else LogError("GetWeaponStyle: unknown actor type");
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
	else LogError("GetWeaponStyle: unknown actor type");
	return 0; //error
}

void SetFacingObject(AActor* aActor, AActor* oTarget, int32 bInvert, int32 bInstant)
{
	FRotator r = UKismetMathLibrary::FindLookAtRotation(aActor->GetActorLocation(), oTarget->GetActorLocation());
	aActor->SetActorRotation(r);
}

int32 TriggerPerception(AActor* oPerceivingCreature, AActor* oPerceivedCreature)
{
	//TODO TriggerPerception handle dead creatures, etc
	return TRUE_;
}

void SetCreatureIsStatue(AActor* aActor, int32 value)
{
	aActor->FindComponentByClass<class USkeletalMeshComponent>()->bNoSkeletonUpdate = value;
}

AActor* CreateItemOnObject(FString rItemFileName, AActor* aTarget, int32 nStackSize, FString sTag, int32 bSuppressNotification, int32 bDroppable)
{
	//TODO CreateItemOnObject dynamic based on item type, and other settings
	if (aTarget == nullptr) return nullptr;
	if (aTarget->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aTarget);

		FString sItem = "/Game/Data/Blueprints/GameObjects/Items/";
		sItem += rItemFileName;
		sItem += '.';
		sItem += rItemFileName;
		UBlueprint* itemBlueprint = LoadBlueprintFromPath(FName(*sItem));
		ADA2UE4Item* oItem = GetCurrentWorld()->SpawnActor<ADA2UE4Item>(itemBlueprint->GeneratedClass);

		//add item to backpack
		if (oItem)	oCreature->Inventory.Add(oItem);

		return oItem;
	}
	else LogError("CreateItemOnObject: unknown actor type");
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
	else LogError("SetItemDroppable: unknown actor type");
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
			LogError("Inventory slots can be only 0 and 1, got " + wSet);
			return FALSE_;
		}
	}
	else LogError("EquipItem: unknown actor type");
	return FALSE_;//error
}

float GetItemStat(AActor* aItem, int32  nStatType)
{
	//TODO GetItemStat finish
	if (aItem == nullptr) return 0.f;
	if (aItem->IsA(ADA2UE4Item::StaticClass()))
	{
		ADA2UE4Item* oItem = Cast<ADA2UE4Item>(aItem);
		int _Type = GetBaseItemType(oItem);
		int _Stat = 0;
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
		default: LogError("GetItemStat: unknown stat");
		}
		return _Stat;
	}
	else LogError("GetItemStat: unknown actor type");
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
	else LogError("GetItemEquipSlot: unknown actor type");
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
	else LogError("GetCanDiePermanently: unknown actor type");
	return FALSE_;
}

FEffect EffectImpact(float fDamage, AActor* oWeapon, int32 nVfx, int32 nAbi, int32 nDamageType)
{
	FEffect ef = Effect(EFFECT_TYPE_IMPACT);
	ef = SetEffectObject(ef, 0, oWeapon);
	ef = SetEffectFloat(ef, 0, fDamage);
	ef = SetEffectInteger(ef, 0, nVfx);
	ef = SetEffectInteger(ef, 1, nAbi);
	ef = SetEffectInteger(ef, 2, nDamageType);

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

	else LogError("SetAttackResult: unknown actor type");
}

void SetAimLoopDuration(AActor* aActor, float fDuration)
{
	if (aActor == nullptr) return;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		SetAttackDuration(oCreature, fDuration);
	}
	else LogError("SetAimLoopDuration: unknown actor type");
}

void SetAttackDuration(AActor* aActor, float fDuration)
{
	if (aActor == nullptr) return;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		oCreature->AttackDuration += fDuration;
	}
	else LogError("SetAttackDuration: unknown actor type");
}

void EnableWeaponTrail(AActor* aActor, int32 bEnable)
{
	if (aActor == nullptr) return;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		//TODO EnableWeaponTrail to be implemented
		return;
	}
	else LogError("EnableWeaponTrail: unknown actor type");
}

FVector GetRoamLocation(AActor* aActor)
{
	if (aActor == nullptr) return FVector(0.f);
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		return oCreature->HOME_LOCATION;
	}
	else LogError("GetRoamLocation: unknown actor type");
	return FVector(0.f);
}

void ClearPerceptionList(AActor* aActor)
{
	if (aActor == nullptr) return;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		oCreature->SensedPawns.Empty();
		oCreature->MeleeRingPawns.Empty();
	}
	else LogError("ClearPerceptionList: unknown actor type");
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
			else return nullptr;
		}
	}
	default: 
	{
		LogError("GetObjectByTag unknown object type!!");
		return nullptr; 
	}
	}
}

void InteractWithObject(AActor* oCreature, AActor* oTarget, int32 nInteractionId, int32 nPose, int32 nLoops, int32 bPlayExit, int32 bSkipReposition)
{
	//TODO InteractWithObject
	LogError("InteractWithObject to be implemented!!");
}
