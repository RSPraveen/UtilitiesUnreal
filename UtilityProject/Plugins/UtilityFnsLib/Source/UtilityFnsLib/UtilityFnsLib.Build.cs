// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System;
using System.IO;

public class UtilityFnsLib : ModuleRules
{
	private string ThirdPartPath
	{
		get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "../ThirdParty/")); }
	}
	private string MinizPath
	{
		get { return Path.GetFullPath(Path.Combine(ModuleDirectory, ThirdPartPath, "miniz")); }
	}

	public UtilityFnsLib(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
				MinizPath,
				System.IO.Path.GetFullPath(Target.RelativeEnginePath) + "/Source/Editor/Blutility/Private"
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core","Blutility","UMG","UnrealEd","AssetTools", "CoreUObject",
				"Engine",
				"ContentBrowser", "InputCore", "Projects","SceneOutliner", "HTTP", "Json", "WebBrowser", "Engine"
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Projects",
				"InputCore",
				"EditorFramework",
				"UnrealEd",
				"ToolMenus",
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
