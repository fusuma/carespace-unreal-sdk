#include "CarespaceBlueprintLibrary.h"
#include "Engine/Engine.h"

UCarespaceAPI* UCarespaceBlueprintLibrary::CreateCarespaceAPI(const FString& BaseURL, const FString& APIKey)
{
	return UCarespaceAPI::CreateCarespaceAPI(BaseURL, APIKey);
}

void UCarespaceBlueprintLibrary::LoginToCarespace(UCarespaceAPI* CarespaceAPI, const FString& Email, const FString& Password, const FOnCarespaceLoginComplete& OnComplete)
{
	if (!CarespaceAPI)
	{
		UE_LOG(LogTemp, Error, TEXT("CarespaceBlueprintLibrary: CarespaceAPI is null"));
		OnComplete.ExecuteIfBound(false, TEXT(""));
		return;
	}

	FCarespaceLoginRequest LoginRequest(Email, Password);
	CarespaceAPI->GetAuthAPI()->Login(LoginRequest, OnComplete);
}

FCarespaceCreateUserRequest UCarespaceBlueprintLibrary::MakeCreateUserRequest(const FString& Email, const FString& Name, const FString& FirstName, const FString& LastName, const FString& Role, const FString& Password)
{
	FCarespaceCreateUserRequest Request;
	Request.Email = Email;
	Request.Name = Name;
	Request.FirstName = FirstName;
	Request.LastName = LastName;
	Request.Role = Role;
	Request.Password = Password;
	return Request;
}

FCarespaceClient UCarespaceBlueprintLibrary::MakeClientData(const FString& Name, const FString& Email, const FString& Phone, const FString& Gender, const FCarespaceAddress& Address, const FString& MedicalHistory, const FString& Notes)
{
	FCarespaceClient Client;
	Client.Name = Name;
	Client.Email = Email;
	Client.Phone = Phone;
	Client.Gender = Gender;
	Client.Address = Address;
	Client.MedicalHistory = MedicalHistory;
	Client.Notes = Notes;
	return Client;
}

FCarespaceAddress UCarespaceBlueprintLibrary::MakeAddress(const FString& Street, const FString& City, const FString& State, const FString& ZipCode, const FString& Country)
{
	FCarespaceAddress Address;
	Address.Street = Street;
	Address.City = City;
	Address.State = State;
	Address.ZipCode = ZipCode;
	Address.Country = Country;
	return Address;
}

FCarespaceProgram UCarespaceBlueprintLibrary::MakeProgramData(const FString& Name, const FString& Description, const FString& Category, const FString& Difficulty, int32 Duration, bool bIsTemplate)
{
	FCarespaceProgram Program;
	Program.Name = Name;
	Program.Description = Description;
	Program.Category = Category;
	Program.Difficulty = Difficulty;
	Program.Duration = Duration;
	Program.bIsTemplate = bIsTemplate;
	return Program;
}

FCarespaceExercise UCarespaceBlueprintLibrary::MakeExerciseData(const FString& Name, const FString& Description, const FString& Instructions, const FString& VideoURL, int32 Duration, int32 Repetitions, int32 Sets, int32 RestTime)
{
	FCarespaceExercise Exercise;
	Exercise.Name = Name;
	Exercise.Description = Description;
	Exercise.Instructions = Instructions;
	Exercise.VideoURL = VideoURL;
	Exercise.Duration = Duration;
	Exercise.Repetitions = Repetitions;
	Exercise.Sets = Sets;
	Exercise.RestTime = RestTime;
	return Exercise;
}

bool UCarespaceBlueprintLibrary::IsValidEmail(const FString& Email)
{
	// Simple email validation
	return Email.Contains(TEXT("@")) && Email.Contains(TEXT(".")) && Email.Len() > 5;
}

