#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Http.h"
#include "Json.h"
#include "CarespaceTypes.h"
#include "CarespaceHTTPClient.generated.h"

DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnHTTPResponse, bool, bWasSuccessful, const FString&, ResponseContent, const FCarespaceError&, Error);

UCLASS(BlueprintType)
class CARESPACESDK_API UCarespaceHTTPClient : public UObject
{
	GENERATED_BODY()

public:
	UCarespaceHTTPClient();

	// Configuration
	UFUNCTION(BlueprintCallable, Category = "Carespace")
	void SetBaseURL(const FString& InBaseURL);

	UFUNCTION(BlueprintCallable, Category = "Carespace")
	void SetAPIKey(const FString& InAPIKey);

	UFUNCTION(BlueprintCallable, Category = "Carespace")
	void SetTimeout(float InTimeoutSeconds);

	// HTTP Methods
	UFUNCTION(BlueprintCallable, Category = "Carespace")
	void SendGETRequest(const FString& Endpoint, const TMap<FString, FString>& QueryParameters, const FOnHTTPResponse& OnComplete);

	UFUNCTION(BlueprintCallable, Category = "Carespace")
	void SendPOSTRequest(const FString& Endpoint, const FString& JsonPayload, const FOnHTTPResponse& OnComplete);

	UFUNCTION(BlueprintCallable, Category = "Carespace")
	void SendPUTRequest(const FString& Endpoint, const FString& JsonPayload, const FOnHTTPResponse& OnComplete);

	UFUNCTION(BlueprintCallable, Category = "Carespace")
	void SendDELETERequest(const FString& Endpoint, const FOnHTTPResponse& OnComplete);

	// Utility functions
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Carespace")
	static FString StructToJsonString(const UStruct* StructDefinition, const void* Struct);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Carespace")
	static bool JsonStringToStruct(const FString& JsonString, const UStruct* StructDefinition, void* OutStruct);

private:
	FString BaseURL;
	FString APIKey;
	float TimeoutSeconds;

	void ConfigureRequest(TSharedRef<IHttpRequest> Request, const FString& Verb, const FString& URL);
	void HandleResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FOnHTTPResponse OnComplete);
	FCarespaceError ProcessError(FHttpResponsePtr Response, bool bWasSuccessful);
	FString BuildURL(const FString& Endpoint, const TMap<FString, FString>& QueryParameters = TMap<FString, FString>());
};