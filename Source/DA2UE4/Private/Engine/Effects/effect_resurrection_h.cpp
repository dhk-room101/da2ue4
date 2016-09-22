#include "DA2UE4.h"
#include "effect_resurrection_h.h"
#include "ldf.h"
#include "effect_constants_h.h"
#include "core_h.h"

void ResurrectPartyMembers(int32 bHealFull)
{

	FEffect eRez = EffectResurrection(TRUE_, bHealFull);
	eRez = SetEffectCreator(eRez, GetHero());//GetModule());

	TArray<AActor*> partyMembers = GetPartyList();
	int32 nMembers = partyMembers.Num();
	int32 i;

	for (i = 0; i < nMembers; i++)
	{
		if (IsDead(partyMembers[i]))
		{
			Effects_HandleApplyEffectResurrection(eRez, partyMembers[i]);
		}
	}
}

FEffect EffectResurrection(int32 bApplyInjury, int32 bHealFull)
{
	FEffect eEffect = Effect(EFFECT_TYPE_RESURRECTION);
	eEffect = SetEffectInteger(eEffect, bApplyInjury);
	eEffect = SetEffectInteger(eEffect, bHealFull);
	return eEffect;
}

int32 Effects_HandleApplyEffectResurrection(FEffect eEffect, AActor* oTarget)
{
	int32 bHealFull = GetEffectInteger(eEffect, 1);

	// -------------------------------------------------------------------------
	// Only dead creatures can be resurrected.
	// -------------------------------------------------------------------------
	if (IsDead(oTarget))
	{

		if (!bHealFull)
		{
			SetCurrentHealth(oTarget, 1.00f);
		}
		else
		{
			SetCurrentHealth(oTarget, GetMaxHealth(oTarget));
		}

		// flag injury system
		HandlePlayerResurrect(oTarget, GetEffectInteger(eEffect, 0));
	}
	return TRUE_;
}

void HandlePlayerResurrect(AActor* oCreature, int32 bApplyInjury)
{
	// --------------------------------------------------------------------
	// Restart AI level after it was frozen...
	// --------------------------------------------------------------------
#ifdef DEBUG
	Log_Trace(LOG_CHANNEL_CHARACTER, "HandleEvent_Resurrection", "Forcing AI level normal, then unlocking it!");
#endif // DEBUG

	// Restore AI level on target.
	SetAILevel(oCreature, CSERVERAIMASTER_AI_LEVEL_INVALID);
	SetCreatureFlag(oCreature, CREATURE_RULES_FLAG_DYING, FALSE_);
	// SetCreatureGoreLevel(oCreature, 0.0);

	FGameEvent evRezz = Event(EVENT_TYPE_RESURRECTION);
	evRezz = SetEventInteger(evRezz, bApplyInjury);

	// -------------------------------------------------------------------------
	// Resurrection comes with a free, 25% heal
	// -------------------------------------------------------------------------
	float fHealth = MaxF(1.0f, GetMaxHealth(oCreature) * 0.25);
	SetCurrentHealth(oCreature, fHealth);

	SignalEvent(oCreature, evRezz);
}