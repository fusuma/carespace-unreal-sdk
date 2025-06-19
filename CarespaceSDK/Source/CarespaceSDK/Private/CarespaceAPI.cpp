#include "CarespaceAPI.h"
#include "Json.h"
#include "JsonObjectConverter.h"

UCarespaceAPI::UCarespaceAPI()
{
	HTTPClient = nullptr;
	AuthAPI = nullptr;
}

void UCarespaceAPI::Initialize(const FString& InBaseURL, const FString& InAPIKey)
{
	// Create HTTP client
	HTTPClient = NewObject<UCarespaceHTTPClient>(this);
	HTTPClient->SetBaseURL(InBaseURL);
	if (!InAPIKey.IsEmpty())
	{
		HTTPClient->SetAPIKey(InAPIKey);
	}

	// Create Auth API
	AuthAPI = NewObject<UCarespaceAuthAPI>(this);
	AuthAPI->Initialize(HTTPClient);

	UE_LOG(LogTemp, Log, TEXT("CarespaceAPI initialized with Base URL: %s"), *InBaseURL);
}

void UCarespaceAPI::SetAPIKey(const FString& InAPIKey)
{
	if (HTTPClient)
	{
		HTTPClient->SetAPIKey(InAPIKey);
		UE_LOG(LogTemp, Log, TEXT("CarespaceAPI: API Key updated"));
	}
}

void UCarespaceAPI::SetBaseURL(const FString& InBaseURL)
{
	if (HTTPClient)
	{
		HTTPClient->SetBaseURL(InBaseURL);
		UE_LOG(LogTemp, Log, TEXT("CarespaceAPI: Base URL updated to %s"), *InBaseURL);
	}
}

void UCarespaceAPI::SetTimeout(float TimeoutSeconds)
{
	if (HTTPClient)
	{
		HTTPClient->SetTimeout(TimeoutSeconds);
	}
}

UCarespaceAPI* UCarespaceAPI::CreateCarespaceAPI(const FString& BaseURL, const FString& APIKey)
{
	UCarespaceAPI* NewAPI = NewObject<UCarespaceAPI>();
	NewAPI->Initialize(BaseURL, APIKey);
	return NewAPI;
}

// Users API implementations
void UCarespaceAPI::GetUsers(int32 Page, int32 Limit, const FString& Search, const FOnCarespaceUsersReceived& OnComplete)
{
	if (!HTTPClient)
	{
		UE_LOG(LogTemp, Error, TEXT("CarespaceAPI: Not initialized"));
		OnComplete.ExecuteIfBound(false, TArray<FCarespaceUser>());
		return;
	}

	TMap<FString, FString> QueryParams;
	QueryParams.Add(TEXT("page"), FString::FromInt(Page));
	QueryParams.Add(TEXT("limit"), FString::FromInt(Limit));
	if (!Search.IsEmpty())
	{
		QueryParams.Add(TEXT("search"), Search);
	}

	HTTPClient->SendGETRequest(TEXT("/users"), QueryParams, 
		FOnHTTPResponse::CreateUFunction(this, FName("HandleUsersResponse"), OnComplete));
}

void UCarespaceAPI::GetUser(const FString& UserId, const FOnCarespaceUsersReceived& OnComplete)
{
	if (!HTTPClient)
	{
		UE_LOG(LogTemp, Error, TEXT("CarespaceAPI: Not initialized"));
		OnComplete.ExecuteIfBound(false, TArray<FCarespaceUser>());
		return;
	}

	FString Endpoint = FString::Printf(TEXT("/users/%s"), *UserId);
	HTTPClient->SendGETRequest(Endpoint, TMap<FString, FString>(), 
		FOnHTTPResponse::CreateUFunction(this, FName("HandleSingleUserResponse"), OnComplete));
}

