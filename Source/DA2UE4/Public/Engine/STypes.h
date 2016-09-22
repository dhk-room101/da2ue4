#pragma once

#include "STypes.generated.h"

class Engine;

UENUM() //match gm_* in ldf.h
enum class EGameMode : uint8
{
	GM_INVALID,
	GM_EXPLORE,
	GM_COMBAT,
	GM_DEAD,
	GM_GUI,
	GM_CONVERSATION,
	GM_FLYCAM,
	GM_FIXED,
	GM_PREGAME,
	GM_LOADING,
	GM_MOVIE,
	GM_CHARGEN,
	GM_PARTYPICKER,
};

UENUM()
enum class ECombatResult : uint8
{
	CREATURE_TYPE_NON_COMBATANT,
	NOT_COMBAT_OR_EXPLORE_GAMEMODE,
	CREATURE_COMMAND_IN_PROGRESS,
	CREATURE_RULES_FLAG_AI_OFF_ENABLED,
	AI_LIGHT_ACTIVE, //
	AI_AOE_FLEE,
	AI_PARTIAL_ACTIVE, //
	AI_MODIFIER_IGNORE,
	AI_WEAPON_SET_RANGED,
	AI_WEAPON_SET_MELEE,
	AI_MOVE_AWAY_DISTANCE_MELEE,
	FOLLOWER_NOT_COMBAT_STATE,
	TOO_MANY_TACTICS,
	LAST_TACTIC_FAILED,
	TACTIC_EXECUTED,
	OBJECT_SELF_INACTIVE,
	NOT_IN_COMBAT,
	UNKNOWN_COMBAT_RESULT,
	DEFAULT_ACTION,
};

//TODO effect flag addition for bitwise ops, @sa GetEffectsFlags
//TODO extrapolate effect constructor based on engine/mixed/scripts

