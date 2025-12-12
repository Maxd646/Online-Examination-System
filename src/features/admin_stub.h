#ifndef ADMIN_STUB_H
#define ADMIN_STUB_H

#include "../authentication/user.h"
#include "../database/database.h"
#include "../structure/utils.h"
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <iomanip>

// Enhanced AdminPanel with question management functionality
class AdminPanel
{
private:
    DatabaseManager *dbManager;
    User currentAdmin;

public:
    AdminPanel(DatabaseManager *db, const User &admin)
    {
        dbManager = db;
        currentAdmin = admin;
    }

    void run()
    {
        while (true)
        {
            Utils::clearScreen();
            
            // Bright admin panel header
            cout << "\n+" << string(58, '=') << "+" << endl;
            cout << "|" << setw(35) << "ADMIN PANEL" << setw(23) << "|" << endl;
            cout << "+" << string(58, '=') << "+" << endl;
            
            cout << "\n👋 Welcome " << currentAdmin.getFullName() << "!" << endl;
            cout << "\n📋 Admin Menu:" << endl;
            cout << "❓ 1. Question Management" << endl;
            cout << "👥 2. User Management" << endl;
            cout << "📊 3. View System Statistics" << endl;
            cout << "📈 4. View All Results" << endl;
            cout << "🚪 5. Logout" << endl;

            cout << "\n➤ Enter your choice: ";
            int choice;
            cin >> choice;

            switch (choice)
            {
            case 1:
                questionManagement();
                break;
            case 2:
                userManagement();
                break;
            case 3:
                showStatistics();
                break;
            case 4:
                viewAllResults();
                break;
            case 5:
                cout << "\nLogging out..." << endl;
                return;
            default:
                cout << "Invalid choice! Please try again." << endl;
                Utils::pauseSystem();
            }
        }
    }

private:
    void questionManagement()
    {
        while (true)
        {
            Utils::clearScreen();
            Utils::printHeader("QUESTION MANAGEMENT");
            cout << "1. Add New Question" << endl;
            cout << "2. View All Questions" << endl;
            cout << "3. Search Questions" << endl;
            cout << "4. Update Question" << endl;
            cout << "5. Delete Question" << endl;
            cout << "6. Back to Main Menu" << endl;

            cout << "\nEnter your choice: ";
            int choice;
            cin >> choice;

            switch (choice)
            {
            case 1:
                addQuestion();
                break;
            case 2:
                viewAllQuestions();
                break;
            case 3:
                searchQuestions();
                break;
            case 4:
                updateQuestion();
                break;
            case 5:
                deleteQuestion();
                break;
            case 6:
                return;
            default:
                cout << "Invalid choice!" << endl;
                Utils::pauseSystem();
            }
        }
    }

    void addQuestion()
    {
        Utils::clearScreen();
        Utils::printHeader("ADD NEW QUESTION");

        string subject, questionText, option1, option2, option3, option4, difficulty, explanation;
        int correctAnswer;

        cout << "Subject: ";
        cin.ignore();
        getline(cin, subject);

        cout << "Question Text: ";
        getline(cin, questionText);

        cout << "Option 1: ";
        getline(cin, option1);

        cout << "Option 2: ";
        getline(cin, option2);

        cout << "Option 3: ";
        getline(cin, option3);

        cout << "Option 4: ";
        getline(cin, option4);

        cout << "Correct Answer (1-4): ";
        cin >> correctAnswer;
        correctAnswer--; // Convert to 0-based index

        cout << "Difficulty (Easy/Medium/Hard): ";
        cin >> difficulty;

        cout << "Explanation (optional): ";
        cin.ignore();
        getline(cin, explanation);

        // Validate input
        if (correctAnswer < 0 || correctAnswer > 3)
        {
            cout << "\n✗ Invalid correct answer! Must be between 1-4." << endl;
            Utils::pauseSystem();
            return;
        }

        // Create question object
        vector<string> options = {option1, option2, option3, option4};
        Question newQuestion(0, subject, questionText, options, correctAnswer, difficulty, explanation);
        
        // Set the created_by field to current admin's ID
        newQuestion.setCreatedBy(currentAdmin.getId());

        // Validate question before inserting
        if (!newQuestion.isValid())
        {
            cout << "\n✗ Invalid question data! Please check all fields are filled." << endl;
            Utils::pauseSystem();
            return;
        }

        if (dbManager->insertQuestion(newQuestion))
        {
            cout << "\n✓ Question added successfully!" << endl;
        }
        else
        {
            cout << "\n✗ Failed to add question!" << endl;
        }

        Utils::pauseSystem();
    }

