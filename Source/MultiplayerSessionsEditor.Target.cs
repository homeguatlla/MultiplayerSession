// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class MultiplayerSessionsEditorTarget : TargetRules
{
	public MultiplayerSessionsEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange(new string[]{
			"MultiplayerSessions", 
			"MultiplayerSessionsTests"}
		);
	}
}
