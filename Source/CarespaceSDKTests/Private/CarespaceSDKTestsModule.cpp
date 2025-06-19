#include "CarespaceSDKTestsModule.h"
#include "Modules/ModuleManager.h"

DEFINE_LOG_CATEGORY_STATIC(LogCarespaceSDKTests, Log, All);

#define LOCTEXT_NAMESPACE "FCarespaceSDKTestsModule"

void FCarespaceSDKTestsModule::StartupModule()
{
	UE_LOG(LogCarespaceSDKTests, Log, TEXT("CarespaceSDKTests module has been loaded"));
}

void FCarespaceSDKTestsModule::ShutdownModule()
{
	UE_LOG(LogCarespaceSDKTests, Log, TEXT("CarespaceSDKTests module has been unloaded"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FCarespaceSDKTestsModule, CarespaceSDKTests)