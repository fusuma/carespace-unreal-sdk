# Contributing to Carespace Unreal SDK

Thank you for your interest in contributing to the Carespace Unreal SDK! This document provides guidelines and information for contributors.

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [Getting Started](#getting-started)
- [Development Setup](#development-setup)
- [Making Changes](#making-changes)
- [Testing](#testing)
- [Submitting Changes](#submitting-changes)
- [Coding Standards](#coding-standards)
- [Documentation](#documentation)

## Code of Conduct

This project and everyone participating in it is governed by our Code of Conduct. By participating, you are expected to uphold this code.

## Getting Started

1. Fork the repository on GitHub
2. Clone your fork locally
3. Set up the development environment
4. Create a topic branch for your changes
5. Make your changes and test them
6. Submit a pull request

## Development Setup

### Prerequisites

- Unreal Engine 5.0 or later
- Visual Studio 2022 (Windows) or Xcode (macOS)
- Git for version control

### Initial Setup

1. **Clone the repository:**
   ```bash
   git clone https://github.com/your-username/carespace-unreal-sdk.git
   cd carespace-unreal-sdk
   ```

2. **Set up Unreal Engine:**
   - Ensure Unreal Engine is properly installed
   - The project should build automatically when opened

3. **Install dependencies:**
   - The SDK uses built-in Unreal modules (HTTP, JSON)
   - No additional package installation required

## Making Changes

### Branch Naming

Use descriptive branch names:
- `feature/new-authentication-method`
- `fix/http-client-memory-leak`
- `docs/improve-blueprint-examples`

### Commit Messages

Write clear, concise commit messages:
```
feat: add patient data caching mechanism

- Implement in-memory cache for patient records
- Add cache invalidation on data updates
- Reduce API calls by 60% for repeated queries
```

Use these prefixes:
- `feat:` - New features
- `fix:` - Bug fixes
- `docs:` - Documentation updates
- `refactor:` - Code refactoring
- `test:` - Test additions/updates
- `style:` - Code style changes

## Testing

### Testing Requirements

- All new features must include appropriate tests
- Bug fixes should include regression tests
- Ensure existing tests continue to pass

### Testing Guidelines

1. **Unit Tests**: Test individual components in isolation
2. **Integration Tests**: Test API interactions and data flow
3. **Blueprint Tests**: Verify Blueprint node functionality
4. **Platform Tests**: Test on multiple supported platforms

### Running Tests

#### From Unreal Editor
1. Open **Window > Developer Tools > Session Frontend**
2. Switch to the **Automation** tab
3. Expand **CarespaceSDK** in the test tree
4. Select tests and click **Start Tests**

#### From Command Line
```bash
# Run all SDK tests
UE4Editor-Cmd YourProject.uproject -ExecCmds="Automation RunTests CarespaceSDK;quit" -TestExit="Automation Test Queue Empty" -ReportOutputDir="TestResults" -log

# Run specific category
UE4Editor-Cmd YourProject.uproject -ExecCmds="Automation RunTests CarespaceSDK.API;quit" -TestExit="Automation Test Queue Empty" -ReportOutputDir="TestResults" -log
```

For detailed testing guidelines, see [TESTING.md](TESTING.md).

## Submitting Changes

### Pull Request Process

1. **Update Documentation**: Ensure README.md and relevant docs are updated
2. **Update Changelog**: Add entries to CHANGELOG.md
3. **Test Thoroughly**: Verify changes work across supported platforms
4. **Create Pull Request**: Use the provided template

### Pull Request Template

```markdown
## Description
Brief description of changes made.

## Type of Change
- [ ] Bug fix
- [ ] New feature
- [ ] Documentation update
- [ ] Refactoring

## Testing
- [ ] Unit tests pass
- [ ] Integration tests pass
- [ ] Manual testing completed
- [ ] Cross-platform testing done

## Documentation
- [ ] Code comments updated
- [ ] README.md updated (if needed)
- [ ] CHANGELOG.md updated

## Screenshots/Examples
Include relevant screenshots or code examples.
```

## Coding Standards

### C++ Guidelines

#### Naming Conventions
- **Classes**: PascalCase (`CarespaceAPI`)
- **Functions**: PascalCase (`GetPatientData`)
- **Variables**: camelCase (`patientId`)
- **Constants**: UPPER_SNAKE_CASE (`MAX_RETRY_COUNT`)
- **Private Members**: Prefix with underscore (`_httpClient`)

#### Code Style
```cpp
// Use clear, descriptive names
void ProcessPatientData(const FPatientData& PatientData)
{
    // Use early returns to reduce nesting
    if (!PatientData.IsValid())
    {
        return;
    }
    
    // Clear variable names and proper spacing
    const FString PatientId = PatientData.GetId();
    
    // Use const when possible
    const bool bIsAdult = PatientData.Age >= 18;
}
```

#### Memory Management
- Use smart pointers where appropriate
- Follow Unreal's memory management patterns
- Avoid raw pointers for managed objects
- Use `UPROPERTY()` for garbage collected objects

### Blueprint Guidelines

- Use clear, descriptive node names
- Group related functionality
- Add comments for complex logic
- Follow Blueprint naming conventions

### Documentation Standards

#### Code Comments
```cpp
/**
 * Authenticates a user with the Carespace API using email and password.
 * 
 * @param Email The user's email address
 * @param Password The user's password
 * @param OnSuccess Delegate called when authentication succeeds
 * @param OnFailure Delegate called when authentication fails
 * 
 * @return True if the request was initiated successfully, false otherwise
 */
bool Login(const FString& Email, const FString& Password, 
           const FOnLoginSuccess& OnSuccess, 
           const FOnLoginFailure& OnFailure);
```

#### Header Documentation
- Include file purpose and main classes
- Document major dependencies
- Include usage examples for complex classes

## Documentation

### Documentation Requirements

- All public APIs must be documented
- Include practical examples
- Update README.md for new features
- Maintain accurate Blueprint documentation

### Documentation Style

- Use clear, concise language
- Include code examples
- Provide context and use cases
- Keep examples up to date

## Questions and Support

- **Issues**: Use GitHub Issues for bug reports and feature requests
- **Discussions**: Use GitHub Discussions for questions and ideas
- **Email**: Contact support@carespace.ai for sensitive issues

## Recognition

Contributors will be recognized in:
- CHANGELOG.md
- Project documentation
- Release notes

Thank you for contributing to the Carespace Unreal SDK!