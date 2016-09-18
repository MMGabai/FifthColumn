// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
using UnrealBuildTool;

public class FifthColumn : ModuleRules
{
	public FifthColumn(TargetInfo Target)
	{
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "AssetRegistry", "AIModule", });

        PrivateDependencyModuleNames.AddRange(new string[] 	{"InputCore","Slate","SlateCore", });
	}
}
