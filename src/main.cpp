#include <iostream>
#include <memory>
#include "structure/utils.h"
#include "authentication/user.h"
#include "authentication/simple_auth.h"
#include "features/admin_stub.h"
#include "features/student_stub.h"

class SimpleExamSystem {
private:
    std::unique_ptr<DatabaseManager> dbManager;
    std::unique_ptr<UserManager> userManager;
    std::unique_ptr<SimpleAuthManager> authManager;
    
public:
    SimpleExamSystem() {
        // Initialize components
        dbManager = std::make_unique<DatabaseManager>("database/exam.db");
        userManager = std::make_unique<UserManager>();
        authManager = std::make_unique<SimpleAuthManager>(userManager.get());
        
        // Initialize database
        if (!dbManager->initializeDatabase()) {
            std::cerr << "Failed to initialize database!" << std::endl;
            exit(1);
        }
        
        loadDefaultUsers();
    }
    
    void run() {
        Utils::printWelcomeMessage();
        
        while (true) {
            if (!authManager->isUserLoggedIn()) {
                showMainMenu();
            } else {
                showUserDashboard();
            }
        }
    }
    
private:
    void showMainMenu() {
        Utils::clearScreen();
        Utils::printHeader("ONLINE EXAMINATION SYSTEM");
        
        std::cout << "1. Login" << std::endl;
        std::cout << "2. Register (Student)" << std::endl;
        std::cout << "3. Guest Mode (Practice)" << std::endl;
        std::cout << "4. System Information" << std::endl;
        std::cout << "5. Exit" << std::endl;
        
        std::cout << "\nEnter your choice: ";
        int choice;
        std::cin >> choice;
        
        switch (choice) {
            case 1:
                handleLogin();
                break;
            case 2:
                handleRegistration();
                break;
            case 3:
                handleGuestMode();
                break;
            case 4:
                showSystemInformation();
                break;
            case 5:
                exitSystem();
                break;
            default:
                std::cout << "Invalid choice! Please try again." << std::endl;
                Utils::pauseSystem();
        }
    }
    
    void handleLogin() {
        Utils::clearScreen();
        Utils::printHeader("USER LOGIN");
        
        std::string username, password;
        std::cout << "Username: ";
        std::cin >> username;
        std::cout << "Password: ";
        std::cin >> password;
        
        AuthResult result = authManager->login(username, password);
        
        if (result == AuthResult::SUCCESS) {
            std::cout << "\nLogin successful! Welcome " 
                     << authManager->getCurrentUser()->getFullName() << std::endl;
            Utils::pauseSystem();
        } else {
            std::cout << "\nLogin failed: " << authManager->getAuthResultMessage(result) << std::endl;
            Utils::pauseSystem();
        }
    }
    
    void handleRegistration() {
        Utils::clearScreen();
        Utils::printHeader("STUDENT REGISTRATION");
        
        std::string username, password, confirmPassword, email, fullName;
        
        std::cout << "Full Name: ";
        std::cin.ignore();
        std::getline(std::cin, fullName);
        
        std::cout << "Username: ";
        std::cin >> username;
        
        std::cout << "Email: ";
        std::cin >> email;
        
        std::cout << "Password: ";
        std::cin >> password;
        
        std::cout << "Confirm Password: ";
        std::cin >> confirmPassword;
        
        if (password != confirmPassword) {
            std::cout << "\nPasswords don't match!" << std::endl;
            Utils::pauseSystem();
            return;
        }
        
        if (authManager->registerUser(username, password, email, fullName, UserRole::STUDENT)) {
            std::cout << "\nRegistration successful! You can now login." << std::endl;
            
            // Also add to database
            User* newUser = userManager->findUserByUsername(username);
            if (newUser) {
                dbManager->insertUser(*newUser);
            }
        } else {
            std::cout << "\nRegistration failed!" << std::endl;
            std::cout << "Please check the following requirements:" << std::endl;
            std::cout << "- Username: 3-20 characters, letters, numbers, and underscore only" << std::endl;
            std::cout << "- Password: At least 6 characters" << std::endl;
            std::cout << "- Email: Valid email format (e.g., user@domain.com)" << std::endl;
            std::cout << "- Full Name: Cannot be empty" << std::endl;
            std::cout << "- Username must be unique (not already taken)" << std::endl;
        }
        
        Utils::pauseSystem();
    }
    
    void handleGuestMode() {
        Utils::clearScreen();
        Utils::printHeader("GUEST MODE - PRACTICE EXAM");
        
        auto questions = dbManager->getRandomQuestions(3);
        if (questions.empty()) {
            std::cout << "No questions available for practice mode." << std::endl;
            Utils::pauseSystem();
            return;
        }
        
        int score = 0;
        for (size_t i = 0; i < questions.size(); ++i) {
            std::cout << "\nQuestion " << (i + 1) << ":" << std::endl;
            questions[i].display();
            
            std::cout << "\nYour answer (1-4): ";
            int answer;
            std::cin >> answer;
            
            if (answer - 1 == questions[i].getCorrectAnswer()) {
                std::cout << "Correct!" << std::endl;
                score++;
            } else {
                std::cout << "Wrong! Correct answer was: " 
                         << (questions[i].getCorrectAnswer() + 1) << std::endl;
            }
        }
        
        std::cout << "\n=== PRACTICE RESULTS ===" << std::endl;
        std::cout << "Score: " << score << "/" << questions.size() << std::endl;
        std::cout << "Percentage: " << (score * 100.0 / questions.size()) << "%" << std::endl;
        
        Utils::pauseSystem();
    }
    
