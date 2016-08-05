// Fill out your copyright notice in the Description page of Project Settings.
//TODO do to player the same as creature when/if needed

#include "DA2UE4.h"
#include "DA2UE4Creature.h"
#include "DA2UE4PlayerComponent.h"
#include "ldf.h"
#include "log_h.h"
#include "core_h.h"
#include "events_h.h"
#include "utility_h.h"

void UDA2UE4PlayerComponent::EquipDefaultItem(AActor* OBJECT_SELF, int32 nAppType, int32 nTargetSlot, FString s2daColumnName)
{
	// Equip default creature item based on appearance (only if none equipped yet)
	AActor* oItem = GetItemInEquipSlot(nTargetSlot, OBJECT_SELF);
	FString rDefaultCreatureItem;
	AActor* oDefaultCreatureoItem;
	FString sItem;
	if (!IsObjectValid(oItem))
	{
		rDefaultCreatureItem = GetM2DAString(TABLE_APPEARANCE, s2daColumnName, nAppType);
		if (rDefaultCreatureItem == "****")
			rDefaultCreatureItem = sItem = s2daColumnName;
		else sItem = rDefaultCreatureItem;
#ifdef DEBUG
		Log_Trace(LOG_CHANNEL_EVENTS, "player_core.EVENT_TYPE_SPAWN", "Default creature item for this appearance: [" + sItem + "] for inventory slot: " +
			IntToString(nTargetSlot));
#endif
		if (sItem != "")
		{
			oDefaultCreatureoItem = CreateItemOnObject(rDefaultCreatureItem, OBJECT_SELF);
			SetItemDroppable(oDefaultCreatureoItem, FALSE_);
			if (IsObjectValid(oDefaultCreatureoItem))
			{
#ifdef DEBUG
				Log_Trace(LOG_CHANNEL_EVENTS, "player_core.EVENT_TYPE_SPAWN", "Creating default item on creature");
#endif
				EquipItem(OBJECT_SELF, oDefaultCreatureoItem, nTargetSlot);
			}
		}
	}
}


// Called every frame
void UDA2UE4PlayerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ADA2UE4Creature* OBJECT_SELF = Cast<ADA2UE4Creature>(this->GetOwner());

	//if not player, return
	if (OBJECT_SELF->RANK != CREATURE_RANK_PLAYER) { return; }

	//if no event, return
	if (OBJECT_SELF->qEvents.Num() == 0) { return; }

#ifdef DEBUG
	LogWarning("player component");
#endif // DEBUG

	FGameEvent ev = GetCurrentEvent(this->GetOwner());
	if (ev.nGameEventType == EVENT_TYPE_INVALID) return; //possible delayed events in queue
	int32 nEventType = GetEventType(ev);

#ifdef DEBUG
	ev = SetEventString(ev, 0, *(this->GetName()));
	Log_Events("", ev);
#endif

	switch (nEventType)
	{
	case EVENT_TYPE_SPAWN:
	{
		//TODO Implement EVENT_TYPE_SPAWN
#ifdef DEBUG
		LogWarning("I'm alive " + this->GetOwner()->GetName());
#endif

		// -----------------------------------------------------------------
		// Force default item equip from the 2da
		// -----------------------------------------------------------------
		int32 nAppType = GetAppearanceType(OBJECT_SELF);
		EquipDefaultItem(OBJECT_SELF, nAppType, INVENTORY_SLOT_MAIN, "DefaultWeapon");
		EquipDefaultItem(OBJECT_SELF, nAppType, INVENTORY_SLOT_CHEST, "DefaultArmor");

		break;
	}
	default:
	{
#ifdef DEBUG
		LogWarning("player event not found, redirecting to rules");
#endif
		OBJECT_SELF->qEvents.Insert(ev, 0);//reinsert
		OBJECT_SELF->bScriptRulesRedirected = TRUE_;
		break;
	}
	}
}
