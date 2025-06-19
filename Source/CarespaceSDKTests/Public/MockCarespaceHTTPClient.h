#pragma once

#include "CoreMinimal.h"
#include "CarespaceHTTPClient.h"
#include "CarespaceTypes.h"
#include "MockCarespaceHTTPClient.generated.h"

/**
 * Mock HTTP client for unit testing Carespace SDK functionality.
 * This class simulates HTTP responses without making actual network calls,
 * allowing for deterministic and fast unit tests.
 */
UCLASS(BlueprintType)
class CARESPACESDKTESTS_API UMockCarespaceHTTPClient : public UCarespaceHTTPClient
{
	GENERATED_BODY()

public:
	UMockCarespaceHTTPClient();

	// Mock configuration
	struct FMockResponse
	{
		bool bShouldSucceed;
		FString ResponseContent;
		FCarespaceError Error;
		float DelaySeconds;

		FMockResponse()
			: bShouldSucceed(true)
			, ResponseContent(TEXT("{}"))
			, DelaySeconds(0.0f)
		{
		}

		FMockResponse(bool bSuccess, const FString& Content, float Delay = 0.0f)
			: bShouldSucceed(bSuccess)
			, ResponseContent(Content)
			, DelaySeconds(Delay)
		{
		}

		FMockResponse(const FCarespaceError& InError, float Delay = 0.0f)
			: bShouldSucceed(false)
			, ResponseContent(TEXT(""))
			, Error(InError)
			, DelaySeconds(Delay)
		{
		}
	};

	// Override HTTP methods to return mock responses
	virtual void Get(const FString& Endpoint, const FOnCarespaceHTTPResponse& OnComplete) override;
	virtual void Post(const FString& Endpoint, const FString& Data, const FOnCarespaceHTTPResponse& OnComplete) override;
	virtual void Put(const FString& Endpoint, const FString& Data, const FOnCarespaceHTTPResponse& OnComplete) override;
	virtual void Delete(const FString& Endpoint, const FOnCarespaceHTTPResponse& OnComplete) override;

	// Mock configuration methods
	/**
	 * Sets up a mock response for a specific endpoint and HTTP method.
	 * 
	 * @param Method HTTP method ("GET", "POST", "PUT", "DELETE")
	 * @param Endpoint The endpoint URL pattern to match
	 * @param Response The mock response to return
	 */
	UFUNCTION(BlueprintCallable, Category = "Mock")
	void SetMockResponse(const FString& Method, const FString& Endpoint, const FMockResponse& Response);

	/**
	 * Sets up a mock response for the next request regardless of endpoint.
	 * 
	 * @param Response The mock response to return
	 */
	UFUNCTION(BlueprintCallable, Category = "Mock")
	void SetNextResponse(const FMockResponse& Response);

	/**
	 * Clears all configured mock responses.
	 */
	UFUNCTION(BlueprintCallable, Category = "Mock")
	void ClearMockResponses();

	/**
	 * Enables or disables network delay simulation.
	 * 
	 * @param bEnabled Whether to simulate network delays
	 */
	UFUNCTION(BlueprintCallable, Category = "Mock")
	void SetNetworkDelayEnabled(bool bEnabled);

	/**
	 * Returns the number of requests made to a specific endpoint.
	 * 
	 * @param Method HTTP method to check
	 * @param Endpoint Endpoint to check
	 * @return Number of requests made
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mock")
	int32 GetRequestCount(const FString& Method, const FString& Endpoint) const;

	/**
	 * Returns the total number of requests made.
	 * 
	 * @return Total request count
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mock")
	int32 GetTotalRequestCount() const;

	/**
	 * Gets the last request data sent to a specific endpoint.
	 * 
	 * @param Method HTTP method
	 * @param Endpoint Endpoint to check
	 * @return Last request data or empty string if none
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Mock")
	FString GetLastRequestData(const FString& Method, const FString& Endpoint) const;

	/**
	 * Resets all request tracking data.
	 */
	UFUNCTION(BlueprintCallable, Category = "Mock")
	void ResetRequestTracking();

	// Helper methods for common test scenarios
	/**
	 * Configures mock to simulate authentication success.
	 */
	UFUNCTION(BlueprintCallable, Category = "Mock|Helpers")
	void MockAuthenticationSuccess();

	/**
	 * Configures mock to simulate authentication failure.
	 */
	UFUNCTION(BlueprintCallable, Category = "Mock|Helpers")
	void MockAuthenticationFailure();

	/**
	 * Configures mock to simulate network timeout.
	 */
	UFUNCTION(BlueprintCallable, Category = "Mock|Helpers")
	void MockNetworkTimeout();

	/**
	 * Configures mock to simulate server error.
	 */
	UFUNCTION(BlueprintCallable, Category = "Mock|Helpers")
	void MockServerError();

private:
	// Mock response storage
	TMap<FString, FMockResponse> MockResponses;
	TQueue<FMockResponse> NextResponses;

	// Request tracking
	struct FRequestInfo
	{
		FString Method;
		FString Endpoint;
		FString Data;
		FDateTime Timestamp;
	};

	TArray<FRequestInfo> RequestHistory;
	TMap<FString, int32> RequestCounts;

	// Configuration
	bool bNetworkDelayEnabled;

	// Helper methods
	FString GetRequestKey(const FString& Method, const FString& Endpoint) const;
	FMockResponse GetMockResponseForRequest(const FString& Method, const FString& Endpoint) const;
	void TrackRequest(const FString& Method, const FString& Endpoint, const FString& Data);
	void ExecuteMockResponse(const FMockResponse& Response, const FOnCarespaceHTTPResponse& OnComplete);

	// Timer handle for delayed responses
	FTimerHandle DelayTimerHandle;
};