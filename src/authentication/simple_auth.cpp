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
    if (password.length() < 6)
    {
        return false;
    }

    // Password is valid if it's at least 6 characters
    // More flexible validation - allow numbers-only or letters-only passwords
    return true;
}

bool SimpleAuthManager::isValidEmail(const string &email) const
{
    // Simple email validation
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
    return !username.empty() && !password.empty() &&
           username.length() <= 50 && password.length() <= 100;
}

bool SimpleAuthManager::validateRegistrationInput(const string &username, const string &password,
                                                  const string &email, const string &fullName) const
{
    return isValidUsername(username) &&
           isValidPassword(password) &&
           isValidEmail(email) &&
           !fullName.empty() &&
           fullName.length() <= 100;
}