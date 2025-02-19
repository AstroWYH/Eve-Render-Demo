// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class EveRender : ModuleRules
{
	public EveRender(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput",
			// "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "NavigationSystem", "AIModule",
			// "Niagara", "EnhancedInput",
			"RHI",
			"RenderCore",
			"Renderer",
			"Slate",
			"SlateCore",
			"UMG",
		});
		
		PrivateIncludePaths.AddRange(new string[] 
		{
			Path.Combine(EngineDirectory, "Source/Runtime/Renderer/Private")
		});

		AddEngineThirdPartyPrivateStaticDependencies(Target, "HLSLCC");
	}
}