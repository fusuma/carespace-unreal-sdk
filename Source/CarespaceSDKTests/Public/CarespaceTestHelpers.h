#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CarespaceTypes.h"
#include "MockCarespaceHTTPClient.h"
#include "CarespaceTestHelpers.generated.h"

/**
 * Utility class providing helper functions for Carespace SDK testing.
 * Contains factory methods for creating test data, common test scenarios,
 * and validation functions to simplify test writing.
 */
UCLASS(BlueprintType)
class CARESPACESDKTESTS_API UCarespaceTestHelpers : public UObject
{
	GENERATED_BODY()

public:
	// Test data factory methods
	/**
	 * Creates a mock user for testing purposes.
	 * 
	 * @param UserId Optional custom user ID (default: generated)
	 * @param Email Optional custom email (default: generated)
	 * @param Role Optional user role (default: "clinician")
	 * @return Mock user with test data
	 */
	UFUNCTION(BlueprintCallable, Category = "Test Helpers|Factory")
	static FCarespaceUser CreateMockUser(const FString& UserId = TEXT(""), const FString& Email = TEXT(""), const FString& Role = TEXT("clinician"));

	/**
	 * Creates a mock client (patient) for testing purposes.
	 * 
	 * @param ClientId Optional custom client ID (default: generated)
	 * @param FirstName Optional first name (default: generated)
	 * @param LastName Optional last name (default: generated)
	 * @return Mock client with test data
	 */
	UFUNCTION(BlueprintCallable, Category = "Test Helpers|Factory")
	static FCarespaceClient CreateMockClient(const FString& ClientId = TEXT(""), const FString& FirstName = TEXT(""), const FString& LastName = TEXT(""));

	/**
	 * Creates a mock program for testing purposes.
	 * 
	 * @param ProgramId Optional custom program ID (default: generated)
	 * @param Category Optional program category (default: "physical-therapy")
	 * @param DurationWeeks Optional duration in weeks (default: 8)
	 * @return Mock program with test data
	 */
	UFUNCTION(BlueprintCallable, Category = "Test Helpers|Factory")
	static FCarespaceProgram CreateMockProgram(const FString& ProgramId = TEXT(""), const FString& Category = TEXT("physical-therapy"), int32 DurationWeeks = 8);

	/**
	 * Creates a mock login request for testing.
	 * 
	 * @param Email Optional email (default: generated)
	 * @param Password Optional password (default: "TestPassword123")
	 * @return Mock login request
	 */
	UFUNCTION(BlueprintCallable, Category = "Test Helpers|Factory")
	static FCarespaceLoginRequest CreateMockLoginRequest(const FString& Email = TEXT(""), const FString& Password = TEXT("TestPassword123"));

	/**
	 * Creates a mock create user request for testing.
	 * 
	 * @param Email Optional email (default: generated)
	 * @param Role Optional role (default: "clinician")
	 * @return Mock create user request
	 */
	UFUNCTION(BlueprintCallable, Category = "Test Helpers|Factory")
	static FCarespaceCreateUserRequest CreateMockCreateUserRequest(const FString& Email = TEXT(""), const FString& Role = TEXT("clinician"));

	// Test scenario setup methods
	/**
	 * Sets up a mock HTTP client with successful responses for all common endpoints.
	 * 
	 * @param MockClient The mock client to configure
	 */
	UFUNCTION(BlueprintCallable, Category = "Test Helpers|Scenarios")
	static void SetupSuccessfulMockResponses(UMockCarespaceHTTPClient* MockClient);

	/**
	 * Sets up a mock HTTP client to simulate authentication failures.
	 * 
	 * @param MockClient The mock client to configure
	 */
	UFUNCTION(BlueprintCallable, Category = "Test Helpers|Scenarios")
	static void SetupAuthenticationFailureScenario(UMockCarespaceHTTPClient* MockClient);

	/**
	 * Sets up a mock HTTP client to simulate network connectivity issues.
	 * 
	 * @param MockClient The mock client to configure
	 */
	UFUNCTION(BlueprintCallable, Category = "Test Helpers|Scenarios")
	static void SetupNetworkErrorScenario(UMockCarespaceHTTPClient* MockClient);

	/**
	 * Sets up a mock HTTP client to simulate server errors.
	 * 
	 * @param MockClient The mock client to configure
	 */
	UFUNCTION(BlueprintCallable, Category = "Test Helpers|Scenarios")
	static void SetupServerErrorScenario(UMockCarespaceHTTPClient* MockClient);

	/**
	 * Sets up a mock HTTP client to simulate rate limiting.
	 * 
	 * @param MockClient The mock client to configure
	 */
	UFUNCTION(BlueprintCallable, Category = "Test Helpers|Scenarios")
	static void SetupRateLimitScenario(UMockCarespaceHTTPClient* MockClient);

