#pragma once

//::///////////////////////////////////////////////
//:: sys_rubberband_h
//:: Copyright (c) 2007 Bioware Corp.
//:://////////////////////////////////////////////
/*
Implementation of the homing 'rubberband' system for creature AI.
*/
//:://////////////////////////////////////////////
//:: Created By: Ferret Baudoin
//:: Created On: Aug. 27, 2007
//:://////////////////////////////////////////////                       

class Engine;

// const FString RUBBER_HOME_LOCATION_X = "RUBBER_HOME_LOCATION_X";
// const FString RUBBER_HOME_LOCATION_Y = "RUBBER_HOME_LOCATION_Y";
// const FString RUBBER_HOME_LOCATION_Z = "RUBBER_HOME_LOCATION_Z";
// const FString RUBBER_HOME_LOCATION_FACING = "RUBBER_HOME_LOCATION_FACING";

/** @brief Gets a creature's home location.
*
* @param oTarget The creature whose home location to fetch.
**/
FVector Rubber_GetHome(AActor* aActor);

void Rubber_SetHome(AActor* aActor);

/** @brief Sets the roam radius for the creature.
*
*
*   @author Pat LaBine
*
*/
void SetRoamRadius(AActor* oCreature, float fRadius);