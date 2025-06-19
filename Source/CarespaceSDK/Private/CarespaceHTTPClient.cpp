#include "CarespaceHTTPClient.h"
#include "HttpModule.h"
#include "JsonObjectConverter.h"

UCarespaceHTTPClient::UCarespaceHTTPClient()
{
	BaseURL = TEXT("https://api-dev.carespace.ai");
	APIKey = TEXT("");
	TimeoutSeconds = 30.0f;
}

void UCarespaceHTTPClient::SetBaseURL(const FString& InBaseURL)
{
	BaseURL = InBaseURL;
}

void UCarespaceHTTPClient::SetAPIKey(const FString& InAPIKey)
{
	APIKey = InAPIKey;
}

void UCarespaceHTTPClient::SetTimeout(float InTimeoutSeconds)
{
	TimeoutSeconds = InTimeoutSeconds;
}

void UCarespaceHTTPClient::SendGETRequest(const FString& Endpoint, const TMap<FString, FString>& QueryParameters, const FOnHTTPResponse& OnComplete)
{
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	ConfigureRequest(Request, TEXT("GET"), BuildURL(Endpoint, QueryParameters));
	
	Request->OnProcessRequestComplete().BindUObject(this, &UCarespaceHTTPClient::HandleResponse, OnComplete);
	Request->ProcessRequest();
}

void UCarespaceHTTPClient::SendPOSTRequest(const FString& Endpoint, const FString& JsonPayload, const FOnHTTPResponse& OnComplete)
{
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	ConfigureRequest(Request, TEXT("POST"), BuildURL(Endpoint));
	
	if (!JsonPayload.IsEmpty())
	{
		Request->SetContentAsString(JsonPayload);
	}
	
	Request->OnProcessRequestComplete().BindUObject(this, &UCarespaceHTTPClient::HandleResponse, OnComplete);
	Request->ProcessRequest();
}

void UCarespaceHTTPClient::SendPUTRequest(const FString& Endpoint, const FString& JsonPayload, const FOnHTTPResponse& OnComplete)
{
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	ConfigureRequest(Request, TEXT("PUT"), BuildURL(Endpoint));
	
	if (!JsonPayload.IsEmpty())
	{
		Request->SetContentAsString(JsonPayload);
	}
	
	Request->OnProcessRequestComplete().BindUObject(this, &UCarespaceHTTPClient::HandleResponse, OnComplete);
	Request->ProcessRequest();
}

void UCarespaceHTTPClient::SendDELETERequest(const FString& Endpoint, const FOnHTTPResponse& OnComplete)
{
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	ConfigureRequest(Request, TEXT("DELETE"), BuildURL(Endpoint));
	
	Request->OnProcessRequestComplete().BindUObject(this, &UCarespaceHTTPClient::HandleResponse, OnComplete);
	Request->ProcessRequest();
}

void UCarespaceHTTPClient::ConfigureRequest(TSharedRef<IHttpRequest> Request, const FString& Verb, const FString& URL)
{
	Request->SetURL(URL);
	Request->SetVerb(Verb);
	Request->SetTimeout(TimeoutSeconds);
	
	// Set common headers
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetHeader(TEXT("Accept"), TEXT("application/json"));
	
	// Set authorization header if API key is provided
	if (!APIKey.IsEmpty())
	{
		Request->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *APIKey));
	}
}

void UCarespaceHTTPClient::HandleResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FOnHTTPResponse OnComplete)
{
	FString ResponseContent;
	FCarespaceError Error;

	if (bWasSuccessful && Response.IsValid())
	{
		ResponseContent = Response->GetContentAsString();
		
		// Check for HTTP errors
		int32 ResponseCode = Response->GetResponseCode();
		if (ResponseCode >= 200 && ResponseCode < 300)
		{
			// Success
			OnComplete.ExecuteIfBound(true, ResponseContent, Error);
			return;
		}
		else
		{
			Error = ProcessError(Response, bWasSuccessful);
		}
	}
	else
	{
		Error = ProcessError(Response, bWasSuccessful);
	}

	OnComplete.ExecuteIfBound(false, ResponseContent, Error);
}

