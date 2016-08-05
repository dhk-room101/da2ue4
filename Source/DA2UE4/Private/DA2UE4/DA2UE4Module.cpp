// Fill out your copyright notice in the Description page of Project Settings.

#include "DA2UE4.h"
#include "DA2UE4Module.h"
#include "ldf.h"
#include "log_h.h"
#include "events_h.h"

// Sets default values
ADA2UE4Module::ADA2UE4Module()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADA2UE4Module::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADA2UE4Module::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if (this->qEvents.Num() == 0) { return; }

	LogWarning("Game Instance");

	FGameEvent ev = GetCurrentEvent(this);
	if (ev.nGameEventType == EVENT_TYPE_INVALID) return; //possible delayed events in queue
	int32 nEventType = GetEventType(ev);

#ifdef DEBUG
	ev = SetEventString(ev, 0, *(this->GetName()));
	Log_Events("", ev);
#endif

	switch (nEventType)
	{
	case EVENT_TYPE_DELAYED_GM_CHANGE:
	{
		break;
	}
	default:
	{
#ifdef DEBUG
		LogWarning("module event not found");
#endif
		break;
	}
	}

}