    void viewAllQuestions()
    {
        Utils::clearScreen();
        Utils::printHeader("ALL QUESTIONS");

        auto questions = dbManager->getAllQuestions();
        if (questions.empty())
        {
            cout << "No questions found!" << endl;
        }
        else
        {
            for (size_t i = 0; i < questions.size(); ++i)
            {
                cout << "\n--- Question " << (i + 1) << " ---" << endl;
                cout << "ID: " << questions[i].getId() << endl;
                cout << "Subject: " << questions[i].getSubject() << endl;
                cout << "Difficulty: " << questions[i].getDifficulty() << endl;
                questions[i].displayWithAnswer();
                cout << string(50, '-') << endl;
            }
        }

        Utils::pauseSystem();
    }

    void searchQuestions()
    {
        Utils::clearScreen();
        Utils::printHeader("SEARCH QUESTIONS");

        cout << "1. Search by Subject" << endl;
        cout << "2. Search by Difficulty" << endl;
        cout << "3. Search by Keyword" << endl;

        cout << "\nEnter choice: ";
        int choice;
        cin >> choice;

        vector<Question> results;

        switch (choice)
        {
        case 1:
        {
            cout << "Enter subject: ";
            string subject;
            cin.ignore();
            getline(cin, subject);
            results = dbManager->getQuestionsBySubject(subject);
            break;
        }
        case 2:
        {
            cout << "Enter difficulty: ";
            string difficulty;
            cin >> difficulty;
            results = dbManager->getQuestionsByDifficulty(difficulty);
            break;
        }
        case 3:
        {
            cout << "Enter keyword: ";
            string keyword;
            cin.ignore();
            getline(cin, keyword);
            results = dbManager->searchQuestions(keyword);
            break;
        }
        default:
            cout << "Invalid choice!" << endl;
            Utils::pauseSystem();
            return;
        }

        cout << "\nSearch Results (" << results.size() << " found):" << endl;
        for (size_t i = 0; i < results.size(); ++i)
        {
            cout << "\n--- Result " << (i + 1) << " ---" << endl;
            cout << "ID: " << results[i].getId() << endl;
            results[i].display();
        }

        Utils::pauseSystem();
    }

    void updateQuestion()
    {
        Utils::clearScreen();
        Utils::printHeader("UPDATE QUESTION");

        cout << "Enter Question ID to update: ";
        int questionId;
        cin >> questionId;

        Question question = dbManager->getQuestionById(questionId);
        if (question.getId() == 0)
        {
            cout << "Question not found!" << endl;
            Utils::pauseSystem();
            return;
        }

        cout << "\nCurrent Question:" << endl;
        question.displayWithAnswer();

        cout << "\nEnter new values (press Enter to keep current):" << endl;

        string input;
        cin.ignore();

        cout << "Subject [" << question.getSubject() << "]: ";
        getline(cin, input);
        if (!input.empty())
            question.setSubject(input);

        cout << "Question Text [" << question.getQuestionText() << "]: ";
        getline(cin, input);
        if (!input.empty())
            question.setQuestionText(input);

        cout << "Difficulty [" << question.getDifficulty() << "]: ";
        getline(cin, input);
        if (!input.empty())
            question.setDifficulty(input);

        if (dbManager->updateQuestion(question))
        {
            cout << "\n✓ Question updated successfully!" << endl;
        }
        else
        {
            cout << "\n✗ Failed to update question!" << endl;
        }

        Utils::pauseSystem();
    }

    void deleteQuestion()
    {
        Utils::clearScreen();
        Utils::printHeader("DELETE QUESTION");

        cout << "Enter Question ID to delete: ";
        int questionId;
        cin >> questionId;

        Question question = dbManager->getQuestionById(questionId);
        if (question.getId() == 0)
        {
            cout << "Question not found!" << endl;
            Utils::pauseSystem();
            return;
        }

        cout << "\nQuestion to delete:" << endl;
        question.display();

        cout << "\nAre you sure you want to delete this question? (y/N): ";
        char confirm;
        cin >> confirm;

        if (confirm == 'y' || confirm == 'Y')
        {
            if (dbManager->deleteQuestion(questionId))
            {
                cout << "\n✓ Question deleted successfully!" << endl;
            }
            else
            {
                cout << "\n✗ Failed to delete question!" << endl;
            }
        }
        else
        {
            cout << "\nDeletion cancelled." << endl;
        }

        Utils::pauseSystem();
    }

