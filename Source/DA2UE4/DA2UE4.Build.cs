// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DA2UE4 : ModuleRules
{
    public DA2UE4(TargetInfo Target)
    {
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });
        PublicDependencyModuleNames.AddRange(new string[] { "Json", "JsonUtilities" }); //for Json
        PublicDependencyModuleNames.AddRange(new string[] { "GameplayTasks" }); //for BT
        PublicDependencyModuleNames.AddRange(new string[] { "UMG", "Slate", "SlateCore" }); //for UMG
    }
}