/*i.e.: Engine effects (Effect constructor and destructor is handled by engine)
const int32 EFFECT_TYPE_VISUAL_EFFECT = 4;
const int32 EFFECT_TYPE_CAMERA_SHAKE = 6;
const int32 EFFECT_TYPE_PARALYZE = 8;
const int32 EFFECT_TYPE_MOVEMENT_RATE = 9;
const int32 EFFECT_TYPE_ALPHA = 10;
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
	}

	FCommand(const int32 _nType)
		:nCommandType(_nType)
	{
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

		FAbility()
		:nAbilityID(0)
	{}

	FAbility(const int32 _nAbilityID)
		:nAbilityID(_nAbilityID)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ability)
		int32 nAbilityID = 0; //ABILITY_INVALID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ability)
		int32 nAbilityType = 0; //ABILITY_TYPE_INVALID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ability)
		float fLastUsed = 0.f;

	FORCEINLINE bool operator==(const FAbility& Other) const
	{
		if (nAbilityID != Other.nAbilityID) return false;
		if (nAbilityType != Other.nAbilityType) return false;
		if (fLastUsed != Other.fLastUsed) return false;
		return true;
	}

};

USTRUCT()
struct FItemProperty
{
	GENERATED_BODY()

		FItemProperty()
		:nItemPropertyID(0)
	{}

	FItemProperty(const int32 _nItemPropertyID)
		:nItemPropertyID(_nItemPropertyID)
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
	int32 nAttackResult = 0; // - COMBAT_RESULT_* constant
	int32 nDeathblowType = 0;
	float fAttackDuration = 0.f; // - Duration of the aim loop for ranged weapons
	FEffect eImpactEffect = 0; // - Impact Effect
};

struct FConvAnimationList
{
	FString Animation;
	float StartTime = 0.f;
	float Weight = 0.f;
	float Speed = 0.f;
	float StartOffset = 0.f;
	float BlendIn = 0.f;
	float EndOffset = 0.f;
	float BlendOut = 0.f;
};

struct FConvAnimationListList
{
	FString Tag;
	bool Locked = false;
	TArray<FConvAnimationList> AnimationList;
};

// what is says, transition between nodes, mainly from npc->player and viceversa
struct FConvTransition
{
	bool IsLink;
	int32 LineIndex = 0;
};

// node in conversation, not related to what is in progress
struct FConvNode
{
	//DHK
	int32 lineIndex = 0;	//Keeps track of position in the Conversation branch

	//DAO
	int32 StringID = 0;
	int32 LanguageID = 0;
	int32 ConditionScriptURI = 0;
	int32 ConditionParameter = 0;
	FString ConditionParameterText = "";
	int32 ConditionPlotURI = 0;
	int32 ConditionPlotFlag = 0;
	bool ConditionResult = false;
	int32 ActionScriptURI = 0;
	int32 ActionParameter = 0;
	FString ActionParameterText = "";
	int32 ActionPlotURI = 0;
	int32 ActionPlotFlag = 0;
	bool ActionResult = false;
	FString dwText = "";//from text to dialog wheel text
	FString dwMood = "";//DHK
	int32 dwWheel = -1;//DHK
	FString spokenText = "";//DHK
	bool TextRequiresReTranslation = false;
	bool TextRequiresReRecording = false;
	FString Speaker = "";
	FString PreviousSpeaker = "";
	FString Listener = "";
	int32 icon = 0;
	FString Comment = "";
	int32 FastPath = 0;
	FString SlideShowTexture = "";
	FString VoiceOverTag = "";
	FString VoiceOverComment = "";
	FString EditorComment = "";
	int32 LineVisibility = 0;
	bool Ambient = false;
	bool SkipLine = false;
	int32 StageURI = 0;
	FString StageTag = "";
	bool StageAtCurrentLocation = false;
	FString CameraTag = "";
	bool CameraLocked = false;
	FString SecondaryCameratag = "";
	float SecondaryCameraDelay = 0.f;
	int32 Emotion = 0;
	int32 CustomCutsceneURI = 0;
	FString SpeakerAnimation = "";
	bool RevertAnimation = false;
	bool LockAnimations = false;
	bool PlaySoundEvents = false;
	int32 RoboBradSeed = 0;
	bool RoboBradSeedOverride = false;
	bool RoboBradLocked = false;
	int32 PreviewAreaURI = 0;
	bool PreviewStageUseFirstMatch = false;
	FVector PreviewStagePosition = FVector(0.f);
	FVector PreviewStageOrientation = FVector(0.f);
	bool UseAnimationDuration = false;
	bool NoVOInGame = false;
	bool Narration = false;
	bool PreCacheVO = false;
	TArray<FConvTransition> TransitionList;
	//TODO FConvNode Lists
// 	CinematicsInfoList 
	TArray<FConvAnimationListList> AnimationListList;
// 	CustomCutsceneParameterList 
// 	PreviewTagMappingList 
};

//a complete conversation data
struct FConversation
{
	//TODO add other conv attributes
	int32 ResRefID = 0;
	TArray<int32> StartList;
	TArray<FConvNode> NPCLineList;
	TArray<FConvNode> PlayerLineList;
};

//on an instance of wheel in progress now
struct FConvLine
{
	int32 lineLocation = -1;
	FString iconID = "";
	int32 lineIndex = -1;
	FString owner = "";
};

struct FPlotAssistInfoList
{
	FName PlotAdvancerTag;
	bool AdvancesPlot;
};

struct FPlotNode
{
	int64 Flag = 0;
	FString Name = "";
	bool Final = false;
	bool Repeatable = false;
	FString JournalText = "";
	int32 JournalTextStringID = 0;
	bool JournalTextRequiresReTranslation = false;
	int32 RewardID = 0;
	FString Comment = "";
	int32 DefaultValue = 0;
	FString AreaLocationTag = "";
	int32 OfferID = 0;
	TArray<FPlotAssistInfoList> PlotAssistInfoList;
};

struct FPlotElement
{
	FPlotNode pNode;
	int32 pValue = 0;//true/false
};

struct FPlot
{
	int32 ResRefID = 0; // DHK
	FString ResRefName = "";
	bool LocalCopy = false;
	FString Name = "";
	int32 NameStringID = 0;
	bool NameRequiresReTranslation = false;
	FString GUID = "";
	int32 ScriptURI = 0;
	int32 Priority = 0;
	FString JournalImage = "";
	int32 ParentPlotURI = 0;
	int32 EntryType = 0;
	bool AllowPausing = false;
	TArray<FPlotElement> StatusList;
};
