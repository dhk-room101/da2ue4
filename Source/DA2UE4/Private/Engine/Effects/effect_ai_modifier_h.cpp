#include "DA2UE4.h"
#include "effect_ai_modifier_h.h"

int32 Effects_HasAIModifier(AActor* oTarget, int32 nModifier)
{
	if (!IsObjectValid(oTarget))
	{
		return FALSE_;
	}

	TArray<FEffect> eEffects = GetEffects(oTarget, EFFECT_TYPE_AI_MODIFIER);

	// Note: If this ends up to be a performance liability, we can make
	//       the effect write a stat integer on the creature instead.

	// In general I expect this loop to have 1 or 2 entries tops.

	int32 nSize = eEffects.Num();
	int32 i;

	for (i = 0; i < nSize; i++)
	{
		if (GetEffectInteger(eEffects[i], 0) == nModifier)
		{
			return TRUE_;
		}
	}

	return FALSE_;
}