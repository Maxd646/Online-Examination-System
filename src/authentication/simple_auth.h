#ifndef SIMPLE_AUTH_H
#define SIMPLE_AUTH_H

#include <string>
#include <vector>
#include "user.h"

// Simple authentication result
enum class AuthResult {
    SUCCESS = 0,
    INVALID_CREDENTIALS = 1,
    ACCOUNT_LOCKED = 2,
    USER_NOT_FOUND = 3,
    USERNAME_EXISTS = 4,
    INVALID_INPUT = 5
};

// Simple Authentication Manager
class SimpleAuthManager {
private:
    UserManager* userManager;
    User* currentUser;
    bool isLoggedIn;
    
public:
    SimpleAuthManager(UserManager* userMgr);
    ~SimpleAuthManager();
    
    // Basic authentication
    AuthResult login(const std::string& username, const std::string& password);
    bool logout();
    bool registerUser(const std::string& username, const std::string& password, 
                     const std::string& email, const std::string& fullName, 
                     UserRole role = UserRole::STUDENT);
    
    // Session management
    bool isUserLoggedIn() const { return isLoggedIn; }
    User* getCurrentUser() { return currentUser; }
    const User* getCurrentUser() const { return currentUser; }
    
    // Password management
    bool changePassword(const std::string& oldPassword, const std::string& newPassword);
    
    // Validation helpers
    bool isValidUsername(const std::string& username) const;
    bool isValidPassword(const std::string& password) const;
    bool isValidEmail(const std::string& email) const;
    
    // Utility methods
    std::string getAuthResultMessage(AuthResult result) const;
    void displayCurrentUser() const;
    
private:
    bool validateLoginInput(const std::string& username, const std::string& password) const;
    bool validateRegistrationInput(const std::string& username, const std::string& password, 
                                  const std::string& email, const std::string& fullName) const;
};

#endif // SIMPLE_AUTH_H