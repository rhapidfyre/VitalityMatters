// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class VitalityMatters : ModuleRules
{
	public VitalityMatters(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				//"VitalityMatters/Public","VitalityMatters/Public/lib",
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				//"VitalityMatters/Private","VitalityMatters/Private/lib"
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"EnhancedInput"
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
