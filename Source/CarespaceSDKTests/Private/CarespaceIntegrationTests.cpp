#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Engine/Engine.h"
#include "CarespaceAPI.h"
#include "MockCarespaceHTTPClient.h"
#include "CarespaceTypes.h"

DEFINE_LOG_CATEGORY_STATIC(LogCarespaceIntegrationTests, Log, All);

/**
 * Integration test for complete authentication workflow.
 * Tests the full flow from login to API usage to logout.
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCarespaceFullAuthWorkflowTest, "CarespaceSDK.Integration.AuthWorkflow", 
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FCarespaceFullAuthWorkflowTest::RunTest(const FString& Parameters)
{
	UCarespaceAPI* API = UCarespaceAPI::CreateCarespaceAPI();
	TestNotNull("API should be created", API);
	
	if (!API)
	{
		return false;
	}
	
	UMockCarespaceHTTPClient* MockClient = NewObject<UMockCarespaceHTTPClient>();
	TestNotNull("Mock client should be created", MockClient);
	
	// Step 1: Set up authentication success
	MockClient->MockAuthenticationSuccess();
	
	// Step 2: Login
	bool bLoginCompleted = false;
	bool bLoginSuccessful = false;
	FCarespaceLoginResponse LoginResponse;
	
	FCarespaceLoginRequest LoginRequest;
	LoginRequest.Email = TEXT("integration@test.com");
	LoginRequest.Password = TEXT("testPassword123");
	
	FOnCarespaceLoginComplete OnLoginComplete;
	OnLoginComplete.BindLambda([&](bool bWasSuccessful, const FCarespaceLoginResponse& Response, const FCarespaceError& Error)
	{
		bLoginCompleted = true;
		bLoginSuccessful = bWasSuccessful;
		LoginResponse = Response;
	});
	
	API->GetAuthAPI()->Login(LoginRequest, OnLoginComplete);
	FPlatformProcess::Sleep(0.1f);
	
	TestTrue("Login should complete", bLoginCompleted);
	TestTrue("Login should be successful", bLoginSuccessful);
	TestFalse("Access token should not be empty", LoginResponse.AccessToken.IsEmpty());
	
	if (!bLoginSuccessful)
	{
		return false;
	}
	
	// Step 3: Set API key from login response
	API->SetAPIKey(LoginResponse.AccessToken);
	
	// Step 4: Make authenticated API call (get users)
	UMockCarespaceHTTPClient::FMockResponse UsersResponse;
	UsersResponse.bShouldSucceed = true;
	UsersResponse.ResponseContent = TEXT(R"({
		"success": true,
		"data": {
			"users": [
				{
					"id": "user_integration_1",
					"email": "integration1@test.com",
					"firstName": "Integration",
					"lastName": "User1",
					"role": "clinician"
				}
			],
			"pagination": {
				"page": 1,
				"limit": 20,
				"total": 1,
				"totalPages": 1
			}
		}
	})");
	
	MockClient->SetMockResponse(TEXT("GET"), TEXT("/api/users"), UsersResponse);
	
	bool bUsersCompleted = false;
	bool bUsersSuccessful = false;
	TArray<FCarespaceUser> Users;
	
	FOnCarespaceUsersReceived OnUsersReceived;
	OnUsersReceived.BindLambda([&](bool bWasSuccessful, const TArray<FCarespaceUser>& ReceivedUsers, const FCarespaceError& Error)
	{
		bUsersCompleted = true;
		bUsersSuccessful = bWasSuccessful;
		Users = ReceivedUsers;
	});
	
	API->GetUsers(1, 20, TEXT(""), OnUsersReceived);
	FPlatformProcess::Sleep(0.1f);
	
	TestTrue("Users request should complete", bUsersCompleted);
	TestTrue("Users request should be successful", bUsersSuccessful);
	TestEqual("Should receive 1 user", Users.Num(), 1);
	
	// Step 5: Logout
	UMockCarespaceHTTPClient::FMockResponse LogoutResponse;
	LogoutResponse.bShouldSucceed = true;
	LogoutResponse.ResponseContent = TEXT(R"({"success": true, "message": "Logged out successfully"})");
	
	MockClient->SetMockResponse(TEXT("POST"), TEXT("/api/auth/logout"), LogoutResponse);
	
	bool bLogoutCompleted = false;
	bool bLogoutSuccessful = false;
	
	FOnCarespaceRequestComplete OnLogoutComplete;
	OnLogoutComplete.BindLambda([&](bool bWasSuccessful, const FCarespaceError& Error)
	{
		bLogoutCompleted = true;
		bLogoutSuccessful = bWasSuccessful;
	});
	
	API->GetAuthAPI()->Logout(OnLogoutComplete);
	FPlatformProcess::Sleep(0.1f);
	
	TestTrue("Logout should complete", bLogoutCompleted);
	TestTrue("Logout should be successful", bLogoutSuccessful);
	
	// Verify the complete flow made the expected requests
	TestEqual("Should have made login request", MockClient->GetRequestCount(TEXT("POST"), TEXT("/api/auth/login")), 1);
	TestEqual("Should have made users request", MockClient->GetRequestCount(TEXT("GET"), TEXT("/api/users")), 1);
	TestEqual("Should have made logout request", MockClient->GetRequestCount(TEXT("POST"), TEXT("/api/auth/logout")), 1);
	
	return !HasAnyErrors();
}

/**
 * Integration test for token refresh during API usage.
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCarespaceTokenRefreshIntegrationTest, "CarespaceSDK.Integration.TokenRefresh", 
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FCarespaceTokenRefreshIntegrationTest::RunTest(const FString& Parameters)
{
	UCarespaceAPI* API = UCarespaceAPI::CreateCarespaceAPI();
	UMockCarespaceHTTPClient* MockClient = NewObject<UMockCarespaceHTTPClient>();
	
	// Simulate initial authentication
	MockClient->MockAuthenticationSuccess();
	
	// Simulate expired token scenario - first request fails, then refresh succeeds
	FCarespaceError ExpiredTokenError;
	ExpiredTokenError.Type = ECarespaceErrorType::AuthenticationError;
	ExpiredTokenError.Message = TEXT("Token expired");
	ExpiredTokenError.Code = TEXT("TOKEN_EXPIRED");
	
	UMockCarespaceHTTPClient::FMockResponse ExpiredTokenResponse(ExpiredTokenError);
	MockClient->SetMockResponse(TEXT("GET"), TEXT("/api/users"), ExpiredTokenResponse);
	
	// Set up refresh token response
	UMockCarespaceHTTPClient::FMockResponse RefreshResponse;
	RefreshResponse.bShouldSucceed = true;
	RefreshResponse.ResponseContent = TEXT(R"({
		"success": true,
		"data": {
			"accessToken": "refreshed_access_token_98765",
			"refreshToken": "refreshed_refresh_token_54321"
		}
	})");
	
	MockClient->SetMockResponse(TEXT("POST"), TEXT("/api/auth/refresh"), RefreshResponse);
	
	// Test token refresh functionality
	bool bRefreshCompleted = false;
	bool bRefreshSuccessful = false;
	FCarespaceLoginResponse RefreshResult;
	
	FOnCarespaceLoginComplete OnRefreshComplete;
	OnRefreshComplete.BindLambda([&](bool bWasSuccessful, const FCarespaceLoginResponse& Response, const FCarespaceError& Error)
	{
		bRefreshCompleted = true;
		bRefreshSuccessful = bWasSuccessful;
		RefreshResult = Response;
	});
	
	API->GetAuthAPI()->RefreshToken(TEXT("original_refresh_token"), OnRefreshComplete);
	FPlatformProcess::Sleep(0.1f);
	
	TestTrue("Token refresh should complete", bRefreshCompleted);
	TestTrue("Token refresh should be successful", bRefreshSuccessful);
	TestEqual("New access token should be provided", RefreshResult.AccessToken, TEXT("refreshed_access_token_98765"));
	
	return !HasAnyErrors();
}

/**
 * Integration test for error handling across multiple API calls.
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCarespaceErrorHandlingIntegrationTest, "CarespaceSDK.Integration.ErrorHandling", 
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FCarespaceErrorHandlingIntegrationTest::RunTest(const FString& Parameters)
{
	UCarespaceAPI* API = UCarespaceAPI::CreateCarespaceAPI();
	UMockCarespaceHTTPClient* MockClient = NewObject<UMockCarespaceHTTPClient>();
	
	// Test sequence of different error types
	TArray<ECarespaceErrorType> ErrorTypes = {
		ECarespaceErrorType::NetworkError,
		ECarespaceErrorType::ValidationError,
		ECarespaceErrorType::ServerError
	};
	
	TArray<FString> ErrorMessages = {
		TEXT("Network connection failed"),
		TEXT("Invalid request parameters"),
		TEXT("Internal server error")
	};
	
	for (int32 i = 0; i < ErrorTypes.Num(); ++i)
	{
		FCarespaceError TestError;
		TestError.Type = ErrorTypes[i];
		TestError.Message = ErrorMessages[i];
		TestError.Code = FString::Printf(TEXT("ERROR_%d"), i);
		
		UMockCarespaceHTTPClient::FMockResponse ErrorResponse(TestError);
		MockClient->SetNextResponse(ErrorResponse);
		
		bool bCallbackExecuted = false;
		bool bRequestSuccessful = true;
		FCarespaceError ReceivedError;
		
		FOnCarespaceUsersReceived OnUsersReceived;
		OnUsersReceived.BindLambda([&](bool bWasSuccessful, const TArray<FCarespaceUser>& Users, const FCarespaceError& Error)
		{
			bCallbackExecuted = true;
			bRequestSuccessful = bWasSuccessful;
			ReceivedError = Error;
		});
		
		API->GetUsers(1, 20, TEXT(""), OnUsersReceived);
		FPlatformProcess::Sleep(0.1f);
		
		FString TestDescription = FString::Printf(TEXT("Error type %d callback should execute"), i);
		TestTrue(TestDescription, bCallbackExecuted);
		
		TestDescription = FString::Printf(TEXT("Error type %d request should fail"), i);
		TestFalse(TestDescription, bRequestSuccessful);
		
		TestDescription = FString::Printf(TEXT("Error type %d should match"), i);
		TestEqual(TestDescription, (int32)ReceivedError.Type, (int32)ErrorTypes[i]);
		
		TestDescription = FString::Printf(TEXT("Error message %d should match"), i);
		TestEqual(TestDescription, ReceivedError.Message, ErrorMessages[i]);
	}
	
	return !HasAnyErrors();
}

/**
 * Integration test for data consistency across multiple API operations.
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCarespaceDataConsistencyIntegrationTest, "CarespaceSDK.Integration.DataConsistency", 
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FCarespaceDataConsistencyIntegrationTest::RunTest(const FString& Parameters)
{
	UCarespaceAPI* API = UCarespaceAPI::CreateCarespaceAPI();
	UMockCarespaceHTTPClient* MockClient = NewObject<UMockCarespaceHTTPClient>();
	
	// Test creating a user, then retrieving it to ensure data consistency
	
	// Step 1: Create user response
	UMockCarespaceHTTPClient::FMockResponse CreateUserResponse;
	CreateUserResponse.bShouldSucceed = true;
	CreateUserResponse.ResponseContent = TEXT(R"({
		"success": true,
		"data": {
			"user": {
				"id": "new_user_123",
				"email": "newuser@test.com",
				"firstName": "New",
				"lastName": "User",
				"role": "clinician"
			}
		}
	})");
	
	MockClient->SetMockResponse(TEXT("POST"), TEXT("/api/users"), CreateUserResponse);
	
	// Step 2: Get single user response
	UMockCarespaceHTTPClient::FMockResponse GetUserResponse;
	GetUserResponse.bShouldSucceed = true;
	GetUserResponse.ResponseContent = TEXT(R"({
		"success": true,
		"data": {
			"user": {
				"id": "new_user_123",
				"email": "newuser@test.com",
				"firstName": "New",
				"lastName": "User",
				"role": "clinician"
			}
		}
	})");
	
	MockClient->SetMockResponse(TEXT("GET"), TEXT("/api/users/new_user_123"), GetUserResponse);
	
	// Create user
	FCarespaceCreateUserRequest CreateRequest;
	CreateRequest.Email = TEXT("newuser@test.com");
	CreateRequest.FirstName = TEXT("New");
	CreateRequest.LastName = TEXT("User");
	CreateRequest.Role = TEXT("clinician");
	CreateRequest.Password = TEXT("securePassword123");
	
	bool bCreateCompleted = false;
	bool bCreateSuccessful = false;
	TArray<FCarespaceUser> CreatedUsers;
	
	FOnCarespaceUsersReceived OnUserCreated;
	OnUserCreated.BindLambda([&](bool bWasSuccessful, const TArray<FCarespaceUser>& Users, const FCarespaceError& Error)
	{
		bCreateCompleted = true;
		bCreateSuccessful = bWasSuccessful;
		CreatedUsers = Users;
	});
	
	API->CreateUser(CreateRequest, OnUserCreated);
	FPlatformProcess::Sleep(0.1f);
	
	TestTrue("User creation should complete", bCreateCompleted);
	TestTrue("User creation should be successful", bCreateSuccessful);
	TestEqual("Should receive created user", CreatedUsers.Num(), 1);
	
	if (CreatedUsers.Num() > 0)
	{
		// Retrieve the created user
		bool bGetCompleted = false;
		bool bGetSuccessful = false;
		TArray<FCarespaceUser> RetrievedUsers;
		
		FOnCarespaceUsersReceived OnUserRetrieved;
		OnUserRetrieved.BindLambda([&](bool bWasSuccessful, const TArray<FCarespaceUser>& Users, const FCarespaceError& Error)
		{
			bGetCompleted = true;
			bGetSuccessful = bWasSuccessful;
			RetrievedUsers = Users;
		});
		
		API->GetUser(CreatedUsers[0].Id, OnUserRetrieved);
		FPlatformProcess::Sleep(0.1f);
		
		TestTrue("User retrieval should complete", bGetCompleted);
		TestTrue("User retrieval should be successful", bGetSuccessful);
		TestEqual("Should retrieve the user", RetrievedUsers.Num(), 1);
		
		if (RetrievedUsers.Num() > 0)
		{
			// Verify data consistency
			TestEqual("User ID should match", CreatedUsers[0].Id, RetrievedUsers[0].Id);
			TestEqual("User email should match", CreatedUsers[0].Email, RetrievedUsers[0].Email);
			TestEqual("User first name should match", CreatedUsers[0].FirstName, RetrievedUsers[0].FirstName);
			TestEqual("User last name should match", CreatedUsers[0].LastName, RetrievedUsers[0].LastName);
			TestEqual("User role should match", CreatedUsers[0].Role, RetrievedUsers[0].Role);
		}
	}
	
	// Verify the expected API calls were made
	TestEqual("Should have made create user request", MockClient->GetRequestCount(TEXT("POST"), TEXT("/api/users")), 1);
	TestEqual("Should have made get user request", MockClient->GetRequestCount(TEXT("GET"), TEXT("/api/users/new_user_123")), 1);
	
	return !HasAnyErrors();
}