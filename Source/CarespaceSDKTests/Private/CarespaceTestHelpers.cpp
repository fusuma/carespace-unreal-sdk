#include "CarespaceTestHelpers.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Misc/DateTime.h"
#include "HAL/PlatformFilemanager.h"

int32 UCarespaceTestHelpers::TestIdCounter = 1;

FCarespaceUser UCarespaceTestHelpers::CreateMockUser(const FString& UserId, const FString& Email, const FString& Role)
{
	FCarespaceUser MockUser;
	
	MockUser.Id = UserId.IsEmpty() ? GenerateTestId(TEXT("user")) : UserId;
	MockUser.Email = Email.IsEmpty() ? GenerateTestEmail(TEXT("testuser")) : Email;
	MockUser.FirstName = TEXT("Test");
	MockUser.LastName = TEXT("User");
	MockUser.Role = Role;
	MockUser.bIsActive = true;
	MockUser.CreatedAt = FDateTime::Now();
	MockUser.UpdatedAt = FDateTime::Now();
	MockUser.OrganizationId = GenerateTestId(TEXT("org"));
	
	return MockUser;
}

FCarespaceClient UCarespaceTestHelpers::CreateMockClient(const FString& ClientId, const FString& FirstName, const FString& LastName)
{
	FCarespaceClient MockClient;
	
	MockClient.Id = ClientId.IsEmpty() ? GenerateTestId(TEXT("client")) : ClientId;
	MockClient.FirstName = FirstName.IsEmpty() ? TEXT("Test") : FirstName;
	MockClient.LastName = LastName.IsEmpty() ? TEXT("Client") : LastName;
	MockClient.Email = GenerateTestEmail(TEXT("testclient"));
	MockClient.Phone = TEXT("+1234567890");
	MockClient.Gender = TEXT("other");
	MockClient.MedicalRecordNumber = GenerateTestId(TEXT("MRN"));
	MockClient.bIsActive = true;
	MockClient.CreatedAt = FDateTime::Now();
	MockClient.UpdatedAt = FDateTime::Now();
	MockClient.AssignedClinicianId = GenerateTestId(TEXT("clinician"));
	
	// Add some test conditions and medications
	MockClient.Conditions.Add(TEXT("Test Condition"));
	MockClient.Medications.Add(TEXT("Test Medication"));
	
	// Set up test address
	MockClient.Address.Street = TEXT("123 Test Street");
	MockClient.Address.City = TEXT("Test City");
	MockClient.Address.State = TEXT("TS");
	MockClient.Address.ZipCode = TEXT("12345");
	MockClient.Address.Country = TEXT("Test Country");
	
	return MockClient;
}

FCarespaceProgram UCarespaceTestHelpers::CreateMockProgram(const FString& ProgramId, const FString& Category, int32 DurationWeeks)
{
	FCarespaceProgram MockProgram;
	
	MockProgram.Id = ProgramId.IsEmpty() ? GenerateTestId(TEXT("program")) : ProgramId;
	MockProgram.Name = TEXT("Test Program");
	MockProgram.Description = TEXT("A test program for unit testing");
	MockProgram.Category = Category;
	MockProgram.DurationWeeks = DurationWeeks;
	MockProgram.SessionsPerWeek = 3;
	MockProgram.DifficultyLevel = TEXT("beginner");
	MockProgram.bIsPublished = true;
	MockProgram.CreatedAt = FDateTime::Now();
	MockProgram.UpdatedAt = FDateTime::Now();
	MockProgram.CreatedByUserId = GenerateTestId(TEXT("user"));
	
	// Add test conditions
	MockProgram.TargetConditions.Add(TEXT("Test Condition"));
	
	// Add test exercise
	FCarespaceExercise TestExercise;
	TestExercise.Id = GenerateTestId(TEXT("exercise"));
	TestExercise.Name = TEXT("Test Exercise");
	TestExercise.Description = TEXT("A test exercise");
	TestExercise.DurationSeconds = 60;
	TestExercise.Repetitions = 10;
	TestExercise.Sets = 3;
	MockProgram.Exercises.Add(TestExercise);
	
	return MockProgram;
}