void UCarespaceAPI::CreateUser(const FCarespaceCreateUserRequest& UserRequest, const FOnCarespaceUsersReceived& OnComplete)
{
	if (!HTTPClient)
	{
		UE_LOG(LogTemp, Error, TEXT("CarespaceAPI: Not initialized"));
		OnComplete.ExecuteIfBound(false, TArray<FCarespaceUser>());
		return;
	}

	FString JsonPayload = UCarespaceHTTPClient::StructToJsonString(FCarespaceCreateUserRequest::StaticStruct(), &UserRequest);
	HTTPClient->SendPOSTRequest(TEXT("/users"), JsonPayload, 
		FOnHTTPResponse::CreateUFunction(this, FName("HandleSingleUserResponse"), OnComplete));
}

// Clients API implementations
void UCarespaceAPI::GetClients(int32 Page, int32 Limit, const FString& Search, const FOnCarespaceClientsReceived& OnComplete)
{
	if (!HTTPClient)
	{
		UE_LOG(LogTemp, Error, TEXT("CarespaceAPI: Not initialized"));
		OnComplete.ExecuteIfBound(false, TArray<FCarespaceClient>());
		return;
	}

	TMap<FString, FString> QueryParams;
	QueryParams.Add(TEXT("page"), FString::FromInt(Page));
	QueryParams.Add(TEXT("limit"), FString::FromInt(Limit));
	if (!Search.IsEmpty())
	{
		QueryParams.Add(TEXT("search"), Search);
	}

	HTTPClient->SendGETRequest(TEXT("/clients"), QueryParams, 
		FOnHTTPResponse::CreateUFunction(this, FName("HandleClientsResponse"), OnComplete));
}

void UCarespaceAPI::GetClient(const FString& ClientId, const FOnCarespaceClientsReceived& OnComplete)
{
	if (!HTTPClient)
	{
		UE_LOG(LogTemp, Error, TEXT("CarespaceAPI: Not initialized"));
		OnComplete.ExecuteIfBound(false, TArray<FCarespaceClient>());
		return;
	}

	FString Endpoint = FString::Printf(TEXT("/clients/%s"), *ClientId);
	HTTPClient->SendGETRequest(Endpoint, TMap<FString, FString>(), 
		FOnHTTPResponse::CreateUFunction(this, FName("HandleSingleClientResponse"), OnComplete));
}

void UCarespaceAPI::CreateClient(const FCarespaceClient& ClientData, const FOnCarespaceClientsReceived& OnComplete)
{
	if (!HTTPClient)
	{
		UE_LOG(LogTemp, Error, TEXT("CarespaceAPI: Not initialized"));
		OnComplete.ExecuteIfBound(false, TArray<FCarespaceClient>());
		return;
	}

	FString JsonPayload = UCarespaceHTTPClient::StructToJsonString(FCarespaceClient::StaticStruct(), &ClientData);
	HTTPClient->SendPOSTRequest(TEXT("/clients"), JsonPayload, 
		FOnHTTPResponse::CreateUFunction(this, FName("HandleSingleClientResponse"), OnComplete));
}

// Programs API implementations
void UCarespaceAPI::GetPrograms(int32 Page, int32 Limit, const FString& Category, const FOnCarespaceProgramsReceived& OnComplete)
{
	if (!HTTPClient)
	{
		UE_LOG(LogTemp, Error, TEXT("CarespaceAPI: Not initialized"));
		OnComplete.ExecuteIfBound(false, TArray<FCarespaceProgram>());
		return;
	}

	TMap<FString, FString> QueryParams;
	QueryParams.Add(TEXT("page"), FString::FromInt(Page));
	QueryParams.Add(TEXT("limit"), FString::FromInt(Limit));
	if (!Category.IsEmpty())
	{
		QueryParams.Add(TEXT("category"), Category);
	}

	HTTPClient->SendGETRequest(TEXT("/programs"), QueryParams, 
		FOnHTTPResponse::CreateUFunction(this, FName("HandleProgramsResponse"), OnComplete));
}

