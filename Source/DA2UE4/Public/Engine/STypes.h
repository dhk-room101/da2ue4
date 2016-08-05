#pragma once

#include "STypes.generated.h"

class Engine;

//creature structs

//TODO effect flag addition for bitwise ops, @sa GetEffectsFlags
//TODO extrapolate effect constructor based on engine/mixed/scripts

/*i.e.: Engine effects (Effect constructor and destructor is handled by engine)
const int32 EFFECT_TYPE_VISUAL_EFFECT     = 4;
const int32 EFFECT_TYPE_CAMERA_SHAKE      = 6;
const int32 EFFECT_TYPE_PARALYZE          = 8;
const int32 EFFECT_TYPE_MOVEMENT_RATE     = 9;
const int32 EFFECT_TYPE_ALPHA             = 10;
*/
USTRUCT()
struct FEffect
{
	GENERATED_BODY()

	FEffect()
		:nEffectType(0)
	{}

	FEffect(const int32 _nEffectType)
		:nEffectType(_nEffectType)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effect)
		int32 nEffectType = 0; // EFFECT_TYPE_INVALID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effect)
		int32 nAbilityID = 0; // ABILITY_INVALID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effect)
		AActor* oCreator = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effect)
		int32 nDurationType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effect)
		int32 nEffectID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effect)
		TArray<int32> nList;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effect)
		TArray<float> fList;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effect)
		TArray<AActor*> oList;

	FORCEINLINE bool operator==(const FEffect& Other) const
	{
		if (nEffectType != Other.nEffectType) return false;
		if (nAbilityID != Other.nAbilityID) return false;
		if (oCreator != Other.oCreator) return false;
		if (nDurationType != Other.nDurationType) return false;
		if (nEffectID != Other.nEffectID) return false;
		if (nList != Other.nList) return false;
		if (fList != Other.fList) return false;
		if (oList != Other.oList) return false;
		return true;
	}

	FORCEINLINE bool operator!=(const FEffect& Other) const
	{
		if (nEffectType == Other.nEffectType) return false;
		if (nAbilityID == Other.nAbilityID) return false;
		if (oCreator == Other.oCreator) return false;
		if (nDurationType == Other.nDurationType) return false;
		if (nEffectID == Other.nEffectID) return false;
		if (nList == Other.nList) return false;
		if (fList == Other.fList) return false;
		if (oList == Other.oList) return false;
		return true;
	}
};

USTRUCT()
struct FGameEvent
{
	GENERATED_BODY()

	FGameEvent()
	:nGameEventType(0)
	{}

	FGameEvent(const int32 _nGameEventType)
		:nGameEventType(_nGameEventType)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Event)
		int32 nGameEventType = 0; // EVENT_TYPE_INVALID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Event)
		TArray<int32> nList;//lists of event references
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Event)
		TArray<AActor*> oList;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Event)
		TArray<FString> sList;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Event)
		TArray<float> fList;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Event)
		TArray<FVector> vList;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Event)
		float signalTime = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Event)
		FEffect eEffect;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Event)
		AActor* oCreator = nullptr;
	
	FORCEINLINE bool operator==(const FGameEvent& Other) const
	{
		if (nGameEventType != Other.nGameEventType) return false;
		if (nList != Other.nList) return false;
		if (oList != Other.oList) return false;
		if (sList != Other.sList) return false;
		if (fList != Other.fList) return false;
		if (vList != Other.vList) return false;
		if (signalTime != Other.signalTime) return false;
		if (eEffect != Other.eEffect) return false;
		if (oCreator != Other.oCreator) return false;
		return true;
	}
};

USTRUCT()
struct FThreat
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Threat)
		AActor* oThreat;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Threat)
		float fThreat;

	FORCEINLINE bool operator==(const FThreat& Other) const
	{
		if (oThreat != Other.oThreat) return false;
		if (fThreat != Other.fThreat) return false;
		return true;
	}
};

USTRUCT()
struct FTactic
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tactics)
		FString sTacticTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tactics)
		int32 nTacticID; //index?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tactics)
		int32 nTacticTargetType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tactics)
		AActor* oTacticTarget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tactics)
		int32 nTacticCondition;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tactics)
		AActor* oTacticCondition;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tactics)
		int32 nTacticCommand;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tactics)
		int32 nTacticSubCommand;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tactics)
		int32 bEnabled;

	FORCEINLINE bool operator==(const FTactic& Other) const
	{
		if (sTacticTag != Other.sTacticTag) return false;
		if (nTacticID != Other.nTacticID) return false;
		if (nTacticTargetType != Other.nTacticTargetType) return false;
		if (oTacticTarget != Other.oTacticTarget) return false;
		if (nTacticCondition != Other.nTacticCondition) return false;
		if (oTacticCondition != Other.oTacticCondition) return false;
		if (nTacticCommand != Other.nTacticCommand) return false;
		if (nTacticSubCommand != Other.nTacticSubCommand) return false;
		if (bEnabled != Other.bEnabled) return false;
		return true;
	}

};

