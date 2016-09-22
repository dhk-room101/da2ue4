#pragma once

class Engine;

#include "ldf.h"

int32 WR_ClearAllCommands(AActor* oActor, int32 bHardClear = FALSE_);

void WR_AddCommand(AActor* oActor, FCommand cCommand, int32 bAddToFront = FALSE_, int32 bStatic = FALSE_, int32 nOverrideAddBehavior = -1, float fTimeout = 0.0);

void WR_SetObjectActive(AActor* aActor, int32 bStatus, int32 nAnimation = -1, int32 nVFX = -1);

// -----------------------------------------------------------------------------
// @brief  Forces two creatures to see each other,implicitly canceling any stealth
//         that might exist on the perceived creature
// @author georg
// -----------------------------------------------------------------------------
int32 WR_TriggerPerception(AActor* oPerceiver, AActor* oPerceivedCreature);

void WR_SetGameMode(int32 nGameMode);

int32 WR_GetPlotFlag(int64 nPlotHash, int32 nFlag); //int32 nCallScript = FALSE_ in original

void WR_SetPlotFlag(int64 nPlotHash, int32 nFlag, int32 nValue); //int32 nCallScript = FALSE_ in original