// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "STypes.h"

#include "GameFramework/Character.h"
#include "DA2UE4Creature.generated.h"

class UDA2UE4MeshDataAsset;
class UDA2UE4AnimInstance;

UCLASS(Blueprintable)
class ADA2UE4Creature : public ACharacter
{
	GENERATED_BODY()

public:
	ADA2UE4Creature(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void PostInitializeComponents() override;

	/** A decal that projects to the cursor location. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UDecalComponent* CursorToWorld;

	//DHK
	UPROPERTY()
		UDA2UE4MeshDataAsset* LoadedMeshesDatabase = nullptr;
	
	//montages
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Anim)
		UAnimMontage* AnimMontageDefault;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Anim)
		UAnimMontage* AnimMontageLower;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Anim)
		UAnimMontage* AnimMontageUpperRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Anim)
		UAnimMontage* AnimMontageUpperLeft;

	UPROPERTY()
		UAnimSequence* AnimSequence;

	UPROPERTY()
		UAnimSequence* AnimSequenceIdle;

	UPROPERTY()
		USkeleton* Skeleton;

	UPROPERTY()
		USkeletalMeshComponent* MeshHEDComponent = nullptr;
	static FName MeshHEDComponentName;

	UPROPERTY()
		USkeletalMeshComponent* MeshARMComponent = nullptr;
	static FName MeshARMComponentName;

	UPROPERTY()
		USkeletalMeshComponent* MeshBOOComponent = nullptr;
	static FName MeshBOOComponentName;

	UPROPERTY()
		USkeletalMeshComponent* MeshGLVComponent = nullptr;
	static FName MeshGLVComponentName;

	UPROPERTY()
		USkeletalMeshComponent* MeshHARComponent = nullptr;
	static FName MeshHARComponentName;

	UPROPERTY()
		USkeletalMeshComponent* MeshEYEComponent = nullptr;
	static FName MeshEYEComponentName;

	// Direct ID mesh changing "request" methods, usable with inventory loading, swapping systems... etc
	bool ChangeHEDMeshByID(int32 IDCode);
	bool ChangeARMMeshByID(int32 IDCode);
	bool ChangeBOOMeshByID(int32 IDCode);
	bool ChangeGLVMeshByID(int32 IDCode);
	bool ChangeHARMeshByID(int32 IDCode);
	bool ChangeEYEMeshByID(int32 IDCode);

	bool ChangeHEDMatByID(int32 IDCode);
	bool ChangeARMMatByID(int32 IDCode);
	bool ChangeBOOMatByID(int32 IDCode);
	bool ChangeGLVMatByID(int32 IDCode);
	bool ChangeHARMatByID(int32 IDCode);
	bool ChangeEYEMatByID(int32 IDCode);

	void SetOutline(int32 nStencil, bool bEnable);
	void CreatureSpawn();
	void EquipDefaultItem(AActor* OBJECT_SELF, int32 nAppType, int32 nTargetSlot, FString s2daColumnName);

	void StopCustomMontages();
	void SetupCustomMontages();

	void AddThreat(APawn* oThreat, float fThreat);

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Assigned at the Character level (instead of Controller)
	* so we may use different creature behaviors while re-using one controller.
	*/
	UPROPERTY()
		class UBehaviorTree* BehaviorTree;

	UPROPERTY(VisibleAnywhere, Category = "AI")
		class UPawnSensingComponent* PawnSensingComp;

	UPROPERTY()
		TArray<int32> HostilityTargetGroup;
	//threat list to be sorted
	UPROPERTY()
		TArray<FThreat> Threats; //@sa SensedPawns
	UPROPERTY()
		TArray<APawn*> SensedPawns; //@sa Threats
	UPROPERTY()
		float SenseTimeOut = 0;
	UPROPERTY()
		float LastSensedTime = 0;

	UPROPERTY()
		TArray<FGameEvent> qEvents;//TODO obsolete?
	UPROPERTY()
		TArray<FTactic> Tactics;

	TArray<FCombatAttackResultStruct> AttackResults;
	UPROPERTY()
		float AttackDuration = 0.f; //cumulative for melee and ranged if required

	UPROPERTY()
		TArray<FCommand> qCommands;
	UPROPERTY()
		FCommand currentCommand;
	UPROPERTY()
		FCommand previousCommand;

	UPROPERTY()
		int32 bInteractive = 0;
	UPROPERTY()
		int32 bHostilityChanged = 0;
	UPROPERTY()
		int32 bSpawned = 0;
	UPROPERTY()
		int32 bBlend = 0;

	//TODO effects, can be same effect from different abilities? TArray/TSet?
	UPROPERTY()
		TArray<FEffect> Effects;
	//for bitwise ops
	UPROPERTY()
		int32 EffectFlags = 0;

	//abilities list
	UPROPERTY()
		TArray<FAbility> Abilities;

	UPROPERTY()
		TArray<AActor*> Inventory; //Backpack
	TMap<int32, AActor*> INVENTORY_SLOTS; //GEAR
	UPROPERTY()
		int32 nActiveWeaponSet = 0; //main/0 as default, or secondary/1

	//DHK - extrapolated from Properties.json via getproperty()
	//Depletables
	//3-depletable
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties)
		FActorProperty Health;
	//3-depletable
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties)
		FActorProperty Mana_Stamina;
	//2-simple
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties)
		FActorProperty CurrentClass;
	//2-simple //it's all CAPS in Properties.json
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties)
		FActorProperty AI_BEHAVIOR;
	//2-simple
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties)
		FActorProperty Level;
	//2-simple
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties)
		FActorProperty Experience;
	//1-attribute
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties)
		FActorProperty Attack;
	//1-attribute
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties)
		FActorProperty Defense;
	//1-attribute
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties)
		FActorProperty SpellPower;
	//1-attribute
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties)
		FActorProperty Strength;
	//1-attribute
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties)
		FActorProperty Magic;
	//1-attribute. was cunning
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties)
		FActorProperty Intelligence;
	//1-attribute
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties)
		FActorProperty Armor;
	//1-attribute
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties)
		FActorProperty Fatigue;
	//1-attribute AP?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties)
		FActorProperty AP_BONUS;
	//1-attribute
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties)
		FActorProperty DamageBonus;
	//1-attribute
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties)
		FActorProperty CriticalRange;
	//1-attribute
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties)
		FActorProperty Displacement;
	//1-attribute
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties)
		FActorProperty Ranged_Crit_Modifier;
	//1-attribute
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties)
		FActorProperty Melee_Crit_Modifier;
	//1-attribute
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties)
		FActorProperty Flanking_Angle;
	//1-attribute
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties)
		FActorProperty Missile_Shield;
	//1-attribute
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties)
		FActorProperty Ranged_Aim_Speed;
	//1-attribute
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties)
		FActorProperty AttackSpeedModifier;

	//TODO rename creature bXXX?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesBoolean)
		int32 bImmortal = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesBoolean)
		int32 bPlot = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesBoolean)
		int32 bApproval = 0;
	// if disabled on followers, they'll be Forrest Gump :D
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesBoolean)
		int32 bTactics = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesBoolean)
		int32 NoPermDeath = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesBio)
		int32 GENDER = -1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesBio)
		int32 RACE = -1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesBio)
		int32 CORECLASS = -1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesBio)
		int32 TEAM = -1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesBio)
		int32 GROUP = -1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesBio)
		int32 RANK = -1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesBio)
		int32 APPROVAL = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesBio)
		int32 PACKAGE = -1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesBio)
		int32 PACKAGEAI = -1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesBio)
		int32 PACKAGETYPE = -1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesBio)
		int32 CONVERSATION = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesCombat)
		int32 COMBAT_STATE = 0;
	//combatant on by default
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesCombat)
		int32 COMBATANT_TYPE = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesCombat)
		int32 CREATURE_SPAWN_DEAD = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesCombat)
		FName AI_TARGET_OVERRIDE;
	//-1 permanent override
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesCombat)
		int32 AI_TARGET_OVERRIDE_DUR_COUNT = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesCombat)
		int32 SPAWN_HOSTILE_LYING_ON_GROUND = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesCombat)
		int32 GO_HOSTILE_ON_PERCEIVE_PC = 0;
	// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesCombat)
	// 		FName AI_THREAT_TARGET = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesCombat)
		int32 AI_THREAT_HATED_RACE = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesCombat)
		int32 AI_THREAT_HATED_CLASS = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesCombat)
		int32 AI_THREAT_HATED_GENDER = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesCombat)
		int32 AI_THREAT_SWITCH_TIMER_MIN = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesCombat)
		int32 AI_THREAT_TARGET_SWITCH_COUNTER = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesCombat)
		float AI_THREAT_GENERATE_EXTRA_THREAT = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesCombat)
		int32 AI_CUSTOM_AI_ACTIVE = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesCombat)
		int32 CREATURE_HAS_TIMER_ATTACK = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesCombat)
		int32 AI_LIGHT_ACTIVE = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesCombat)
		int32 AI_MOVE_TIMER = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesCombat)
		int32 AI_HELP_TEAM_STATUS = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesCombat)
		int32 AI_LAST_TACTIC = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesCombat)
		int32 AI_WAIT_TIMER = 0;
	// CREATURE_COUNTER_3 used to enable/disable stomp. 1 is for disabled
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesCombat)
		int32 CREATURE_COUNTER_3 = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesCombat)
		int32 COMBAT_LAST_WEAPON = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesParty)
		int32 FOLLOWER_STATE = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ambient)
		int32 RUBBER_HOME_ENABLED = 1;
	//ROAM_RADIUS
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ambient)
		float ROAM_DISTANCE = 1000; //TODO remove hard coded values
	//ROAM_LOCATION
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ambient)
		FVector	HOME_LOCATION = FVector(0.f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ambient)
		int32 AI_DISABLE_PATH_BLOCKED_ACTION = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ambient)
		FGameEvent AMBIENT_EVENT;
	// Bitmask of int32 AMBIENT_SYSTEM_XXX values
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ambient)
		int32 AMBIENT_SYSTEM_STATE = 0;
	// Ambient movement pattern constant int32 AMBIENT_MOVE_XXX (0-7)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ambient)
		int32 AMBIENT_MOVE_PATTERN = 0;
	// Prefix of ambient movement destination (waypoint or creature)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ambient)
		FName AMBIENT_MOVE_PREFIX;
	// Ambient animation pattern - index into ambient_ai.xls
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ambient)
		int32 AMBIENT_ANIM_PATTERN = 0;
	// Animation frequency x.y = min.max, -1.0 = all (in order)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ambient)
		int32 AMBIENT_ANIM_FREQ = 0;
	// If non-zero, takes precedence over int32 AMBIENT_ANIM_PATTERN
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ambient)
		int32 AMBIENT_ANIM_PATTERN_OVERRIDE = 0;
	// If non-zero, takes precedence over int32 AMBIENT_ANIM_FRE
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ambient)
		int32 AMBIENT_ANIM_FREQ_OVERRIDE = 0;
	// If non-zero, the number of times the override animation pattern is used.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ambient)
		int32 AMBIENT_ANIM_OVERRIDE_COUNT = 0;
	// anim state: 0 = start move phase, -1 = start anim phase, +ve = # anims left to play
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ambient)
		int32 AMBIENT_ANIM_STATE = 0;
	// loword = # of the waypoint moved to last, hiword = direction of travel
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ambient)
		int32 AMBIENT_MOVE_STATE = 0;
	// number of NPC/WPs available to move to
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ambient)
		int32 AMBIENT_MOVE_COUNT = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ambient)
		int32 AMBIENT_COMMAND = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ambient)
		int32 AMBIENT_TICK_COUNT = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SoundSet)
		int32 SHOUTS_ACTIVE = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SoundSet)
		FName SHOUTS_DIALOG_OVERRIDE;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SoundSet)
		float SHOUTS_DELAY = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SoundSet)
		int32 AMB_SYSTEM_DIALOG = 0;
	//TODO look more into sounds
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SoundSet)
		int32 SOUND_SET_FLAGS_0 = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SoundSet)
		int32 SOUND_SET_FLAGS_1 = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SoundSet)
		int32 SOUND_SET_FLAGS_2 = 0;

	//bitwise
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesFlags)
		int32 CREATURE_RULES_FLAG0 = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesFlags)
		int32 AI_FLAG_PREFERS_RANGED = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesFlags)
		int32 AI_FLAG_STATIONARY = 0;