    void showUserDashboard() {
        const User* currentUser = authManager->getCurrentUser();
        if (!currentUser) {
            authManager->logout();
            return;
        }
        
        Utils::clearScreen();
        Utils::printHeader("USER DASHBOARD");
        
        std::cout << "Welcome, " << currentUser->getFullName() << "!" << std::endl;
        std::cout << "Role: " << currentUser->roleToString() << std::endl;
        
        std::cout << "\n1. View Profile" << std::endl;
        std::cout << "2. Change Password" << std::endl;
        
        if (currentUser->getRole() == UserRole::ADMIN) {
            std::cout << "3. Admin Panel" << std::endl;
        } else if (currentUser->getRole() == UserRole::STUDENT) {
            std::cout << "3. Student Panel" << std::endl;
        }
        
        std::cout << "4. Logout" << std::endl;
        
        std::cout << "\nEnter your choice: ";
        int choice;
        std::cin >> choice;
        
        switch (choice) {
            case 1:
                showProfile();
                break;
            case 2:
                changePassword();
                break;
            case 3:
                if (currentUser->getRole() == UserRole::ADMIN) {
                    showAdminPanel();
                } else if (currentUser->getRole() == UserRole::STUDENT) {
                    showStudentPanel();
                }
                break;
            case 4:
                handleLogout();
                break;
            default:
                std::cout << "Invalid choice!" << std::endl;
                Utils::pauseSystem();
        }
    }
    
    void showProfile() {
        Utils::clearScreen();
        authManager->displayCurrentUser();
        Utils::pauseSystem();
    }
    
    void changePassword() {
        Utils::clearScreen();
        Utils::printHeader("CHANGE PASSWORD");
        
        std::string oldPassword, newPassword, confirmPassword;
        
        std::cout << "Current Password: ";
        std::cin >> oldPassword;
        
        std::cout << "New Password: ";
        std::cin >> newPassword;
        
        std::cout << "Confirm New Password: ";
        std::cin >> confirmPassword;
        
        if (newPassword != confirmPassword) {
            std::cout << "\nNew passwords don't match!" << std::endl;
        } else if (authManager->changePassword(oldPassword, newPassword)) {
            std::cout << "\nPassword changed successfully!" << std::endl;
            
            // Update in database
            const User* currentUser = authManager->getCurrentUser();
            if (currentUser) {
                dbManager->updateUser(*currentUser);
            }
        } else {
            std::cout << "\nFailed to change password. Please check your current password." << std::endl;
        }
        
        Utils::pauseSystem();
    }
    
    void showAdminPanel() {
        const User* currentUser = authManager->getCurrentUser();
        AdminPanel adminPanel(dbManager.get(), *currentUser);
        adminPanel.run();
    }
    
    void showStudentPanel() {
        const User* currentUser = authManager->getCurrentUser();
        StudentPanel studentPanel(dbManager.get(), *currentUser);
        studentPanel.run();
    }
    
    void handleLogout() {
        if (authManager->logout()) {
            std::cout << "\nLogged out successfully!" << std::endl;
        }
        Utils::pauseSystem();
    }
    
    void showSystemInformation() {
        Utils::clearScreen();
        Utils::printHeader("SYSTEM INFORMATION");
        
        std::cout << "Online Examination System v2.0" << std::endl;
        std::cout << "Simple C++ Implementation with DSA" << std::endl;
        std::cout << "Database: SQLite" << std::endl;
        std::cout << "\nFeatures:" << std::endl;
        std::cout << "- User Registration & Login" << std::endl;
        std::cout << "- Question Management" << std::endl;
        std::cout << "- Exam Taking" << std::endl;
        std::cout << "- Result Tracking" << std::endl;
        
        std::cout << "\nDatabase Statistics:" << std::endl;
        std::cout << "Total Users: " << dbManager->getTotalUsers() << std::endl;
        std::cout << "Total Questions: " << dbManager->getTotalQuestions() << std::endl;
        std::cout << "Total Results: " << dbManager->getTotalExamResults() << std::endl;
        
        Utils::pauseSystem();
    }
    
    void exitSystem() {
        Utils::printGoodbye();
        std::cout << "Thank you for using Online Examination System!" << std::endl;
        exit(0);
    }
    
    void loadDefaultUsers() {
        // Create default admin if not exists
        if (userManager->findUserByUsername("admin") == nullptr) {
            User defaultAdmin(1, "admin", "admin123", "admin@exam.com", 
                            "System Administrator", UserRole::ADMIN);
            userManager->addUser(defaultAdmin);
            dbManager->insertUser(defaultAdmin);
        }
        
        // Load existing users from database
        auto dbUsers = dbManager->getAllUsers();
        for (const auto& user : dbUsers) {
            if (userManager->findUserByUsername(user.getUsername()) == nullptr) {
                userManager->addUser(user);
            }
        }
    }
};

int main() {
    try {
        SimpleExamSystem system;
        system.run();
    } catch (const std::exception& e) {
        std::cerr << "System Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown system error occurred!" << std::endl;
        return 1;
    }
    
    return 0;
}