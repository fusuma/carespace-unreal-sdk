# Carespace Unreal Engine SDK

A comprehensive Unreal Engine plugin for integrating with the Carespace API. Build powerful healthcare and rehabilitation games and applications with both C++ and Blueprint support.

## Features

- 🎮 **Full Unreal Engine Integration** - Native UE plugin with C++ and Blueprint support
- 🔗 **Complete API Coverage** - Authentication, Users, Clients, Programs, and more
- 📘 **Blueprint Friendly** - Extensive Blueprint node library for visual scripting
- 🚀 **Async Operations** - Non-blocking HTTP requests with delegate callbacks
- 🛡️ **Error Handling** - Comprehensive error types and handling
- 🔧 **Cross-Platform** - Windows, Mac, Linux, iOS, Android support
- 🎯 **Game Development Focus** - Optimized for game and interactive application development

## Requirements

- Unreal Engine 5.0+
- Platform support: Windows, Mac, Linux, iOS, Android
- HTTP and Json modules (included with UE)

## Installation

### Option 1: Download Plugin
1. Download the latest release from [GitHub Releases](https://github.com/carespace/unreal-sdk/releases)
2. Extract to your project's `Plugins/` directory
3. Restart Unreal Editor
4. Enable the "Carespace SDK" plugin in the Plugin Browser

### Option 2: Git Submodule
```bash
cd YourProject/Plugins
git submodule add https://github.com/carespace/unreal-sdk.git CarespaceSDK
```

### Option 3: Manual Installation
1. Create a `Plugins/CarespaceSDK/` directory in your project
2. Copy all plugin files to this directory
3. Restart Unreal Editor
4. Enable the plugin

## Quick Start

### Blueprint Setup

1. **Create Carespace API Instance**
   - In your Blueprint, use the "Create Carespace API" node
   - Set your Base URL and API Key
   - Store the reference for later use

2. **Initialize and Login**
   ```
   Create Carespace API → Store in Variable
   ↓
   Login to Carespace (Email, Password) → On Login Complete
   ↓
   Set API Key (Access Token from login)
   ```

3. **Make API Calls**
   ```
   Get Users → On Users Received → Process User Array
   Get Clients → On Clients Received → Process Client Array
   Get Programs → On Programs Received → Process Program Array
   ```

### C++ Setup

```cpp
// In your header file
UPROPERTY(BlueprintReadWrite, Category = "Carespace")
class UCarespaceAPI* CarespaceAPI;

// In BeginPlay or initialization
void AYourActor::BeginPlay()
{
    Super::BeginPlay();
    
    // Create and initialize Carespace API
    CarespaceAPI = UCarespaceAPI::CreateCarespaceAPI(
        TEXT("https://api.carespace.ai"), 
        TEXT("your-api-key")
    );
}

// Making API calls
void AYourActor::LoginUser()
{
    if (!CarespaceAPI) return;
    
    FCarespaceLoginRequest LoginRequest(TEXT("user@example.com"), TEXT("password"));
    
    FOnCarespaceLoginComplete LoginDelegate;
    LoginDelegate.BindUFunction(this, FName("OnLoginComplete"));
    
    CarespaceAPI->GetAuthAPI()->Login(LoginRequest, LoginDelegate);
}

UFUNCTION()
void AYourActor::OnLoginComplete(bool bWasSuccessful, const FString& AccessToken)
{
    if (bWasSuccessful)
    {
        CarespaceAPI->SetAPIKey(AccessToken);
        UE_LOG(LogTemp, Log, TEXT("Login successful!"));
        
        // Now you can make authenticated API calls
        GetUsers();
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Login failed"));
    }
}

void AYourActor::GetUsers()
{
    FOnCarespaceUsersReceived UsersDelegate;
    UsersDelegate.BindUFunction(this, FName("OnUsersReceived"));
    
    CarespaceAPI->GetUsers(1, 20, TEXT(""), UsersDelegate);
}

UFUNCTION()
void AYourActor::OnUsersReceived(bool bWasSuccessful, const TArray<FCarespaceUser>& Users)
{
    if (bWasSuccessful)
    {
        for (const FCarespaceUser& User : Users)
        {
            UE_LOG(LogTemp, Log, TEXT("User: %s (%s)"), *User.Name, *User.Email);
        }
    }
}
```

## API Reference

### Core Classes

#### UCarespaceAPI
Main SDK class providing access to all endpoints.

```cpp
// Create instance
UCarespaceAPI* API = UCarespaceAPI::CreateCarespaceAPI(BaseURL, APIKey);

// Configuration
API->SetAPIKey(TEXT("your-api-key"));
API->SetBaseURL(TEXT("https://api.carespace.ai"));
API->SetTimeout(30.0f);

// Access sub-APIs
UCarespaceAuthAPI* AuthAPI = API->GetAuthAPI();
```

#### UCarespaceAuthAPI
Authentication operations.

```cpp
// Login
FCarespaceLoginRequest LoginRequest(Email, Password);
AuthAPI->Login(LoginRequest, OnLoginComplete);

// Logout
AuthAPI->Logout(OnLogoutComplete);

// Password operations
AuthAPI->ForgotPassword(Email, OnComplete);
AuthAPI->ResetPassword(Token, NewPassword, OnComplete);
AuthAPI->ChangePassword(CurrentPassword, NewPassword, OnComplete);
```

### Data Types

#### FCarespaceUser
```cpp
USTRUCT(BlueprintType)
struct FCarespaceUser
{
    UPROPERTY(BlueprintReadOnly)
    FString Id;
    
    UPROPERTY(BlueprintReadOnly)
    FString Email;
    
    UPROPERTY(BlueprintReadOnly)
    FString Name;
    
    UPROPERTY(BlueprintReadOnly)
    FString FirstName;
    
    UPROPERTY(BlueprintReadOnly)
    FString LastName;
    
    UPROPERTY(BlueprintReadOnly)
    FString Role;
    
    UPROPERTY(BlueprintReadOnly)
    bool bIsActive;
    
    UPROPERTY(BlueprintReadOnly)
    FDateTime CreatedAt;
    
    UPROPERTY(BlueprintReadOnly)
    FDateTime UpdatedAt;
};
```

#### FCarespaceClient
```cpp
USTRUCT(BlueprintType)
struct FCarespaceClient
{
    UPROPERTY(BlueprintReadOnly)
    FString Id;
    
    UPROPERTY(BlueprintReadWrite)
    FString Name;
    
    UPROPERTY(BlueprintReadWrite)
    FString Email;
    
    UPROPERTY(BlueprintReadWrite)
    FString Phone;
    
    UPROPERTY(BlueprintReadWrite)
    FDateTime DateOfBirth;
    
    UPROPERTY(BlueprintReadWrite)
    FString Gender;
    
    UPROPERTY(BlueprintReadWrite)
    FCarespaceAddress Address;
    
    UPROPERTY(BlueprintReadWrite)
    FString MedicalHistory;
    
    UPROPERTY(BlueprintReadWrite)
    FString Notes;
    
    // ... other properties
};
```

#### FCarespaceProgram
```cpp
USTRUCT(BlueprintType)
struct FCarespaceProgram
{
    UPROPERTY(BlueprintReadOnly)
    FString Id;
    
    UPROPERTY(BlueprintReadWrite)
    FString Name;
    
    UPROPERTY(BlueprintReadWrite)
    FString Description;
    
    UPROPERTY(BlueprintReadWrite)
    FString Category;
    
    UPROPERTY(BlueprintReadWrite)
    FString Difficulty;
    
    UPROPERTY(BlueprintReadWrite)
    int32 Duration;
    
    UPROPERTY(BlueprintReadOnly)
    TArray<FCarespaceExercise> Exercises;
    
    // ... other properties
};
```

### Blueprint Function Library

#### UCarespaceBlueprintLibrary
Utility functions for Blueprint development.

```cpp
// Factory methods
UCarespaceAPI* CreateCarespaceAPI(BaseURL, APIKey);

// Quick authentication
void LoginToCarespace(CarespaceAPI, Email, Password, OnComplete);

// Data creation utilities
FCarespaceCreateUserRequest MakeCreateUserRequest(Email, Name, FirstName, LastName, Role, Password);
FCarespaceClient MakeClientData(Name, Email, Phone, Gender, Address, MedicalHistory, Notes);
FCarespaceProgram MakeProgramData(Name, Description, Category, Difficulty, Duration, bIsTemplate);

// Validation utilities
bool IsValidEmail(Email);
bool IsValidPhone(Phone);

// Formatting utilities
FString FormatFullName(FirstName, LastName);
FString FormatDuration(DurationInSeconds);
FString FormatDate(DateTime);

// Array utilities
int32 GetUserCount(Users);
FCarespaceUser FindUserByID(Users, UserID, bFound);
```

## Common Use Cases

### Healthcare Game Integration

```cpp
// Patient progress tracking in a rehabilitation game
void ARehabilitationGame::TrackPatientProgress(const FString& ClientId, int32 SessionScore)
{
    // Get client data
    FOnCarespaceClientsReceived ClientDelegate;
    ClientDelegate.BindUFunction(this, FName("OnClientReceived"));
    CarespaceAPI->GetClient(ClientId, ClientDelegate);
}

UFUNCTION()
void ARehabilitationGame::OnClientReceived(bool bWasSuccessful, const TArray<FCarespaceClient>& Clients)
{
    if (bWasSuccessful && Clients.Num() > 0)
    {
        FCarespaceClient Client = Clients[0];
        
        // Update client notes with game progress
        Client.Notes += FString::Printf(TEXT("\\nGame Session: Score %d"), CurrentSessionScore);
        
        // Update client data
        FOnCarespaceClientsReceived UpdateDelegate;
        UpdateDelegate.BindUFunction(this, FName("OnClientUpdated"));
        CarespaceAPI->CreateClient(Client, UpdateDelegate); // This would be UpdateClient in a real implementation
    }
}
```

### Exercise Program Visualization

```cpp
// Display exercise programs in-game
void AExerciseVisualizer::LoadExercisePrograms()
{
    FOnCarespaceProgramsReceived ProgramsDelegate;
    ProgramsDelegate.BindUFunction(this, FName("OnProgramsReceived"));
    CarespaceAPI->GetPrograms(1, 50, TEXT("rehabilitation"), ProgramsDelegate);
}

UFUNCTION()
void AExerciseVisualizer::OnProgramsReceived(bool bWasSuccessful, const TArray<FCarespaceProgram>& Programs)
{
    if (bWasSuccessful)
    {
        for (const FCarespaceProgram& Program : Programs)
        {
            // Create visual representation of each program
            CreateProgramWidget(Program);
            
            // Load exercises for detailed view
            for (const FCarespaceExercise& Exercise : Program.Exercises)
            {
                CreateExerciseActor(Exercise);
            }
        }
    }
}
```

### Real-time Data Dashboard

```cpp
// Create a real-time dashboard widget
class UHealthcareDashboard : public UUserWidget
{
    // Timer for periodic updates
    FTimerHandle UpdateTimer;
    
    virtual void NativeConstruct() override
    {
        Super::NativeConstruct();
        
        // Update dashboard every 30 seconds
        GetWorld()->GetTimerManager().SetTimer(UpdateTimer, this, 
            &UHealthcareDashboard::UpdateDashboard, 30.0f, true);
    }
    
    void UpdateDashboard()
    {
        // Refresh user data
        CarespaceAPI->GetUsers(1, 100, TEXT(""), 
            FOnCarespaceUsersReceived::CreateUFunction(this, FName("UpdateUserStats")));
            
        // Refresh client data
        CarespaceAPI->GetClients(1, 100, TEXT(""), 
            FOnCarespaceClientsReceived::CreateUFunction(this, FName("UpdateClientStats")));
    }
};
```

## Error Handling

### Error Types

```cpp
UENUM(BlueprintType)
enum class ECarespaceErrorType : uint8
{
    None,
    NetworkError,
    AuthenticationError,
    ValidationError,
    ServerError,
    UnknownError
};
```

### Handling Errors in Blueprints

1. Check the "Was Successful" output of API calls
2. Use "Get Error Description" node to get human-readable error messages
3. Use "Is Authentication Error" / "Is Network Error" for specific error handling

### Handling Errors in C++

```cpp
UFUNCTION()
void AYourActor::OnAPICallComplete(bool bWasSuccessful, const FCarespaceError& Error)
{
    if (!bWasSuccessful)
    {
        switch (Error.ErrorType)
        {
            case ECarespaceErrorType::AuthenticationError:
                // Redirect to login screen
                HandleAuthenticationError();
                break;
                
            case ECarespaceErrorType::NetworkError:
                // Show network error message
                ShowNetworkErrorDialog();
                break;
                
            case ECarespaceErrorType::ValidationError:
                // Show validation error
                ShowValidationError(Error.ErrorMessage);
                break;
                
            default:
                // Generic error handling
                ShowGenericError(Error.ErrorMessage);
                break;
        }
    }
}
```

## Configuration

### Environment URLs

```cpp
// Development
CarespaceAPI->SetBaseURL(TEXT("https://api-dev.carespace.ai"));

// Staging
CarespaceAPI->SetBaseURL(TEXT("https://api-staging.carespace.ai"));

// Production
CarespaceAPI->SetBaseURL(TEXT("https://api.carespace.ai"));
```

### Project Settings

Add these settings to your `DefaultEngine.ini`:

```ini
[/Script/Engine.Engine]
+ActiveGameNameRedirects=(OldGameName="CarespaceSDK",NewGameName="/Script/CarespaceSDK")

[HTTP]
HttpTimeout=30.0
HttpConnectionTimeout=10.0
```

## Performance Considerations

### Async Best Practices

1. **Always use async calls** - Never block the game thread
2. **Cache frequently used data** - Store user/client data locally
3. **Implement pagination** - Use reasonable page sizes (10-50 items)
4. **Handle timeouts gracefully** - Set appropriate timeout values
5. **Debounce rapid calls** - Avoid making multiple identical requests

### Memory Management

```cpp
// Proper cleanup in your game classes
void AYourGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // Clear any pending HTTP requests
    if (CarespaceAPI)
    {
        CarespaceAPI = nullptr;
    }
    
    Super::EndPlay(EndPlayReason);
}
```

## Debugging

### Enable Verbose Logging

Add to `DefaultEngine.ini`:

```ini
[Core.Log]
LogHttp=Verbose
LogTemp=Verbose
```

### Common Debug Commands

```cpp
// Log API responses
UE_LOG(LogTemp, Log, TEXT("API Response: %s"), *ResponseContent);

// Verify API key
UE_LOG(LogTemp, Log, TEXT("API Key set: %s"), CarespaceAPI->GetHTTPClient() ? TEXT("Yes") : TEXT("No"));

// Check network connectivity
UE_LOG(LogTemp, Log, TEXT("Base URL: %s"), *CarespaceAPI->GetHTTPClient()->GetBaseURL());
```

## Examples

### Complete Blueprint Example

1. **Level Blueprint Setup**:
   ```
   Begin Play → Create Carespace API → Store in Level Variable
   ```

2. **Login Widget**:
   ```
   Email Input → Login Button → Login to Carespace → 
   On Success: Open Main Menu
   On Failure: Show Error Message
   ```

3. **Patient List Widget**:
   ```
   Widget Construct → Get Clients → 
   On Clients Received → Populate List View → 
   For Each Client: Create Client List Item
   ```

### Complete C++ Example

See the example project in the `Examples/` directory for a full implementation including:
- Login system
- Patient dashboard
- Exercise program browser
- Real-time data updates
- Error handling

## Troubleshooting

### Common Issues

1. **Plugin not loading**: Check UE version compatibility and plugin dependencies
2. **HTTP errors**: Verify API key and network connectivity
3. **Blueprint nodes missing**: Ensure plugin is enabled and project is recompiled
4. **Crash on startup**: Check for conflicting plugins or missing dependencies

### Support

- 📧 Email: [support@carespace.ai](mailto:support@carespace.ai)
- 🐛 Issues: [GitHub Issues](https://github.com/carespace/unreal-sdk/issues)
- 📖 Documentation: [docs.carespace.ai/unreal](https://docs.carespace.ai/unreal)
- 💬 Discord: [Carespace Developer Community](https://discord.gg/carespace)

## License

This plugin is licensed under the MIT License. See [LICENSE](LICENSE) file for details.

## Contributing

1. Fork the repository
2. Create a feature branch
3. Add your changes with tests
4. Submit a pull request

---

**Built with ❤️ for the Unreal Engine community by the Carespace Team**