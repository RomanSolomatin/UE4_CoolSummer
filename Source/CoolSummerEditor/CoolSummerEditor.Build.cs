// Copyright (c) Jong-il Hong 2015. All rights reserved.

using UnrealBuildTool;

public class CoolSummerEditor : ModuleRules
{
	public CoolSummerEditor(TargetInfo Target)
	{
		PublicDependencyModuleNames.AddRange(new string[] { "Engine", "Core", "CoreUObject", "UnrealEd", "BlueprintGraph", "Slate", "GraphEditor", "AnimGraph" });

		PrivateDependencyModuleNames.AddRange(new string[] { "CoolSummer" });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");
		// if ((Target.Platform == UnrealTargetPlatform.Win32) || (Target.Platform == UnrealTargetPlatform.Win64))
		// {
		//		if (UEBuildConfiguration.bCompileSteamOSS == true)
		//		{
		//			DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
		//		}
		// }
	}
}