FCarespaceLoginRequest UCarespaceTestHelpers::CreateMockLoginRequest(const FString& Email, const FString& Password)
{
	FCarespaceLoginRequest MockRequest;
	
	MockRequest.Email = Email.IsEmpty() ? GenerateTestEmail(TEXT("testuser")) : Email;
	MockRequest.Password = Password;
	
	return MockRequest;
}

FCarespaceCreateUserRequest UCarespaceTestHelpers::CreateMockCreateUserRequest(const FString& Email, const FString& Role)
{
	FCarespaceCreateUserRequest MockRequest;
	
	MockRequest.Email = Email.IsEmpty() ? GenerateTestEmail(TEXT("newuser")) : Email;
	MockRequest.FirstName = TEXT("New");
	MockRequest.LastName = TEXT("User");
	MockRequest.Role = Role;
	MockRequest.Password = TEXT("TestPassword123!");
	MockRequest.OrganizationId = GenerateTestId(TEXT("org"));
	
	return MockRequest;
}

void UCarespaceTestHelpers::SetupSuccessfulMockResponses(UMockCarespaceHTTPClient* MockClient)
{
	if (!MockClient)
	{
		return;
	}
	
	// Setup successful authentication
	MockClient->MockAuthenticationSuccess();
	
	// Setup successful users response
	TArray<FCarespaceUser> TestUsers;
	TestUsers.Add(CreateMockUser());
	FString UsersResponse = GenerateUsersListResponse(TestUsers);
	
	UMockCarespaceHTTPClient::FMockResponse UsersListResponse;
	UsersListResponse.bShouldSucceed = true;
	UsersListResponse.ResponseContent = UsersResponse;
	MockClient->SetMockResponse(TEXT("GET"), TEXT("/api/users"), UsersListResponse);
	
	// Setup successful clients response
	TArray<FCarespaceClient> TestClients;
	TestClients.Add(CreateMockClient());
	FString ClientsResponse = GenerateClientsListResponse(TestClients);
	
	UMockCarespaceHTTPClient::FMockResponse ClientsListResponse;
	ClientsListResponse.bShouldSucceed = true;
	ClientsListResponse.ResponseContent = ClientsResponse;
	MockClient->SetMockResponse(TEXT("GET"), TEXT("/api/clients"), ClientsListResponse);
	
	// Setup successful programs response
	TArray<FCarespaceProgram> TestPrograms;
	TestPrograms.Add(CreateMockProgram());
	FString ProgramsResponse = GenerateProgramsListResponse(TestPrograms);
	
	UMockCarespaceHTTPClient::FMockResponse ProgramsListResponse;
	ProgramsListResponse.bShouldSucceed = true;
	ProgramsListResponse.ResponseContent = ProgramsResponse;
	MockClient->SetMockResponse(TEXT("GET"), TEXT("/api/programs"), ProgramsListResponse);
}

void UCarespaceTestHelpers::SetupAuthenticationFailureScenario(UMockCarespaceHTTPClient* MockClient)
{
	if (!MockClient)
	{
		return;
	}
	
	MockClient->MockAuthenticationFailure();
}

void UCarespaceTestHelpers::SetupNetworkErrorScenario(UMockCarespaceHTTPClient* MockClient)
{
	if (!MockClient)
	{
		return;
	}
	
	MockClient->MockNetworkTimeout();
}

void UCarespaceTestHelpers::SetupServerErrorScenario(UMockCarespaceHTTPClient* MockClient)
{
	if (!MockClient)
	{
		return;
	}
	
	MockClient->MockServerError();
}

void UCarespaceTestHelpers::SetupRateLimitScenario(UMockCarespaceHTTPClient* MockClient)
{
	if (!MockClient)
	{
		return;
	}
	
	FCarespaceError RateLimitError;
	RateLimitError.Type = ECarespaceErrorType::RateLimited;
	RateLimitError.Message = TEXT("Rate limit exceeded");
	RateLimitError.Code = TEXT("RATE_LIMITED");
	
	UMockCarespaceHTTPClient::FMockResponse RateLimitResponse(RateLimitError);
	MockClient->SetNextResponse(RateLimitResponse);
}

