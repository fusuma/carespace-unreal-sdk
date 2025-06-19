using UnrealBuildTool;

public class CarespaceSDKTests : ModuleRules
{
	public CarespaceSDKTests(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"CarespaceSDK",
			"UnrealEd",
			"ToolMenus",
			"AutomationController",
			"HTTP",
			"Json",
			"JsonUtilities"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Slate",
			"SlateCore",
			"EditorStyle",
			"EditorWidgets",
			"DesktopWidgets",
			"PropertyEditor",
			"SharedSettingsWidgets",
			"SourceControl"
		});

		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.AddRange(new string[]
			{
				"UnrealEd",
				"BlueprintGraph",
				"KismetCompiler",
				"EditorSubsystem"
			});
		}

		// Enable automation testing
		PublicDefinitions.Add("WITH_AUTOMATION_TESTS=1");
		
		// Optimization settings for tests
		if (Target.Configuration == UnrealTargetConfiguration.Test)
		{
			bUseUnity = false;
			OptimizeCode = CodeOptimization.Never;
		}
	}
}