void UCarespaceAPI::GetProgram(const FString& ProgramId, const FOnCarespaceProgramsReceived& OnComplete)
{
	if (!HTTPClient)
	{
		UE_LOG(LogTemp, Error, TEXT("CarespaceAPI: Not initialized"));
		OnComplete.ExecuteIfBound(false, TArray<FCarespaceProgram>());
		return;
	}

	FString Endpoint = FString::Printf(TEXT("/programs/%s"), *ProgramId);
	HTTPClient->SendGETRequest(Endpoint, TMap<FString, FString>(), 
		FOnHTTPResponse::CreateUFunction(this, FName("HandleSingleProgramResponse"), OnComplete));
}

void UCarespaceAPI::CreateProgram(const FCarespaceProgram& ProgramData, const FOnCarespaceProgramsReceived& OnComplete)
{
	if (!HTTPClient)
	{
		UE_LOG(LogTemp, Error, TEXT("CarespaceAPI: Not initialized"));
		OnComplete.ExecuteIfBound(false, TArray<FCarespaceProgram>());
		return;
	}

	FString JsonPayload = UCarespaceHTTPClient::StructToJsonString(FCarespaceProgram::StaticStruct(), &ProgramData);
	HTTPClient->SendPOSTRequest(TEXT("/programs"), JsonPayload, 
		FOnHTTPResponse::CreateUFunction(this, FName("HandleSingleProgramResponse"), OnComplete));
}

// Response handlers
void UCarespaceAPI::HandleUsersResponse(bool bWasSuccessful, const FString& ResponseContent, const FCarespaceError& Error, FOnCarespaceUsersReceived OnComplete)
{
	if (!bWasSuccessful)
	{
		UE_LOG(LogTemp, Error, TEXT("CarespaceAPI: GetUsers failed - %s"), *Error.ErrorMessage);
		OnComplete.ExecuteIfBound(false, TArray<FCarespaceUser>());
		return;
	}

	TArray<FCarespaceUser> Users = ParseUsersFromJson(ResponseContent);
	OnComplete.ExecuteIfBound(true, Users);
}

void UCarespaceAPI::HandleSingleUserResponse(bool bWasSuccessful, const FString& ResponseContent, const FCarespaceError& Error, FOnCarespaceUsersReceived OnComplete)
{
	if (!bWasSuccessful)
	{
		UE_LOG(LogTemp, Error, TEXT("CarespaceAPI: User operation failed - %s"), *Error.ErrorMessage);
		OnComplete.ExecuteIfBound(false, TArray<FCarespaceUser>());
		return;
	}

	FCarespaceUser User = ParseUserFromJson(ResponseContent);
	TArray<FCarespaceUser> Users;
	Users.Add(User);
	OnComplete.ExecuteIfBound(true, Users);
}

void UCarespaceAPI::HandleClientsResponse(bool bWasSuccessful, const FString& ResponseContent, const FCarespaceError& Error, FOnCarespaceClientsReceived OnComplete)
{
	if (!bWasSuccessful)
	{
		UE_LOG(LogTemp, Error, TEXT("CarespaceAPI: GetClients failed - %s"), *Error.ErrorMessage);
		OnComplete.ExecuteIfBound(false, TArray<FCarespaceClient>());
		return;
	}

	TArray<FCarespaceClient> Clients = ParseClientsFromJson(ResponseContent);
	OnComplete.ExecuteIfBound(true, Clients);
}

void UCarespaceAPI::HandleSingleClientResponse(bool bWasSuccessful, const FString& ResponseContent, const FCarespaceError& Error, FOnCarespaceClientsReceived OnComplete)
{
	if (!bWasSuccessful)
	{
		UE_LOG(LogTemp, Error, TEXT("CarespaceAPI: Client operation failed - %s"), *Error.ErrorMessage);
		OnComplete.ExecuteIfBound(false, TArray<FCarespaceClient>());
		return;
	}

	FCarespaceClient Client = ParseClientFromJson(ResponseContent);
	TArray<FCarespaceClient> Clients;
	Clients.Add(Client);
	OnComplete.ExecuteIfBound(true, Clients);
}

