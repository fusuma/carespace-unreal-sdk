#include "MockCarespaceHTTPClient.h"
#include "Engine/World.h"
#include "TimerManager.h"

UMockCarespaceHTTPClient::UMockCarespaceHTTPClient()
	: bNetworkDelayEnabled(false)
{
}

void UMockCarespaceHTTPClient::Get(const FString& Endpoint, const FOnCarespaceHTTPResponse& OnComplete)
{
	TrackRequest(TEXT("GET"), Endpoint, TEXT(""));
	FMockResponse Response = GetMockResponseForRequest(TEXT("GET"), Endpoint);
	ExecuteMockResponse(Response, OnComplete);
}

void UMockCarespaceHTTPClient::Post(const FString& Endpoint, const FString& Data, const FOnCarespaceHTTPResponse& OnComplete)
{
	TrackRequest(TEXT("POST"), Endpoint, Data);
	FMockResponse Response = GetMockResponseForRequest(TEXT("POST"), Endpoint);
	ExecuteMockResponse(Response, OnComplete);
}

void UMockCarespaceHTTPClient::Put(const FString& Endpoint, const FString& Data, const FOnCarespaceHTTPResponse& OnComplete)
{
	TrackRequest(TEXT("PUT"), Endpoint, Data);
	FMockResponse Response = GetMockResponseForRequest(TEXT("PUT"), Endpoint);
	ExecuteMockResponse(Response, OnComplete);
}

void UMockCarespaceHTTPClient::Delete(const FString& Endpoint, const FOnCarespaceHTTPResponse& OnComplete)
{
	TrackRequest(TEXT("DELETE"), Endpoint, TEXT(""));
	FMockResponse Response = GetMockResponseForRequest(TEXT("DELETE"), Endpoint);
	ExecuteMockResponse(Response, OnComplete);
}

void UMockCarespaceHTTPClient::SetMockResponse(const FString& Method, const FString& Endpoint, const FMockResponse& Response)
{
	FString Key = GetRequestKey(Method, Endpoint);
	MockResponses.Add(Key, Response);
}

void UMockCarespaceHTTPClient::SetNextResponse(const FMockResponse& Response)
{
	NextResponses.Enqueue(Response);
}

void UMockCarespaceHTTPClient::ClearMockResponses()
{
	MockResponses.Empty();
	while (!NextResponses.IsEmpty())
	{
		FMockResponse DummyResponse;
		NextResponses.Dequeue(DummyResponse);
	}
}

void UMockCarespaceHTTPClient::SetNetworkDelayEnabled(bool bEnabled)
{
	bNetworkDelayEnabled = bEnabled;
}

int32 UMockCarespaceHTTPClient::GetRequestCount(const FString& Method, const FString& Endpoint) const
{
	FString Key = GetRequestKey(Method, Endpoint);
	const int32* Count = RequestCounts.Find(Key);
	return Count ? *Count : 0;
}

int32 UMockCarespaceHTTPClient::GetTotalRequestCount() const
{
	return RequestHistory.Num();
}

FString UMockCarespaceHTTPClient::GetLastRequestData(const FString& Method, const FString& Endpoint) const
{
	for (int32 i = RequestHistory.Num() - 1; i >= 0; --i)
	{
		const FRequestInfo& Request = RequestHistory[i];
		if (Request.Method == Method && Request.Endpoint == Endpoint)
		{
			return Request.Data;
		}
	}
	return TEXT("");
}

void UMockCarespaceHTTPClient::ResetRequestTracking()
{
	RequestHistory.Empty();
	RequestCounts.Empty();
}

void UMockCarespaceHTTPClient::MockAuthenticationSuccess()
{
	FMockResponse LoginResponse;
	LoginResponse.bShouldSucceed = true;
	LoginResponse.ResponseContent = TEXT(R"({
		"success": true,
		"data": {
			"accessToken": "mock_access_token_12345",
			"refreshToken": "mock_refresh_token_67890",
			"user": {
				"id": "user_123",
				"email": "test@example.com",
				"firstName": "Test",
				"lastName": "User",
				"role": "clinician"
			}
		}
	})");

	SetMockResponse(TEXT("POST"), TEXT("/api/auth/login"), LoginResponse);

	FMockResponse RefreshResponse;
	RefreshResponse.bShouldSucceed = true;
	RefreshResponse.ResponseContent = TEXT(R"({
		"success": true,
		"data": {
			"accessToken": "mock_new_access_token_12345",
			"refreshToken": "mock_new_refresh_token_67890"
		}
	})");

	SetMockResponse(TEXT("POST"), TEXT("/api/auth/refresh"), RefreshResponse);
}

