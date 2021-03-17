// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MultiplayerSessions : ModuleRules
{
	public MultiplayerSessions(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"HeadMountedDisplay", 
			//Online Platform connectivity specific
			"OnlineSubsystem", 
			"OnlineSubsystemUtils",
			//Steam
			"OnlineSubsystemSteam"
		});
		
		//Online Platform connectivity specific
		DynamicallyLoadedModuleNames.Add("OnlineSubsystemNull");
	}
}