	// Validation helper methods
	/**
	 * Validates that a user object contains expected test data.
	 * 
	 * @param User The user to validate
	 * @param ExpectedId Expected user ID
	 * @param ExpectedEmail Expected email
	 * @param ExpectedRole Expected role
	 * @return True if user matches expected data
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Test Helpers|Validation")
	static bool ValidateUser(const FCarespaceUser& User, const FString& ExpectedId, const FString& ExpectedEmail, const FString& ExpectedRole);

	/**
	 * Validates that a client object contains expected test data.
	 * 
	 * @param Client The client to validate
	 * @param ExpectedId Expected client ID
	 * @param ExpectedFirstName Expected first name
	 * @param ExpectedLastName Expected last name
	 * @return True if client matches expected data
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Test Helpers|Validation")
	static bool ValidateClient(const FCarespaceClient& Client, const FString& ExpectedId, const FString& ExpectedFirstName, const FString& ExpectedLastName);

	/**
	 * Validates that a program object contains expected test data.
	 * 
	 * @param Program The program to validate
	 * @param ExpectedId Expected program ID
	 * @param ExpectedName Expected program name
	 * @param ExpectedCategory Expected category
	 * @return True if program matches expected data
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Test Helpers|Validation")
	static bool ValidateProgram(const FCarespaceProgram& Program, const FString& ExpectedId, const FString& ExpectedName, const FString& ExpectedCategory);

	/**
	 * Validates that an error object contains expected error data.
	 * 
	 * @param Error The error to validate
	 * @param ExpectedType Expected error type
	 * @param ExpectedMessage Expected error message
	 * @param ExpectedCode Expected error code
	 * @return True if error matches expected data
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Test Helpers|Validation")
	static bool ValidateError(const FCarespaceError& Error, ECarespaceErrorType ExpectedType, const FString& ExpectedMessage, const FString& ExpectedCode);

	// JSON response generators
	/**
	 * Generates a JSON response for a successful users list request.
	 * 
	 * @param Users Array of users to include in response
	 * @param Page Current page number
	 * @param Limit Items per page
	 * @param Total Total number of users
	 * @return JSON string response
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Test Helpers|JSON")
	static FString GenerateUsersListResponse(const TArray<FCarespaceUser>& Users, int32 Page = 1, int32 Limit = 20, int32 Total = 0);

	/**
	 * Generates a JSON response for a successful single user request.
	 * 
	 * @param User The user to include in response
	 * @return JSON string response
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Test Helpers|JSON")
	static FString GenerateSingleUserResponse(const FCarespaceUser& User);

	/**
	 * Generates a JSON response for a successful clients list request.
	 * 
	 * @param Clients Array of clients to include in response
	 * @param Page Current page number
	 * @param Limit Items per page
	 * @param Total Total number of clients
	 * @return JSON string response
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Test Helpers|JSON")
	static FString GenerateClientsListResponse(const TArray<FCarespaceClient>& Clients, int32 Page = 1, int32 Limit = 20, int32 Total = 0);

	/**
	 * Generates a JSON response for a successful programs list request.
	 * 
	 * @param Programs Array of programs to include in response
	 * @param Page Current page number
	 * @param Limit Items per page
	 * @param Total Total number of programs
	 * @return JSON string response
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Test Helpers|JSON")
	static FString GenerateProgramsListResponse(const TArray<FCarespaceProgram>& Programs, int32 Page = 1, int32 Limit = 20, int32 Total = 0);

	/**
	 * Generates a JSON response for a successful login request.
	 * 
	 * @param AccessToken Access token to include
	 * @param RefreshToken Refresh token to include
	 * @param User User data to include
	 * @return JSON string response
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Test Helpers|JSON")
	static FString GenerateLoginResponse(const FString& AccessToken, const FString& RefreshToken, const FCarespaceUser& User);

	/**
	 * Generates a JSON error response.
	 * 
	 * @param ErrorCode Error code
	 * @param ErrorMessage Error message
	 * @param ErrorDetails Optional error details
	 * @return JSON string error response
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Test Helpers|JSON")
	static FString GenerateErrorResponse(const FString& ErrorCode, const FString& ErrorMessage, const FString& ErrorDetails = TEXT(""));

	// Utility methods
	/**
	 * Generates a unique test ID with optional prefix.
	 * 
	 * @param Prefix Optional prefix for the ID
	 * @return Unique test ID
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Test Helpers|Utilities")
	static FString GenerateTestId(const FString& Prefix = TEXT("test"));

	/**
	 * Generates a test email address.
	 * 
	 * @param Username Optional username part
	 * @param Domain Optional domain part
	 * @return Test email address
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Test Helpers|Utilities")
	static FString GenerateTestEmail(const FString& Username = TEXT(""), const FString& Domain = TEXT("test.com"));

	/**
	 * Waits for asynchronous operations to complete in tests.
	 * 
	 * @param MaxWaitTime Maximum time to wait in seconds
	 * @param CheckInterval How often to check in seconds
	 * @param Condition Lambda function that returns true when condition is met
	 */
	UFUNCTION(BlueprintCallable, Category = "Test Helpers|Utilities")
	static bool WaitForCondition(float MaxWaitTime = 5.0f, float CheckInterval = 0.1f, const TFunction<bool()>& Condition = nullptr);

	/**
	 * Simulates a delay in tests (useful for testing timeouts).
	 * 
	 * @param DelaySeconds Number of seconds to delay
	 */
	UFUNCTION(BlueprintCallable, Category = "Test Helpers|Utilities")
	static void SimulateDelay(float DelaySeconds);

private:
	static int32 TestIdCounter;
};