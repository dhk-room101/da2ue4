// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataAsset.h"
#include "DA2UE4MeshDataAsset.generated.h"

//Holds the information about a single character "part"
USTRUCT()
struct FMeshInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "DATA", meta = (ToolTip = "The Mesh ID"))
		int32 MeshID;

	UPROPERTY(EditAnywhere, Category = "DATA", meta = (ToolTip = "Related Asset"))
		TAssetPtr<USkeletalMesh> MeshResource;

	UPROPERTY(EditAnywhere, Category = "DATA", meta = (ToolTip = "Related Asset"))
		TAssetPtr<UMaterial> MeshMat;

	FMeshInfo()
	{
		MeshID = 0;
		MeshResource = FStringAssetReference("");
		MeshMat = FStringAssetReference("");
	}
};

/**
 *
 */
UCLASS(Blueprintable)
class DA2UE4_API UDA2UE4MeshDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UDA2UE4MeshDataAsset();

	//Exposes the array as editable on editor
	UPROPERTY(EditAnywhere, Category = "Model List", meta = (ToolTip = "Asset Info"))
		TArray<FMeshInfo> MeshList;
};
