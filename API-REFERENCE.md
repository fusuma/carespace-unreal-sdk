# Carespace API Reference

This document provides detailed technical reference for all endpoints, data structures, and authentication methods available in the Carespace Unreal SDK.

## Table of Contents

- [Authentication](#authentication)
- [Core Data Types](#core-data-types)
- [API Endpoints](#api-endpoints)
- [Error Handling](#error-handling)
- [Rate Limiting](#rate-limiting)
- [SDK Classes](#sdk-classes)

## Authentication

### Overview
The Carespace API uses JWT (JSON Web Token) based authentication. All API requests (except login and registration) require a valid access token.

### Authentication Flow
1. **Login**: Exchange email/password for access and refresh tokens
2. **API Requests**: Include access token in Authorization header
3. **Token Refresh**: Use refresh token to get new access token when it expires
4. **Logout**: Invalidate tokens on server

### Token Management
- **Access Token**: Short-lived (typically 15 minutes), used for API requests
- **Refresh Token**: Long-lived (typically 7 days), used to obtain new access tokens
- **Token Storage**: Store securely in application, never expose in logs

### Authentication Endpoints

#### POST /api/auth/login
Authenticates user and returns tokens.

**Request Body:**
```json
{
  "email": "user@example.com",
  "password": "securePassword123"
}
```

**Response:**
```json
{
  "success": true,
  "data": {
    "accessToken": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...",
    "refreshToken": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...",
    "user": {
      "id": "user_123",
      "email": "user@example.com",
      "firstName": "John",
      "lastName": "Doe",
      "role": "clinician"
    }
  }
}
```

#### POST /api/auth/refresh
Refreshes an expired access token.

**Request Body:**
```json
{
  "refreshToken": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9..."
}
```

**Response:**
```json
{
  "success": true,
  "data": {
    "accessToken": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...",
    "refreshToken": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9..."
  }
}
```

#### POST /api/auth/logout
Invalidates user session.

**Headers:**
```
Authorization: Bearer <access_token>
```

**Response:**
```json
{
  "success": true,
  "message": "Successfully logged out"
}
```

#### POST /api/auth/forgot-password
Initiates password reset process.

**Request Body:**
```json
{
  "email": "user@example.com"
}
```

#### POST /api/auth/reset-password
Completes password reset with token.

**Request Body:**
```json
{
  "token": "reset_token_from_email",
  "password": "newSecurePassword123"
}
```

#### PUT /api/auth/change-password
Changes password for authenticated user.

**Headers:**
```
Authorization: Bearer <access_token>
```

**Request Body:**
```json
{
  "currentPassword": "oldPassword123",
  "newPassword": "newSecurePassword123"
}
```

## Core Data Types

### User Types

#### FCarespaceUser
```cpp
USTRUCT(BlueprintType)
struct FCarespaceUser
{
    UPROPERTY(BlueprintReadWrite)
    FString Id;

    UPROPERTY(BlueprintReadWrite)
    FString Email;

    UPROPERTY(BlueprintReadWrite)
    FString FirstName;

    UPROPERTY(BlueprintReadWrite)
    FString LastName;

    UPROPERTY(BlueprintReadWrite)
    FString Role; // "admin", "clinician", "patient"

    UPROPERTY(BlueprintReadWrite)
    FDateTime CreatedAt;

    UPROPERTY(BlueprintReadWrite)
    FDateTime UpdatedAt;

    UPROPERTY(BlueprintReadWrite)
    bool bIsActive;

    UPROPERTY(BlueprintReadWrite)
    FString OrganizationId;
};
```

#### FCarespaceCreateUserRequest
```cpp
USTRUCT(BlueprintType)
struct FCarespaceCreateUserRequest
{
    UPROPERTY(BlueprintReadWrite)
    FString Email;

    UPROPERTY(BlueprintReadWrite)
    FString FirstName;

    UPROPERTY(BlueprintReadWrite)
    FString LastName;

    UPROPERTY(BlueprintReadWrite)
    FString Role;

    UPROPERTY(BlueprintReadWrite)
    FString Password;

    UPROPERTY(BlueprintReadWrite)
    FString OrganizationId;
};
```

### Client (Patient) Types

#### FCarespaceClient
```cpp
USTRUCT(BlueprintType)
struct FCarespaceClient
{
    UPROPERTY(BlueprintReadWrite)
    FString Id;

    UPROPERTY(BlueprintReadWrite)
    FString FirstName;

    UPROPERTY(BlueprintReadWrite)
    FString LastName;

    UPROPERTY(BlueprintReadWrite)
    FDateTime DateOfBirth;

    UPROPERTY(BlueprintReadWrite)
    FString Gender; // "male", "female", "other", "prefer-not-to-say"

    UPROPERTY(BlueprintReadWrite)
    FString Email;

    UPROPERTY(BlueprintReadWrite)
    FString Phone;

    UPROPERTY(BlueprintReadWrite)
    FCarespaceAddress Address;

    UPROPERTY(BlueprintReadWrite)
    FString MedicalRecordNumber;

    UPROPERTY(BlueprintReadWrite)
    TArray<FString> Conditions;

    UPROPERTY(BlueprintReadWrite)
    TArray<FString> Medications;

    UPROPERTY(BlueprintReadWrite)
    FString AssignedClinicianId;

    UPROPERTY(BlueprintReadWrite)
    FDateTime CreatedAt;

    UPROPERTY(BlueprintReadWrite)
    FDateTime UpdatedAt;

    UPROPERTY(BlueprintReadWrite)
    bool bIsActive;
};
```

### Program Types

#### FCarespaceProgram
```cpp
USTRUCT(BlueprintType)
struct FCarespaceProgram
{
    UPROPERTY(BlueprintReadWrite)
    FString Id;

    UPROPERTY(BlueprintReadWrite)
    FString Name;

    UPROPERTY(BlueprintReadWrite)
    FString Description;

    UPROPERTY(BlueprintReadWrite)
    FString Category; // "physical-therapy", "occupational-therapy", "speech-therapy"

    UPROPERTY(BlueprintReadWrite)
    TArray<FCarespaceExercise> Exercises;

    UPROPERTY(BlueprintReadWrite)
    int32 DurationWeeks;

    UPROPERTY(BlueprintReadWrite)
    int32 SessionsPerWeek;

    UPROPERTY(BlueprintReadWrite)
    FString DifficultyLevel; // "beginner", "intermediate", "advanced"

    UPROPERTY(BlueprintReadWrite)
    TArray<FString> TargetConditions;

    UPROPERTY(BlueprintReadWrite)
    FString CreatedByUserId;

    UPROPERTY(BlueprintReadWrite)
    FDateTime CreatedAt;

    UPROPERTY(BlueprintReadWrite)
    FDateTime UpdatedAt;

    UPROPERTY(BlueprintReadWrite)
    bool bIsPublished;
};
```

## API Endpoints

### Users API

#### GET /api/users
Retrieves paginated list of users.

**Query Parameters:**
- `page` (int): Page number (default: 1)
- `limit` (int): Items per page (default: 20, max: 100)
- `search` (string): Search term for name or email
- `role` (string): Filter by user role
- `organizationId` (string): Filter by organization

**Response:**
```json
{
  "success": true,
  "data": {
    "users": [...],
    "pagination": {
      "page": 1,
      "limit": 20,
      "total": 150,
      "totalPages": 8
    }
  }
}
```

#### GET /api/users/{id}
Retrieves specific user by ID.

#### POST /api/users
Creates new user.

#### PUT /api/users/{id}
Updates existing user.

#### DELETE /api/users/{id}
Deactivates user (soft delete).

### Clients API

#### GET /api/clients
Retrieves paginated list of clients.

**Query Parameters:**
- `page` (int): Page number (default: 1)
- `limit` (int): Items per page (default: 20, max: 100)
- `search` (string): Search term for name or MRN
- `assignedClinicianId` (string): Filter by assigned clinician
- `condition` (string): Filter by medical condition

#### GET /api/clients/{id}
Retrieves specific client by ID.

#### POST /api/clients
Creates new client.

#### PUT /api/clients/{id}
Updates existing client.

#### DELETE /api/clients/{id}
Deactivates client.

### Programs API

#### GET /api/programs
Retrieves paginated list of programs.

**Query Parameters:**
- `page` (int): Page number (default: 1)
- `limit` (int): Items per page (default: 20, max: 100)
- `category` (string): Filter by program category
- `difficulty` (string): Filter by difficulty level
- `condition` (string): Filter by target condition
- `published` (bool): Filter by publication status

#### GET /api/programs/{id}
Retrieves specific program by ID.

#### POST /api/programs
Creates new program.

#### PUT /api/programs/{id}
Updates existing program.

#### DELETE /api/programs/{id}
Deletes program.

### Sessions API

#### GET /api/sessions
Retrieves client sessions.

#### POST /api/sessions
Creates new session.

#### PUT /api/sessions/{id}
Updates session data.

#### GET /api/sessions/{id}/results
Retrieves session performance data.

## Error Handling

### Error Response Format
```json
{
  "success": false,
  "error": {
    "code": "VALIDATION_ERROR",
    "message": "Invalid input parameters",
    "details": {
      "field": "email",
      "reason": "Invalid email format"
    }
  }
}
```

### Common Error Codes

| Code | HTTP Status | Description |
|------|-------------|-------------|
| `VALIDATION_ERROR` | 400 | Invalid request parameters |
| `UNAUTHORIZED` | 401 | Missing or invalid authentication |
| `FORBIDDEN` | 403 | Insufficient permissions |
| `NOT_FOUND` | 404 | Resource not found |
| `CONFLICT` | 409 | Resource already exists |
| `RATE_LIMITED` | 429 | Too many requests |
| `SERVER_ERROR` | 500 | Internal server error |

### SDK Error Handling
```cpp
// C++ Error Handling
void OnAPIFailure(const FCarespaceError& Error)
{
    switch (Error.Type)
    {
        case ECarespaceErrorType::NetworkError:
            // Handle network connectivity issues
            break;
        case ECarespaceErrorType::AuthenticationError:
            // Redirect to login or refresh token
            break;
        case ECarespaceErrorType::ValidationError:
            // Show validation messages to user
            break;
        case ECarespaceErrorType::ServerError:
            // Show generic error message
            break;
    }
}
```

## Rate Limiting

### Limits
- **Authentication endpoints**: 5 requests per minute per IP
- **Standard endpoints**: 1000 requests per hour per user
- **Bulk operations**: 100 requests per hour per user

### Headers
```
X-RateLimit-Limit: 1000
X-RateLimit-Remaining: 999
X-RateLimit-Reset: 1640995200
```

### Handling Rate Limits
```cpp
// The SDK automatically handles rate limiting with exponential backoff
// Configure retry behavior:
HTTPClient->SetMaxRetries(3);
HTTPClient->SetRetryDelay(1.0f); // Base delay in seconds
```

## SDK Classes

### UCarespaceAPI
Main SDK entry point providing high-level API access.

**Key Methods:**
- `Initialize(BaseURL, APIKey)`: Configure SDK
- `GetAuthAPI()`: Access authentication functions
- `GetUsers()`, `GetClients()`, `GetPrograms()`: Data retrieval
- `CreateUser()`, `CreateClient()`, `CreateProgram()`: Data creation

### UCarespaceAuthAPI
Authentication and user management.

**Key Methods:**
- `Login(LoginRequest, OnComplete)`: User authentication
- `RefreshToken(RefreshToken, OnComplete)`: Token refresh
- `ForgotPassword(Email, OnComplete)`: Password reset initiation
- `ChangePassword(CurrentPwd, NewPwd, OnComplete)`: Password change

### UCarespaceHTTPClient
Low-level HTTP client for direct API access.

**Key Methods:**
- `Get(Endpoint, OnComplete)`: HTTP GET request
- `Post(Endpoint, Data, OnComplete)`: HTTP POST request
- `Put(Endpoint, Data, OnComplete)`: HTTP PUT request
- `Delete(Endpoint, OnComplete)`: HTTP DELETE request

### UCarespaceBlueprintLibrary
Static Blueprint helper functions.

**Key Methods:**
- `CreateAPI(BaseURL, APIKey)`: Factory method
- `ParseJSONToUser(JSONString)`: JSON parsing utilities
- `ValidateEmail(Email)`: Input validation helpers

## Best Practices

### Security
- Never log or expose authentication tokens
- Implement automatic token refresh
- Use HTTPS for all communications
- Validate all user inputs before API calls

### Performance
- Implement local caching for frequently accessed data
- Use pagination for large datasets
- Batch multiple operations when possible
- Implement proper error handling and retry logic

### Error Handling
- Always check for network connectivity
- Implement graceful degradation for offline scenarios
- Provide meaningful error messages to users
- Log errors for debugging without exposing sensitive data

### Memory Management
- Properly manage delegate lifecycles
- Avoid circular references with callbacks
- Use weak references for UI callbacks
- Clean up resources in BeginDestroy

## Support

For additional support and questions:
- **Documentation**: https://docs.carespace.ai/unreal
- **API Status**: https://status.carespace.ai
- **Support**: https://support.carespace.ai
- **GitHub Issues**: https://github.com/carespace/unreal-sdk/issues