private:

	FStringAssetReference HEDAssetToLoad;
	FStringAssetReference ARMAssetToLoad;
	FStringAssetReference BOOAssetToLoad;
	FStringAssetReference GLVAssetToLoad;
	FStringAssetReference HARAssetToLoad;
	FStringAssetReference EYEAssetToLoad;

	FStringAssetReference HEDMatAssetToLoad;
	FStringAssetReference ARMMatAssetToLoad;
	FStringAssetReference BOOMatAssetToLoad;
	FStringAssetReference GLVMatAssetToLoad;
	FStringAssetReference HARMatAssetToLoad;
	FStringAssetReference EYEMatAssetToLoad;

	// Method to setup/initialize skeletal mesh components
	void InitSkeletalMeshComponent(TWeakObjectPtr<class USkeletalMeshComponent> SMeshPointer, bool AttachToParent);

	// Method to setup the new character
	void InitDefaultMeshes(int32 HEDID, int32 ARMID, int32 BOOID, int32 GLVID, int32 HARID, int32 EYEID);

	// Delegates to be "shoot" at end of loading processes
	void DoAsyncHEDMeshChange();
	void DoAsyncARMMeshChange();
	void DoAsyncBOOMeshChange();
	void DoAsyncGLVMeshChange();
	void DoAsyncHARMeshChange();
	void DoAsyncEYEMeshChange();

	void DoAsyncHEDMatChange();
	void DoAsyncARMMatChange();
	void DoAsyncBOOMatChange();
	void DoAsyncGLVMatChange();
	void DoAsyncHARMatChange();
	void DoAsyncEYEMatChange();

	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* TopDownCameraComponent = nullptr;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom = nullptr;

protected:
	UFUNCTION()
		void OnSeePlayer(APawn* Pawn);

	UFUNCTION()
		void OnHearNoise(APawn* Pawn, const FVector& Location, float Volume);

	UFUNCTION()//BlueprintNativeEvent, Category = "Collision")
		void BeginCursorOver(UPrimitiveComponent* HitComp);

	UFUNCTION()//BlueprintNativeEvent, Category = "Collision")
		void EndCursorOver(UPrimitiveComponent* HitComp);

	UFUNCTION(Category = Default)
		void OnClick(UPrimitiveComponent* HitComp, FKey ButtonClicked = EKeys::LeftMouseButton);
};