USTRUCT()
struct FActorProperty
{
	GENERATED_BODY()

	FActorProperty() : nPropertyID(0)
	{}

	FActorProperty(const int32 _nPropertyID)
		:nPropertyID(_nPropertyID)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties)
		int32 nPropertyID = 0; //PROPERTY_INVALID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties)
		int32 nPropertyType = 0; //PROPERTY_TYPE_INVALID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties)
		float fValueTotal; //Max for depletables, Total for derived, simple and attribute
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties)
		float fValueBase;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties)
		float fValueCurrent; // for depletables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties)
		float fValueModifier;

	FORCEINLINE bool operator==(const FActorProperty& Other) const
	{
		if (nPropertyID != Other.nPropertyID) return false;
		if (nPropertyType != Other.nPropertyType) return false;
		if (fValueTotal != Other.fValueTotal) return false;
		if (fValueBase != Other.fValueBase) return false;
		if (fValueCurrent != Other.fValueCurrent) return false;
		if (fValueModifier != Other.fValueModifier) return false;
		return true;
	}
};

USTRUCT()
struct FCommand
{
	GENERATED_BODY()

	FCommand() : nCommandType(0)
	{
		//initialize an empty float array with 5 elem wrappers_h is trying to push one too early :D
		int32 i = 0;
		while (i < 5) { fList.Add(0.f); i++; }
	}

	FCommand(const int32 _nType)
		:nCommandType(_nType)
	{
		int32 i = 0;
		while (i < 5) { fList.Add(0.f); i++; }
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Command)
		int32 nCommandType = 0; //COMMAND_TYPE_INVALID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Command)
		int32 nCommandStatus; // 0 = COMMAND_IN_PROGRESS
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Command)
		int32 nCommandResult = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Command)
		int32 bStatic = 0; //must finish
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Command)
		TArray<float> fList;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Command)
		TArray<int32> nList;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Command)
		TArray<AActor*> oList;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Command)
		TArray<FVector> vList;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Command)
		TArray<FString> sList;

	FORCEINLINE bool operator==(const FCommand& Other) const
	{
		if (nCommandType != Other.nCommandType) return false;
		if (nCommandStatus != Other.nCommandStatus) return false;
		if (nCommandResult != Other.nCommandResult) return false;
		if (bStatic != Other.bStatic) return false;
		if (fList != Other.fList) return false;
		if (nList != Other.nList) return false;
		if (oList != Other.oList) return false;
		if (vList != Other.vList) return false;
		if (sList != Other.sList) return false;
		return true;
	}
};

USTRUCT()
struct FAbility
{
	GENERATED_BODY()

	FAbility() : nAbilityID(0)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ability)
		int32 nAbilityID = 0; //ABILITY_INVALID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ability)
		int32 nAbilityType = 0; //ABILITY_TYPE_INVALID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ability)
		float fLastUsed; 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ability)
		int32 bSustained;

	FORCEINLINE bool operator==(const FAbility& Other) const
	{
		if (nAbilityID != Other.nAbilityID) return false;
		if (nAbilityType != Other.nAbilityType) return false;
		if (fLastUsed != Other.fLastUsed) return false;
		if (bSustained != Other.bSustained) return false;
		return true;
	}
	
};

USTRUCT()
struct FItemProperty
{
	GENERATED_BODY()

	FItemProperty() : nItemPropertyID(0)
	{}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ability)
		int32 nItemPropertyID = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ability)
		int32 nPower = 0;

	FORCEINLINE bool operator==(const FItemProperty& Other) const
	{
		if (nItemPropertyID != Other.nItemPropertyID) return false;
		if (nPower != Other.nPower) return false;
		return true;
	}
};

//no reflection
// -----------------------------------------------------------------------------
// Attack Result struct, used by Combat_PerformAttack*
// -----------------------------------------------------------------------------
struct FCombatAttackResultStruct
{
	int32     nAttackResult;      //  - COMBAT_RESULT_* constant
	int32     nDeathblowType;
	float   fAttackDuration;   //  - Duration of the aim loop for ranged weapons
	FEffect eImpactEffect;       //  - Impact Effect
};
