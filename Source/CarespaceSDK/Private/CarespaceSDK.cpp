#include "CarespaceSDK.h"

#define LOCTEXT_NAMESPACE "FCarespaceSDKModule"

void FCarespaceSDKModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file
	UE_LOG(LogTemp, Log, TEXT("CarespaceSDK module started"));
}

void FCarespaceSDKModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	UE_LOG(LogTemp, Log, TEXT("CarespaceSDK module shutdown"));
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCarespaceSDKModule, CarespaceSDK)