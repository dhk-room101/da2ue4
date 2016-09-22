// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "DA2UE4.h"
#include "DA2UE4GameMode.h"
#include "DA2UE4PlayerController.h"
#include "DA2UE4Creature.h"
#include "DA2UE4Library.h"
#include "DA2UE4GameInstance.h"

ADA2UE4GameMode::ADA2UE4GameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ADA2UE4PlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Data/Blueprints/BP2Creature"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

//reset singleton when PIE ends
void ADA2UE4GameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (EndPlayReason == EEndPlayReason::EndPlayInEditor)
	{
		bool IsValid;
		//UDA2UE4Library::GetDA2UE4Data(IsValid)->DA2UE4CurrentModule->PLAYER_LEADER_STORE = nullptr;
		//UDA2UE4Library::GetDA2UE4Data(IsValid)->DA2UE4CurrentModule = nullptr;
		UDA2UE4Library::GetDA2UE4Data(IsValid)->DA2UE4CurrentWorld = nullptr;
	}
}
