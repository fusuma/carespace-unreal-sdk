#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CarespaceHTTPClient.h"
#include "CarespaceAuthAPI.h"
#include "CarespaceTypes.h"
#include "CarespaceAPI.generated.h"

/**
 * Main SDK class providing high-level access to the Carespace API.
 * This class serves as the primary entry point for all Carespace functionality,
 * managing HTTP communications, authentication, and providing convenient
 * methods for common operations like user, client, and program management.
 * 
 * Usage:
 *   UCarespaceAPI* API = UCarespaceAPI::CreateCarespaceAPI("https://api.carespace.ai", "your-api-key");
 *   API->GetAuthAPI()->Login("user@example.com", "password", OnSuccess, OnFailure);
 */
UCLASS(BlueprintType, Blueprintable)
class CARESPACESDK_API UCarespaceAPI : public UObject
{
	GENERATED_BODY()

public:
	UCarespaceAPI();

	// Configuration
	/**
	 * Initializes the Carespace API with the specified base URL and API key.
	 * This method must be called before making any API requests.
	 * 
	 * @param InBaseURL The base URL for the Carespace API (default: development server)
	 * @param InAPIKey The API key for authentication (can be set later with SetAPIKey)
	 */
	UFUNCTION(BlueprintCallable, Category = "Carespace")
	void Initialize(const FString& InBaseURL = TEXT("https://api-dev.carespace.ai"), const FString& InAPIKey = TEXT(""));

	/**
	 * Sets the API key used for authenticating requests.
	 * This key will be included in the Authorization header of all requests.
	 * 
	 * @param InAPIKey The API key provided by Carespace
	 */
	UFUNCTION(BlueprintCallable, Category = "Carespace")
	void SetAPIKey(const FString& InAPIKey);

	/**
	 * Updates the base URL for API requests.
	 * Useful for switching between development, staging, and production environments.
	 * 
	 * @param InBaseURL The new base URL (e.g., "https://api.carespace.ai")
	 */
	UFUNCTION(BlueprintCallable, Category = "Carespace")
	void SetBaseURL(const FString& InBaseURL);

	/**
	 * Sets the timeout duration for HTTP requests.
	 * 
	 * @param TimeoutSeconds Maximum time to wait for a response (default: 30 seconds)
	 */
	UFUNCTION(BlueprintCallable, Category = "Carespace")
	void SetTimeout(float TimeoutSeconds);

	// API Access
	/**
	 * Returns the authentication API instance for login, logout, and user management.
	 * 
	 * @return Pointer to the authentication API instance
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Carespace")
	UCarespaceAuthAPI* GetAuthAPI() const { return AuthAPI; }

	/**
	 * Returns the HTTP client instance for direct API access.
	 * Use this for custom endpoints not covered by the high-level API methods.
	 * 
	 * @return Pointer to the HTTP client instance
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Carespace")
	UCarespaceHTTPClient* GetHTTPClient() const { return HTTPClient; }

	// Quick access methods for common operations
	/**
	 * Retrieves a paginated list of users from the Carespace API.
	 * 
	 * @param Page Page number to retrieve (1-based, default: 1)
	 * @param Limit Number of users per page (default: 20, max: 100)
	 * @param Search Optional search term to filter users by name or email
	 * @param OnComplete Delegate called when the request completes with user data or error
	 */
	UFUNCTION(BlueprintCallable, Category = "Carespace|Users")
	void GetUsers(int32 Page = 1, int32 Limit = 20, const FString& Search = TEXT(""), const FOnCarespaceUsersReceived& OnComplete = FOnCarespaceUsersReceived());

	/**
	 * Retrieves detailed information for a specific user.
	 * 
	 * @param UserId Unique identifier of the user to retrieve
	 * @param OnComplete Delegate called when the request completes with user data or error
	 */
	UFUNCTION(BlueprintCallable, Category = "Carespace|Users")
	void GetUser(const FString& UserId, const FOnCarespaceUsersReceived& OnComplete);

	/**
	 * Creates a new user in the Carespace system.
	 * 
	 * @param UserRequest Complete user information including email, name, and role
	 * @param OnComplete Delegate called when the request completes with created user data or error
	 */
	UFUNCTION(BlueprintCallable, Category = "Carespace|Users")
	void CreateUser(const FCarespaceCreateUserRequest& UserRequest, const FOnCarespaceUsersReceived& OnComplete);

	/**
	 * Retrieves a paginated list of clients (patients) from the Carespace API.
	 * 
	 * @param Page Page number to retrieve (1-based, default: 1)
	 * @param Limit Number of clients per page (default: 20, max: 100)
	 * @param Search Optional search term to filter clients by name or identifier
	 * @param OnComplete Delegate called when the request completes with client data or error
	 */
	UFUNCTION(BlueprintCallable, Category = "Carespace|Clients")
	void GetClients(int32 Page = 1, int32 Limit = 20, const FString& Search = TEXT(""), const FOnCarespaceClientsReceived& OnComplete = FOnCarespaceClientsReceived());

