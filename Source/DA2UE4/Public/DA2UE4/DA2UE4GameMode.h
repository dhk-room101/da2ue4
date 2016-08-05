// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameMode.h"
#include "DA2UE4GameMode.generated.h"

UCLASS()
class ADA2UE4GameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ADA2UE4GameMode();
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};



