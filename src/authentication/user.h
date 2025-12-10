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
    std::string username;
    std::string password;
    std::string email;
    std::string fullName;
    UserRole role;
    UserStatus status;
    std::string createdAt;
    std::string lastLogin;
    int loginAttempts;
    bool isLocked;
    std::string lockTime;
    
public:
    // Constructors
    User();
    User(int id, const std::string& username, const std::string& password, 
         const std::string& email, const std::string& fullName, UserRole role);
    
    // Getters
    int getId() const { return id; }
    std::string getUsername() const { return username; }
    std::string getPassword() const { return password; }
    std::string getEmail() const { return email; }
    std::string getFullName() const { return fullName; }
    UserRole getRole() const { return role; }
    UserStatus getStatus() const { return status; }
    std::string getCreatedAt() const { return createdAt; }
    std::string getLastLogin() const { return lastLogin; }
    int getLoginAttempts() const { return loginAttempts; }
    bool getIsLocked() const { return isLocked; }
    
    // Setters
    void setId(int id) { this->id = id; }
    void setUsername(const std::string& username) { this->username = username; }
    void setPassword(const std::string& password) { this->password = password; }
    void setEmail(const std::string& email) { this->email = email; }
    void setFullName(const std::string& fullName) { this->fullName = fullName; }
    void setRole(UserRole role) { this->role = role; }
    void setStatus(UserStatus status) { this->status = status; }
    void setLastLogin(const std::string& lastLogin) { this->lastLogin = lastLogin; }
    
    // Authentication methods
    bool verifyPassword(const std::string& inputPassword) const;
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
    std::string roleToString() const;
    std::string statusToString() const;
    static UserRole stringToRole(const std::string& roleStr);
    static UserStatus stringToStatus(const std::string& statusStr);
    
    // Display methods
    void displayProfile() const;
    void displayBasicInfo() const;
    
    // Operators
    bool operator==(const User& other) const;
    bool operator<(const User& other) const;
    friend std::ostream& operator<<(std::ostream& os, const User& user);
};

// User management with advanced DSA
class UserManager {
private:
    std::vector<User> users;
    
public:
    UserManager();
    ~UserManager();
    
    // User CRUD operations
    bool addUser(const User& user);
    bool removeUser(int userId);
    bool updateUser(const User& user);
    User* findUser(int userId);
    User* findUserByUsername(const std::string& username);
    const User* findUser(int userId) const;
    const User* findUserByUsername(const std::string& username) const;
    
    // User management
    bool changePassword(int userId, const std::string& oldPassword, const std::string& newPassword);
    bool resetPassword(int userId, const std::string& newPassword);
    bool lockUser(int userId);
    bool unlockUser(int userId);
    bool activateUser(int userId);
    bool deactivateUser(int userId);
    
    // Search and filter
    std::vector<User> searchUsers(const std::string& keyword) const;
    std::vector<User> getUsersByRole(UserRole role) const;
    std::vector<User> getUsersByStatus(UserStatus status) const;
    std::vector<User> getActiveUsers() const;
    std::vector<User> getLockedUsers() const;
    std::vector<User> getAllUsers() const { return users; }
    
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
    bool exportUsers(const std::string& filename) const;
    bool importUsers(const std::string& filename);
    
private:
    int findUserIndex(int userId) const;
    int findUserIndexByUsername(const std::string& username) const;
    bool isUsernameUnique(const std::string& username, int excludeUserId = -1) const;
    bool isEmailUnique(const std::string& email, int excludeUserId = -1) const;
};

#endif // USER_H