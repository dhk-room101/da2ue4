// Fill out your copyright notice in the Description page of Project Settings.

#include "DA2UE4.h"
#include "DA2UE4Creature.h"
#include "DA2UE4CreatureController.h"
#include "DA2UE4TargetPoint.h"

//custom
#include "events_h.h"
#include "ldf.h"

ADA2UE4CreatureController::ADA2UE4CreatureController(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void ADA2UE4CreatureController::Possess(class APawn* InPawn)
{
	Super::Possess(InPawn);

	ADA2UE4Creature* aCreature = Cast<ADA2UE4Creature>(InPawn);

	if (aCreature)
	{
		SpawnCreature(InPawn);

		//add hostility defaults
		aCreature->SetDefaultHostilities();
	}
}

void ADA2UE4CreatureController::UnPossess()
{
	Super::UnPossess();
}

void ADA2UE4CreatureController::TriggerEventPerceptionAppear(APawn* SensedPawn, int32 bPerceived, int32 bHostile, int32 bAbility, int32 bHostilityChanged)
{
	if (bPerceived)
	{
		FGameEvent ev = Event(EVENT_TYPE_PERCEPTION_APPEAR);
		ev = SetEventObject(ev, 0, SensedPawn);
		ev = SetEventInteger(ev, 0, bHostile);//Hostile True
		ev = SetEventInteger(ev, 1, bAbility);//Stealth false
		ev = SetEventInteger(ev, 2, bHostilityChanged);//Hostility changed True
		SignalEvent(GetPawn(), ev);
	}
	else
	{
		FGameEvent ev = Event(EVENT_TYPE_PERCEPTION_DISAPPEAR);
		ev = SetEventObject(ev, 0, SensedPawn);
		SignalEvent(GetPawn(), ev);
	}
}

void ADA2UE4CreatureController::SpawnCreature(APawn* InPawn)
{
	ADA2UE4Creature* aCreature = Cast<ADA2UE4Creature>(InPawn);

	//trigger EVENT_TYPE_SPAWN
	FGameEvent ev = Event(EVENT_TYPE_SPAWN);
	ev = SetEventObject(ev, 0, this);
	ev = SetEventObject(ev, 0, aCreature);
	SignalEvent(aCreature, ev);
}