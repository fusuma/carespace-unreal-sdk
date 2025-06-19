#include "CarespaceAuthAPI.h"
#include "Json.h"

UCarespaceAuthAPI::UCarespaceAuthAPI()
{
	HTTPClient = nullptr;
}

void UCarespaceAuthAPI::Initialize(UCarespaceHTTPClient* InHTTPClient)
{
	HTTPClient = InHTTPClient;
}

void UCarespaceAuthAPI::Login(const FCarespaceLoginRequest& LoginRequest, const FOnCarespaceLoginComplete& OnComplete)
{
	if (!HTTPClient)
	{
		UE_LOG(LogTemp, Error, TEXT("CarespaceAuthAPI: HTTPClient is not initialized"));
		OnComplete.ExecuteIfBound(false, TEXT(""));
		return;
	}

	// Convert request to JSON
	FString JsonPayload = UCarespaceHTTPClient::StructToJsonString(FCarespaceLoginRequest::StaticStruct(), &LoginRequest);

	// Create delegate for handling response
	FOnHTTPResponse ResponseDelegate;
	ResponseDelegate.BindUFunction(this, FName("HandleLoginResponse"));
	ResponseDelegate.BindUFunction(this, FName("HandleLoginResponse"));

	// Add the OnComplete delegate as a parameter - we'll need to bind it differently
	HTTPClient->SendPOSTRequest(TEXT("/auth/login"), JsonPayload, 
		FOnHTTPResponse::CreateUFunction(this, FName("HandleLoginResponse"), OnComplete));
}

void UCarespaceAuthAPI::Logout(const FOnCarespaceRequestComplete& OnComplete)
{
	if (!HTTPClient)
	{
		UE_LOG(LogTemp, Error, TEXT("CarespaceAuthAPI: HTTPClient is not initialized"));
		OnComplete.ExecuteIfBound(false);
		return;
	}

	HTTPClient->SendPOSTRequest(TEXT("/auth/logout"), TEXT(""), 
		FOnHTTPResponse::CreateUFunction(this, FName("HandleGenericResponse"), OnComplete));
}

void UCarespaceAuthAPI::RefreshToken(const FString& RefreshToken, const FOnCarespaceLoginComplete& OnComplete)
{
	if (!HTTPClient)
	{
		UE_LOG(LogTemp, Error, TEXT("CarespaceAuthAPI: HTTPClient is not initialized"));
		OnComplete.ExecuteIfBound(false, TEXT(""));
		return;
	}

	// Create JSON payload
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField(TEXT("refresh_token"), RefreshToken);

	FString JsonPayload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonPayload);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	HTTPClient->SendPOSTRequest(TEXT("/auth/refresh"), JsonPayload, 
		FOnHTTPResponse::CreateUFunction(this, FName("HandleLoginResponse"), OnComplete));
}

void UCarespaceAuthAPI::ForgotPassword(const FString& Email, const FOnCarespaceRequestComplete& OnComplete)
{
	if (!HTTPClient)
	{
		UE_LOG(LogTemp, Error, TEXT("CarespaceAuthAPI: HTTPClient is not initialized"));
		OnComplete.ExecuteIfBound(false);
		return;
	}

	// Create JSON payload
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField(TEXT("email"), Email);

	FString JsonPayload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonPayload);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	HTTPClient->SendPOSTRequest(TEXT("/auth/forgot-password"), JsonPayload, 
		FOnHTTPResponse::CreateUFunction(this, FName("HandleGenericResponse"), OnComplete));
}

void UCarespaceAuthAPI::ResetPassword(const FString& Token, const FString& NewPassword, const FOnCarespaceRequestComplete& OnComplete)
{
	if (!HTTPClient)
	{
		UE_LOG(LogTemp, Error, TEXT("CarespaceAuthAPI: HTTPClient is not initialized"));
		OnComplete.ExecuteIfBound(false);
		return;
	}

	// Create JSON payload
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField(TEXT("token"), Token);
	JsonObject->SetStringField(TEXT("password"), NewPassword);

	FString JsonPayload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonPayload);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	HTTPClient->SendPOSTRequest(TEXT("/auth/reset-password"), JsonPayload, 
		FOnHTTPResponse::CreateUFunction(this, FName("HandleGenericResponse"), OnComplete));
}

void UCarespaceAuthAPI::ChangePassword(const FString& CurrentPassword, const FString& NewPassword, const FOnCarespaceRequestComplete& OnComplete)
{
	if (!HTTPClient)
	{
		UE_LOG(LogTemp, Error, TEXT("CarespaceAuthAPI: HTTPClient is not initialized"));
		OnComplete.ExecuteIfBound(false);
		return;
	}

	// Create JSON payload
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField(TEXT("current_password"), CurrentPassword);
	JsonObject->SetStringField(TEXT("new_password"), NewPassword);

	FString JsonPayload;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonPayload);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	HTTPClient->SendPOSTRequest(TEXT("/auth/change-password"), JsonPayload, 
		FOnHTTPResponse::CreateUFunction(this, FName("HandleGenericResponse"), OnComplete));
}

void UCarespaceAuthAPI::HandleLoginResponse(bool bWasSuccessful, const FString& ResponseContent, const FCarespaceError& Error, FOnCarespaceLoginComplete OnComplete)
{
	if (!bWasSuccessful)
	{
		UE_LOG(LogTemp, Error, TEXT("CarespaceAuthAPI: Login failed - %s"), *Error.ErrorMessage);
		OnComplete.ExecuteIfBound(false, TEXT(""));
		return;
	}

	// Parse access token from response
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseContent);

	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		FString AccessToken;
		if (JsonObject->TryGetStringField(TEXT("access_token"), AccessToken))
		{
			UE_LOG(LogTemp, Log, TEXT("CarespaceAuthAPI: Login successful"));
			OnComplete.ExecuteIfBound(true, AccessToken);
			return;
		}
	}

	UE_LOG(LogTemp, Error, TEXT("CarespaceAuthAPI: Failed to parse access token from response"));
	OnComplete.ExecuteIfBound(false, TEXT(""));
}

void UCarespaceAuthAPI::HandleGenericResponse(bool bWasSuccessful, const FString& ResponseContent, const FCarespaceError& Error, FOnCarespaceRequestComplete OnComplete)
{
	if (!bWasSuccessful)
	{
		UE_LOG(LogTemp, Error, TEXT("CarespaceAuthAPI: Request failed - %s"), *Error.ErrorMessage);
	}

	OnComplete.ExecuteIfBound(bWasSuccessful);
}