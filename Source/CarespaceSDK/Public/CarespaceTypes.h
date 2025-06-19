#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "CarespaceTypes.generated.h"

// Forward declarations
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnCarespaceRequestComplete, bool, bWasSuccessful);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnCarespaceLoginComplete, bool, bWasSuccessful, const FString&, AccessToken);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnCarespaceUsersReceived, bool, bWasSuccessful, const TArray<FCarespaceUser>&, Users);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnCarespaceClientsReceived, bool, bWasSuccessful, const TArray<FCarespaceClient>&, Clients);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnCarespaceProgramsReceived, bool, bWasSuccessful, const TArray<FCarespaceProgram>&, Programs);

UENUM(BlueprintType)
enum class ECarespaceErrorType : uint8
{
	None UMETA(DisplayName = "No Error"),
	NetworkError UMETA(DisplayName = "Network Error"),
	AuthenticationError UMETA(DisplayName = "Authentication Error"),
	ValidationError UMETA(DisplayName = "Validation Error"),
	ServerError UMETA(DisplayName = "Server Error"),
	UnknownError UMETA(DisplayName = "Unknown Error")
};

USTRUCT(BlueprintType)
struct CARESPACESDK_API FCarespaceError
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Carespace")
	ECarespaceErrorType ErrorType = ECarespaceErrorType::None;

	UPROPERTY(BlueprintReadOnly, Category = "Carespace")
	FString ErrorMessage;

	UPROPERTY(BlueprintReadOnly, Category = "Carespace")
	int32 StatusCode = 0;

	FCarespaceError()
	{
		ErrorType = ECarespaceErrorType::None;
		ErrorMessage = TEXT("");
		StatusCode = 0;
	}

	FCarespaceError(ECarespaceErrorType InErrorType, const FString& InErrorMessage, int32 InStatusCode = 0)
	{
		ErrorType = InErrorType;
		ErrorMessage = InErrorMessage;
		StatusCode = InStatusCode;
	}
};

USTRUCT(BlueprintType)
struct CARESPACESDK_API FCarespaceUser
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Carespace")
	FString Id;

	UPROPERTY(BlueprintReadOnly, Category = "Carespace")
	FString Email;

	UPROPERTY(BlueprintReadOnly, Category = "Carespace")
	FString Name;

	UPROPERTY(BlueprintReadOnly, Category = "Carespace")
	FString FirstName;

	UPROPERTY(BlueprintReadOnly, Category = "Carespace")
	FString LastName;

	UPROPERTY(BlueprintReadOnly, Category = "Carespace")
	FString Role;

	UPROPERTY(BlueprintReadOnly, Category = "Carespace")
	bool bIsActive = true;

	UPROPERTY(BlueprintReadOnly, Category = "Carespace")
	FDateTime CreatedAt;

	UPROPERTY(BlueprintReadOnly, Category = "Carespace")
	FDateTime UpdatedAt;

	FCarespaceUser()
	{
		Id = TEXT("");
		Email = TEXT("");
		Name = TEXT("");
		FirstName = TEXT("");
		LastName = TEXT("");
		Role = TEXT("");
		bIsActive = true;
		CreatedAt = FDateTime::MinValue();
		UpdatedAt = FDateTime::MinValue();
	}
};

USTRUCT(BlueprintType)
struct CARESPACESDK_API FCarespaceAddress
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Carespace")
	FString Street;

	UPROPERTY(BlueprintReadWrite, Category = "Carespace")
	FString City;

	UPROPERTY(BlueprintReadWrite, Category = "Carespace")
	FString State;

	UPROPERTY(BlueprintReadWrite, Category = "Carespace")
	FString ZipCode;

	UPROPERTY(BlueprintReadWrite, Category = "Carespace")
	FString Country;

	FCarespaceAddress()
	{
		Street = TEXT("");
		City = TEXT("");
		State = TEXT("");
		ZipCode = TEXT("");
		Country = TEXT("");
	}
};

USTRUCT(BlueprintType)
struct CARESPACESDK_API FCarespaceClient
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Carespace")
	FString Id;

	UPROPERTY(BlueprintReadWrite, Category = "Carespace")
	FString Name;

	UPROPERTY(BlueprintReadWrite, Category = "Carespace")
	FString Email;

	UPROPERTY(BlueprintReadWrite, Category = "Carespace")
	FString Phone;

	UPROPERTY(BlueprintReadWrite, Category = "Carespace")
	FDateTime DateOfBirth;

	UPROPERTY(BlueprintReadWrite, Category = "Carespace")
	FString Gender;

	UPROPERTY(BlueprintReadWrite, Category = "Carespace")
	FCarespaceAddress Address;

	UPROPERTY(BlueprintReadWrite, Category = "Carespace")
	FString MedicalHistory;

	UPROPERTY(BlueprintReadWrite, Category = "Carespace")
	FString Notes;

	UPROPERTY(BlueprintReadWrite, Category = "Carespace")
	bool bIsActive = true;

	UPROPERTY(BlueprintReadOnly, Category = "Carespace")
	FDateTime CreatedAt;

	UPROPERTY(BlueprintReadOnly, Category = "Carespace")
	FDateTime UpdatedAt;

	FCarespaceClient()
	{
		Id = TEXT("");
		Name = TEXT("");
		Email = TEXT("");
		Phone = TEXT("");
		DateOfBirth = FDateTime::MinValue();
		Gender = TEXT("");
		MedicalHistory = TEXT("");
		Notes = TEXT("");
		bIsActive = true;
		CreatedAt = FDateTime::MinValue();
		UpdatedAt = FDateTime::MinValue();
	}
};

