#include "DA2UE4.h"
#include "DA2UE4Creature.h"
#include "sys_rubberband_h.h"
#include "ldf.h"
#include "wrappers_h.h"

FVector Rubber_GetHome(AActor* aActor)
{
	if (aActor == nullptr) return FVector(0.f);
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		return oCreature->HOME_LOCATION;
	}
	else LogError("Rubber_GetHome: unknown actor type");
	return FVector(0.f);
}

void Rubber_SetHome(AActor* aActor)
{
	if (aActor == nullptr) return;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		oCreature->HOME_LOCATION = aActor->GetActorLocation();
	}
	else LogError("Rubber_SetHome: unknown actor type");
}

void SetRoamRadius(AActor* aActor, float fRadius)
{
	if (aActor == nullptr) return;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		oCreature->ROAM_DISTANCE = fRadius;
	}
	else LogError("Rubber_SetHome: unknown actor type");
}