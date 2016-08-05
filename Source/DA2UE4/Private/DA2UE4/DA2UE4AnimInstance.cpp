// Fill out your copyright notice in the Description page of Project Settings.

#include "DA2UE4.h"
#include "DA2UE4AnimInstance.h"
#include "ldf.h"

FAnimNode_AssetPlayerBase* UDA2UE4AnimInstance::GetAssetFromState()
{
	LogError("got it");
	return GetRelevantAssetPlayerFromState(0,1);
}


