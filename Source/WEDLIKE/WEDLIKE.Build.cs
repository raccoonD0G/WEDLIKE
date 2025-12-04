// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class WEDLIKE : ModuleRules
{
	public WEDLIKE(ReadOnlyTargetRules Target) : base(Target)
	{

        PrivateIncludePaths.AddRange(new string[] { "WEDLIKE" });
        
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "Paper2D", "UMG", "MediaAssets", "Media", "MediaUtils", "Niagara", "VisionMotionKit", "RecordKit", "NDIIO", "ExtendedWidgetKit", "EasyGameKit"});

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
