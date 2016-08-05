#pragma once

// -----------------------------------------------------------------------------
// effect_resurrection_h
// -----------------------------------------------------------------------------
/*
Effect to revive a dead creature.
'Dead' is defined as health <= 0
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

// -----------------------------------------------------------------------------
// This defines the distance the player has to have to his party members
// before they resurrect after combat.
//
// Put it too long and people will stand up and get whacked down by monsters
// because the player lost perception to them
//
// Put it too short and it gets tedious.
// -----------------------------------------------------------------------------
const float RULES_RESURRECTION_DISTANCE = 7.5f;

void ResurrectPartyMembers(int32 bHealFull = FALSE_);

/** ----------------------------------------------------------------------------
@brief Returns an effect which restores a dead creature to life.
*
* Constructor for the resurrection effect. When applied to a dead creature, this
* effect instantly restores the creature to life with 1 health. If the creature
* is alive then this effect does nothing.
*
* @author David Sims
*
* @return a valid effect of type EFFECT_TYPE_RESURRECTION.
-----------------------------------------------------------------------------**/
FEffect EffectResurrection(int32 bApplyInjury = FALSE_, int32 bHealFull = FALSE_);

// -----------------------------------------------------------------------------
// This utility function handles the application of EffectResurrection and
// should never be called directly except in Effects_h
// -----------------------------------------------------------------------------
int32 Effects_HandleApplyEffectResurrection(FEffect eEffect, AActor* oTarget);

void HandlePlayerResurrect(AActor* oCreature, int32 bApplyInjury = TRUE_);