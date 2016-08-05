// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "STypes.h"
#include "DA2UE4Module.generated.h"

class ADA2UE4Creature;

UCLASS()
class DA2UE4_API ADA2UE4Module : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADA2UE4Module();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(VisibleAnywhere, Category = "Module Global Variables") //EditDefaultsOnly
		ADA2UE4Creature* PLAYER_LEADER_STORE = nullptr;

	//TODO getter/setter?
	//GM_INVALID
	UPROPERTY(VisibleAnywhere, Category = "Module Global Variables")
		int32 GAME_MODE = -1;

	UPROPERTY(VisibleAnywhere, Category = "Module Global Variables")
		int32 DISABLE_FOLLOWER_DIALOG;
	UPROPERTY(VisibleAnywhere, Category = "Module Global Variables")
		FString PARTY_OVERRIDE_DIALOG = "NONE";
	UPROPERTY(VisibleAnywhere, Category = "Module Global Variables")
		int32 PARTY_OVERRIDE_DIALOG_ACTIVE;
	UPROPERTY(VisibleAnywhere, Category = "Module Global Variables")
		int32 AMB_SYSTEM_DIALOG;
	UPROPERTY(VisibleAnywhere, Category = "Module Global Variables")
		int32 AI_PARTY_CLEAR_TO_ATTACK;
	UPROPERTY(VisibleAnywhere, Category = "Module Global Variables")
		int32 AI_DISABLE_TABLES;
	//yes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Module Global Variables")
		int32 AI_USE_GUI_TABLES_FOR_FOLLOWERS = 1;

	TArray<AActor*> Party;

	//queue events
	TArray<FGameEvent> qEvents;
	
};
