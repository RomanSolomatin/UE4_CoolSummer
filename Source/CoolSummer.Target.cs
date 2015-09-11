// Copyright (c) Jong-il Hong 2015. All rights reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class CoolSummerTarget : TargetRules
{
	public CoolSummerTarget(TargetInfo Target)
	{
		Type = TargetType.Game;
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
		OutExtraModuleNames.AddRange( new string[] { "CoolSummer" } );
	}
}
