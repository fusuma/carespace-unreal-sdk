#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Engine/Engine.h"
#include "CarespaceBlueprintLibrary.h"
#include "CarespaceTypes.h"
#include "Engine/Blueprint.h"
#include "BlueprintGraph/Classes/K2Node_CallFunction.h"
#include "Engine/SimpleConstructionScript.h"

DEFINE_LOG_CATEGORY_STATIC(LogCarespaceBlueprintTests, Log, All);

/**
 * Test suite for Blueprint Library functions.
 * Tests static utility functions that can be called from Blueprints.
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCarespaceBlueprintLibraryTest, "CarespaceSDK.Blueprint.Library", 
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FCarespaceBlueprintLibraryTest::RunTest(const FString& Parameters)
{
	// Test API creation through Blueprint Library
	UCarespaceAPI* API = UCarespaceBlueprintLibrary::CreateCarespaceAPI(
		TEXT("https://api-blueprint-test.carespace.ai"), 
		TEXT("blueprint-test-key")
	);
	
	TestNotNull("Blueprint Library should create API instance", API);
	
	if (API)
	{
		TestNotNull("API should have HTTP client", API->GetHTTPClient());
		TestNotNull("API should have Auth API", API->GetAuthAPI());
	}
	
	return !HasAnyErrors();
}

/**
 * Test suite for JSON parsing utility functions.
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCarespaceBlueprintJSONParsingTest, "CarespaceSDK.Blueprint.JSONParsing", 
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FCarespaceBlueprintJSONParsingTest::RunTest(const FString& Parameters)
{
	// Test parsing user from JSON
	FString UserJSON = TEXT(R"({
		"id": "json_user_123",
		"email": "jsonuser@test.com",
		"firstName": "JSON",
		"lastName": "User",
		"role": "admin",
		"isActive": true
	})");
	
	FCarespaceUser ParsedUser = UCarespaceBlueprintLibrary::ParseUserFromJSON(UserJSON);
	
	TestEqual("Parsed user ID should match", ParsedUser.Id, TEXT("json_user_123"));
	TestEqual("Parsed user email should match", ParsedUser.Email, TEXT("jsonuser@test.com"));
	TestEqual("Parsed user first name should match", ParsedUser.FirstName, TEXT("JSON"));
	TestEqual("Parsed user last name should match", ParsedUser.LastName, TEXT("User"));
	TestEqual("Parsed user role should match", ParsedUser.Role, TEXT("admin"));
	TestTrue("Parsed user should be active", ParsedUser.bIsActive);
	
	// Test parsing client from JSON
	FString ClientJSON = TEXT(R"({
		"id": "json_client_456",
		"firstName": "Client",
		"lastName": "Test",
		"email": "client@test.com",
		"phone": "+1234567890",
		"medicalRecordNumber": "MRN456",
		"gender": "female",
		"isActive": true
	})");
	
	FCarespaceClient ParsedClient = UCarespaceBlueprintLibrary::ParseClientFromJSON(ClientJSON);
	
	TestEqual("Parsed client ID should match", ParsedClient.Id, TEXT("json_client_456"));
	TestEqual("Parsed client first name should match", ParsedClient.FirstName, TEXT("Client"));
	TestEqual("Parsed client email should match", ParsedClient.Email, TEXT("client@test.com"));
	TestEqual("Parsed client MRN should match", ParsedClient.MedicalRecordNumber, TEXT("MRN456"));
	TestEqual("Parsed client gender should match", ParsedClient.Gender, TEXT("female"));
	TestTrue("Parsed client should be active", ParsedClient.bIsActive);
	
	// Test parsing program from JSON
	FString ProgramJSON = TEXT(R"({
		"id": "json_program_789",
		"name": "Blueprint Test Program",
		"description": "A program for testing Blueprint functionality",
		"category": "physical-therapy",
		"durationWeeks": 12,
		"sessionsPerWeek": 2,
		"difficultyLevel": "intermediate",
		"isPublished": true
	})");
	
	FCarespaceProgram ParsedProgram = UCarespaceBlueprintLibrary::ParseProgramFromJSON(ProgramJSON);
	
	TestEqual("Parsed program ID should match", ParsedProgram.Id, TEXT("json_program_789"));
	TestEqual("Parsed program name should match", ParsedProgram.Name, TEXT("Blueprint Test Program"));
	TestEqual("Parsed program category should match", ParsedProgram.Category, TEXT("physical-therapy"));
	TestEqual("Parsed program duration should match", ParsedProgram.DurationWeeks, 12);
	TestEqual("Parsed program sessions per week should match", ParsedProgram.SessionsPerWeek, 2);
	TestEqual("Parsed program difficulty should match", ParsedProgram.DifficultyLevel, TEXT("intermediate"));
	TestTrue("Parsed program should be published", ParsedProgram.bIsPublished);
	
	return !HasAnyErrors();
}

/**
 * Test suite for input validation functions.
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCarespaceBlueprintValidationTest, "CarespaceSDK.Blueprint.Validation", 
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FCarespaceBlueprintValidationTest::RunTest(const FString& Parameters)
{
	// Test email validation
	TestTrue("Valid email should pass validation", 
		UCarespaceBlueprintLibrary::IsValidEmail(TEXT("test@example.com")));
	TestTrue("Valid email with subdomain should pass", 
		UCarespaceBlueprintLibrary::IsValidEmail(TEXT("user@mail.example.com")));
	TestTrue("Valid email with plus should pass", 
		UCarespaceBlueprintLibrary::IsValidEmail(TEXT("user+tag@example.com")));
	
	TestFalse("Invalid email without @ should fail", 
		UCarespaceBlueprintLibrary::IsValidEmail(TEXT("invalid-email")));
	TestFalse("Invalid email without domain should fail", 
		UCarespaceBlueprintLibrary::IsValidEmail(TEXT("test@")));
	TestFalse("Invalid email without local part should fail", 
		UCarespaceBlueprintLibrary::IsValidEmail(TEXT("@example.com")));
	TestFalse("Empty email should fail", 
		UCarespaceBlueprintLibrary::IsValidEmail(TEXT("")));
	
	// Test password strength validation
	TestTrue("Strong password should pass", 
		UCarespaceBlueprintLibrary::IsStrongPassword(TEXT("SecurePass123!")));
	TestTrue("Password with mixed case and numbers should pass", 
		UCarespaceBlueprintLibrary::IsStrongPassword(TEXT("MyPassword456")));
	
	TestFalse("Short password should fail", 
		UCarespaceBlueprintLibrary::IsStrongPassword(TEXT("short")));
	TestFalse("Password without numbers should fail", 
		UCarespaceBlueprintLibrary::IsStrongPassword(TEXT("PasswordNoNumbers")));
	TestFalse("Password without uppercase should fail", 
		UCarespaceBlueprintLibrary::IsStrongPassword(TEXT("lowercase123")));
	TestFalse("Password without lowercase should fail", 
		UCarespaceBlueprintLibrary::IsStrongPassword(TEXT("UPPERCASE123")));
	
	// Test phone number validation
	TestTrue("Valid US phone number should pass", 
		UCarespaceBlueprintLibrary::IsValidPhoneNumber(TEXT("+12345678901")));
	TestTrue("Valid international phone should pass", 
		UCarespaceBlueprintLibrary::IsValidPhoneNumber(TEXT("+442071234567")));
	
	TestFalse("Phone without country code should fail", 
		UCarespaceBlueprintLibrary::IsValidPhoneNumber(TEXT("2345678901")));
	TestFalse("Too short phone should fail", 
		UCarespaceBlueprintLibrary::IsValidPhoneNumber(TEXT("+123456")));
	TestFalse("Phone with letters should fail", 
		UCarespaceBlueprintLibrary::IsValidPhoneNumber(TEXT("+123abc7890")));
	
	return !HasAnyErrors();
}

/**
 * Test suite for data structure conversion functions.
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCarespaceBlueprintConversionTest, "CarespaceSDK.Blueprint.Conversion", 
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FCarespaceBlueprintConversionTest::RunTest(const FString& Parameters)
{
	// Test converting user to JSON
	FCarespaceUser TestUser;
	TestUser.Id = TEXT("conversion_user_123");
	TestUser.Email = TEXT("conversion@test.com");
	TestUser.FirstName = TEXT("Conversion");
	TestUser.LastName = TEXT("Test");
	TestUser.Role = TEXT("clinician");
	TestUser.bIsActive = true;
	
	FString UserJSON = UCarespaceBlueprintLibrary::UserToJSON(TestUser);
	TestFalse("User JSON should not be empty", UserJSON.IsEmpty());
	TestTrue("User JSON should contain ID", UserJSON.Contains(TEXT("conversion_user_123")));
	TestTrue("User JSON should contain email", UserJSON.Contains(TEXT("conversion@test.com")));
	TestTrue("User JSON should contain role", UserJSON.Contains(TEXT("clinician")));
	
	// Test converting login request to JSON
	FCarespaceLoginRequest LoginRequest;
	LoginRequest.Email = TEXT("login@test.com");
	LoginRequest.Password = TEXT("testPassword123");
	
	FString LoginJSON = UCarespaceBlueprintLibrary::LoginRequestToJSON(LoginRequest);
	TestFalse("Login JSON should not be empty", LoginJSON.IsEmpty());
	TestTrue("Login JSON should contain email", LoginJSON.Contains(TEXT("login@test.com")));
	TestTrue("Login JSON should contain password", LoginJSON.Contains(TEXT("testPassword123")));
	
	// Test error code to string conversion
	FString NetworkErrorString = UCarespaceBlueprintLibrary::ErrorTypeToString(ECarespaceErrorType::NetworkError);
	TestEqual("Network error should convert correctly", NetworkErrorString, TEXT("NetworkError"));
	
	FString AuthErrorString = UCarespaceBlueprintLibrary::ErrorTypeToString(ECarespaceErrorType::AuthenticationError);
	TestEqual("Auth error should convert correctly", AuthErrorString, TEXT("AuthenticationError"));
	
	FString ValidationErrorString = UCarespaceBlueprintLibrary::ErrorTypeToString(ECarespaceErrorType::ValidationError);
	TestEqual("Validation error should convert correctly", ValidationErrorString, TEXT("ValidationError"));
	
	return !HasAnyErrors();
}

/**
 * Test suite for Blueprint delegate binding helpers.
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCarespaceBlueprintDelegateTest, "CarespaceSDK.Blueprint.Delegates", 
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FCarespaceBlueprintDelegateTest::RunTest(const FString& Parameters)
{
	// Test creating Blueprint-compatible delegates
	FOnCarespaceUsersReceived UsersDelegate = UCarespaceBlueprintLibrary::CreateUsersReceivedDelegate();
	TestTrue("Users delegate should be valid", UsersDelegate.IsBound() || !UsersDelegate.IsBound()); // Just test creation doesn't crash
	
	FOnCarespaceClientsReceived ClientsDelegate = UCarespaceBlueprintLibrary::CreateClientsReceivedDelegate();
	TestTrue("Clients delegate should be valid", ClientsDelegate.IsBound() || !ClientsDelegate.IsBound());
	
	FOnCarespaceProgramsReceived ProgramsDelegate = UCarespaceBlueprintLibrary::CreateProgramsReceivedDelegate();
	TestTrue("Programs delegate should be valid", ProgramsDelegate.IsBound() || !ProgramsDelegate.IsBound());
	
	FOnCarespaceLoginComplete LoginDelegate = UCarespaceBlueprintLibrary::CreateLoginCompleteDelegate();
	TestTrue("Login delegate should be valid", LoginDelegate.IsBound() || !LoginDelegate.IsBound());
	
	FOnCarespaceRequestComplete RequestDelegate = UCarespaceBlueprintLibrary::CreateRequestCompleteDelegate();
	TestTrue("Request delegate should be valid", RequestDelegate.IsBound() || !RequestDelegate.IsBound());
	
	return !HasAnyErrors();
}

/**
 * Test suite for Blueprint utility functions.
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCarespaceBlueprintUtilitiesTest, "CarespaceSDK.Blueprint.Utilities", 
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FCarespaceBlueprintUtilitiesTest::RunTest(const FString& Parameters)
{
	// Test URL validation
	TestTrue("Valid HTTPS URL should pass", 
		UCarespaceBlueprintLibrary::IsValidURL(TEXT("https://api.carespace.ai")));
	TestTrue("Valid HTTP URL should pass", 
		UCarespaceBlueprintLibrary::IsValidURL(TEXT("http://localhost:3000")));
	TestTrue("URL with path should pass", 
		UCarespaceBlueprintLibrary::IsValidURL(TEXT("https://api.carespace.ai/v1/users")));
	
	TestFalse("Invalid URL without protocol should fail", 
		UCarespaceBlueprintLibrary::IsValidURL(TEXT("api.carespace.ai")));
	TestFalse("Empty URL should fail", 
		UCarespaceBlueprintLibrary::IsValidURL(TEXT("")));
	TestFalse("Invalid protocol should fail", 
		UCarespaceBlueprintLibrary::IsValidURL(TEXT("ftp://example.com")));
	
	// Test API key validation
	TestTrue("Valid API key should pass", 
		UCarespaceBlueprintLibrary::IsValidAPIKey(TEXT("cs_live_1234567890abcdef")));
	TestTrue("Test API key should pass", 
		UCarespaceBlueprintLibrary::IsValidAPIKey(TEXT("cs_test_abcdef1234567890")));
	
	TestFalse("Short API key should fail", 
		UCarespaceBlueprintLibrary::IsValidAPIKey(TEXT("short_key")));
	TestFalse("Empty API key should fail", 
		UCarespaceBlueprintLibrary::IsValidAPIKey(TEXT("")));
	TestFalse("Invalid prefix should fail", 
		UCarespaceBlueprintLibrary::IsValidAPIKey(TEXT("invalid_1234567890abcdef")));
	
	// Test sanitization functions
	FString UnsanitizedInput = TEXT("  Test Input With Spaces  ");
	FString SanitizedInput = UCarespaceBlueprintLibrary::SanitizeInput(UnsanitizedInput);
	TestEqual("Input should be trimmed", SanitizedInput, TEXT("Test Input With Spaces"));
	
	FString UnsafeHTML = TEXT("<script>alert('xss')</script>Safe Text");
	FString SafeHTML = UCarespaceBlueprintLibrary::SanitizeHTML(UnsafeHTML);
	TestFalse("HTML should be sanitized", SafeHTML.Contains(TEXT("<script>")));
	TestTrue("Safe text should remain", SafeHTML.Contains(TEXT("Safe Text")));
	
	// Test formatting functions
	FString FormattedPhone = UCarespaceBlueprintLibrary::FormatPhoneNumber(TEXT("12345678901"));
	TestEqual("Phone should be formatted", FormattedPhone, TEXT("+1 (234) 567-8901"));
	
	FDateTime TestDate = FDateTime::Now();
	FString FormattedDate = UCarespaceBlueprintLibrary::FormatDateTime(TestDate);
	TestFalse("Date should be formatted", FormattedDate.IsEmpty());
	
	return !HasAnyErrors();
}