#ifndef USER_H
#define USER_H

#include <string>
#include <vector>
#include <chrono>
#include "../structure/utils.h"

enum class UserRole {
    ADMIN = 1,
    STUDENT = 2,
    INSTRUCTOR = 3
};

enum class UserStatus {
    ACTIVE = 1,
    INACTIVE = 2,
    SUSPENDED = 3,
    PENDING = 4
};

class User {
private:
    int id;
    string username;
    string password;
    string email;
    string fullName;
    UserRole role;
    UserStatus status;
    string createdAt;
    string lastLogin;
    int loginAttempts;
    bool isLocked;
    string lockTime;
    
public:
    // Constructors
    User();
    User(int id, const string& username, const string& password, 
         const string& email, const string& fullName, UserRole role);
    
    // Getters
    int getId() const { return id; }
    string getUsername() const { return username; }
    string getPassword() const { return password; }
    string getEmail() const { return email; }
    string getFullName() const { return fullName; }
    UserRole getRole() const { return role; }
    UserStatus getStatus() const { return status; }
    string getCreatedAt() const { return createdAt; }
    string getLastLogin() const { return lastLogin; }
    int getLoginAttempts() const { return loginAttempts; }
    bool getIsLocked() const { return isLocked; }
    
    // Setters
    void setId(int id) { this->id = id; }
    void setUsername(const string& username) { this->username = username; }
    void setPassword(const string& password) { this->password = password; }
    void setEmail(const string& email) { this->email = email; }
    void setFullName(const string& fullName) { this->fullName = fullName; }
    void setRole(UserRole role) { this->role = role; }
    void setStatus(UserStatus status) { this->status = status; }
    void setLastLogin(const string& lastLogin) { this->lastLogin = lastLogin; }
    
    // Authentication methods
    bool verifyPassword(const string& inputPassword) const;
    void updateLastLogin();
    void incrementLoginAttempts();
    void resetLoginAttempts();
    void lockAccount();
    void unlockAccount();
    bool isAccountLocked() const;
    
    // Validation methods
    bool isValidUsername() const;
    bool isValidEmail() const;
    bool isValidPassword() const;
    bool isActive() const;
    
    // Utility methods
    string roleToString() const;
    string statusToString() const;
    static UserRole stringToRole(const string& roleStr);
    static UserStatus stringToStatus(const string& statusStr);
    
    // Display methods
    void displayProfile() const;
    void displayBasicInfo() const;
    
    // Operators
    bool operator==(const User& other) const;
    bool operator<(const User& other) const;
    friend ostream& operator<<(ostream& os, const User& user);
};

// User management with advanced DSA
class UserManager {
private:
    vector<User> users;
    
public:
    UserManager();
    ~UserManager();
    
    // User CRUD operations
    bool addUser(const User& user);
    bool removeUser(int userId);
    bool updateUser(const User& user);
    User* findUser(int userId);
    User* findUserByUsername(const string& username);
    const User* findUser(int userId) const;
    const User* findUserByUsername(const string& username) const;
    
    // User management
    bool changePassword(int userId, const string& oldPassword, const string& newPassword);
    bool resetPassword(int userId, const string& newPassword);
    bool lockUser(int userId);
    bool unlockUser(int userId);
    bool activateUser(int userId);
    bool deactivateUser(int userId);
    
    // Search and filter
    vector<User> searchUsers(const string& keyword) const;
    vector<User> getUsersByRole(UserRole role) const;
    vector<User> getUsersByStatus(UserStatus status) const;
    vector<User> getActiveUsers() const;
    vector<User> getLockedUsers() const;
    vector<User> getAllUsers() const { return users; }
    
    // Statistics
    int getTotalUsers() const { return users.size(); }
    int getUserCountByRole(UserRole role) const;
    int getUserCountByStatus(UserStatus status) const;
    
    // Sorting
    void sortByUsername();
    void sortByRole();
    void sortByLastLogin();
    void sortById();
    
    // Display methods
    void displayAllUsers() const;
    void displayUsersByRole(UserRole role) const;
    void displayUserStatistics() const;
    
    // Backup and restore
    bool exportUsers(const string& filename) const;
    bool importUsers(const string& filename);
    
private:
    int findUserIndex(int userId) const;
    int findUserIndexByUsername(const string& username) const;
    bool isUsernameUnique(const string& username, int excludeUserId = -1) const;
    bool isEmailUnique(const string& email, int excludeUserId = -1) const;
};

#endif // USER_H