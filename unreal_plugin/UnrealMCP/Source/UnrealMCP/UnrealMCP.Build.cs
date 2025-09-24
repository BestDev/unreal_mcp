// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UnrealMCP : ModuleRules
{
	public UnrealMCP(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"HTTPServer",
				"Json",
				"JsonUtilities",
				"HTTP",
				"Sockets",
				"DeveloperSettings",
				"InputCore"
			}
		);

		// Editor-only dependencies
		if (Target.Type == TargetType.Editor)
		{
			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"UnrealEd",
					"ToolMenus",
					"EditorStyle",
					"EditorWidgets",
					"Slate",
					"SlateCore",
					"BlueprintGraph",
					"KismetCompiler",
					"EditorSubsystem"
				}
			);
		}
	}
}