void UCarespaceAPI::HandleProgramsResponse(bool bWasSuccessful, const FString& ResponseContent, const FCarespaceError& Error, FOnCarespaceProgramsReceived OnComplete)
{
	if (!bWasSuccessful)
	{
		UE_LOG(LogTemp, Error, TEXT("CarespaceAPI: GetPrograms failed - %s"), *Error.ErrorMessage);
		OnComplete.ExecuteIfBound(false, TArray<FCarespaceProgram>());
		return;
	}

	TArray<FCarespaceProgram> Programs = ParseProgramsFromJson(ResponseContent);
	OnComplete.ExecuteIfBound(true, Programs);
}

void UCarespaceAPI::HandleSingleProgramResponse(bool bWasSuccessful, const FString& ResponseContent, const FCarespaceError& Error, FOnCarespaceProgramsReceived OnComplete)
{
	if (!bWasSuccessful)
	{
		UE_LOG(LogTemp, Error, TEXT("CarespaceAPI: Program operation failed - %s"), *Error.ErrorMessage);
		OnComplete.ExecuteIfBound(false, TArray<FCarespaceProgram>());
		return;
	}

	FCarespaceProgram Program = ParseProgramFromJson(ResponseContent);
	TArray<FCarespaceProgram> Programs;
	Programs.Add(Program);
	OnComplete.ExecuteIfBound(true, Programs);
}

// Utility parsing methods
TArray<FCarespaceUser> UCarespaceAPI::ParseUsersFromJson(const FString& JsonString)
{
	TArray<FCarespaceUser> Users;
	
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		const TArray<TSharedPtr<FJsonValue>>* DataArray;
		if (JsonObject->TryGetArrayField(TEXT("data"), DataArray))
		{
			for (const TSharedPtr<FJsonValue>& Value : *DataArray)
			{
				FCarespaceUser User;
				if (FJsonObjectConverter::JsonObjectToUStruct(Value->AsObject().ToSharedRef(), FCarespaceUser::StaticStruct(), &User))
				{
					Users.Add(User);
				}
			}
		}
	}

	return Users;
}

TArray<FCarespaceClient> UCarespaceAPI::ParseClientsFromJson(const FString& JsonString)
{
	TArray<FCarespaceClient> Clients;
	
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		const TArray<TSharedPtr<FJsonValue>>* DataArray;
		if (JsonObject->TryGetArrayField(TEXT("data"), DataArray))
		{
			for (const TSharedPtr<FJsonValue>& Value : *DataArray)
			{
				FCarespaceClient Client;
				if (FJsonObjectConverter::JsonObjectToUStruct(Value->AsObject().ToSharedRef(), FCarespaceClient::StaticStruct(), &Client))
				{
					Clients.Add(Client);
				}
			}
		}
	}

	return Clients;
}

TArray<FCarespaceProgram> UCarespaceAPI::ParseProgramsFromJson(const FString& JsonString)
{
	TArray<FCarespaceProgram> Programs;
	
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		const TArray<TSharedPtr<FJsonValue>>* DataArray;
		if (JsonObject->TryGetArrayField(TEXT("data"), DataArray))
		{
			for (const TSharedPtr<FJsonValue>& Value : *DataArray)
			{
				FCarespaceProgram Program;
				if (FJsonObjectConverter::JsonObjectToUStruct(Value->AsObject().ToSharedRef(), FCarespaceProgram::StaticStruct(), &Program))
				{
					Programs.Add(Program);
				}
			}
		}
	}

	return Programs;
}

FCarespaceUser UCarespaceAPI::ParseUserFromJson(const FString& JsonString)
{
	FCarespaceUser User;
	UCarespaceHTTPClient::JsonStringToStruct(JsonString, FCarespaceUser::StaticStruct(), &User);
	return User;
}

FCarespaceClient UCarespaceAPI::ParseClientFromJson(const FString& JsonString)
{
	FCarespaceClient Client;
	UCarespaceHTTPClient::JsonStringToStruct(JsonString, FCarespaceClient::StaticStruct(), &Client);
	return Client;
}

FCarespaceProgram UCarespaceAPI::ParseProgramFromJson(const FString& JsonString)
{
	FCarespaceProgram Program;
	UCarespaceHTTPClient::JsonStringToStruct(JsonString, FCarespaceProgram::StaticStruct(), &Program);
	return Program;
}