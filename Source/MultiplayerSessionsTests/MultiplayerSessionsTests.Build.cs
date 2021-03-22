// Copyright © 2020 Super Teams Mobile, S.L. All rights reserved

using UnrealBuildTool;


public class MultiplayerSessionsTests : ModuleRules
{
	public MultiplayerSessionsTests(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		bUseRTTI = true;

		PublicDependencyModuleNames.AddRange(new string[]{});
		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"FunctionalTesting",
			"MultiplayerSessions",
			"UMG"
		});
	}
}