    void userManagement()
    {
        while (true) {
            Utils::clearScreen();
            Utils::printHeader("USER MANAGEMENT");
            
            cout << "1. View All Users" << endl;
            cout << "2. Search User" << endl;
            cout << "3. User Statistics" << endl;
            cout << "4. Manage Student Status" << endl;
            cout << "5. Back to Main Menu" << endl;
            
            cout << "\nEnter your choice: ";
            int choice;
            cin >> choice;
            
            switch (choice) {
                case 1:
                    viewAllUsers();
                    break;
                case 2:
                    searchUser();
                    break;
                case 3:
                    userStatistics();
                    break;
                case 4:
                    manageStudentStatus();
                    break;
                case 5:
                    return;
                default:
                    cout << "Invalid choice!" << endl;
                    Utils::pauseSystem();
            }
        }
    }
    
    void viewAllUsers()
    {
        Utils::clearScreen();
        
        // Bright header
        cout << "\n+" << string(88, '=') << "+" << endl;
        cout << "|" << setw(44) << "ALL USERS" << setw(44) << "|" << endl;
        cout << "+" << string(88, '=') << "+" << endl;

        auto users = dbManager->getAllUsers();
        cout << "\n📊 Total Users Found: " << users.size() << endl;
        
        if (users.empty()) {
            cout << "\n❌ No users found in database!" << endl;
            Utils::pauseSystem();
            return;
        }
        
        cout << "\n📋 User List:" << endl;
        cout << "┌─────┬───────────────┬─────────────────────────┬────────────────────┬────────────┬──────────┐" << endl;
        cout << "│ " << left << setw(3) << "ID" 
             << " │ " << setw(13) << "Username" 
             << " │ " << setw(23) << "Full Name" 
             << " │ " << setw(18) << "Email"
             << " │ " << setw(10) << "Role" 
             << " │ " << setw(8) << "Status" << " │" << endl;
        cout << "├─────┼───────────────┼─────────────────────────┼────────────────────┼────────────┼──────────┤" << endl;

        for (const auto& user : users)
        {
            string username = user.getUsername();
            string fullName = user.getFullName();
            string email = user.getEmail();
            
            // Truncate long strings to fit columns
            if (username.length() > 13) username = username.substr(0, 10) + "...";
            if (fullName.length() > 23) fullName = fullName.substr(0, 20) + "...";
            if (email.length() > 18) email = email.substr(0, 15) + "...";
            
            cout << "│ " << left << setw(3) << user.getId()
                 << " │ " << setw(13) << username
                 << " │ " << setw(23) << fullName
                 << " │ " << setw(18) << email
                 << " │ " << setw(10) << user.roleToString()
                 << " │ " << setw(8) << user.statusToString() << " │" << endl;
        }
        
        cout << "└─────┴───────────────┴─────────────────────────┴────────────────────┴────────────┴──────────┘" << endl;
        cout << "\n✅ User list displayed successfully!" << endl;
        Utils::pauseSystem();
    }
    
    void searchUser()
    {
        Utils::clearScreen();
        Utils::printHeader("SEARCH USER");
        
        cout << "Enter username to search: ";
        string username;
        cin >> username;
        
        User user = dbManager->getUserByUsername(username);
        
        if (user.getId() == 0) {
            cout << "\nUser '" << username << "' not found!" << endl;
            cout << "\nAvailable users:" << endl;
            auto allUsers = dbManager->getAllUsers();
            for (const auto& u : allUsers) {
                cout << "  - " << u.getUsername() << " (" << u.roleToString() << ")" << endl;
            }
        } else {
            cout << "\nUser Found:" << endl;
            cout << string(50, '-') << endl;
            cout << "ID: " << user.getId() << endl;
            cout << "Username: " << user.getUsername() << endl;
            cout << "Full Name: " << user.getFullName() << endl;
            cout << "Email: " << user.getEmail() << endl;
            cout << "Role: " << user.roleToString() << endl;
            cout << "Status: " << user.statusToString() << endl;
            cout << "Last Login: " << (user.getLastLogin().empty() ? "Never" : user.getLastLogin()) << endl;
            cout << "Login Attempts: " << user.getLoginAttempts() << endl;
            
            // Show exam statistics for students
            if (user.getRole() == UserRole::STUDENT) {
                auto results = dbManager->getExamResultsByUser(user.getId());
                cout << "\nExam Statistics:" << endl;
                cout << "Total Exams: " << results.size() << endl;
                
                if (!results.empty()) {
                    double totalScore = 0;
                    for (const auto& result : results) {
                        totalScore += result.getPercentage();
                    }
                    cout << "Average Score: " << (totalScore / results.size()) << "%" << endl;
                }
            }
        }
        
        Utils::pauseSystem();
    }
    
