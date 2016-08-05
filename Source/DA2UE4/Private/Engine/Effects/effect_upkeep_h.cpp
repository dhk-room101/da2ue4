#include "DA2UE4.h"
#include "DA2UE4Creature.h"
#include "effect_upkeep_h.h"
#include "ui_h.h"

void Effects_RemoveUpkeepEffect(AActor* oCaster, int32 nAbilityId)
{
	if (oCaster->IsA(ADA2UE4Creature::StaticClass()))
	{
		TArray<FEffect> effects = GetEffects(oCaster, EFFECT_TYPE_UPKEEP, nAbilityId);

		int32 nSize = effects.Num();
		int32 i;

		Log_Trace(LOG_CHANNEL_EFFECTS, "effect_upkeep_h.Effects_RemoveUpkeepEffect", "removing " + IntToString(nSize) + " upkeep effects for ability id: " + IntToString(nAbilityId), oCaster);
		UI_DisplayAbilityMessage(oCaster, nAbilityId, TRUE_);

		for (i = 0; i < nSize; i++)
		{
			RemoveEffect(oCaster, effects[i]);
		}
	}
	else LogError("Effects_RemoveUpkeepEffect: unknown actor type");
}