FCarespaceError UCarespaceHTTPClient::ProcessError(FHttpResponsePtr Response, bool bWasSuccessful)
{
	FCarespaceError Error;

	if (!bWasSuccessful)
	{
		Error.ErrorType = ECarespaceErrorType::NetworkError;
		Error.ErrorMessage = TEXT("Network request failed");
		return Error;
	}

	if (!Response.IsValid())
	{
		Error.ErrorType = ECarespaceErrorType::UnknownError;
		Error.ErrorMessage = TEXT("Invalid response");
		return Error;
	}

	int32 ResponseCode = Response->GetResponseCode();
	Error.StatusCode = ResponseCode;

	// Try to parse error message from response
	FString ResponseContent = Response->GetContentAsString();
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseContent);

	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		if (JsonObject->HasField(TEXT("message")))
		{
			Error.ErrorMessage = JsonObject->GetStringField(TEXT("message"));
		}
		else if (JsonObject->HasField(TEXT("error")))
		{
			Error.ErrorMessage = JsonObject->GetStringField(TEXT("error"));
		}
	}

	// Set error type based on status code
	if (ResponseCode == 401)
	{
		Error.ErrorType = ECarespaceErrorType::AuthenticationError;
		if (Error.ErrorMessage.IsEmpty())
		{
			Error.ErrorMessage = TEXT("Authentication failed. Please check your API key.");
		}
	}
	else if (ResponseCode >= 400 && ResponseCode < 500)
	{
		Error.ErrorType = ECarespaceErrorType::ValidationError;
		if (Error.ErrorMessage.IsEmpty())
		{
			Error.ErrorMessage = FString::Printf(TEXT("Client error: %d"), ResponseCode);
		}
	}
	else if (ResponseCode >= 500)
	{
		Error.ErrorType = ECarespaceErrorType::ServerError;
		if (Error.ErrorMessage.IsEmpty())
		{
			Error.ErrorMessage = FString::Printf(TEXT("Server error: %d"), ResponseCode);
		}
	}
	else
	{
		Error.ErrorType = ECarespaceErrorType::UnknownError;
		if (Error.ErrorMessage.IsEmpty())
		{
			Error.ErrorMessage = FString::Printf(TEXT("Unknown error: %d"), ResponseCode);
		}
	}

	return Error;
}

FString UCarespaceHTTPClient::BuildURL(const FString& Endpoint, const TMap<FString, FString>& QueryParameters)
{
	FString URL = BaseURL + Endpoint;

	if (QueryParameters.Num() > 0)
	{
		TArray<FString> QueryPairs;
		for (const auto& Param : QueryParameters)
		{
			if (!Param.Value.IsEmpty())
			{
				QueryPairs.Add(FString::Printf(TEXT("%s=%s"), 
					*FGenericPlatformHttp::UrlEncode(Param.Key), 
					*FGenericPlatformHttp::UrlEncode(Param.Value)));
			}
		}

		if (QueryPairs.Num() > 0)
		{
			URL += TEXT("?") + FString::Join(QueryPairs, TEXT("&"));
		}
	}

	return URL;
}

FString UCarespaceHTTPClient::StructToJsonString(const UStruct* StructDefinition, const void* Struct)
{
	FString OutputString;
	FJsonObjectConverter::UStructToJsonObjectString(StructDefinition, Struct, OutputString, 0, 0);
	return OutputString;
}

bool UCarespaceHTTPClient::JsonStringToStruct(const FString& JsonString, const UStruct* StructDefinition, void* OutStruct)
{
	return FJsonObjectConverter::JsonObjectStringToUStruct(JsonString, StructDefinition, OutStruct, 0, 0);
}