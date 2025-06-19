# Testing Guide - Carespace Unreal SDK

This document provides comprehensive guidelines for testing the Carespace Unreal SDK, including setup instructions, testing patterns, and best practices.

## Table of Contents

- [Overview](#overview)
- [Test Infrastructure](#test-infrastructure)
- [Running Tests](#running-tests)
- [Writing Tests](#writing-tests)
- [Test Categories](#test-categories)
- [Mock Testing](#mock-testing)
- [CI/CD Integration](#cicd-integration)
- [Troubleshooting](#troubleshooting)

## Overview

The Carespace Unreal SDK includes a comprehensive testing framework built on Unreal Engine's Automation Testing system. The test suite covers:

- **Unit Tests**: Individual component testing
- **Integration Tests**: API workflow testing  
- **Blueprint Tests**: Visual scripting functionality
- **Mock Testing**: Network-independent testing
- **Performance Tests**: Load and stress testing

## Test Infrastructure

### Module Structure

The testing infrastructure consists of:

```
Source/CarespaceSDKTests/
├── Public/
│   ├── CarespaceSDKTestsModule.h      # Test module interface
│   ├── MockCarespaceHTTPClient.h      # Mock HTTP client for testing
│   └── CarespaceTestHelpers.h         # Test utility functions
└── Private/
    ├── CarespaceSDKTestsModule.cpp     # Test module implementation
    ├── MockCarespaceHTTPClient.cpp     # Mock client implementation
    ├── CarespaceTestHelpers.cpp        # Test utilities implementation
    ├── CarespaceAPITests.cpp           # Main API tests
    ├── CarespaceAuthAPITests.cpp       # Authentication tests
    ├── CarespaceIntegrationTests.cpp   # Integration tests
    └── CarespaceBlueprintTests.cpp     # Blueprint tests
```

### Dependencies

The test module depends on:
- Core Unreal Engine modules
- CarespaceSDK module
- HTTP and JSON modules
- UnrealEd for editor functionality
- AutomationController for test execution

## Running Tests

### From Unreal Editor

1. Open your project in Unreal Editor
2. Go to **Window > Developer Tools > Session Frontend**
3. Switch to the **Automation** tab
4. Expand **CarespaceSDK** in the test tree
5. Select the tests you want to run
6. Click **Start Tests**

### From Command Line

```bash
# Run all CarespaceSDK tests
UE4Editor-Cmd YourProject.uproject -ExecCmds="Automation RunTests CarespaceSDK;quit" -TestExit="Automation Test Queue Empty" -ReportOutputDir="TestResults" -log

# Run specific test category
UE4Editor-Cmd YourProject.uproject -ExecCmds="Automation RunTests CarespaceSDK.API;quit" -TestExit="Automation Test Queue Empty" -ReportOutputDir="TestResults" -log

# Run with specific filters
UE4Editor-Cmd YourProject.uproject -ExecCmds="Automation RunTests CarespaceSDK.Auth;quit" -TestExit="Automation Test Queue Empty" -ReportOutputDir="TestResults" -log
```

### CI/CD Pipeline

Tests are automatically run via GitHub Actions on:
- Push to main/develop branches
- Pull requests
- Manual workflow dispatch

See `.github/workflows/test.yml` for configuration details.

## Writing Tests

### Basic Test Structure

```cpp
#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "CarespaceAPI.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FYourTestName, "CarespaceSDK.Category.TestName", 
    EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FYourTestName::RunTest(const FString& Parameters)
{
    // Arrange - Set up test data and conditions
    UCarespaceAPI* API = UCarespaceAPI::CreateCarespaceAPI();
    TestNotNull("API should be created", API);
    
    // Act - Execute the functionality being tested
    API->Initialize("https://api-test.carespace.ai", "test-key");
    
    // Assert - Verify the results
    TestNotNull("HTTP Client should be initialized", API->GetHTTPClient());
    TestNotNull("Auth API should be initialized", API->GetAuthAPI());
    
    return !HasAnyErrors();
}
```

### Using Mock HTTP Client

```cpp
#include "MockCarespaceHTTPClient.h"

bool FYourAPITest::RunTest(const FString& Parameters)
{
    // Create API and mock client
    UCarespaceAPI* API = UCarespaceAPI::CreateCarespaceAPI();
    UMockCarespaceHTTPClient* MockClient = NewObject<UMockCarespaceHTTPClient>();
    
    // Set up mock responses
    UMockCarespaceHTTPClient::FMockResponse Response;
    Response.bShouldSucceed = true;
    Response.ResponseContent = TEXT(R"({"success": true, "data": {}})");
    MockClient->SetMockResponse(TEXT("GET"), TEXT("/api/users"), Response);
    
    // Test with mock
    bool bCallbackExecuted = false;
    FOnCarespaceUsersReceived OnComplete;
    OnComplete.BindLambda([&](bool bSuccess, const TArray<FCarespaceUser>& Users, const FCarespaceError& Error)
    {
        bCallbackExecuted = true;
        TestTrue("Request should succeed", bSuccess);
    });
    
    API->GetUsers(1, 20, TEXT(""), OnComplete);
    
    // Wait for async completion
    FPlatformProcess::Sleep(0.1f);
    
    TestTrue("Callback should execute", bCallbackExecuted);
    return !HasAnyErrors();
}
```

### Using Test Helpers

```cpp
#include "CarespaceTestHelpers.h"

bool FYourHelperTest::RunTest(const FString& Parameters)
{
    // Create test data using helpers
    FCarespaceUser TestUser = UCarespaceTestHelpers::CreateMockUser();
    TestEqual("User should have test ID", TestUser.Id.Left(4), TEXT("user"));
    
    // Validate data using helpers
    bool bValid = UCarespaceTestHelpers::ValidateUser(
        TestUser, TestUser.Id, TestUser.Email, TestUser.Role);
    TestTrue("User should be valid", bValid);
    
    // Generate test JSON
    FString UserJSON = UCarespaceTestHelpers::GenerateSingleUserResponse(TestUser);
    TestFalse("JSON should not be empty", UserJSON.IsEmpty());
    
    return !HasAnyErrors();
}
```

## Test Categories

### Unit Tests (`CarespaceSDK.API.*`)

Test individual components in isolation:

- **Initialization**: API creation and configuration
- **User Management**: CRUD operations for users
- **Client Management**: CRUD operations for clients  
- **Program Management**: CRUD operations for programs
- **Error Handling**: Error response processing

### Authentication Tests (`CarespaceSDK.Auth.*`)

Test authentication workflows:

- **Login Success**: Valid credential authentication
- **Login Failure**: Invalid credential handling
- **Token Refresh**: Access token renewal
- **Password Reset**: Forgot/reset password flow
- **Logout**: Session termination

### Integration Tests (`CarespaceSDK.Integration.*`)

Test complete workflows:

- **Auth Workflow**: Login → API calls → Logout
- **Token Refresh**: Automatic token renewal
- **Error Handling**: End-to-end error scenarios
- **Data Consistency**: Create → Read → Update workflows

### Blueprint Tests (`CarespaceSDK.Blueprint.*`)

Test Blueprint functionality:

- **Library Functions**: Static utility functions
- **JSON Parsing**: Data structure conversion
- **Input Validation**: Email, password, phone validation
- **Delegate Helpers**: Blueprint-compatible callbacks

## Mock Testing

### Mock HTTP Client Features

The `UMockCarespaceHTTPClient` provides:

```cpp
// Set specific endpoint responses
MockClient->SetMockResponse("GET", "/api/users", SuccessResponse);

// Queue next response regardless of endpoint
MockClient->SetNextResponse(ErrorResponse);

// Pre-configured scenarios
MockClient->MockAuthenticationSuccess();
MockClient->MockAuthenticationFailure();
MockClient->MockNetworkTimeout();
MockClient->MockServerError();

// Request tracking
int32 RequestCount = MockClient->GetRequestCount("GET", "/api/users");
FString LastData = MockClient->GetLastRequestData("POST", "/api/auth/login");

// Network simulation
MockClient->SetNetworkDelayEnabled(true);
```

### Common Mock Scenarios

```cpp
// Success scenario
UCarespaceTestHelpers::SetupSuccessfulMockResponses(MockClient);

// Authentication failure
UCarespaceTestHelpers::SetupAuthenticationFailureScenario(MockClient);

// Network error
UCarespaceTestHelpers::SetupNetworkErrorScenario(MockClient);

// Server error
UCarespaceTestHelpers::SetupServerErrorScenario(MockClient);

// Rate limiting
UCarespaceTestHelpers::SetupRateLimitScenario(MockClient);
```

## CI/CD Integration

### GitHub Actions Workflow

The CI/CD pipeline includes:

1. **Build**: Compile SDK and test modules
2. **Test**: Run all automation tests
3. **Lint**: Code formatting and style checks
4. **Security**: Vulnerability scanning
5. **Documentation**: Validate documentation completeness
6. **Package**: Create release artifacts

### Test Results

Test results are:
- Uploaded as artifacts
- Parsed for pass/fail status
- Displayed in pull request checks
- Used to gate deployments

### Performance Benchmarks

The pipeline tracks:
- Test execution time
- Build performance
- Code coverage metrics
- Memory usage during tests

## Best Practices

### Test Naming

Use descriptive test names that follow the pattern:
```
CategorySDK.Subcategory.SpecificFunctionality
```

Examples:
- `CarespaceSDK.API.UserManagement`
- `CarespaceSDK.Auth.LoginSuccess`
- `CarespaceSDK.Integration.AuthWorkflow`

### Test Organization

- Group related tests in the same file
- Use clear test descriptions
- Follow Arrange-Act-Assert pattern
- Keep tests independent and isolated

### Mock Usage Guidelines

- Use mocks for external dependencies
- Test both success and failure scenarios
- Verify request parameters and counts
- Simulate real-world timing and delays

### Assertion Best Practices

```cpp
// Good: Descriptive error messages
TestEqual("User ID should match expected", User.Id, ExpectedId);

// Good: Check multiple conditions
TestNotNull("API should be created", API);
TestNotNull("HTTP client should exist", API->GetHTTPClient());

// Good: Validate async operations
TestTrue("Callback should execute", bCallbackExecuted);
TestTrue("Request should succeed", bRequestSuccessful);
```

### Performance Considerations

- Use `FPlatformProcess::Sleep()` sparingly
- Mock time-consuming operations
- Test with realistic data sizes
- Monitor memory usage in long-running tests

## Troubleshooting

### Common Issues

#### Tests Not Running
```
Problem: Tests don't appear in automation window
Solution: Ensure CarespaceSDKTests module is built and loaded
```

#### Mock Responses Not Working
```
Problem: Mock client returns default responses
Solution: Verify mock setup happens before API calls
```

#### Async Test Failures
```
Problem: Callbacks never execute in tests
Solution: Add appropriate delays or use WaitForCondition helper
```

#### Build Errors
```
Problem: Test module fails to compile
Solution: Check dependencies in CarespaceSDKTests.Build.cs
```

### Debug Techniques

1. **Logging**: Add detailed logs to track test execution
2. **Breakpoints**: Use debugger to step through test logic
3. **Mock Verification**: Check request counts and data
4. **Isolation**: Run individual tests to isolate issues

### Test Environment Setup

For local development:

1. Ensure Unreal Engine is properly installed
2. Build project in Development configuration
3. Load CarespaceSDKTests module
4. Run tests from Session Frontend

For CI/CD:

1. Configure Unreal Engine in build environment
2. Set up automation test execution
3. Configure result parsing and reporting
4. Set up artifact storage for test outputs

## Contributing Test Cases

When adding new functionality:

1. Write tests before implementation (TDD)
2. Cover both success and failure paths
3. Include edge cases and boundary conditions
4. Update this documentation with new patterns
5. Ensure tests pass in CI pipeline

For more information about testing patterns and examples, see the test files in `Source/CarespaceSDKTests/Private/`.