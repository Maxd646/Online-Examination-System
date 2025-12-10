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

AuthResult SimpleAuthManager::login(const std::string &username, const std::string &password)
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

bool SimpleAuthManager::registerUser(const std::string &username, const std::string &password,
                                     const std::string &email, const std::string &fullName,
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

bool SimpleAuthManager::changePassword(const std::string &oldPassword, const std::string &newPassword)
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

bool SimpleAuthManager::isValidUsername(const std::string &username) const
{
    if (username.length() < 3 || username.length() > 20)
    {
        return false;
    }

    // Check for valid characters (alphanumeric and underscore)
    return std::all_of(username.begin(), username.end(), [](char c)
                       { return std::isalnum(c) || c == '_'; });
}

bool SimpleAuthManager::isValidPassword(const std::string &password) const
{
    if (password.length() < 6)
    {
        return false;
    }

    // Password is valid if it's at least 6 characters
    // More flexible validation - allow numbers-only or letters-only passwords
    return true;
}

bool SimpleAuthManager::isValidEmail(const std::string &email) const
{
    // Simple email validation
    std::regex emailRegex(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    return std::regex_match(email, emailRegex);
}

std::string SimpleAuthManager::getAuthResultMessage(AuthResult result) const
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
        std::cout << "\n=== CURRENT USER ===" << std::endl;
        std::cout << "Username: " << currentUser->getUsername() << std::endl;
        std::cout << "Full Name: " << currentUser->getFullName() << std::endl;
        std::cout << "Email: " << currentUser->getEmail() << std::endl;
        std::cout << "Role: " << currentUser->roleToString() << std::endl;
        std::cout << "Status: " << currentUser->statusToString() << std::endl;
    }
    else
    {
        std::cout << "No user currently logged in." << std::endl;
    }
}

bool SimpleAuthManager::validateLoginInput(const std::string &username, const std::string &password) const
{
    return !username.empty() && !password.empty() &&
           username.length() <= 50 && password.length() <= 100;
}

bool SimpleAuthManager::validateRegistrationInput(const std::string &username, const std::string &password,
                                                  const std::string &email, const std::string &fullName) const
{
    return isValidUsername(username) &&
           isValidPassword(password) &&
           isValidEmail(email) &&
           !fullName.empty() &&
           fullName.length() <= 100;
}