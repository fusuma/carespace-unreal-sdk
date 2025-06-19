#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CarespaceHTTPClient.h"
#include "CarespaceAuthAPI.h"
#include "CarespaceTypes.h"
#include "CarespaceAPI.generated.h"

UCLASS(BlueprintType, Blueprintable)
class CARESPACESDK_API UCarespaceAPI : public UObject
{
	GENERATED_BODY()

public:
	UCarespaceAPI();

	// Configuration
	UFUNCTION(BlueprintCallable, Category = "Carespace")
	void Initialize(const FString& InBaseURL = TEXT("https://api-dev.carespace.ai"), const FString& InAPIKey = TEXT(""));

	UFUNCTION(BlueprintCallable, Category = "Carespace")
	void SetAPIKey(const FString& InAPIKey);

	UFUNCTION(BlueprintCallable, Category = "Carespace")
	void SetBaseURL(const FString& InBaseURL);

	UFUNCTION(BlueprintCallable, Category = "Carespace")
	void SetTimeout(float TimeoutSeconds);

	// API Access
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Carespace")
	UCarespaceAuthAPI* GetAuthAPI() const { return AuthAPI; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Carespace")
	UCarespaceHTTPClient* GetHTTPClient() const { return HTTPClient; }

	// Quick access methods for common operations
	UFUNCTION(BlueprintCallable, Category = "Carespace|Users")
	void GetUsers(int32 Page = 1, int32 Limit = 20, const FString& Search = TEXT(""), const FOnCarespaceUsersReceived& OnComplete = FOnCarespaceUsersReceived());

	UFUNCTION(BlueprintCallable, Category = "Carespace|Users")
	void GetUser(const FString& UserId, const FOnCarespaceUsersReceived& OnComplete);

	UFUNCTION(BlueprintCallable, Category = "Carespace|Users")
	void CreateUser(const FCarespaceCreateUserRequest& UserRequest, const FOnCarespaceUsersReceived& OnComplete);

	UFUNCTION(BlueprintCallable, Category = "Carespace|Clients")
	void GetClients(int32 Page = 1, int32 Limit = 20, const FString& Search = TEXT(""), const FOnCarespaceClientsReceived& OnComplete = FOnCarespaceClientsReceived());

	UFUNCTION(BlueprintCallable, Category = "Carespace|Clients")
	void GetClient(const FString& ClientId, const FOnCarespaceClientsReceived& OnComplete);

	UFUNCTION(BlueprintCallable, Category = "Carespace|Clients")
	void CreateClient(const FCarespaceClient& ClientData, const FOnCarespaceClientsReceived& OnComplete);

	UFUNCTION(BlueprintCallable, Category = "Carespace|Programs")
	void GetPrograms(int32 Page = 1, int32 Limit = 20, const FString& Category = TEXT(""), const FOnCarespaceProgramsReceived& OnComplete = FOnCarespaceProgramsReceived());

	UFUNCTION(BlueprintCallable, Category = "Carespace|Programs")
	void GetProgram(const FString& ProgramId, const FOnCarespaceProgramsReceived& OnComplete);

	UFUNCTION(BlueprintCallable, Category = "Carespace|Programs")
	void CreateProgram(const FCarespaceProgram& ProgramData, const FOnCarespaceProgramsReceived& OnComplete);

	// Static factory method
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