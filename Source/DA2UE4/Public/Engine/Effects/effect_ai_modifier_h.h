#pragma once

// -----------------------------------------------------------------------------
// effect_ai_modifier
// -----------------------------------------------------------------------------
/*
Effect: AI Modifier

Used to apply modifiers taken into account by the game AI to the
creature

Example: AI_MODIFIER_IGNORE - AI ignores the target as if it was
not there

All these required manual integration into the game AI

*/
// -----------------------------------------------------------------------------
// Owner: Georg Zoeller
// -----------------------------------------------------------------------------


#include "log_h.h"
#include "core_h.h"
#include "m2da_constants_h.h"
#include "wrappers_h.h"
#include "events_h.h"
#include "effect_constants_h.h"

const int32 AI_MODIFIER_IGNORE = 1;

int32 Effects_HasAIModifier(AActor* oTarget, int32 nModifier);