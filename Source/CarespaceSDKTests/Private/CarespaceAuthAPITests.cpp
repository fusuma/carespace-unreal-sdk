#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Engine/Engine.h"
#include "CarespaceAuthAPI.h"
#include "MockCarespaceHTTPClient.h"
#include "CarespaceTypes.h"

DEFINE_LOG_CATEGORY_STATIC(LogCarespaceAuthAPITests, Log, All);

/**
 * Test suite for UCarespaceAuthAPI initialization and configuration.
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCarespaceAuthAPIInitializationTest, "CarespaceSDK.Auth.Initialization", 
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FCarespaceAuthAPIInitializationTest::RunTest(const FString& Parameters)
{
	// Test AuthAPI creation
	UCarespaceAuthAPI* AuthAPI = NewObject<UCarespaceAuthAPI>();
	TestNotNull("AuthAPI should be created successfully", AuthAPI);
	
	if (AuthAPI)
	{
		// Test initialization with HTTP client
		UMockCarespaceHTTPClient* MockClient = NewObject<UMockCarespaceHTTPClient>();
		TestNotNull("Mock HTTP client should be created", MockClient);
		
		// Initialize should not crash
		AuthAPI->Initialize(MockClient);
		TestTrue("Initialize should execute without error", true);
	}
	
	return !HasAnyErrors();
}

/**
 * Test suite for successful login functionality.
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCarespaceAuthAPILoginSuccessTest, "CarespaceSDK.Auth.LoginSuccess", 
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FCarespaceAuthAPILoginSuccessTest::RunTest(const FString& Parameters)
{
	UCarespaceAuthAPI* AuthAPI = NewObject<UCarespaceAuthAPI>();
	UMockCarespaceHTTPClient* MockClient = NewObject<UMockCarespaceHTTPClient>();
	
	TestNotNull("AuthAPI should be created", AuthAPI);
	TestNotNull("Mock client should be created", MockClient);
	
	if (!AuthAPI || !MockClient)
	{
		return false;
	}
	
	AuthAPI->Initialize(MockClient);
	
	// Set up successful login response
	MockClient->MockAuthenticationSuccess();
	
	// Prepare login request
	FCarespaceLoginRequest LoginRequest;
	LoginRequest.Email = TEXT("test@example.com");
	LoginRequest.Password = TEXT("password123");
	
	bool bCallbackExecuted = false;
	bool bLoginSuccessful = false;
	FCarespaceLoginResponse ReceivedResponse;
	
	FOnCarespaceLoginComplete OnLoginComplete;
	OnLoginComplete.BindLambda([&](bool bWasSuccessful, const FCarespaceLoginResponse& Response, const FCarespaceError& Error)
	{
		bCallbackExecuted = true;
		bLoginSuccessful = bWasSuccessful;
		ReceivedResponse = Response;
	});
	
	// Execute login
	AuthAPI->Login(LoginRequest, OnLoginComplete);
	
	// Allow time for mock response processing
	FPlatformProcess::Sleep(0.1f);
	
	TestTrue("Login callback should be executed", bCallbackExecuted);
	TestTrue("Login should be successful", bLoginSuccessful);
	TestEqual("Access token should be set", ReceivedResponse.AccessToken, TEXT("mock_access_token_12345"));
	TestEqual("Refresh token should be set", ReceivedResponse.RefreshToken, TEXT("mock_refresh_token_67890"));
	TestEqual("User ID should be set", ReceivedResponse.User.Id, TEXT("user_123"));
	TestEqual("User email should match request", ReceivedResponse.User.Email, TEXT("test@example.com"));
	
	// Verify request was made correctly
	TestEqual("Should have made 1 POST request to login endpoint", 
		MockClient->GetRequestCount(TEXT("POST"), TEXT("/api/auth/login")), 1);
	
	// Verify request data contains credentials
	FString LastRequestData = MockClient->GetLastRequestData(TEXT("POST"), TEXT("/api/auth/login"));
	TestTrue("Request should contain email", LastRequestData.Contains(TEXT("test@example.com")));
	TestTrue("Request should contain password", LastRequestData.Contains(TEXT("password123")));
	
	return !HasAnyErrors();
}

/**
 * Test suite for failed login functionality.
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCarespaceAuthAPILoginFailureTest, "CarespaceSDK.Auth.LoginFailure", 
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FCarespaceAuthAPILoginFailureTest::RunTest(const FString& Parameters)
{
	UCarespaceAuthAPI* AuthAPI = NewObject<UCarespaceAuthAPI>();
	UMockCarespaceHTTPClient* MockClient = NewObject<UMockCarespaceHTTPClient>();
	
	AuthAPI->Initialize(MockClient);
	
	// Set up failed login response
	MockClient->MockAuthenticationFailure();
	
	FCarespaceLoginRequest LoginRequest;
	LoginRequest.Email = TEXT("invalid@example.com");
	LoginRequest.Password = TEXT("wrongpassword");
	
	bool bCallbackExecuted = false;
	bool bLoginSuccessful = true;
	FCarespaceError ReceivedError;
	
	FOnCarespaceLoginComplete OnLoginComplete;
	OnLoginComplete.BindLambda([&](bool bWasSuccessful, const FCarespaceLoginResponse& Response, const FCarespaceError& Error)
	{
		bCallbackExecuted = true;
		bLoginSuccessful = bWasSuccessful;
		ReceivedError = Error;
	});
	
	AuthAPI->Login(LoginRequest, OnLoginComplete);
	
	FPlatformProcess::Sleep(0.1f);
	
	TestTrue("Login callback should be executed", bCallbackExecuted);
	TestFalse("Login should fail", bLoginSuccessful);
	TestEqual("Error type should be authentication error", 
		(int32)ReceivedError.Type, (int32)ECarespaceErrorType::AuthenticationError);
	TestEqual("Error message should match", ReceivedError.Message, TEXT("Invalid credentials"));
	TestEqual("Error code should match", ReceivedError.Code, TEXT("INVALID_CREDENTIALS"));
	
	return !HasAnyErrors();
}

/**
 * Test suite for token refresh functionality.
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCarespaceAuthAPIRefreshTokenTest, "CarespaceSDK.Auth.RefreshToken", 
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FCarespaceAuthAPIRefreshTokenTest::RunTest(const FString& Parameters)
{
	UCarespaceAuthAPI* AuthAPI = NewObject<UCarespaceAuthAPI>();
	UMockCarespaceHTTPClient* MockClient = NewObject<UMockCarespaceHTTPClient>();
	
	AuthAPI->Initialize(MockClient);
	
	// Set up successful refresh response
	UMockCarespaceHTTPClient::FMockResponse RefreshResponse;
	RefreshResponse.bShouldSucceed = true;
	RefreshResponse.ResponseContent = TEXT(R"({
		"success": true,
		"data": {
			"accessToken": "new_access_token_98765",
			"refreshToken": "new_refresh_token_54321"
		}
	})");
	
	MockClient->SetMockResponse(TEXT("POST"), TEXT("/api/auth/refresh"), RefreshResponse);
	
	bool bCallbackExecuted = false;
	bool bRefreshSuccessful = false;
	FCarespaceLoginResponse ReceivedResponse;
	
	FOnCarespaceLoginComplete OnRefreshComplete;
	OnRefreshComplete.BindLambda([&](bool bWasSuccessful, const FCarespaceLoginResponse& Response, const FCarespaceError& Error)
	{
		bCallbackExecuted = true;
		bRefreshSuccessful = bWasSuccessful;
		ReceivedResponse = Response;
	});
	
	AuthAPI->RefreshToken(TEXT("old_refresh_token_12345"), OnRefreshComplete);
	
	FPlatformProcess::Sleep(0.1f);
	
	TestTrue("Refresh callback should be executed", bCallbackExecuted);
	TestTrue("Refresh should be successful", bRefreshSuccessful);
	TestEqual("New access token should be set", ReceivedResponse.AccessToken, TEXT("new_access_token_98765"));
	TestEqual("New refresh token should be set", ReceivedResponse.RefreshToken, TEXT("new_refresh_token_54321"));
	
	// Verify request data contains refresh token
	FString LastRequestData = MockClient->GetLastRequestData(TEXT("POST"), TEXT("/api/auth/refresh"));
	TestTrue("Request should contain refresh token", LastRequestData.Contains(TEXT("old_refresh_token_12345")));
	
	return !HasAnyErrors();
}

/**
 * Test suite for logout functionality.
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCarespaceAuthAPILogoutTest, "CarespaceSDK.Auth.Logout", 
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FCarespaceAuthAPILogoutTest::RunTest(const FString& Parameters)
{
	UCarespaceAuthAPI* AuthAPI = NewObject<UCarespaceAuthAPI>();
	UMockCarespaceHTTPClient* MockClient = NewObject<UMockCarespaceHTTPClient>();
	
	AuthAPI->Initialize(MockClient);
	
	// Set up successful logout response
	UMockCarespaceHTTPClient::FMockResponse LogoutResponse;
	LogoutResponse.bShouldSucceed = true;
	LogoutResponse.ResponseContent = TEXT(R"({
		"success": true,
		"message": "Successfully logged out"
	})");
	
	MockClient->SetMockResponse(TEXT("POST"), TEXT("/api/auth/logout"), LogoutResponse);
	
	bool bCallbackExecuted = false;
	bool bLogoutSuccessful = false;
	
	FOnCarespaceRequestComplete OnLogoutComplete;
	OnLogoutComplete.BindLambda([&](bool bWasSuccessful, const FCarespaceError& Error)
	{
		bCallbackExecuted = true;
		bLogoutSuccessful = bWasSuccessful;
	});
	
	AuthAPI->Logout(OnLogoutComplete);
	
	FPlatformProcess::Sleep(0.1f);
	
	TestTrue("Logout callback should be executed", bCallbackExecuted);
	TestTrue("Logout should be successful", bLogoutSuccessful);
	TestEqual("Should have made 1 POST request to logout endpoint", 
		MockClient->GetRequestCount(TEXT("POST"), TEXT("/api/auth/logout")), 1);
	
	return !HasAnyErrors();
}

/**
 * Test suite for forgot password functionality.
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCarespaceAuthAPIForgotPasswordTest, "CarespaceSDK.Auth.ForgotPassword", 
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FCarespaceAuthAPIForgotPasswordTest::RunTest(const FString& Parameters)
{
	UCarespaceAuthAPI* AuthAPI = NewObject<UCarespaceAuthAPI>();
	UMockCarespaceHTTPClient* MockClient = NewObject<UMockCarespaceHTTPClient>();
	
	AuthAPI->Initialize(MockClient);
	
	// Set up successful forgot password response
	UMockCarespaceHTTPClient::FMockResponse ForgotPasswordResponse;
	ForgotPasswordResponse.bShouldSucceed = true;
	ForgotPasswordResponse.ResponseContent = TEXT(R"({
		"success": true,
		"message": "Password reset email sent"
	})");
	
	MockClient->SetMockResponse(TEXT("POST"), TEXT("/api/auth/forgot-password"), ForgotPasswordResponse);
	
	bool bCallbackExecuted = false;
	bool bRequestSuccessful = false;
	
	FOnCarespaceRequestComplete OnForgotPasswordComplete;
	OnForgotPasswordComplete.BindLambda([&](bool bWasSuccessful, const FCarespaceError& Error)
	{
		bCallbackExecuted = true;
		bRequestSuccessful = bWasSuccessful;
	});
	
	AuthAPI->ForgotPassword(TEXT("user@example.com"), OnForgotPasswordComplete);
	
	FPlatformProcess::Sleep(0.1f);
	
	TestTrue("Forgot password callback should be executed", bCallbackExecuted);
	TestTrue("Forgot password request should be successful", bRequestSuccessful);
	
	// Verify request data contains email
	FString LastRequestData = MockClient->GetLastRequestData(TEXT("POST"), TEXT("/api/auth/forgot-password"));
	TestTrue("Request should contain email", LastRequestData.Contains(TEXT("user@example.com")));
	
	return !HasAnyErrors();
}

/**
 * Test suite for reset password functionality.
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCarespaceAuthAPIResetPasswordTest, "CarespaceSDK.Auth.ResetPassword", 
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FCarespaceAuthAPIResetPasswordTest::RunTest(const FString& Parameters)
{
	UCarespaceAuthAPI* AuthAPI = NewObject<UCarespaceAuthAPI>();
	UMockCarespaceHTTPClient* MockClient = NewObject<UMockCarespaceHTTPClient>();
	
	AuthAPI->Initialize(MockClient);
	
	// Set up successful reset password response
	UMockCarespaceHTTPClient::FMockResponse ResetPasswordResponse;
	ResetPasswordResponse.bShouldSucceed = true;
	ResetPasswordResponse.ResponseContent = TEXT(R"({
		"success": true,
		"message": "Password reset successfully"
	})");
	
	MockClient->SetMockResponse(TEXT("POST"), TEXT("/api/auth/reset-password"), ResetPasswordResponse);
	
	bool bCallbackExecuted = false;
	bool bRequestSuccessful = false;
	
	FOnCarespaceRequestComplete OnResetPasswordComplete;
	OnResetPasswordComplete.BindLambda([&](bool bWasSuccessful, const FCarespaceError& Error)
	{
		bCallbackExecuted = true;
		bRequestSuccessful = bWasSuccessful;
	});
	
	AuthAPI->ResetPassword(TEXT("reset_token_123"), TEXT("newSecurePassword456"), OnResetPasswordComplete);
	
	FPlatformProcess::Sleep(0.1f);
	
	TestTrue("Reset password callback should be executed", bCallbackExecuted);
	TestTrue("Reset password request should be successful", bRequestSuccessful);
	
	// Verify request data contains token and new password
	FString LastRequestData = MockClient->GetLastRequestData(TEXT("POST"), TEXT("/api/auth/reset-password"));
	TestTrue("Request should contain reset token", LastRequestData.Contains(TEXT("reset_token_123")));
	TestTrue("Request should contain new password", LastRequestData.Contains(TEXT("newSecurePassword456")));
	
	return !HasAnyErrors();
}

/**
 * Test suite for change password functionality.
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCarespaceAuthAPIChangePasswordTest, "CarespaceSDK.Auth.ChangePassword", 
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FCarespaceAuthAPIChangePasswordTest::RunTest(const FString& Parameters)
{
	UCarespaceAuthAPI* AuthAPI = NewObject<UCarespaceAuthAPI>();
	UMockCarespaceHTTPClient* MockClient = NewObject<UMockCarespaceHTTPClient>();
	
	AuthAPI->Initialize(MockClient);
	
	// Set up successful change password response
	UMockCarespaceHTTPClient::FMockResponse ChangePasswordResponse;
	ChangePasswordResponse.bShouldSucceed = true;
	ChangePasswordResponse.ResponseContent = TEXT(R"({
		"success": true,
		"message": "Password changed successfully"
	})");
	
	MockClient->SetMockResponse(TEXT("PUT"), TEXT("/api/auth/change-password"), ChangePasswordResponse);
	
	bool bCallbackExecuted = false;
	bool bRequestSuccessful = false;
	
	FOnCarespaceRequestComplete OnChangePasswordComplete;
	OnChangePasswordComplete.BindLambda([&](bool bWasSuccessful, const FCarespaceError& Error)
	{
		bCallbackExecuted = true;
		bRequestSuccessful = bWasSuccessful;
	});
	
	AuthAPI->ChangePassword(TEXT("oldPassword123"), TEXT("newPassword456"), OnChangePasswordComplete);
	
	FPlatformProcess::Sleep(0.1f);
	
	TestTrue("Change password callback should be executed", bCallbackExecuted);
	TestTrue("Change password request should be successful", bRequestSuccessful);
	
	// Verify request data contains both passwords
	FString LastRequestData = MockClient->GetLastRequestData(TEXT("PUT"), TEXT("/api/auth/change-password"));
	TestTrue("Request should contain current password", LastRequestData.Contains(TEXT("oldPassword123")));
	TestTrue("Request should contain new password", LastRequestData.Contains(TEXT("newPassword456")));
	
	return !HasAnyErrors();
}