bool UCarespaceTestHelpers::ValidateUser(const FCarespaceUser& User, const FString& ExpectedId, const FString& ExpectedEmail, const FString& ExpectedRole)
{
	return User.Id == ExpectedId && 
		   User.Email == ExpectedEmail && 
		   User.Role == ExpectedRole;
}

bool UCarespaceTestHelpers::ValidateClient(const FCarespaceClient& Client, const FString& ExpectedId, const FString& ExpectedFirstName, const FString& ExpectedLastName)
{
	return Client.Id == ExpectedId && 
		   Client.FirstName == ExpectedFirstName && 
		   Client.LastName == ExpectedLastName;
}

bool UCarespaceTestHelpers::ValidateProgram(const FCarespaceProgram& Program, const FString& ExpectedId, const FString& ExpectedName, const FString& ExpectedCategory)
{
	return Program.Id == ExpectedId && 
		   Program.Name == ExpectedName && 
		   Program.Category == ExpectedCategory;
}

bool UCarespaceTestHelpers::ValidateError(const FCarespaceError& Error, ECarespaceErrorType ExpectedType, const FString& ExpectedMessage, const FString& ExpectedCode)
{
	return Error.Type == ExpectedType && 
		   Error.Message == ExpectedMessage && 
		   Error.Code == ExpectedCode;
}

FString UCarespaceTestHelpers::GenerateUsersListResponse(const TArray<FCarespaceUser>& Users, int32 Page, int32 Limit, int32 Total)
{
	if (Total == 0)
	{
		Total = Users.Num();
	}
	
	int32 TotalPages = (Total + Limit - 1) / Limit;
	
	FString UsersArray = TEXT("[");
	for (int32 i = 0; i < Users.Num(); ++i)
	{
		if (i > 0)
		{
			UsersArray += TEXT(",");
		}
		
		const FCarespaceUser& User = Users[i];
		UsersArray += FString::Printf(TEXT(R"({
			"id": "%s",
			"email": "%s",
			"firstName": "%s",
			"lastName": "%s",
			"role": "%s",
			"isActive": %s,
			"organizationId": "%s"
		})"), 
		*User.Id, *User.Email, *User.FirstName, *User.LastName, *User.Role,
		User.bIsActive ? TEXT("true") : TEXT("false"), *User.OrganizationId);
	}
	UsersArray += TEXT("]");
	
	return FString::Printf(TEXT(R"({
		"success": true,
		"data": {
			"users": %s,
			"pagination": {
				"page": %d,
				"limit": %d,
				"total": %d,
				"totalPages": %d
			}
		}
	})"), *UsersArray, Page, Limit, Total, TotalPages);
}

FString UCarespaceTestHelpers::GenerateSingleUserResponse(const FCarespaceUser& User)
{
	return FString::Printf(TEXT(R"({
		"success": true,
		"data": {
			"user": {
				"id": "%s",
				"email": "%s",
				"firstName": "%s",
				"lastName": "%s",
				"role": "%s",
				"isActive": %s,
				"organizationId": "%s"
			}
		}
	})"), 
	*User.Id, *User.Email, *User.FirstName, *User.LastName, *User.Role,
	User.bIsActive ? TEXT("true") : TEXT("false"), *User.OrganizationId);
}

FString UCarespaceTestHelpers::GenerateClientsListResponse(const TArray<FCarespaceClient>& Clients, int32 Page, int32 Limit, int32 Total)
{
	if (Total == 0)
	{
		Total = Clients.Num();
	}
	
	int32 TotalPages = (Total + Limit - 1) / Limit;
	
	FString ClientsArray = TEXT("[");
	for (int32 i = 0; i < Clients.Num(); ++i)
	{
		if (i > 0)
		{
			ClientsArray += TEXT(",");
		}
		
		const FCarespaceClient& Client = Clients[i];
		ClientsArray += FString::Printf(TEXT(R"({
			"id": "%s",
			"firstName": "%s",
			"lastName": "%s",
			"email": "%s",
			"phone": "%s",
			"gender": "%s",
			"medicalRecordNumber": "%s",
			"isActive": %s
		})"), 
		*Client.Id, *Client.FirstName, *Client.LastName, *Client.Email, *Client.Phone,
		*Client.Gender, *Client.MedicalRecordNumber, Client.bIsActive ? TEXT("true") : TEXT("false"));
	}
	ClientsArray += TEXT("]");
	
	return FString::Printf(TEXT(R"({
		"success": true,
		"data": {
			"clients": %s,
			"pagination": {
				"page": %d,
				"limit": %d,
				"total": %d,
				"totalPages": %d
			}
		}
	})"), *ClientsArray, Page, Limit, Total, TotalPages);
}

