// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class MultiplayerSessionsTarget : TargetRules
{
	public MultiplayerSessionsTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		
		// enable logs and debugging for Shipping builds
		if (Configuration == UnrealTargetConfiguration.Shipping)
		{
			//BuildEnvironment = TargetBuildEnvironment.Unique;
			bUseChecksInShipping = true;
			bUseLoggingInShipping = true;
		}
		
		ExtraModuleNames.Add("MultiplayerSessions");
	}
}
