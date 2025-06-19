#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CarespaceHTTPClient.h"
#include "CarespaceTypes.h"
#include "CarespaceAuthAPI.generated.h"

/**
 * Authentication API class providing user authentication and account management functionality.
 * This class handles login/logout operations, token management, and password-related functions
 * for the Carespace platform. All authentication operations are performed asynchronously
 * with appropriate delegate callbacks for success and failure scenarios.
 * 
 * Usage:
 *   UCarespaceAuthAPI* AuthAPI = API->GetAuthAPI();
 *   FCarespaceLoginRequest LoginReq;
 *   LoginReq.Email = "user@example.com";
 *   LoginReq.Password = "password123";
 *   AuthAPI->Login(LoginReq, OnLoginSuccess, OnLoginFailure);
 */
UCLASS(BlueprintType)
class CARESPACESDK_API UCarespaceAuthAPI : public UObject
{
	GENERATED_BODY()

public:
	UCarespaceAuthAPI();

	/**
	 * Initializes the authentication API with the HTTP client for making requests.
	 * This method must be called before using any authentication functions.
	 * 
	 * @param InHTTPClient Configured HTTP client instance for API communication
	 */
	UFUNCTION(BlueprintCallable, Category = "Carespace|Auth")
	void Initialize(UCarespaceHTTPClient* InHTTPClient);

	// Authentication methods
	/**
	 * Authenticates a user with email and password credentials.
	 * Upon successful login, the response will contain access and refresh tokens
	 * that should be stored for subsequent API calls.
	 * 
	 * @param LoginRequest Contains user email and password for authentication
	 * @param OnComplete Delegate called when login completes with tokens or error information
	 */
	UFUNCTION(BlueprintCallable, Category = "Carespace|Auth")
	void Login(const FCarespaceLoginRequest& LoginRequest, const FOnCarespaceLoginComplete& OnComplete);

	/**
	 * Logs out the current user and invalidates their session on the server.
	 * This should be called when the user explicitly logs out or when the application closes.
	 * 
	 * @param OnComplete Delegate called when logout completes or fails
	 */
	UFUNCTION(BlueprintCallable, Category = "Carespace|Auth")
	void Logout(const FOnCarespaceRequestComplete& OnComplete);

	/**
	 * Refreshes an expired access token using a valid refresh token.
	 * This should be called automatically when API requests return 401 Unauthorized errors.
	 * 
	 * @param RefreshToken The refresh token obtained from a previous login
	 * @param OnComplete Delegate called when refresh completes with new tokens or error
	 */
	UFUNCTION(BlueprintCallable, Category = "Carespace|Auth")
	void RefreshToken(const FString& RefreshToken, const FOnCarespaceLoginComplete& OnComplete);

	/**
	 * Initiates a password reset process by sending a reset email to the specified address.
	 * The user will receive an email with a reset token to complete the password change.
	 * 
	 * @param Email Email address of the user requesting password reset
	 * @param OnComplete Delegate called when the reset email request completes
	 */
	UFUNCTION(BlueprintCallable, Category = "Carespace|Auth")
	void ForgotPassword(const FString& Email, const FOnCarespaceRequestComplete& OnComplete);

	/**
	 * Completes the password reset process using a token received via email.
	 * This method should be called with the token from the password reset email.
	 * 
	 * @param Token Reset token received in the password reset email
	 * @param NewPassword The new password to set for the user account
	 * @param OnComplete Delegate called when password reset completes or fails
	 */
	UFUNCTION(BlueprintCallable, Category = "Carespace|Auth")
	void ResetPassword(const FString& Token, const FString& NewPassword, const FOnCarespaceRequestComplete& OnComplete);

	/**
	 * Changes the password for the currently authenticated user.
	 * Requires the user to provide their current password for security verification.
	 * 
	 * @param CurrentPassword The user's current password for verification
	 * @param NewPassword The new password to set for the account
	 * @param OnComplete Delegate called when password change completes or fails
	 */
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