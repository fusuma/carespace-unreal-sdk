#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CarespaceHTTPClient.h"
#include "CarespaceTypes.h"
#include "CarespaceAuthAPI.generated.h"

UCLASS(BlueprintType)
class CARESPACESDK_API UCarespaceAuthAPI : public UObject
{
	GENERATED_BODY()

public:
	UCarespaceAuthAPI();

	// Initialize with HTTP client
	UFUNCTION(BlueprintCallable, Category = "Carespace|Auth")
	void Initialize(UCarespaceHTTPClient* InHTTPClient);

	// Authentication methods
	UFUNCTION(BlueprintCallable, Category = "Carespace|Auth")
	void Login(const FCarespaceLoginRequest& LoginRequest, const FOnCarespaceLoginComplete& OnComplete);

	UFUNCTION(BlueprintCallable, Category = "Carespace|Auth")
	void Logout(const FOnCarespaceRequestComplete& OnComplete);

	UFUNCTION(BlueprintCallable, Category = "Carespace|Auth")
	void RefreshToken(const FString& RefreshToken, const FOnCarespaceLoginComplete& OnComplete);

	UFUNCTION(BlueprintCallable, Category = "Carespace|Auth")
	void ForgotPassword(const FString& Email, const FOnCarespaceRequestComplete& OnComplete);

	UFUNCTION(BlueprintCallable, Category = "Carespace|Auth")
	void ResetPassword(const FString& Token, const FString& NewPassword, const FOnCarespaceRequestComplete& OnComplete);

	UFUNCTION(BlueprintCallable, Category = "Carespace|Auth")
	void ChangePassword(const FString& CurrentPassword, const FString& NewPassword, const FOnCarespaceRequestComplete& OnComplete);

private:
	UPROPERTY()
	UCarespaceHTTPClient* HTTPClient;

	// Response handlers
	UFUNCTION()
	void HandleLoginResponse(bool bWasSuccessful, const FString& ResponseContent, const FCarespaceError& Error, FOnCarespaceLoginComplete OnComplete);

	UFUNCTION()
	void HandleGenericResponse(bool bWasSuccessful, const FString& ResponseContent, const FCarespaceError& Error, FOnCarespaceRequestComplete OnComplete);
};