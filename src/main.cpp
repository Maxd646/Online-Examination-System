#include <iostream>
#include <memory>
#include <limits>
#include <iomanip>
#include "structure/utils.h"
#include "authentication/user.h"
#include "authentication/simple_auth.h"
#include "features/admin_stub.h"
#include "features/student_stub.h"
using namespace std;

class SimpleExamSystem
{
private:
    unique_ptr<DatabaseManager> dbManager;
    unique_ptr<UserManager> userManager;
    unique_ptr<SimpleAuthManager> authManager;

public:
    SimpleExamSystem()
    {
        // Initialize components
        dbManager = make_unique<DatabaseManager>("database/exam.db");
        userManager = make_unique<UserManager>();
        authManager = make_unique<SimpleAuthManager>(userManager.get());

        // Initialize database
        if (!dbManager->initializeDatabase())
        {
            cerr << "Failed to initialize database!" << endl;
            exit(1);
        }

        loadDefaultUsers();
    }

    void run()
    {
        Utils::printWelcomeMessage();

        while (true)
        {
            if (!authManager->isUserLoggedIn())
            {
                showMainMenu();
            }
            else
            {
                showUserDashboard();
            }
        }
    }

private:
    void showMainMenu()
    {
        Utils::clearScreen();

        // Bright main menu
        cout << "\n+" << string(58, '=') << "+" << endl;
        cout << "|" << setw(35) << "ONLINE EXAMINATION SYSTEM" << setw(23) << "|" << endl;
        cout << "|" << setw(35) << "Advanced C++ with DSA" << setw(23) << "|" << endl;
        cout << "+" << string(58, '=') << "+" << endl;

        cout << " 1. Login" << endl;
        cout << " 2. Register (Student)" << endl;
        cout << " 3. Guest Mode (Practice)" << endl;
        cout << "  4. System Information" << endl;
        cout << " 5. Exit" << endl;

        cout << "\nEnter your choice: ";
        int choice;
        cin >> choice;

        switch (choice)
        {
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

    void handleLogin()
    {
        Utils::clearScreen();
        Utils::printHeader("USER LOGIN");

        string username, password;
        cout << "Username: ";
        cin >> username;
        cout << "Password: ";
        cin >> password;

        // Use database authentication directly
        User dbUser;
        if (dbManager->authenticateUser(username, password, dbUser))
        {
            // Successful authentication
            cout << "\n✓ Login successful! Welcome " << dbUser.getFullName() << endl;
            cout << "Role: " << dbUser.roleToString() << endl;

            // Add/update user in memory for auth manager
            User *memUser = userManager->findUserByUsername(username);
            if (memUser)
            {
                userManager->updateUser(dbUser);
            }
            else
            {
                userManager->addUser(dbUser);
            }

            // Set current user in auth manager (this should work now)
            AuthResult result = authManager->login(username, password);
            if (result != AuthResult::SUCCESS)
            {
                cout << "Warning: Auth manager sync issue, but login successful." << endl;
            }
        }
        else
        {
            // Check specific failure reason
            User checkUser = dbManager->getUserByUsername(username);
            if (checkUser.getId() == 0)
            {
                cout << "\n✗ Login failed: User not found" << endl;
            }
            else if (checkUser.isAccountLocked())
            {
                cout << "\n✗ Login failed: Account is locked due to too many failed attempts" << endl;
            }
            else
            {
                cout << "\n✗ Login failed: Invalid password" << endl;
            }
        }

        Utils::pauseSystem();
    }

    void handleRegistration()
    {
        Utils::clearScreen();
        Utils::printHeader("STUDENT REGISTRATION");

        string username, password, confirmPassword, email, fullName;

        cout << "Full Name: ";
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // FIXED
        getline(cin, fullName);

        cout << "Username: ";
        getline(cin, username);

        cout << "Email: ";
        getline(cin, email);

        cout << "Password: ";
        getline(cin, password);

        cout << "Confirm Password: ";
        getline(cin, confirmPassword);

        if (password != confirmPassword)
        {
            cout << "\nPasswords don't match!" << endl;
            Utils::pauseSystem();
            return;
        }

        // Check if username already exists in database
        User existingUser = dbManager->getUserByUsername(username);
        if (existingUser.getId() != 0)
        {
            cout << "\n✗ Registration failed! Username already exists." << endl;
            Utils::pauseSystem();
            return;
        }

        // Create user directly for database (let database generate ID)
        User newUser(0, username, password, email, fullName, UserRole::STUDENT);

        // Save directly to database
        cout << "\nAttempting to save user to database..." << endl;

        if (dbManager->insertUser(newUser))
        {
            cout << "\n✓ Registration successful!" << endl;
            cout << "Your account has been created and saved to database." << endl;

            // Verify the user was actually saved
            User dbUser = dbManager->getUserByUsername(username);
            if (dbUser.getId() != 0)
            {
                cout << "✓ User verified in database with ID: " << dbUser.getId() << endl;
                userManager->addUser(dbUser);
                cout << "✓ User added to memory for immediate use" << endl;
            }
            else
            {
                cout << "⚠ Warning: Could not retrieve user from database after insertion!" << endl;
            }

            cout << "You can now login with your credentials." << endl;

            // Show total users for verification
            int totalUsers = dbManager->getTotalUsers();
            cout << "Total users in database: " << totalUsers << endl;
        }
        else
        {
            cout << "\n✗ Registration failed!" << endl;
            cout << "Could not save user to database. Please try again." << endl;
            cout << "Please check that:" << endl;
            cout << "  - Username is unique" << endl;
            cout << "  - Email is unique" << endl;
            cout << "  - All fields are properly filled" << endl;
        }

        Utils::pauseSystem();
    }

    void handleGuestMode()
    {
        Utils::clearScreen();
        Utils::printHeader("GUEST MODE - PRACTICE EXAM");

        auto questions = dbManager->getRandomQuestions(3);
        if (questions.empty())
        {
            cout << "No questions available for practice mode." << endl;
            Utils::pauseSystem();
            return;
        }

        int score = 0;
        for (size_t i = 0; i < questions.size(); ++i)
        {
            cout << "\nQuestion " << (i + 1) << ":" << endl;
            questions[i].display();

            cout << "\nYour answer (1-4): ";
            int answer;
            cin >> answer;

            if (answer - 1 == questions[i].getCorrectAnswer())
            {
                cout << "Correct!" << endl;
                score++;
            }
            else
            {
                cout << "Wrong! Correct answer was: "
                     << (questions[i].getCorrectAnswer() + 1) << endl;
            }
        }

        cout << "\n=== PRACTICE RESULTS ===" << endl;
        cout << "Score: " << score << "/" << questions.size() << endl;
        cout << "Percentage: " << (score * 100.0 / questions.size()) << "%" << endl;

        Utils::pauseSystem();
    }

    void showUserDashboard()
    {
        const User *currentUser = authManager->getCurrentUser();
        if (!currentUser)
        {
            authManager->logout();
            return;
        }

        Utils::clearScreen();
        Utils::printHeader("USER DASHBOARD");

        cout << "Welcome, " << currentUser->getFullName() << "!" << endl;
        cout << "Role: " << currentUser->roleToString() << endl;

        cout << "\n1. View Profile" << endl;
        cout << "2. Change Password" << endl;

        if (currentUser->getRole() == UserRole::ADMIN)
        {
            cout << "3. Admin Panel" << endl;
        }
        else if (currentUser->getRole() == UserRole::STUDENT)
        {
            cout << "3. Student Panel" << endl;
        }

        cout << "4. Logout" << endl;

        cout << "\nEnter your choice: ";
        int choice;
        cin >> choice;

        switch (choice)
        {
        case 1:
            showProfile();
            break;
        case 2:
            changePassword();
            break;
        case 3:
            if (currentUser->getRole() == UserRole::ADMIN)
            {
                showAdminPanel();
            }
            else if (currentUser->getRole() == UserRole::STUDENT)
            {
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

    void showProfile()
    {
        Utils::clearScreen();
        authManager->displayCurrentUser();
        Utils::pauseSystem();
    }

    void changePassword()
    {
        Utils::clearScreen();
        Utils::printHeader("CHANGE PASSWORD");

        string oldPassword, newPassword, confirmPassword;

        cout << "Current Password: ";
        cin >> oldPassword;

        cout << "New Password: ";
        cin >> newPassword;

        cout << "Confirm New Password: ";
        cin >> confirmPassword;

        if (newPassword != confirmPassword)
        {
            cout << "\nNew passwords don't match!" << endl;
        }
        else if (authManager->changePassword(oldPassword, newPassword))
        {
            cout << "\nPassword changed successfully!" << endl;

            // Update in database
            const User *currentUser = authManager->getCurrentUser();
            if (currentUser)
            {
                dbManager->updateUser(*currentUser);
            }
        }
        else
        {
            cout << "\nFailed to change password. Please check your current password." << endl;
        }

        Utils::pauseSystem();
    }

    void showAdminPanel()
    {
        const User *currentUser = authManager->getCurrentUser();
        AdminPanel adminPanel(dbManager.get(), *currentUser);
        adminPanel.run();
    }

    void showStudentPanel()
    {
        const User *currentUser = authManager->getCurrentUser();
        StudentPanel studentPanel(dbManager.get(), *currentUser);
        studentPanel.run();
    }

    void handleLogout()
    {
        if (authManager->logout())
        {
            cout << "\nLogged out successfully!" << endl;
        }
        Utils::pauseSystem();
    }

    void showSystemInformation()
    {
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

    void exitSystem()
    {
        Utils::printGoodbye();
        cout << "Thank you for using Online Examination System!" << endl;
        exit(0);
    }

    void loadDefaultUsers()
    {
        cout << "Initializing user system..." << endl;

        // Load all existing users from database into memory
        auto dbUsers = dbManager->getAllUsers();
        cout << "Found " << dbUsers.size() << " users in database" << endl;

        for (const auto &user : dbUsers)
        {
            // Skip users with empty usernames (corrupted data)
            if (user.getUsername().empty())
            {
                cout << "  Skipping user with empty username (ID: " << user.getId() << ")" << endl;
                continue;
            }

            if (userManager->findUserByUsername(user.getUsername()) == nullptr)
            {
                userManager->addUser(user);
                cout << "  Loaded: " << user.getUsername() << " (" << user.roleToString() << ")" << endl;
            }
        }

        // Verify admin user exists
        User existingAdmin = dbManager->getUserByUsername("admin");
        if (existingAdmin.getId() == 0)
        {
            cout << "Admin user not found, creating..." << endl;
            User defaultAdmin(0, "admin", "admin123", "admin@exam.com",
                              "System Administrator", UserRole::ADMIN);
            if (dbManager->insertUser(defaultAdmin))
            {
                User dbAdmin = dbManager->getUserByUsername("admin");
                if (dbAdmin.getId() != 0)
                {
                    userManager->addUser(dbAdmin);
                    cout << "✓ Default admin user created" << endl;
                }
                else
                {
                    cout << "✗ Failed to retrieve created admin user" << endl;
                }
            }
            else
            {
                cout << "✗ Failed to create admin user (may already exist with different email)" << endl;
            }
        }
        else
        {
            cout << "✓ Admin user verified in database (ID: " << existingAdmin.getId() << ")" << endl;
        }

        cout << "✓ User system ready. Total users: " << userManager->getTotalUsers() << endl;
    }
};

int main()
{
    try
    {
        SimpleExamSystem system;
        system.run();
    }
    catch (const exception &e)
    {
        cerr << "System Error: " << e.what() << endl;
        return 1;
    }
    catch (...)
    {
        cerr << "Unknown system error occurred!" << endl;
        return 1;
    }

    return 0;
}