void UMockCarespaceHTTPClient::MockAuthenticationFailure()
{
	FCarespaceError AuthError;
	AuthError.Type = ECarespaceErrorType::AuthenticationError;
	AuthError.Message = TEXT("Invalid credentials");
	AuthError.Code = TEXT("INVALID_CREDENTIALS");

	FMockResponse LoginResponse(AuthError);
	SetMockResponse(TEXT("POST"), TEXT("/api/auth/login"), LoginResponse);
}

void UMockCarespaceHTTPClient::MockNetworkTimeout()
{
	FCarespaceError TimeoutError;
	TimeoutError.Type = ECarespaceErrorType::NetworkError;
	TimeoutError.Message = TEXT("Request timed out");
	TimeoutError.Code = TEXT("TIMEOUT");

	FMockResponse TimeoutResponse(TimeoutError, 5.0f);
	SetNextResponse(TimeoutResponse);
}

void UMockCarespaceHTTPClient::MockServerError()
{
	FCarespaceError ServerError;
	ServerError.Type = ECarespaceErrorType::ServerError;
	ServerError.Message = TEXT("Internal server error");
	ServerError.Code = TEXT("INTERNAL_ERROR");

	FMockResponse ErrorResponse(ServerError);
	SetNextResponse(ErrorResponse);
}

FString UMockCarespaceHTTPClient::GetRequestKey(const FString& Method, const FString& Endpoint) const
{
	return FString::Printf(TEXT("%s:%s"), *Method, *Endpoint);
}

UMockCarespaceHTTPClient::FMockResponse UMockCarespaceHTTPClient::GetMockResponseForRequest(const FString& Method, const FString& Endpoint) const
{
	// Check if there's a queued next response
	if (!NextResponses.IsEmpty())
	{
		FMockResponse NextResponse;
		const_cast<UMockCarespaceHTTPClient*>(this)->NextResponses.Dequeue(NextResponse);
		return NextResponse;
	}

	// Check for specific endpoint response
	FString Key = GetRequestKey(Method, Endpoint);
	const FMockResponse* FoundResponse = MockResponses.Find(Key);
	if (FoundResponse)
	{
		return *FoundResponse;
	}

	// Return default success response
	FMockResponse DefaultResponse;
	DefaultResponse.bShouldSucceed = true;
	DefaultResponse.ResponseContent = TEXT(R"({"success": true, "data": {}})");
	return DefaultResponse;
}

void UMockCarespaceHTTPClient::TrackRequest(const FString& Method, const FString& Endpoint, const FString& Data)
{
	FRequestInfo RequestInfo;
	RequestInfo.Method = Method;
	RequestInfo.Endpoint = Endpoint;
	RequestInfo.Data = Data;
	RequestInfo.Timestamp = FDateTime::Now();

	RequestHistory.Add(RequestInfo);

	FString Key = GetRequestKey(Method, Endpoint);
	int32& Count = RequestCounts.FindOrAdd(Key, 0);
	Count++;
}

void UMockCarespaceHTTPClient::ExecuteMockResponse(const FMockResponse& Response, const FOnCarespaceHTTPResponse& OnComplete)
{
	if (!OnComplete.IsBound())
	{
		return;
	}

	float Delay = bNetworkDelayEnabled ? Response.DelaySeconds : 0.0f;

	if (Delay > 0.0f)
	{
		// Execute response with delay
		UWorld* World = GetWorld();
		if (World)
		{
			FTimerDelegate TimerDelegate;
			TimerDelegate.BindLambda([this, Response, OnComplete]()
			{
				OnComplete.ExecuteIfBound(Response.bShouldSucceed, Response.ResponseContent, Response.Error);
			});

			World->GetTimerManager().SetTimer(DelayTimerHandle, TimerDelegate, Delay, false);
		}
		else
		{
			// Fallback: execute immediately if no world context
			OnComplete.ExecuteIfBound(Response.bShouldSucceed, Response.ResponseContent, Response.Error);
		}
	}
	else
	{
		// Execute immediately
		OnComplete.ExecuteIfBound(Response.bShouldSucceed, Response.ResponseContent, Response.Error);
	}
}