    void userStatistics()
    {
        Utils::clearScreen();
        Utils::printHeader("USER STATISTICS");
        
        auto users = dbManager->getAllUsers();
        
        int adminCount = 0, studentCount = 0;
        int activeCount = 0, inactiveCount = 0;
        
        for (const auto& user : users) {
            if (user.getRole() == UserRole::ADMIN) adminCount++;
            else if (user.getRole() == UserRole::STUDENT) studentCount++;
            
            if (user.getStatus() == UserStatus::ACTIVE) activeCount++;
            else inactiveCount++;
        }
        
        cout << "User Statistics:" << endl;
        cout << string(40, '-') << endl;
        cout << "Total Users: " << users.size() << endl;
        cout << "Administrators: " << adminCount << endl;
        cout << "Students: " << studentCount << endl;
        cout << "Active Users: " << activeCount << endl;
        cout << "Inactive Users: " << inactiveCount << endl;
        
        cout << "\nExam Statistics:" << endl;
        cout << string(40, '-') << endl;
        cout << "Total Exam Results: " << dbManager->getTotalExamResults() << endl;
        
        Utils::pauseSystem();
    }
    
    void manageStudentStatus()
    {
        Utils::clearScreen();
        Utils::printHeader("MANAGE STUDENT STATUS");
        
        cout << "Enter student username: ";
        string username;
        cin >> username;
        
        User user = dbManager->getUserByUsername(username);
        if (user.getId() == 0) {
            cout << "\nUser not found!" << endl;
            Utils::pauseSystem();
            return;
        }
        
        if (user.getRole() != UserRole::STUDENT) {
            cout << "\nUser is not a student!" << endl;
            Utils::pauseSystem();
            return;
        }
        
        cout << "\nCurrent Status: " << user.statusToString() << endl;
        cout << "\nSelect new status:" << endl;
        cout << "1. Active" << endl;
        cout << "2. Inactive" << endl;
        cout << "3. Suspended" << endl;
        cout << "4. Cancel" << endl;
        
        cout << "\nEnter choice: ";
        int choice;
        cin >> choice;
        
        UserStatus newStatus;
        switch (choice) {
            case 1: newStatus = UserStatus::ACTIVE; break;
            case 2: newStatus = UserStatus::INACTIVE; break;
            case 3: newStatus = UserStatus::SUSPENDED; break;
            case 4: return;
            default:
                cout << "Invalid choice!" << endl;
                Utils::pauseSystem();
                return;
        }
        
        user.setStatus(newStatus);
        if (dbManager->updateUser(user)) {
            cout << "\nUser status updated successfully!" << endl;
        } else {
            cout << "\nFailed to update user status!" << endl;
        }
        
        Utils::pauseSystem();
    }

    void showStatistics()
    {
        Utils::clearScreen();
        Utils::printHeader("SYSTEM STATISTICS");

        cout << "Total Users: " << dbManager->getTotalUsers() << endl;
        cout << "Total Questions: " << dbManager->getTotalQuestions() << endl;
        cout << "Total Exam Results: " << dbManager->getTotalExamResults() << endl;

        // Show question distribution by subject
        auto questions = dbManager->getAllQuestions();
        map<string, int> subjectCount;
        for (const auto &q : questions)
        {
            subjectCount[q.getSubject()]++;
        }

        cout << "\nQuestions by Subject:" << endl;
        for (const auto &pair : subjectCount)
        {
            cout << "  " << pair.first << ": " << pair.second << endl;
        }

        Utils::pauseSystem();
    }

    void viewAllResults()
    {
        Utils::clearScreen();
        Utils::printHeader("ALL EXAM RESULTS");

        auto results = dbManager->getAllExamResults();
        if (results.empty())
        {
            cout << "No exam results found!" << endl;
        }
        else
        {
            cout << "Total Results: " << results.size() << endl;
            cout << string(80, '-') << endl;
            cout << "User\t\tScore\t\tPercentage\tDate" << endl;
            cout << string(80, '-') << endl;

            for (const auto &result : results)
            {
                cout << result.getUsername() << "\t\t"
                     << result.getScore() << "/" << result.getTotalQuestions() << "\t\t"
                     << result.getPercentage() << "%\t\t"
                     << result.getExamDate() << endl;
            }
        }

        Utils::pauseSystem();
    }
};

#endif // ADMIN_STUB_H