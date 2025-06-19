#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CarespaceTypes.h"
#include "CarespaceAPI.h"
#include "CarespaceBlueprintLibrary.generated.h"

/**
 * Blueprint function library for easy access to Carespace API functionality
 */
UCLASS()
class CARESPACESDK_API UCarespaceBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// Factory methods
	UFUNCTION(BlueprintCallable, Category = "Carespace", CallInEditor = true, meta = (DisplayName = "Create Carespace API"))
	static UCarespaceAPI* CreateCarespaceAPI(const FString& BaseURL = TEXT("https://api-dev.carespace.ai"), const FString& APIKey = TEXT(""));

	// Quick authentication
	UFUNCTION(BlueprintCallable, Category = "Carespace|Auth", meta = (DisplayName = "Login to Carespace"))
	static void LoginToCarespace(UCarespaceAPI* CarespaceAPI, const FString& Email, const FString& Password, const FOnCarespaceLoginComplete& OnComplete);

	// User utilities
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Carespace|Users", meta = (DisplayName = "Make Create User Request"))
	static FCarespaceCreateUserRequest MakeCreateUserRequest(const FString& Email, const FString& Name = TEXT(""), const FString& FirstName = TEXT(""), const FString& LastName = TEXT(""), const FString& Role = TEXT("client"), const FString& Password = TEXT(""));

	// Client utilities
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Carespace|Clients", meta = (DisplayName = "Make Client Data"))
	static FCarespaceClient MakeClientData(const FString& Name, const FString& Email = TEXT(""), const FString& Phone = TEXT(""), const FString& Gender = TEXT(""), const FCarespaceAddress& Address = FCarespaceAddress(), const FString& MedicalHistory = TEXT(""), const FString& Notes = TEXT(""));

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Carespace|Clients", meta = (DisplayName = "Make Address"))
	static FCarespaceAddress MakeAddress(const FString& Street = TEXT(""), const FString& City = TEXT(""), const FString& State = TEXT(""), const FString& ZipCode = TEXT(""), const FString& Country = TEXT(""));

	// Program utilities
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Carespace|Programs", meta = (DisplayName = "Make Program Data"))
	static FCarespaceProgram MakeProgramData(const FString& Name, const FString& Description = TEXT(""), const FString& Category = TEXT(""), const FString& Difficulty = TEXT("beginner"), int32 Duration = 30, bool bIsTemplate = false);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Carespace|Programs", meta = (DisplayName = "Make Exercise Data"))
	static FCarespaceExercise MakeExerciseData(const FString& Name, const FString& Description = TEXT(""), const FString& Instructions = TEXT(""), const FString& VideoURL = TEXT(""), int32 Duration = 30, int32 Repetitions = 10, int32 Sets = 3, int32 RestTime = 60);

	// Validation utilities
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Carespace|Validation", meta = (DisplayName = "Is Valid Email"))
	static bool IsValidEmail(const FString& Email);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Carespace|Validation", meta = (DisplayName = "Is Valid Phone"))
	static bool IsValidPhone(const FString& Phone);

	// Formatting utilities
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Carespace|Format", meta = (DisplayName = "Format Full Name"))
	static FString FormatFullName(const FString& FirstName, const FString& LastName);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Carespace|Format", meta = (DisplayName = "Format Duration"))
	static FString FormatDuration(int32 DurationInSeconds);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Carespace|Format", meta = (DisplayName = "Format Date"))
	static FString FormatDate(const FDateTime& DateTime);

	// Error handling utilities
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Carespace|Error", meta = (DisplayName = "Get Error Description"))
	static FString GetErrorDescription(const FCarespaceError& Error);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Carespace|Error", meta = (DisplayName = "Is Authentication Error"))
	static bool IsAuthenticationError(const FCarespaceError& Error);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Carespace|Error", meta = (DisplayName = "Is Network Error"))
	static bool IsNetworkError(const FCarespaceError& Error);

	// Array utilities
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Carespace|Array", meta = (DisplayName = "Get User Count"))
	static int32 GetUserCount(const TArray<FCarespaceUser>& Users);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Carespace|Array", meta = (DisplayName = "Get Client Count"))
	static int32 GetClientCount(const TArray<FCarespaceClient>& Clients);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Carespace|Array", meta = (DisplayName = "Get Program Count"))
	static int32 GetProgramCount(const TArray<FCarespaceProgram>& Programs);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Carespace|Array", meta = (DisplayName = "Find User By ID"))
	static FCarespaceUser FindUserByID(const TArray<FCarespaceUser>& Users, const FString& UserID, bool& bFound);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Carespace|Array", meta = (DisplayName = "Find Client By ID"))
	static FCarespaceClient FindClientByID(const TArray<FCarespaceClient>& Clients, const FString& ClientID, bool& bFound);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Carespace|Array", meta = (DisplayName = "Find Program By ID"))
	static FCarespaceProgram FindProgramByID(const TArray<FCarespaceProgram>& Programs, const FString& ProgramID, bool& bFound);
};