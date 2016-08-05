#include "DA2UE4.h"
#include "DA2UE4Creature.h"
#include "commands_h.h"
#include "ldf.h"

int32 GetCommandType(FCommand cCommand)
{
	return cCommand.nCommandType;
}

FCommand GetPreviousCommand(AActor* aActor)
{
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* aCreature = Cast<ADA2UE4Creature>(aActor);

		return aCreature->previousCommand;
	}
	else LogError("GetPreviousCommand: unknown actor type");
	return Command(COMMAND_TYPE_INVALID);
}

FCommand GetCurrentCommand(AActor* aActor)
{
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* aCreature = Cast<ADA2UE4Creature>(aActor);

		return aCreature->currentCommand;
	}
	else LogError("GetCurrentCommand: unknown actor type");
	return Command(COMMAND_TYPE_INVALID);
}

int32 GetCommandQueueSize(AActor* aActor)
{
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		return oCreature->qCommands.Num();
	}
	else LogError("GetCommandQueueSize: unknown actor type");
	return 0;
}

FCommand GetCommandByIndex(AActor* aActor, int32 nIndex)
{
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		if (nIndex < oCreature->qCommands.Num()) return oCreature->qCommands[nIndex];
		else return FCommand(COMMAND_TYPE_INVALID);
	}
	else LogError("GetCommandByIndex: unknown actor type");
	return FCommand(COMMAND_TYPE_INVALID);
}

int32 AddCommand(AActor* aActor, FCommand cCommand, int32 bAddToFront, int32 bStatic, int32 nOverrideAddBehavior)
{
	//TODO AddCommand nOverrideAddBehavior to be implemented
	if (nOverrideAddBehavior != -1) LogError("nOverrideAddBehavior to be implemented!!!");
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		cCommand.bStatic = bStatic; //must finish
		if (bAddToFront)
			oCreature->qCommands.Insert(cCommand, 0);
		else oCreature->qCommands.Add(cCommand);
	}
	else LogError("GetCommandByIndex: unknown actor type");
	return 0;
}

void SetCommandResult(AActor* aActor, int32 nResult)
{
	if (aActor == nullptr) return;
	if (aActor->IsA(ADA2UE4Creature::StaticClass()))
	{
		ADA2UE4Creature* oCreature = Cast<ADA2UE4Creature>(aActor);
		oCreature->currentCommand.nCommandResult = nResult;
	}
	else LogError("SetCommandResult: unknown actor type");
}

FCommand CommandPlayAnimation(int32 nAnimation, int32 nLoops, int32 bPlayNext, int32 bBlendIn, int32 bRandomizeOffset)
{
	FCommand cCommand = Command(COMMAND_TYPE_PLAY_ANIMATION);
	cCommand = SetCommandInt(cCommand, nAnimation, 0);
	cCommand = SetCommandInt(cCommand, nLoops, 1);
	cCommand = SetCommandInt(cCommand, bPlayNext, 2);
	cCommand = SetCommandInt(cCommand, bBlendIn, 3);
	cCommand = SetCommandInt(cCommand, bRandomizeOffset, 4);
	return cCommand;
}

//TODO CommandMoveAwayFromObject with bTowards = false and GM_COMBAT ON and away from top threat
FCommand CommandMoveAwayFromObject(AActor* oTarget, float fAwayDistance, int32 bRunToLocation)
{
	FCommand cCommand = Command(COMMAND_TYPE_MOVE_TO_LOCATION);
	cCommand = SetCommandFloat(cCommand, fAwayDistance, 0);
	return cCommand;
}

FCommand CommandAttack(AActor* oTarget, int32 nForcedResult)
{
	FCommand cCommand = Command(COMMAND_TYPE_ATTACK);
	cCommand = SetCommandObject(cCommand, oTarget, 0);
	cCommand = SetCommandInt(cCommand, nForcedResult, 0);
	return cCommand;
}

FCommand CommandSwitchWeaponSet(int32 nWeaponSet)
{
	FCommand cCommand = Command(COMMAND_TYPE_SWITCH_WEAPON_SETS);
	cCommand = SetCommandInt(cCommand, nWeaponSet, 0);
	return cCommand;
}

FCommand CommandMoveToActor(AActor* oTarget, int bRunToLocation, float fMinRange, int bUseOriginalPosition, float fMaxRange)
{
	FCommand cCommand = Command(COMMAND_TYPE_MOVE_TO_OBJECT);
	cCommand = SetCommandObject(cCommand, oTarget, 0);
	cCommand = SetCommandInt(cCommand, bRunToLocation, 0);
	cCommand = SetCommandFloat(cCommand, fMinRange, 0);
	cCommand = SetCommandInt(cCommand, bUseOriginalPosition, 1);
	cCommand = SetCommandFloat(cCommand, fMaxRange, 1);
	return cCommand;
}

FCommand CommandMoveToLocation(FVector vVector, int32 bRunToLocation, int32 bDeactivateAtEnd)
{
	FCommand cCommand = Command(COMMAND_TYPE_MOVE_TO_LOCATION);
	cCommand = SetCommandVector(cCommand, vVector, 0);
	cCommand = SetCommandInt(cCommand, bRunToLocation, 0);
	cCommand = SetCommandInt(cCommand, bDeactivateAtEnd, 1);
	return cCommand;
}

FCommand CommandMoveToMultiLocations(TArray<FVector> vVectors, int32 bRunToLocation, int32 nStartingWP, int32 bLoop)
{
	LogError("CommandMoveToMultiLocations to be implemented");
	return FCommand(COMMAND_TYPE_INVALID);
}

FCommand CommandUseAbility(int32 nAbilityId, AActor* oTarget, FVector vTarget, float fConjureTime, FString sAbilitySourceItemTag)
{
	FCommand cCommand = Command(COMMAND_TYPE_USE_ABILITY);
	cCommand = SetCommandInt(cCommand, nAbilityId, 0);
	cCommand = SetCommandObject(cCommand, oTarget, 0);
	cCommand = SetCommandVector(cCommand, vTarget, 0);
	cCommand = SetCommandFloat(cCommand, fConjureTime, 0);
	cCommand = SetCommandString(cCommand, sAbilitySourceItemTag, 0);
	return cCommand;
}

FCommand CommandFly(FVector vVector, int32 bIgnorePathing)
{
	FCommand cCommand = Command(COMMAND_TYPE_FLY);
	cCommand = SetCommandVector(cCommand, vVector, 0);
	return cCommand;
}

FCommand CommandUseObject(AActor* oTarget, int32 nAction)
{
	FCommand cCommand = Command(COMMAND_TYPE_USE_OBJECT);
	cCommand = SetCommandInt(cCommand, nAction, 0);
	return cCommand;
}

FCommand CommandWait(float fSeconds)
{
	FCommand cCommand = Command(COMMAND_TYPE_WAIT);
	cCommand = SetCommandFloat(cCommand, fSeconds, 0);
	return cCommand;
}

FCommand CommandDeathBlow(AActor* oTarget, int32 nDeathType)
{
	FCommand cCommand = Command(COMMAND_TYPE_DEATHBLOW);
	cCommand = SetCommandObject(cCommand, oTarget, 0);
	cCommand = SetCommandInt(cCommand, nDeathType, 0);
	return cCommand;
}

FCommand CommandJumpToLocation(FVector vVector)
{
	FCommand cCommand = Command(COMMAND_TYPE_JUMP_TO_LOCATION);
	cCommand = SetCommandVector(cCommand, vVector, 0);
	return cCommand;
}