	/**
	 * Retrieves detailed information for a specific client.
	 * 
	 * @param ClientId Unique identifier of the client to retrieve
	 * @param OnComplete Delegate called when the request completes with client data or error
	 */
	UFUNCTION(BlueprintCallable, Category = "Carespace|Clients")
	void GetClient(const FString& ClientId, const FOnCarespaceClientsReceived& OnComplete);

	/**
	 * Creates a new client (patient) in the Carespace system.
	 * 
	 * @param ClientData Complete client information including personal details and medical data
	 * @param OnComplete Delegate called when the request completes with created client data or error
	 */
	UFUNCTION(BlueprintCallable, Category = "Carespace|Clients")
	void CreateClient(const FCarespaceClient& ClientData, const FOnCarespaceClientsReceived& OnComplete);

	/**
	 * Retrieves a paginated list of rehabilitation programs from the Carespace API.
	 * 
	 * @param Page Page number to retrieve (1-based, default: 1)
	 * @param Limit Number of programs per page (default: 20, max: 100)
	 * @param Category Optional category filter (e.g., "physical-therapy", "occupational-therapy")
	 * @param OnComplete Delegate called when the request completes with program data or error
	 */
	UFUNCTION(BlueprintCallable, Category = "Carespace|Programs")
	void GetPrograms(int32 Page = 1, int32 Limit = 20, const FString& Category = TEXT(""), const FOnCarespaceProgramsReceived& OnComplete = FOnCarespaceProgramsReceived());

	/**
	 * Retrieves detailed information for a specific rehabilitation program.
	 * 
	 * @param ProgramId Unique identifier of the program to retrieve
	 * @param OnComplete Delegate called when the request completes with program data or error
	 */
	UFUNCTION(BlueprintCallable, Category = "Carespace|Programs")
	void GetProgram(const FString& ProgramId, const FOnCarespaceProgramsReceived& OnComplete);

	/**
	 * Creates a new rehabilitation program in the Carespace system.
	 * 
	 * @param ProgramData Complete program information including exercises, duration, and goals
	 * @param OnComplete Delegate called when the request completes with created program data or error
	 */
	UFUNCTION(BlueprintCallable, Category = "Carespace|Programs")
	void CreateProgram(const FCarespaceProgram& ProgramData, const FOnCarespaceProgramsReceived& OnComplete);

	/**
	 * Static factory method to create and initialize a new Carespace API instance.
	 * This is the recommended way to create the API object in both C++ and Blueprint.
	 * 
	 * @param BaseURL The base URL for the Carespace API (default: development server)
	 * @param APIKey The API key for authentication (optional, can be set later)
	 * @return Newly created and initialized Carespace API instance
	 */
	UFUNCTION(BlueprintCallable, Category = "Carespace", CallInEditor = true)
	static UCarespaceAPI* CreateCarespaceAPI(const FString& BaseURL = TEXT("https://api-dev.carespace.ai"), const FString& APIKey = TEXT(""));

private:
	UPROPERTY()
	UCarespaceHTTPClient* HTTPClient;

	UPROPERTY()
	UCarespaceAuthAPI* AuthAPI;

	// Response handlers
	UFUNCTION()
	void HandleUsersResponse(bool bWasSuccessful, const FString& ResponseContent, const FCarespaceError& Error, FOnCarespaceUsersReceived OnComplete);

	UFUNCTION()
	void HandleSingleUserResponse(bool bWasSuccessful, const FString& ResponseContent, const FCarespaceError& Error, FOnCarespaceUsersReceived OnComplete);

	UFUNCTION()
	void HandleClientsResponse(bool bWasSuccessful, const FString& ResponseContent, const FCarespaceError& Error, FOnCarespaceClientsReceived OnComplete);

	UFUNCTION()
	void HandleSingleClientResponse(bool bWasSuccessful, const FString& ResponseContent, const FCarespaceError& Error, FOnCarespaceClientsReceived OnComplete);

	UFUNCTION()
	void HandleProgramsResponse(bool bWasSuccessful, const FString& ResponseContent, const FCarespaceError& Error, FOnCarespaceProgramsReceived OnComplete);

	UFUNCTION()
	void HandleSingleProgramResponse(bool bWasSuccessful, const FString& ResponseContent, const FCarespaceError& Error, FOnCarespaceProgramsReceived OnComplete);

	// Utility methods
	TArray<FCarespaceUser> ParseUsersFromJson(const FString& JsonString);
	TArray<FCarespaceClient> ParseClientsFromJson(const FString& JsonString);
	TArray<FCarespaceProgram> ParseProgramsFromJson(const FString& JsonString);
	FCarespaceUser ParseUserFromJson(const FString& JsonString);
	FCarespaceClient ParseClientFromJson(const FString& JsonString);
	FCarespaceProgram ParseProgramFromJson(const FString& JsonString);
};