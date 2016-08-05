// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "ldf.h"
#include "STypes.h"
#include "GameFramework/Character.h"
#include "DA2UE4Creature.generated.h"

UCLASS(config = Game)
class ADA2UE4Creature : public ACharacter
{
	GENERATED_BODY()

	/* Last time the player was spotted */
	float LastSeenTime;

	float SensedTimeOut;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, Category = "AI")
		class UPawnSensingComponent* PawnSensingComp;
	UPROPERTY(VisibleAnywhere, Category = "AI")
		class USphereComponent* MeleeRingComp;

protected:
	/* Triggered by pawn sensing component when a pawn is spotted */
	/* When using functions as delegates they need to be marked with UFUNCTION(). We assign this function to FSeePawnDelegate */
	UFUNCTION()
		void OnSeePlayer(APawn* Pawn);
	UFUNCTION()
		void TriggerEnter(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
		void TriggerExit(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	//default values for reference
	int32 DefaultRank = -1; //TODO save the creature rank if joins player

public:

	ADA2UE4Creature(const class FObjectInitializer& ObjectInitializer);

	/*add creature component*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Custom, meta = (AllowPrivateAccess = "true"))
		class UDA2UE4CreatureComponent* DA2UE4Creature;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Custom, meta = (AllowPrivateAccess = "true"))
		class UDA2UE4PlayerComponent* DA2UE4Player;

	/*DHK*/
	void SetDefaultHostilities();
	void AddThreat(APawn* oThreat, float fThreat);
	void JoinParty();
	void LeaveParty();
	int32 bScriptHasCustom = false; //overrides creature only
	int32 bScriptCoreRedirected = false; //creature or player
	int32 bScriptRulesRedirected = false; //rules
	int32 bHostilityChanged = false;
	int32 bInteractive = true;

	int32 EffectFlags; //for bitwise ops

	//queue events
	//TODO remove reflection from commands
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Commands)
		TArray<FCommand> qCommands;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Commands)
		FCommand currentCommand;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Commands)
		FCommand previousCommand;

	//queue events
	TArray<FGameEvent> qEvents;

	//TODO effects, can be same effect from different abilities? TArray/TSet?
	TArray<FEffect> Effects;

	//TODO extrapolate based on active effects, creator, timestamp, etc
	//abilities active
	//TSet<int32> AbilitiesActive;

	//abilities list
	TSet<FAbility> Abilities;

	TArray<AActor*> Inventory; //Backpack
	TMap<int32, AActor*> INVENTORY_SLOTS; //GEAR
	int32 nActiveWeaponSet = 0; //main/0 as default, or secondary/1


	//groups that this creatures should be hostile toward
	TSet<int32> HostilityTargetGroup;

	TSet<APawn*> SensedPawns; //@sa Threats
	TSet<APawn*> MeleeRingPawns;

	//threat list to be sorted
	TArray<FThreat> Threats; //@sa SensedPawns

	TArray<FTactic> Tactics;

	TArray<FCombatAttackResultStruct> AttackResults;
	float AttackDuration; //cumulative for melee and ranged if required

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesBoolean)
		int32 bImmortal;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesBoolean)
		int32 bPlot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesBoolean)
		int32 bApproval;
	// if disabled on followers, they'll be Forrest Gump :D
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesBoolean)
		int32 bTactics;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesBoolean)
		int32 NoPermDeath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesBio)
		int32 GENDER;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesBio)
		int32 RACE;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesBio)
		int32 CORECLASS;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesBio)
		int32 TEAM = -1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesBio)
		int32 GROUP = -1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesBio)
		int32 RANK = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesBio)
		int32 APPROVAL = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesBio)
		int32 PACKAGE;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesBio)
		int32 PACKAGEAI;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesBio)
		int32 PACKAGETYPE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesCombat)
		int32 COMBAT_STATE = 0;
	//combatant on by default
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesCombat)
		int32 COMBATANT_TYPE = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesCombat)
		int32 CREATURE_SPAWN_DEAD = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesCombat)
		AActor* AI_TARGET_OVERRIDE = nullptr;
	//-1 permanent override
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesCombat)
		int32 AI_TARGET_OVERRIDE_DUR_COUNT = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesCombat)
		int32 SPAWN_HOSTILE_LYING_ON_GROUND = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesCombat)
		int32 GO_HOSTILE_ON_PERCEIVE_PC = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesCombat)
		AActor* AI_THREAT_TARGET = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesCombat)
		int32 AI_THREAT_HATED_RACE;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesCombat)
		int32 AI_THREAT_HATED_CLASS;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesCombat)
		int32 AI_THREAT_HATED_GENDER;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesCombat)
		int32 AI_THREAT_SWITCH_TIMER_MIN;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesCombat)
		int32 AI_THREAT_TARGET_SWITCH_COUNTER;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesCombat)
		float AI_THREAT_GENERATE_EXTRA_THREAT;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesCombat)
		int32 AI_CUSTOM_AI_ACTIVE;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesCombat)
		int32 CREATURE_HAS_TIMER_ATTACK;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesCombat)
		int32 AI_LIGHT_ACTIVE;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesCombat)
		int32 AI_MOVE_TIMER;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesCombat)
		int32 AI_HELP_TEAM_STATUS;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesCombat)
		int32 AI_LAST_TACTIC;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesCombat)
		int32 AI_WAIT_TIMER;
	// CREATURE_COUNTER_3 used to enable/disable stomp. 1 is for disabled
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesCombat)
		int32 CREATURE_COUNTER_3 = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesCombat)
		int32 COMBAT_LAST_WEAPON;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesParty)
		int32 FOLLOWER_STATE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ambient)
		int32 RUBBER_HOME_ENABLED = 1;
	//ROAM_RADIUS
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ambient)
		float ROAM_DISTANCE = 1500; //TODO remove hard coded values
	//ROAM_LOCATION
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ambient)
		FVector	HOME_LOCATION; 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ambient)
		int32 AI_DISABLE_PATH_BLOCKED_ACTION;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ambient)
		FGameEvent AMBIENT_EVENT;
	// Bitmask of int32 AMBIENT_SYSTEM_XXX values
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ambient)
		int32 AMBIENT_SYSTEM_STATE;
	// Ambient movement pattern constant int32 AMBIENT_MOVE_XXX (0-7)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ambient)
		int32 AMBIENT_MOVE_PATTERN;
	// Prefix of ambient movement destination (waypoint or creature)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ambient)
		int32 AMBIENT_MOVE_PREFIX;
	// Ambient animation pattern - index into ambient_ai.xls
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ambient)
		int32 AMBIENT_ANIM_PATTERN;
	// Animation frequency x.y = min.max, -1.0 = all (in order)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ambient)
		int32 AMBIENT_ANIM_FREQ;
	// If non-zero, takes precedence over int32 AMBIENT_ANIM_PATTERN
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ambient)
		int32 AMBIENT_ANIM_PATTERN_OVERRIDE;
	// If non-zero, takes precedence over int32 AMBIENT_ANIM_FRE
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ambient)
		int32 AMBIENT_ANIM_FREQ_OVERRIDE;
	// If non-zero, the number of times the override animation pattern is used.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ambient)
		int32 AMBIENT_ANIM_OVERRIDE_COUNT;
	// anim state: 0 = start move phase, -1 = start anim phase, +ve = # anims left to play
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ambient)
		int32 AMBIENT_ANIM_STATE;
	// loword = # of the waypoint moved to last, hiword = direction of travel
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ambient)
		int32 AMBIENT_MOVE_STATE;
	// number of NPC/WPs available to move to
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ambient)
		int32 AMBIENT_MOVE_COUNT;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ambient)
		int32 AMBIENT_COMMAND;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ambient)
		int32 AMBIENT_TICK_COUNT;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SoundSet)
		int32 SHOUTS_ACTIVE;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SoundSet)
		FString SHOUTS_DIALOG_OVERRIDE = "NONE";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SoundSet)
		float SHOUTS_DELAY;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SoundSet)
		int32 AMB_SYSTEM_DIALOG;
	//TODO look more into sounds
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SoundSet)
		int32 SOUND_SET_FLAGS_0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SoundSet)
		int32 SOUND_SET_FLAGS_1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SoundSet)
		int32 SOUND_SET_FLAGS_2;

	//bitwise
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesFlags)
		int32 CREATURE_RULES_FLAG0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesFlags)
		int32 AI_FLAG_PREFERS_RANGED;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PropertiesFlags)
		int32 AI_FLAG_STATIONARY;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

