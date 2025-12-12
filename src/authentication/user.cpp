
#include "user.h"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <random>
#include <iomanip>
#include <regex>
using namespace std;

// User class implementation
User::User() : id(0), role(UserRole::STUDENT), status(UserStatus::ACTIVE), 
               loginAttempts(0), isLocked(false) {
    createdAt = Utils::getCurrentDateTime();
}

User::User(int id, const string& username, const string& password,
           const string& email, const string& fullName, UserRole role)
    : id(id), username(username), password(password), email(email), 
      fullName(fullName), role(role), status(UserStatus::ACTIVE),
      loginAttempts(0), isLocked(false) {
    createdAt = Utils::getCurrentDateTime();
}

bool User::verifyPassword(const string& inputPassword) const {
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
    return all_of(username.begin(), username.end(), [](char c) {
        return isalnum(c) || c == '_';
    });
}

bool User::isValidEmail() const {
    // Simple email validation
    regex emailRegex(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    return regex_match(email, emailRegex);
}

bool User::isValidPassword() const {
    if (password.length() < 6) return false;
    
    // Check for at least one digit and one letter
    bool hasDigit = any_of(password.begin(), password.end(), ::isdigit);
    bool hasLetter = any_of(password.begin(), password.end(), ::isalpha);
    
    return hasDigit && hasLetter;
}

bool User::isActive() const {
    return status == UserStatus::ACTIVE && !isLocked;
}

string User::roleToString() const {
    switch (role) {
        case UserRole::ADMIN: return "Admin";
        case UserRole::STUDENT: return "Student";
        case UserRole::INSTRUCTOR: return "Instructor";
        default: return "Unknown";
    }
}

string User::statusToString() const {
    switch (status) {
        case UserStatus::ACTIVE: return "Active";
        case UserStatus::INACTIVE: return "Inactive";
        case UserStatus::SUSPENDED: return "Suspended";
        case UserStatus::PENDING: return "Pending";
        default: return "Unknown";
    }
}

UserRole User::stringToRole(const string& roleStr) {
    string lower = roleStr;
    transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    
    if (lower == "admin") return UserRole::ADMIN;
    if (lower == "instructor") return UserRole::INSTRUCTOR;
    return UserRole::STUDENT; // Default
}

UserStatus User::stringToStatus(const string& statusStr) {
    string lower = statusStr;
    transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    
    if (lower == "inactive") return UserStatus::INACTIVE;
    if (lower == "suspended") return UserStatus::SUSPENDED;
    if (lower == "pending") return UserStatus::PENDING;
    return UserStatus::ACTIVE; // Default
}

void User::displayProfile() const {
    Utils::printHeader("USER PROFILE");
    cout << "ID: " << id << endl;
    cout << "Username: " << username << endl;
    cout << "Full Name: " << fullName << endl;
    cout << "Email: " << email << endl;
    cout << "Role: " << roleToString() << endl;
    cout << "Status: " << statusToString() << endl;
    cout << "Created: " << createdAt << endl;
    cout << "Last Login: " << (lastLogin.empty() ? "Never" : lastLogin) << endl;
    cout << "Login Attempts: " << loginAttempts << endl;
    cout << "Account Locked: " << (isLocked ? "Yes" : "No") << endl;
}

void User::displayBasicInfo() const {
    cout << "ID: " << setw(5) << id 
              << " | Username: " << setw(15) << username
              << " | Role: " << setw(10) << roleToString()
              << " | Status: " << statusToString() << endl;
}

bool User::operator==(const User& other) const {
    return id == other.id;
}

bool User::operator<(const User& other) const {
    return username < other.username;
}

ostream& operator<<(ostream& os, const User& user) {
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

User* UserManager::findUserByUsername(const string& username) {
    int index = findUserIndexByUsername(username);
    return (index != -1) ? &users[index] : nullptr;
}

const User* UserManager::findUser(int userId) const {
    int index = findUserIndex(userId);
    return (index != -1) ? &users[index] : nullptr;
}

const User* UserManager::findUserByUsername(const string& username) const {
    int index = findUserIndexByUsername(username);
    return (index != -1) ? &users[index] : nullptr;
}

vector<User> UserManager::searchUsers(const string& keyword) const {
    vector<User> results;
    string lowerKeyword = keyword;
    transform(lowerKeyword.begin(), lowerKeyword.end(), lowerKeyword.begin(), ::tolower);
    
    for (const auto& user : users) {
        string lowerUsername = user.getUsername();
        string lowerFullName = user.getFullName();
        string lowerEmail = user.getEmail();
        
        transform(lowerUsername.begin(), lowerUsername.end(), lowerUsername.begin(), ::tolower);
        transform(lowerFullName.begin(), lowerFullName.end(), lowerFullName.begin(), ::tolower);
        transform(lowerEmail.begin(), lowerEmail.end(), lowerEmail.begin(), ::tolower);
        
        if (lowerUsername.find(lowerKeyword) != string::npos ||
            lowerFullName.find(lowerKeyword) != string::npos ||
            lowerEmail.find(lowerKeyword) != string::npos) {
            results.push_back(user);
        }
    }
    
    return results;
}

vector<User> UserManager::getUsersByRole(UserRole role) const {
    vector<User> results;
    for (const auto& user : users) {
        if (user.getRole() == role) {
            results.push_back(user);
        }
    }
    return results;
}

vector<User> UserManager::getUsersByStatus(UserStatus status) const {
    vector<User> results;
    for (const auto& user : users) {
        if (user.getStatus() == status) {
            results.push_back(user);
        }
    }
    return results;
}

vector<User> UserManager::getActiveUsers() const {
    vector<User> results;
    for (const auto& user : users) {
        if (user.isActive()) {
            results.push_back(user);
        }
    }
    return results;
}

int UserManager::getUserCountByRole(UserRole role) const {
    return count_if(users.begin(), users.end(), [role](const User& user) {
        return user.getRole() == role;
    });
}

int UserManager::getUserCountByStatus(UserStatus status) const {
    return count_if(users.begin(), users.end(), [status](const User& user) {
        return user.getStatus() == status;
    });
}

void UserManager::sortByUsername() {
    sort(users.begin(), users.end(), [](const User& a, const User& b) {
        return a.getUsername() < b.getUsername();
    });
}

void UserManager::sortByRole() {
    sort(users.begin(), users.end(), [](const User& a, const User& b) {
        return static_cast<int>(a.getRole()) < static_cast<int>(b.getRole());
    });
}

void UserManager::sortById() {
    sort(users.begin(), users.end(), [](const User& a, const User& b) {
        return a.getId() < b.getId();
    });
}

void UserManager::displayAllUsers() const {
    Utils::printHeader("ALL USERS");
    
    if (users.empty()) {
        cout << "No users found." << endl;
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
        cout << "No users found with the specified role." << endl;
    }
}

void UserManager::displayUserStatistics() const {
    Utils::printHeader("USER STATISTICS");
    
    cout << "Total Users: " << getTotalUsers() << endl;
    
    cout << "\nBy Role:" << endl;
    cout << "Admins: " << getUserCountByRole(UserRole::ADMIN) << endl;
    cout << "Students: " << getUserCountByRole(UserRole::STUDENT) << endl;
    cout << "Instructors: " << getUserCountByRole(UserRole::INSTRUCTOR) << endl;
    
    cout << "\nBy Status:" << endl;
    cout << "Active: " << getUserCountByStatus(UserStatus::ACTIVE) << endl;
    cout << "Inactive: " << getUserCountByStatus(UserStatus::INACTIVE) << endl;
    cout << "Suspended: " << getUserCountByStatus(UserStatus::SUSPENDED) << endl;
    cout << "Pending: " << getUserCountByStatus(UserStatus::PENDING) << endl;
}

int UserManager::findUserIndex(int userId) const {
    for (size_t i = 0; i < users.size(); ++i) {
        if (users[i].getId() == userId) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

int UserManager::findUserIndexByUsername(const string& username) const {
    for (size_t i = 0; i < users.size(); ++i) {
        if (users[i].getUsername() == username) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

bool UserManager::isUsernameUnique(const string& username, int excludeUserId) const {
    for (const auto& user : users) {
        if (user.getUsername() == username && user.getId() != excludeUserId) {
            return false;
        }
    }
    return true;
}

bool UserManager::isEmailUnique(const string& email, int excludeUserId) const {
    for (const auto& user : users) {
        if (user.getEmail() == email && user.getId() != excludeUserId) {
            return false;
        }
    }
    return true;
}