USTRUCT(BlueprintType)
struct CARESPACESDK_API FCarespaceExercise
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Carespace")
	FString Id;

	UPROPERTY(BlueprintReadWrite, Category = "Carespace")
	FString Name;

	UPROPERTY(BlueprintReadWrite, Category = "Carespace")
	FString Description;

	UPROPERTY(BlueprintReadWrite, Category = "Carespace")
	FString Instructions;

	UPROPERTY(BlueprintReadWrite, Category = "Carespace")
	FString VideoURL;

	UPROPERTY(BlueprintReadWrite, Category = "Carespace")
	FString ImageURL;

	UPROPERTY(BlueprintReadWrite, Category = "Carespace")
	int32 Duration = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Carespace")
	int32 Repetitions = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Carespace")
	int32 Sets = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Carespace")
	int32 RestTime = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Carespace")
	int32 Order = 0;

	FCarespaceExercise()
	{
		Id = TEXT("");
		Name = TEXT("");
		Description = TEXT("");
		Instructions = TEXT("");
		VideoURL = TEXT("");
		ImageURL = TEXT("");
		Duration = 0;
		Repetitions = 0;
		Sets = 0;
		RestTime = 0;
		Order = 0;
	}
};

USTRUCT(BlueprintType)
struct CARESPACESDK_API FCarespaceProgram
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Carespace")
	FString Id;

	UPROPERTY(BlueprintReadWrite, Category = "Carespace")
	FString Name;

	UPROPERTY(BlueprintReadWrite, Category = "Carespace")
	FString Description;

	UPROPERTY(BlueprintReadWrite, Category = "Carespace")
	FString Category;

	UPROPERTY(BlueprintReadWrite, Category = "Carespace")
	FString Difficulty;

	UPROPERTY(BlueprintReadWrite, Category = "Carespace")
	int32 Duration = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Carespace")
	bool bIsTemplate = false;

	UPROPERTY(BlueprintReadWrite, Category = "Carespace")
	bool bIsActive = true;

	UPROPERTY(BlueprintReadOnly, Category = "Carespace")
	FString CreatedBy;

	UPROPERTY(BlueprintReadOnly, Category = "Carespace")
	FDateTime CreatedAt;

	UPROPERTY(BlueprintReadOnly, Category = "Carespace")
	FDateTime UpdatedAt;

	UPROPERTY(BlueprintReadOnly, Category = "Carespace")
	TArray<FCarespaceExercise> Exercises;

	FCarespaceProgram()
	{
		Id = TEXT("");
		Name = TEXT("");
		Description = TEXT("");
		Category = TEXT("");
		Difficulty = TEXT("");
		Duration = 0;
		bIsTemplate = false;
		bIsActive = true;
		CreatedBy = TEXT("");
		CreatedAt = FDateTime::MinValue();
		UpdatedAt = FDateTime::MinValue();
	}
};

USTRUCT(BlueprintType)
struct CARESPACESDK_API FCarespaceLoginRequest
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Carespace")
	FString Email;

	UPROPERTY(BlueprintReadWrite, Category = "Carespace")
	FString Password;

	FCarespaceLoginRequest()
	{
		Email = TEXT("");
		Password = TEXT("");
	}

	FCarespaceLoginRequest(const FString& InEmail, const FString& InPassword)
	{
		Email = InEmail;
		Password = InPassword;
	}
};

USTRUCT(BlueprintType)
struct CARESPACESDK_API FCarespaceCreateUserRequest
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Carespace")
	FString Email;

	UPROPERTY(BlueprintReadWrite, Category = "Carespace")
	FString Name;

	UPROPERTY(BlueprintReadWrite, Category = "Carespace")
	FString FirstName;

	UPROPERTY(BlueprintReadWrite, Category = "Carespace")
	FString LastName;

	UPROPERTY(BlueprintReadWrite, Category = "Carespace")
	FString Role;

	UPROPERTY(BlueprintReadWrite, Category = "Carespace")
	FString Password;

	FCarespaceCreateUserRequest()
	{
		Email = TEXT("");
		Name = TEXT("");
		FirstName = TEXT("");
		LastName = TEXT("");
		Role = TEXT("");
		Password = TEXT("");
	}
};