// Copyright (c) Jong-il Hong 2015. All rights reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class CoolSummerEditorTarget : TargetRules
{
	public CoolSummerEditorTarget(TargetInfo Target)
	{
		Type = TargetType.Editor;
	}

	//
	// TargetRules interface.
	//

	public override void SetupBinaries(
		TargetInfo Target,
		ref List<UEBuildBinaryConfiguration> OutBuildBinaryConfigurations,
		ref List<string> OutExtraModuleNames
		)
	{
		OutExtraModuleNames.AddRange( new string[] { "CoolSummer", "CoolSummerEditor" } );
    }
}
