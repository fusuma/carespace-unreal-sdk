#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

/**
 * Module for Carespace SDK unit tests and automation testing.
 * This module provides testing infrastructure and utilities for validating
 * the functionality of the Carespace SDK components.
 */
class CARESPACESDKTESTS_API FCarespaceSDKTestsModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/**
	 * Singleton-like access to this module's interface. This is just for convenience!
	 * Beware of calling this during the shutdown phase, though. Your module might have been unloaded already.
	 *
	 * @return Returns singleton instance, loading the module on demand if needed
	 */
	static inline FCarespaceSDKTestsModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FCarespaceSDKTestsModule>("CarespaceSDKTests");
	}

	/**
	 * Checks to see if this module is loaded and ready. It is only valid to call Get() if IsAvailable() returns true.
	 *
	 * @return True if the module is loaded and ready to use
	 */
	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("CarespaceSDKTests");
	}
};