#include "user.h"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <random>
#include <iomanip>
#include <regex>

// User class implementation
User::User() : id(0), role(UserRole::STUDENT), status(UserStatus::ACTIVE), 
               loginAttempts(0), isLocked(false) {
    createdAt = Utils::getCurrentDateTime();
}

User::User(int id, const std::string& username, const std::string& password,
           const std::string& email, const std::string& fullName, UserRole role)
    : id(id), username(username), password(password), email(email), 
      fullName(fullName), role(role), status(UserStatus::ACTIVE),
      loginAttempts(0), isLocked(false) {
    createdAt = Utils::getCurrentDateTime();
}

bool User::verifyPassword(const std::string& inputPassword) const {
    // In a real application, this would use proper password hashing
    return password == inputPassword;
}

void User::updateLastLogin() {
    lastLogin = Utils::getCurrentDateTime();
}

void User::incrementLoginAttempts() {
    loginAttempts++;
    if (loginAttempts >= 3) { // MAX_LOGIN_ATTEMPTS
        lockAccount();
    }
}

void User::resetLoginAttempts() {
    loginAttempts = 0;
}

void User::lockAccount() {
    isLocked = true;
    lockTime = Utils::getCurrentDateTime();
    status = UserStatus::SUSPENDED;
}

void User::unlockAccount() {
    isLocked = false;
    lockTime = "";
    loginAttempts = 0;
    if (status == UserStatus::SUSPENDED) {
        status = UserStatus::ACTIVE;
    }
}

bool User::isAccountLocked() const {
    return isLocked;
}

bool User::isValidUsername() const {
    if (username.length() < 3 || username.length() > 20) return false;
    
    // Check for valid characters (alphanumeric and underscore)
    return std::all_of(username.begin(), username.end(), [](char c) {
        return std::isalnum(c) || c == '_';
    });
}

