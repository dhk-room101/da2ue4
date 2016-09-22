#include "DA2UE4.h"
#include "effects_h.h"

FEffect EffectModifyMovementSpeed(float fPotency, int32 bHostile)
{
	FEffect eSlow;
	if (bHostile)
	{
		eSlow = Effect(EFFECT_TYPE_MOVEMENT_RATE_DEBUFF);
	}
	else
	{
		eSlow = Effect(EFFECT_TYPE_MOVEMENT_RATE);
	}
	eSlow = SetEffectFloat(eSlow, /*EFFECT_FLOAT_POTENCY, */fPotency);
	return eSlow;
}