FString UCarespaceTestHelpers::GenerateProgramsListResponse(const TArray<FCarespaceProgram>& Programs, int32 Page, int32 Limit, int32 Total)
{
	if (Total == 0)
	{
		Total = Programs.Num();
	}
	
	int32 TotalPages = (Total + Limit - 1) / Limit;
	
	FString ProgramsArray = TEXT("[");
	for (int32 i = 0; i < Programs.Num(); ++i)
	{
		if (i > 0)
		{
			ProgramsArray += TEXT(",");
		}
		
		const FCarespaceProgram& Program = Programs[i];
		ProgramsArray += FString::Printf(TEXT(R"({
			"id": "%s",
			"name": "%s",
			"description": "%s",
			"category": "%s",
			"durationWeeks": %d,
			"sessionsPerWeek": %d,
			"difficultyLevel": "%s",
			"isPublished": %s
		})"), 
		*Program.Id, *Program.Name, *Program.Description, *Program.Category,
		Program.DurationWeeks, Program.SessionsPerWeek, *Program.DifficultyLevel,
		Program.bIsPublished ? TEXT("true") : TEXT("false"));
	}
	ProgramsArray += TEXT("]");
	
	return FString::Printf(TEXT(R"({
		"success": true,
		"data": {
			"programs": %s,
			"pagination": {
				"page": %d,
				"limit": %d,
				"total": %d,
				"totalPages": %d
			}
		}
	})"), *ProgramsArray, Page, Limit, Total, TotalPages);
}

FString UCarespaceTestHelpers::GenerateLoginResponse(const FString& AccessToken, const FString& RefreshToken, const FCarespaceUser& User)
{
	return FString::Printf(TEXT(R"({
		"success": true,
		"data": {
			"accessToken": "%s",
			"refreshToken": "%s",
			"user": {
				"id": "%s",
				"email": "%s",
				"firstName": "%s",
				"lastName": "%s",
				"role": "%s"
			}
		}
	})"), 
	*AccessToken, *RefreshToken, *User.Id, *User.Email, *User.FirstName, *User.LastName, *User.Role);
}

FString UCarespaceTestHelpers::GenerateErrorResponse(const FString& ErrorCode, const FString& ErrorMessage, const FString& ErrorDetails)
{
	FString DetailsJson = ErrorDetails.IsEmpty() ? TEXT("") : FString::Printf(TEXT(R"(,"details": "%s")"), *ErrorDetails);
	
	return FString::Printf(TEXT(R"({
		"success": false,
		"error": {
			"code": "%s",
			"message": "%s"%s
		}
	})"), *ErrorCode, *ErrorMessage, *DetailsJson);
}

FString UCarespaceTestHelpers::GenerateTestId(const FString& Prefix)
{
	return FString::Printf(TEXT("%s_%d_%lld"), *Prefix, TestIdCounter++, FDateTime::Now().GetTicks());
}

FString UCarespaceTestHelpers::GenerateTestEmail(const FString& Username, const FString& Domain)
{
	FString ActualUsername = Username.IsEmpty() ? GenerateTestId(TEXT("user")) : Username;
	return FString::Printf(TEXT("%s@%s"), *ActualUsername, *Domain);
}

bool UCarespaceTestHelpers::WaitForCondition(float MaxWaitTime, float CheckInterval, const TFunction<bool()>& Condition)
{
	if (!Condition)
	{
		return false;
	}
	
	float ElapsedTime = 0.0f;
	while (ElapsedTime < MaxWaitTime)
	{
		if (Condition())
		{
			return true;
		}
		
		FPlatformProcess::Sleep(CheckInterval);
		ElapsedTime += CheckInterval;
	}
	
	return false;
}

void UCarespaceTestHelpers::SimulateDelay(float DelaySeconds)
{
	FPlatformProcess::Sleep(DelaySeconds);
}