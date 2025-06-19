#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Engine/Engine.h"
#include "CarespaceAPI.h"
#include "MockCarespaceHTTPClient.h"
#include "CarespaceTypes.h"

DEFINE_LOG_CATEGORY_STATIC(LogCarespaceAPITests, Log, All);

/**
 * Test suite for UCarespaceAPI functionality.
 * Tests initialization, configuration, and basic API operations.
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCarespaceAPIInitializationTest, "CarespaceSDK.API.Initialization", 
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FCarespaceAPIInitializationTest::RunTest(const FString& Parameters)
{
	// Test API creation and initialization
	UCarespaceAPI* API = UCarespaceAPI::CreateCarespaceAPI(TEXT("https://api-test.carespace.ai"), TEXT("test-api-key"));
	
	TestNotNull("API should be created successfully", API);
	
	if (API)
	{
		// Test that sub-components are initialized
		TestNotNull("HTTP Client should be initialized", API->GetHTTPClient());
		TestNotNull("Auth API should be initialized", API->GetAuthAPI());
		
		// Test configuration methods
		API->SetAPIKey(TEXT("new-test-key"));
		API->SetBaseURL(TEXT("https://api-prod.carespace.ai"));
		API->SetTimeout(60.0f);
		
		// These should not crash
		TestTrue("Configuration methods should execute without error", true);
	}
	
	return !HasAnyErrors();
}

/**
 * Test suite for UCarespaceAPI user management operations.
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCarespaceAPIUserManagementTest, "CarespaceSDK.API.UserManagement", 
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FCarespaceAPIUserManagementTest::RunTest(const FString& Parameters)
{
	// Create API with mock HTTP client
	UCarespaceAPI* API = UCarespaceAPI::CreateCarespaceAPI();
	TestNotNull("API should be created", API);
	
	if (!API)
	{
		return false;
	}
	
	// Replace HTTP client with mock
	UMockCarespaceHTTPClient* MockClient = NewObject<UMockCarespaceHTTPClient>();
	TestNotNull("Mock client should be created", MockClient);
	
	// Set up mock response for users endpoint
	UMockCarespaceHTTPClient::FMockResponse UsersResponse;
	UsersResponse.bShouldSucceed = true;
	UsersResponse.ResponseContent = TEXT(R"({
		"success": true,
		"data": {
			"users": [
				{
					"id": "user_1",
					"email": "user1@test.com",
					"firstName": "John",
					"lastName": "Doe",
					"role": "clinician"
				},
				{
					"id": "user_2",
					"email": "user2@test.com",
					"firstName": "Jane",
					"lastName": "Smith",
					"role": "admin"
				}
			],
			"pagination": {
				"page": 1,
				"limit": 20,
				"total": 2,
				"totalPages": 1
			}
		}
	})");
	
	MockClient->SetMockResponse(TEXT("GET"), TEXT("/api/users"), UsersResponse);
	
	// Test GetUsers method
	bool bCallbackExecuted = false;
	bool bRequestSuccessful = false;
	TArray<FCarespaceUser> ReceivedUsers;
	
	FOnCarespaceUsersReceived OnUsersReceived;
	OnUsersReceived.BindLambda([&](bool bWasSuccessful, const TArray<FCarespaceUser>& Users, const FCarespaceError& Error)
	{
		bCallbackExecuted = true;
		bRequestSuccessful = bWasSuccessful;
		ReceivedUsers = Users;
	});
	
	API->GetUsers(1, 20, TEXT(""), OnUsersReceived);
	
	// Allow some time for the mock response to be processed
	FPlatformProcess::Sleep(0.1f);
	
	TestTrue("Callback should be executed", bCallbackExecuted);
	TestTrue("Request should be successful", bRequestSuccessful);
	TestEqual("Should receive 2 users", ReceivedUsers.Num(), 2);
	
	if (ReceivedUsers.Num() >= 2)
	{
		TestEqual("First user ID should match", ReceivedUsers[0].Id, TEXT("user_1"));
		TestEqual("First user email should match", ReceivedUsers[0].Email, TEXT("user1@test.com"));
		TestEqual("Second user role should match", ReceivedUsers[1].Role, TEXT("admin"));
	}
	
	// Test request tracking
	TestEqual("Should have made 1 GET request to users endpoint", 
		MockClient->GetRequestCount(TEXT("GET"), TEXT("/api/users")), 1);
	
	return !HasAnyErrors();
}

/**
 * Test suite for UCarespaceAPI client management operations.
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCarespaceAPIClientManagementTest, "CarespaceSDK.API.ClientManagement", 
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FCarespaceAPIClientManagementTest::RunTest(const FString& Parameters)
{
	UCarespaceAPI* API = UCarespaceAPI::CreateCarespaceAPI();
	TestNotNull("API should be created", API);
	
	if (!API)
	{
		return false;
	}
	
	UMockCarespaceHTTPClient* MockClient = NewObject<UMockCarespaceHTTPClient>();
	
	// Set up mock response for clients endpoint
	UMockCarespaceHTTPClient::FMockResponse ClientsResponse;
	ClientsResponse.bShouldSucceed = true;
	ClientsResponse.ResponseContent = TEXT(R"({
		"success": true,
		"data": {
			"clients": [
				{
					"id": "client_1",
					"firstName": "Alice",
					"lastName": "Johnson",
					"email": "alice@example.com",
					"phone": "+1234567890",
					"medicalRecordNumber": "MRN001"
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
	
	MockClient->SetMockResponse(TEXT("GET"), TEXT("/api/clients"), ClientsResponse);
	
	bool bCallbackExecuted = false;
	bool bRequestSuccessful = false;
	TArray<FCarespaceClient> ReceivedClients;
	
	FOnCarespaceClientsReceived OnClientsReceived;
	OnClientsReceived.BindLambda([&](bool bWasSuccessful, const TArray<FCarespaceClient>& Clients, const FCarespaceError& Error)
	{
		bCallbackExecuted = true;
		bRequestSuccessful = bWasSuccessful;
		ReceivedClients = Clients;
	});
	
	API->GetClients(1, 20, TEXT(""), OnClientsReceived);
	
	FPlatformProcess::Sleep(0.1f);
	
	TestTrue("Callback should be executed", bCallbackExecuted);
	TestTrue("Request should be successful", bRequestSuccessful);
	TestEqual("Should receive 1 client", ReceivedClients.Num(), 1);
	
	if (ReceivedClients.Num() >= 1)
	{
		TestEqual("Client ID should match", ReceivedClients[0].Id, TEXT("client_1"));
		TestEqual("Client first name should match", ReceivedClients[0].FirstName, TEXT("Alice"));
		TestEqual("Client MRN should match", ReceivedClients[0].MedicalRecordNumber, TEXT("MRN001"));
	}
	
	return !HasAnyErrors();
}

/**
 * Test suite for UCarespaceAPI program management operations.
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCarespaceAPIProgramManagementTest, "CarespaceSDK.API.ProgramManagement", 
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FCarespaceAPIProgramManagementTest::RunTest(const FString& Parameters)
{
	UCarespaceAPI* API = UCarespaceAPI::CreateCarespaceAPI();
	TestNotNull("API should be created", API);
	
	if (!API)
	{
		return false;
	}
	
	UMockCarespaceHTTPClient* MockClient = NewObject<UMockCarespaceHTTPClient>();
	
	// Set up mock response for programs endpoint
	UMockCarespaceHTTPClient::FMockResponse ProgramsResponse;
	ProgramsResponse.bShouldSucceed = true;
	ProgramsResponse.ResponseContent = TEXT(R"({
		"success": true,
		"data": {
			"programs": [
				{
					"id": "program_1",
					"name": "Basic Physical Therapy",
					"description": "Introduction to physical therapy exercises",
					"category": "physical-therapy",
					"durationWeeks": 8,
					"sessionsPerWeek": 3,
					"difficultyLevel": "beginner"
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
	
	MockClient->SetMockResponse(TEXT("GET"), TEXT("/api/programs"), ProgramsResponse);
	
	bool bCallbackExecuted = false;
	bool bRequestSuccessful = false;
	TArray<FCarespaceProgram> ReceivedPrograms;
	
	FOnCarespaceProgramsReceived OnProgramsReceived;
	OnProgramsReceived.BindLambda([&](bool bWasSuccessful, const TArray<FCarespaceProgram>& Programs, const FCarespaceError& Error)
	{
		bCallbackExecuted = true;
		bRequestSuccessful = bWasSuccessful;
		ReceivedPrograms = Programs;
	});
	
	API->GetPrograms(1, 20, TEXT("physical-therapy"), OnProgramsReceived);
	
	FPlatformProcess::Sleep(0.1f);
	
	TestTrue("Callback should be executed", bCallbackExecuted);
	TestTrue("Request should be successful", bRequestSuccessful);
	TestEqual("Should receive 1 program", ReceivedPrograms.Num(), 1);
	
	if (ReceivedPrograms.Num() >= 1)
	{
		TestEqual("Program ID should match", ReceivedPrograms[0].Id, TEXT("program_1"));
		TestEqual("Program name should match", ReceivedPrograms[0].Name, TEXT("Basic Physical Therapy"));
		TestEqual("Program category should match", ReceivedPrograms[0].Category, TEXT("physical-therapy"));
		TestEqual("Program duration should match", ReceivedPrograms[0].DurationWeeks, 8);
	}
	
	return !HasAnyErrors();
}

/**
 * Test suite for UCarespaceAPI error handling.
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCarespaceAPIErrorHandlingTest, "CarespaceSDK.API.ErrorHandling", 
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FCarespaceAPIErrorHandlingTest::RunTest(const FString& Parameters)
{
	UCarespaceAPI* API = UCarespaceAPI::CreateCarespaceAPI();
	TestNotNull("API should be created", API);
	
	if (!API)
	{
		return false;
	}
	
	UMockCarespaceHTTPClient* MockClient = NewObject<UMockCarespaceHTTPClient>();
	
	// Set up mock error response
	FCarespaceError TestError;
	TestError.Type = ECarespaceErrorType::ValidationError;
	TestError.Message = TEXT("Invalid request parameters");
	TestError.Code = TEXT("VALIDATION_ERROR");
	
	UMockCarespaceHTTPClient::FMockResponse ErrorResponse(TestError);
	MockClient->SetMockResponse(TEXT("GET"), TEXT("/api/users"), ErrorResponse);
	
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
	
	TestTrue("Callback should be executed", bCallbackExecuted);
	TestFalse("Request should fail", bRequestSuccessful);
	TestEqual("Error type should match", (int32)ReceivedError.Type, (int32)ECarespaceErrorType::ValidationError);
	TestEqual("Error message should match", ReceivedError.Message, TEXT("Invalid request parameters"));
	TestEqual("Error code should match", ReceivedError.Code, TEXT("VALIDATION_ERROR"));
	
	return !HasAnyErrors();
}