bool UCarespaceBlueprintLibrary::IsValidPhone(const FString& Phone)
{
	// Simple phone validation - contains only numbers, spaces, +, -, (, )
	for (int32 i = 0; i < Phone.Len(); ++i)
	{
		TCHAR Char = Phone[i];
		if (!FChar::IsDigit(Char) && Char != TEXT(' ') && Char != TEXT('+') && 
			Char != TEXT('-') && Char != TEXT('(') && Char != TEXT(')'))
		{
			return false;
		}
	}
	return Phone.Len() >= 10;
}

FString UCarespaceBlueprintLibrary::FormatFullName(const FString& FirstName, const FString& LastName)
{
	if (FirstName.IsEmpty() && LastName.IsEmpty())
	{
		return TEXT("");
	}
	else if (FirstName.IsEmpty())
	{
		return LastName;
	}
	else if (LastName.IsEmpty())
	{
		return FirstName;
	}
	else
	{
		return FString::Printf(TEXT("%s %s"), *FirstName, *LastName);
	}
}

FString UCarespaceBlueprintLibrary::FormatDuration(int32 DurationInSeconds)
{
	int32 Minutes = DurationInSeconds / 60;
	int32 Seconds = DurationInSeconds % 60;
	
	if (Minutes > 0)
	{
		if (Seconds > 0)
		{
			return FString::Printf(TEXT("%dm %ds"), Minutes, Seconds);
		}
		else
		{
			return FString::Printf(TEXT("%dm"), Minutes);
		}
	}
	else
	{
		return FString::Printf(TEXT("%ds"), Seconds);
	}
}

FString UCarespaceBlueprintLibrary::FormatDate(const FDateTime& DateTime)
{
	if (DateTime == FDateTime::MinValue())
	{
		return TEXT("N/A");
	}
	return DateTime.ToString(TEXT("%Y-%m-%d"));
}

FString UCarespaceBlueprintLibrary::GetErrorDescription(const FCarespaceError& Error)
{
	return Error.ErrorMessage;
}

bool UCarespaceBlueprintLibrary::IsAuthenticationError(const FCarespaceError& Error)
{
	return Error.ErrorType == ECarespaceErrorType::AuthenticationError;
}

bool UCarespaceBlueprintLibrary::IsNetworkError(const FCarespaceError& Error)
{
	return Error.ErrorType == ECarespaceErrorType::NetworkError;
}

int32 UCarespaceBlueprintLibrary::GetUserCount(const TArray<FCarespaceUser>& Users)
{
	return Users.Num();
}

int32 UCarespaceBlueprintLibrary::GetClientCount(const TArray<FCarespaceClient>& Clients)
{
	return Clients.Num();
}

int32 UCarespaceBlueprintLibrary::GetProgramCount(const TArray<FCarespaceProgram>& Programs)
{
	return Programs.Num();
}

FCarespaceUser UCarespaceBlueprintLibrary::FindUserByID(const TArray<FCarespaceUser>& Users, const FString& UserID, bool& bFound)
{
	bFound = false;
	for (const FCarespaceUser& User : Users)
	{
		if (User.Id == UserID)
		{
			bFound = true;
			return User;
		}
	}
	return FCarespaceUser();
}

FCarespaceClient UCarespaceBlueprintLibrary::FindClientByID(const TArray<FCarespaceClient>& Clients, const FString& ClientID, bool& bFound)
{
	bFound = false;
	for (const FCarespaceClient& Client : Clients)
	{
		if (Client.Id == ClientID)
		{
			bFound = true;
			return Client;
		}
	}
	return FCarespaceClient();
}

FCarespaceProgram UCarespaceBlueprintLibrary::FindProgramByID(const TArray<FCarespaceProgram>& Programs, const FString& ProgramID, bool& bFound)
{
	bFound = false;
	for (const FCarespaceProgram& Program : Programs)
	{
		if (Program.Id == ProgramID)
		{
			bFound = true;
			return Program;
		}
	}
	return FCarespaceProgram();
}