#include "DA2UE4.h"
#include "trigger_h.h"

#include "trigger_constants_h.h"

#include "ldf.h"
#include "core_h.h"
#include "utility_h.h"

#include "demo_constants_h.h"

void HandleTrigger(int64 nTrigger, AActor* aActor, AActor* aTrigger)
{
	switch (nTrigger)
	{
	case DEMO200TR_ATTACK:
	{
		if (IsPartyMember(aActor) == TRUE_)
		{
			(Cast<APawn>(aActor))->GetMovementComponent()->StopMovementImmediately();

			UT_Talk(aActor, DEMO_CONFRONTATION_R);
			
			Safe_Destroy_Object(aTrigger, 0);
		}

		break;
	}
	default:
		LogError("Unknown trigger " + Int64ToString(nTrigger));
		break;
	}
}
