#include <iostream>
#include <memory>
#include "structure/utils.h"
#include "authentication/user.h"
#include "authentication/simple_auth.h"
#include "features/admin_stub.h"
#include "features/student_stub.h"
using namespace exam_system;

class SimpleExamSystem {
private:
    unique_ptr<DatabaseManager> dbManager;
    unique_ptr<UserManager> userManager;
    unique_ptr<SimpleAuthManager> authManager;
    
public:
    SimpleExamSystem() {
        // Initialize components
        dbManager = make_unique<DatabaseManager>("database/exam.db");
        userManager = make_unique<UserManager>();
        authManager = make_unique<SimpleAuthManager>(userManager.get());
        
        // Initialize database
        if (!dbManager->initializeDatabase()) {
            cerr << "Failed to initialize database!" <<endl;
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
        
        cout << "1. Login" <<endl;
        cout << "2. Register (Student)" << endl;
        cout << "3. Guest Mode (Practice)" << endl;
        cout << "4. System Information" << endl;
        cout << "5. Exit" <<endl;
        
        cout << "\nEnter your choice: ";
        int choice;
        cin >> choice;
        
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
                cout << "Invalid choice! Please try again." << endl;
                Utils::pauseSystem();
        }
    }
    
    void handleLogin() {
        Utils::clearScreen();
        Utils::printHeader("USER LOGIN");
        
        string username, password;
       cout << "Username: ";
       cin >> username;
        cout << "Password: ";
        cin >> password;
        
        AuthResult result = authManager->login(username, password);
        
        if (result == AuthResult::SUCCESS) {
            cout << "\nLogin successful! Welcome " 
                     << authManager->getCurrentUser()->getFullName() << endl;
            Utils::pauseSystem();
        } else {
            cout << "\nLogin failed: " << authManager->getAuthResultMessage(result) << endl;
            Utils::pauseSystem();
        }
    }
    
    void handleRegistration() {
        Utils::clearScreen();
        Utils::printHeader("STUDENT REGISTRATION");
        
        string username, password, confirmPassword, email, fullName;
        
        cout << "Full Name: ";
        cin.ignore();
        getline(cin, fullName);
        
        cout << "Username: ";
        cin >> username;
        
        cout << "Email: ";
        cin >> email;
        
        cout << "Password: ";
        cin >> password;
        
        cout << "Confirm Password: ";
        cin >> confirmPassword;
        
        if (password != confirmPassword) {
            cout << "\nPasswords don't match!" << endl;
            Utils::pauseSystem();
            return;
        }
        
        if (authManager->registerUser(username, password, email, fullName, UserRole::STUDENT)) {
            cout << "\nRegistration successful! You can now login." << endl;
            
            // Also add to database
            User* newUser = userManager->findUserByUsername(username);
            if (newUser) {
                dbManager->insertUser(*newUser);
            }
        } else {
            cout << "\nRegistration failed!" << endl;
            cout << "Please check the following requirements:" << endl;
            cout << "- Username: 3-20 characters, letters, numbers, and underscore only" << endl;
            cout << "- Password: At least 6 characters" << endl;
            cout << "- Email: Valid email format (e.g., user@domain.com)" << endl;
            cout << "- Full Name: Cannot be empty" << endl;
            cout << "- Username must be unique (not already taken)" << endl;
        }
        
        Utils::pauseSystem();
    }
    
    void handleGuestMode() {
        Utils::clearScreen();
        Utils::printHeader("GUEST MODE - PRACTICE EXAM");
        
        auto questions = dbManager->getRandomQuestions(3);
        if (questions.empty()) {
            cout << "No questions available for practice mode." << endl;
            Utils::pauseSystem();
            return;
        }
        
        int score = 0;
        for (size_t i = 0; i < questions.size(); ++i) {
            cout << "\nQuestion " << (i + 1) << ":" << endl;
            questions[i].display();
            
            cout << "\nYour answer (1-4): ";
            int answer;
            cin >> answer;
            
            if (answer - 1 == questions[i].getCorrectAnswer()) {
                cout << "Correct!" << endl;
                score++;
            } else {
                cout << "Wrong! Correct answer was: " 
                         << (questions[i].getCorrectAnswer() + 1) << endl;
            }
        }
        
        cout << "\n=== PRACTICE RESULTS ===" << endl;
        cout << "Score: " << score << "/" << questions.size() << endl;
        cout << "Percentage: " << (score * 100.0 / questions.size()) << "%" << endl;
        
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
        
        cout << "Welcome, " << currentUser->getFullName() << "!" << endl;
        cout << "Role: " << currentUser->roleToString() << endl;
        
        cout << "\n1. View Profile" << endl;
        cout << "2. Change Password" << endl;
        
        if (currentUser->getRole() == UserRole::ADMIN) {
            cout << "3. Admin Panel" << endl;
        } else if (currentUser->getRole() == UserRole::STUDENT) {
            cout << "3. Student Panel" << endl;
        }
        
        cout << "4. Logout" << endl;
        
        cout << "\nEnter your choice: ";
        int choice;
        cin >> choice;
        
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
                cout << "Invalid choice!" << endl;
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
        
        string oldPassword, newPassword, confirmPassword;
        
        cout << "Current Password: ";
        cin >> oldPassword;
        
        cout << "New Password: ";
        cin >> newPassword;
        
        cout << "Confirm New Password: ";
        cin >> confirmPassword;
        
        if (newPassword != confirmPassword) {
            cout << "\nNew passwords don't match!" << endl;
        } else if (authManager->changePassword(oldPassword, newPassword)) {
            cout << "\nPassword changed successfully!" << endl;
            
            // Update in database
            const User* currentUser = authManager->getCurrentUser();
            if (currentUser) {
                dbManager->updateUser(*currentUser);
            }
        } else {
            cout << "\nFailed to change password. Please check your current password." << endl;
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
            cout << "\nLogged out successfully!" << endl;
        }
        Utils::pauseSystem();
    }
    
    void showSystemInformation() {
        Utils::clearScreen();
        Utils::printHeader("SYSTEM INFORMATION");
        
        cout << "Online Examination System v2.0" << endl;
        cout << "Simple C++ Implementation with DSA" << endl;
        cout << "Database: SQLite" << endl;
        cout << "\nFeatures:" << endl;
        cout << "- User Registration & Login" << endl;
        cout << "- Question Management" << endl;
        cout << "- Exam Taking" << endl;
        cout << "- Result Tracking" << endl;
        
        cout << "\nDatabase Statistics:" << endl;
        cout << "Total Users: " << dbManager->getTotalUsers() << endl;
        cout << "Total Questions: " << dbManager->getTotalQuestions() << endl;
        cout << "Total Results: " << dbManager->getTotalExamResults() << endl;
        
        Utils::pauseSystem();
    }
    
    void exitSystem() {
        Utils::printGoodbye();
        cout << "Thank you for using Online Examination System!" << endl;
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
    } catch (const exception& e) {
        cerr << "System Error: " << e.what() << endl;
        return 1;
    } catch (...) {
        cerr << "Unknown system error occurred!" << endl;
        return 1;
    }
    
    return 0;
}