bool User::isValidEmail() const {
    // Simple email validation
    std::regex emailRegex(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    return std::regex_match(email, emailRegex);
}

bool User::isValidPassword() const {
    if (password.length() < 6) return false;
    
    // Check for at least one digit and one letter
    bool hasDigit = std::any_of(password.begin(), password.end(), ::isdigit);
    bool hasLetter = std::any_of(password.begin(), password.end(), ::isalpha);
    
    return hasDigit && hasLetter;
}

bool User::isActive() const {
    return status == UserStatus::ACTIVE && !isLocked;
}

std::string User::roleToString() const {
    switch (role) {
        case UserRole::ADMIN: return "Admin";
        case UserRole::STUDENT: return "Student";
        case UserRole::INSTRUCTOR: return "Instructor";
        default: return "Unknown";
    }
}

std::string User::statusToString() const {
    switch (status) {
        case UserStatus::ACTIVE: return "Active";
        case UserStatus::INACTIVE: return "Inactive";
        case UserStatus::SUSPENDED: return "Suspended";
        case UserStatus::PENDING: return "Pending";
        default: return "Unknown";
    }
}

UserRole User::stringToRole(const std::string& roleStr) {
    std::string lower = roleStr;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    
    if (lower == "admin") return UserRole::ADMIN;
    if (lower == "instructor") return UserRole::INSTRUCTOR;
    return UserRole::STUDENT; // Default
}

UserStatus User::stringToStatus(const std::string& statusStr) {
    std::string lower = statusStr;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    
    if (lower == "inactive") return UserStatus::INACTIVE;
    if (lower == "suspended") return UserStatus::SUSPENDED;
    if (lower == "pending") return UserStatus::PENDING;
    return UserStatus::ACTIVE; // Default
}

void User::displayProfile() const {
    Utils::printHeader("USER PROFILE");
    std::cout << "ID: " << id << std::endl;
    std::cout << "Username: " << username << std::endl;
    std::cout << "Full Name: " << fullName << std::endl;
    std::cout << "Email: " << email << std::endl;
    std::cout << "Role: " << roleToString() << std::endl;
    std::cout << "Status: " << statusToString() << std::endl;
    std::cout << "Created: " << createdAt << std::endl;
    std::cout << "Last Login: " << (lastLogin.empty() ? "Never" : lastLogin) << std::endl;
    std::cout << "Login Attempts: " << loginAttempts << std::endl;
    std::cout << "Account Locked: " << (isLocked ? "Yes" : "No") << std::endl;
}

void User::displayBasicInfo() const {
    std::cout << "ID: " << std::setw(5) << id 
              << " | Username: " << std::setw(15) << username
              << " | Role: " << std::setw(10) << roleToString()
              << " | Status: " << statusToString() << std::endl;
}

bool User::operator==(const User& other) const {
    return id == other.id;
}

bool User::operator<(const User& other) const {
    return username < other.username;
}

std::ostream& operator<<(std::ostream& os, const User& user) {
    os << "User{ID: " << user.id << ", Username: " << user.username 
       << ", Role: " << user.roleToString() << "}";
    return os;
}

// UserManager class implementation
UserManager::UserManager() {
    users.reserve(1000);
}

UserManager::~UserManager() {
    // Cleanup if needed
}

bool UserManager::addUser(const User& user) {
    // Validate user data
    if (!user.isValidUsername() || !user.isValidEmail()) {
        return false;
    }
    
    // Check for unique username and email
    if (!isUsernameUnique(user.getUsername()) || !isEmailUnique(user.getEmail())) {
        return false;
    }
    
    users.push_back(user);
    return true;
}

bool UserManager::removeUser(int userId) {
    int index = findUserIndex(userId);
    if (index == -1) return false;
    
    users.erase(users.begin() + index);
    return true;
}

bool UserManager::updateUser(const User& user) {
    int index = findUserIndex(user.getId());
    if (index == -1) return false;
    
    // Validate updated data
    if (!user.isValidUsername() || !user.isValidEmail()) {
        return false;
    }
    
    // Check uniqueness (excluding current user)
    if (!isUsernameUnique(user.getUsername(), user.getId()) || 
        !isEmailUnique(user.getEmail(), user.getId())) {
        return false;
    }
    
    users[index] = user;
    return true;
}

User* UserManager::findUser(int userId) {
    int index = findUserIndex(userId);
    return (index != -1) ? &users[index] : nullptr;
}

User* UserManager::findUserByUsername(const std::string& username) {
    int index = findUserIndexByUsername(username);
    return (index != -1) ? &users[index] : nullptr;
}

const User* UserManager::findUser(int userId) const {
    int index = findUserIndex(userId);
    return (index != -1) ? &users[index] : nullptr;
}

const User* UserManager::findUserByUsername(const std::string& username) const {
    int index = findUserIndexByUsername(username);
    return (index != -1) ? &users[index] : nullptr;
}

std::vector<User> UserManager::searchUsers(const std::string& keyword) const {
    std::vector<User> results;
    std::string lowerKeyword = keyword;
    std::transform(lowerKeyword.begin(), lowerKeyword.end(), lowerKeyword.begin(), ::tolower);
    
    for (const auto& user : users) {
        std::string lowerUsername = user.getUsername();
        std::string lowerFullName = user.getFullName();
        std::string lowerEmail = user.getEmail();
        
        std::transform(lowerUsername.begin(), lowerUsername.end(), lowerUsername.begin(), ::tolower);
        std::transform(lowerFullName.begin(), lowerFullName.end(), lowerFullName.begin(), ::tolower);
        std::transform(lowerEmail.begin(), lowerEmail.end(), lowerEmail.begin(), ::tolower);
        
        if (lowerUsername.find(lowerKeyword) != std::string::npos ||
            lowerFullName.find(lowerKeyword) != std::string::npos ||
            lowerEmail.find(lowerKeyword) != std::string::npos) {
            results.push_back(user);
        }
    }
    
    return results;
}

std::vector<User> UserManager::getUsersByRole(UserRole role) const {
    std::vector<User> results;
    for (const auto& user : users) {
        if (user.getRole() == role) {
            results.push_back(user);
        }
    }
    return results;
}

std::vector<User> UserManager::getUsersByStatus(UserStatus status) const {
    std::vector<User> results;
    for (const auto& user : users) {
        if (user.getStatus() == status) {
            results.push_back(user);
        }
    }
    return results;
}

std::vector<User> UserManager::getActiveUsers() const {
    std::vector<User> results;
    for (const auto& user : users) {
        if (user.isActive()) {
            results.push_back(user);
        }
    }
    return results;
}

int UserManager::getUserCountByRole(UserRole role) const {
    return std::count_if(users.begin(), users.end(), [role](const User& user) {
        return user.getRole() == role;
    });
}

int UserManager::getUserCountByStatus(UserStatus status) const {
    return std::count_if(users.begin(), users.end(), [status](const User& user) {
        return user.getStatus() == status;
    });
}

void UserManager::sortByUsername() {
    std::sort(users.begin(), users.end(), [](const User& a, const User& b) {
        return a.getUsername() < b.getUsername();
    });
}

void UserManager::sortByRole() {
    std::sort(users.begin(), users.end(), [](const User& a, const User& b) {
        return static_cast<int>(a.getRole()) < static_cast<int>(b.getRole());
    });
}

void UserManager::sortById() {
    std::sort(users.begin(), users.end(), [](const User& a, const User& b) {
        return a.getId() < b.getId();
    });
}

void UserManager::displayAllUsers() const {
    Utils::printHeader("ALL USERS");
    
    if (users.empty()) {
        std::cout << "No users found." << std::endl;
        return;
    }
    
    for (const auto& user : users) {
        user.displayBasicInfo();
    }
}

void UserManager::displayUsersByRole(UserRole role) const {
    Utils::printHeader("USERS BY ROLE");
    
    bool found = false;
    for (const auto& user : users) {
        if (user.getRole() == role) {
            user.displayBasicInfo();
            found = true;
        }
    }
    
    if (!found) {
        std::cout << "No users found with the specified role." << std::endl;
    }
}

void UserManager::displayUserStatistics() const {
    Utils::printHeader("USER STATISTICS");
    
    std::cout << "Total Users: " << getTotalUsers() << std::endl;
    
    std::cout << "\nBy Role:" << std::endl;
    std::cout << "Admins: " << getUserCountByRole(UserRole::ADMIN) << std::endl;
    std::cout << "Students: " << getUserCountByRole(UserRole::STUDENT) << std::endl;
    std::cout << "Instructors: " << getUserCountByRole(UserRole::INSTRUCTOR) << std::endl;
    
    std::cout << "\nBy Status:" << std::endl;
    std::cout << "Active: " << getUserCountByStatus(UserStatus::ACTIVE) << std::endl;
    std::cout << "Inactive: " << getUserCountByStatus(UserStatus::INACTIVE) << std::endl;
    std::cout << "Suspended: " << getUserCountByStatus(UserStatus::SUSPENDED) << std::endl;
    std::cout << "Pending: " << getUserCountByStatus(UserStatus::PENDING) << std::endl;
}

int UserManager::findUserIndex(int userId) const {
    for (size_t i = 0; i < users.size(); ++i) {
        if (users[i].getId() == userId) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

int UserManager::findUserIndexByUsername(const std::string& username) const {
    for (size_t i = 0; i < users.size(); ++i) {
        if (users[i].getUsername() == username) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

bool UserManager::isUsernameUnique(const std::string& username, int excludeUserId) const {
    for (const auto& user : users) {
        if (user.getUsername() == username && user.getId() != excludeUserId) {
            return false;
        }
    }
    return true;
}

bool UserManager::isEmailUnique(const std::string& email, int excludeUserId) const {
    for (const auto& user : users) {
        if (user.getEmail() == email && user.getId() != excludeUserId) {
            return false;
        }
    }
    return true;
}