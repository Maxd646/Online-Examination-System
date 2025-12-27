#include "simple_auth.h"
#include <algorithm>
#include <regex>
#include <iostream>
using namespace std;

SimpleAuthManager::SimpleAuthManager(UserManager *userMgr)
    : userManager(userMgr), currentUser(nullptr), isLoggedIn(false)
{
}

SimpleAuthManager::~SimpleAuthManager()
{
    logout();
}

AuthResult SimpleAuthManager::login(const string &username, const string &password)
{
    // Validate input
    if (!validateLoginInput(username, password))
    {
        return AuthResult::INVALID_INPUT;
    }

    // Find user
    User *user = userManager->findUserByUsername(username);
    if (!user)
    {
        return AuthResult::USER_NOT_FOUND;
    }

    // Check if account is locked
    if (user->isAccountLocked())
    {
        return AuthResult::ACCOUNT_LOCKED;
    }

    // Verify password
    if (!user->verifyPassword(password))
    {
        user->incrementLoginAttempts();
        userManager->updateUser(*user);
        return AuthResult::INVALID_CREDENTIALS;
    }

    // Successful login
    user->resetLoginAttempts();
    user->updateLastLogin();
    userManager->updateUser(*user);

    currentUser = user;
    isLoggedIn = true;

    return AuthResult::SUCCESS;
}

bool SimpleAuthManager::logout()
{
    if (isLoggedIn)
    {
        currentUser = nullptr;
        isLoggedIn = false;
        return true;
    }
    return false;
}

bool SimpleAuthManager::registerUser(const string &username, const string &password,
                                     const string &email, const string &fullName,
                                     UserRole role)
{
    // Validate input
    if (!validateRegistrationInput(username, password, email, fullName))
    {
        return false;
    }

    // Check if username already exists
    if (userManager->findUserByUsername(username) != nullptr)
    {
        return false; // Username exists
    }

    // Generate new user ID (simple approach)
    int newId = userManager->getTotalUsers() + 1;

    // Create new user
    User newUser(newId, username, password, email, fullName, role);

    // Add to user manager
    return userManager->addUser(newUser);
}

bool SimpleAuthManager::changePassword(const string &oldPassword, const string &newPassword)
{
    if (!isLoggedIn || !currentUser)
    {
        return false;
    }

    // Verify old password
    if (!currentUser->verifyPassword(oldPassword))
    {
        return false;
    }

    // Validate new password
    if (!isValidPassword(newPassword))
    {
        return false;
    }

    // Update password
    currentUser->setPassword(newPassword);
    return userManager->updateUser(*currentUser);
}

bool SimpleAuthManager::isValidUsername(const string &username) const
{
    if (username.length() < 3 || username.length() > 20)
    {
        return false;
    }

    // Check for valid characters (alphanumeric and underscore)
    return all_of(username.begin(), username.end(), [](char c)
                       { return isalnum(c) || c == '_'; });
}

bool SimpleAuthManager::isValidPassword(const string &password) const
{
    // Minimum length requirement
    if (password.length() < 6)
    {
        return false;
    }
    
    // Maximum length to prevent buffer overflow
    if (password.length() > 100)
    {
        return false;
    }
    
    // Check for invalid characters (no spaces, tabs, or control characters)
    for (char c : password)
    {
        if (iscntrl(c) || c == ' ')
        {
            return false;
        }
    }
    
    // Password is valid 
    return true;
}

bool SimpleAuthManager::isValidEmail(const string &email) const
{
    // Check if email is empty
    if (email.empty() || email.length() > 100)
    {
        return false;
    }
    
    // Must contain @ symbol
    if (email.find('@') == string::npos)
    {
        return false;
    }
    
    // Must have exactly one @ symbol
    size_t atCount = 0;
    for (char c : email)
    {
        if (c == '@') atCount++;
    }
    if (atCount != 1)
    {
        return false;
    }
    
    // Split email into local and domain parts
    size_t atPos = email.find('@');
    string localPart = email.substr(0, atPos);
    string domainPart = email.substr(atPos + 1);
    
    // Local part validation (before @)
    if (localPart.empty() || localPart.length() > 64)
    {
        return false;
    }
    
    // Domain part validation (after @)
    if (domainPart.empty() || domainPart.length() > 255)
    {
        return false;
    }
    
    // Domain must contain at least one dot
    if (domainPart.find('.') == string::npos)
    {
        return false;
    }
    
    // Domain must have valid TLD (at least 2 characters after last dot)
    size_t lastDot = domainPart.rfind('.');
    if (lastDot == string::npos || lastDot >= domainPart.length() - 1)
    {
        return false;
    }
    string tld = domainPart.substr(lastDot + 1);
    if (tld.length() < 2)
    {
        return false;
    }
    
    // Use regex for more comprehensive validation
    regex emailRegex(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    return regex_match(email, emailRegex);
}

string SimpleAuthManager::getAuthResultMessage(AuthResult result) const
{
    switch (result)
    {
    case AuthResult::SUCCESS:
        return "Login successful";
    case AuthResult::INVALID_CREDENTIALS:
        return "Invalid username or password";
    case AuthResult::ACCOUNT_LOCKED:
        return "Account is locked due to too many failed attempts";
    case AuthResult::USER_NOT_FOUND:
        return "User not found";
    case AuthResult::USERNAME_EXISTS:
        return "Username already exists";
    case AuthResult::INVALID_INPUT:
        return "Invalid input provided";
    default:
        return "Unknown error";
    }
}

void SimpleAuthManager::displayCurrentUser() const
{
    if (isLoggedIn && currentUser)
    {
        cout << "\n=== CURRENT USER ===" << endl;
        cout << "Username: " << currentUser->getUsername() << endl;
        cout << "Full Name: " << currentUser->getFullName() << endl;
        cout << "Email: " << currentUser->getEmail() << endl;
        cout << "Role: " << currentUser->roleToString() << endl;
        cout << "Status: " << currentUser->statusToString() << endl;
    }
    else
    {
        cout << "No user currently logged in." << endl;
    }
}

bool SimpleAuthManager::validateLoginInput(const string &username, const string &password) const
{
    // Check for empty inputs
    if (username.empty() || password.empty())
    {
        return false;
    }
    
    // Check for maximum length to prevent buffer overflow
    if (username.length() > 50 || password.length() > 100)
    {
        return false;
    }
    
    // Check for minimum length
    if (username.length() < 3 || password.length() < 6)
    {
        return false;
    }
    
    // Check for invalid characters in username
    for (char c : username)
    {
        if (iscntrl(c) || c == ' ')
        {
            return false;
        }
    }
    
    // Check for invalid characters in password
    for (char c : password)
    {
        if (iscntrl(c))
        {
            return false;
        }
    }
    
    return true;
}

bool SimpleAuthManager::validateRegistrationInput(const string &username, const string &password,
                                                  const string &email, const string &fullName) const
{
    // Validate username
    if (!isValidUsername(username))
    {
        return false;
    }
    
    // Validate password
    if (!isValidPassword(password))
    {
        return false;
    }
    
    // Validate email
    if (!isValidEmail(email))
    {
        return false;
    }
    
    // Validate full name
    if (fullName.empty() || fullName.length() > 100)
    {
        return false;
    }
    
    // Check for invalid characters in full name (no control characters)
    for (char c : fullName)
    {
        if (iscntrl(c))
        {
            return false;
        }
    }
    
    return true;
}