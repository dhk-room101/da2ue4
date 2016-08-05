#pragma once

// -----------------------------------------------------------------------------
// Effect Includes
// -----------------------------------------------------------------------------
/*
This is the top level include for the effects system. All other files related
to this system can be found under \_Game Effects\.

Include Hierarchy:

Effects_h includes \Game_Effects\effect_*_h includes "Effects_constants_h.h"
includes core_h


*/
// -----------------------------------------------------------------------------
// Owner: Georg Zoeller
// -----------------------------------------------------------------------------

#include "effect_constants_h.h"
#include "events_h.h"
//#include "wrappers_h.h"
#include "m2da_constants_h.h"
#include "core_h.h"
//#include "effect_charm_h.h"
#include "ai_threat_h.h"
#include "STypes.h"

class Engine;

FEffect EffectModifyMovementSpeed(float fPotency, int32 bHostile = FALSE_);
