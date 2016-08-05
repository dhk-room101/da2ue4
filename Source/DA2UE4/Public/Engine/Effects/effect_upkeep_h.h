#pragma once

// -----------------------------------------------------------------------------
// effect_upkeep.nss
// -----------------------------------------------------------------------------
/*
Effect: Upkeep


*/
// -----------------------------------------------------------------------------
// Owner: Georg Zoeller
// -----------------------------------------------------------------------------

#include "m2da_constants_h.h"
#include "events_h.h"
#include "effect_constants_h.h"
//#include "attributes_h"
//#include "config_h"
//#include "ui_h"

const int32 PROPERTY_ATTRIBUTE_FATIGUE = 41;

void Effects_RemoveUpkeepEffect(AActor* oCaster, int32 nAbilityId);