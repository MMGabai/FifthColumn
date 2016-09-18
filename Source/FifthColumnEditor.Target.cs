using UnrealBuildTool;
using System.Collections.Generic;

public class FifthColumnEditorTarget : TargetRules
{
	public FifthColumnEditorTarget(TargetInfo Target)
	{
		Type = TargetType.Editor;
	}

	public override void SetupBinaries(
		TargetInfo Target,
		ref List<UEBuildBinaryConfiguration> OutBuildBinaryConfigurations,
		ref List<string> OutExtraModuleNames
		)
	{
		OutExtraModuleNames.AddRange( new string[] { "FifthColumn" } );
	}
}
