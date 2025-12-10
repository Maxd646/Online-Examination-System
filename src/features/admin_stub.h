#ifndef ADMIN_STUB_H
#define ADMIN_STUB_H

#include "../authentication/user.h"
#include "../database/database.h"
#include "../structure/utils.h"
#include <iostream>
#include <vector>
#include <string>
#include <map>

// Enhanced AdminPanel with question management functionality
class AdminPanel {
private:
    DatabaseManager* dbManager;
    User currentAdmin;
    
public:
    AdminPanel(DatabaseManager* db, const User& admin) 
        : dbManager(db), currentAdmin(admin) {}
    
    void run() {
        while (true) {
            Utils::clearScreen();
            Utils::printHeader("ADMIN PANEL");
            cout << "Welcome " << currentAdmin.getFullName() << endl;
            cout << "\nAdmin Menu:" << endl;
            cout << "1. Question Management" << endl;
            cout << "2. User Management" << endl;
            cout << "3. View System Statistics" << endl;
            cout << "4. View All Results" << endl;
            cout << "5. Logout" << endl;
            
            cout << "\nEnter your choice: ";
            int choice;
            cin >> choice;
            
            switch (choice) {
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
    void questionManagement() {
        while (true) {
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
            
            switch (choice) {
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
    
    void addQuestion() {
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
        
        // Create question object
        vector<string> options = {option1, option2, option3, option4};
        Question newQuestion(0, subject, questionText, options, correctAnswer, difficulty, explanation);
        
        if (dbManager->insertQuestion(newQuestion)) {
            cout << "\n✓ Question added successfully!" << endl;
        } else {
            cout << "\n✗ Failed to add question!" << endl;
        }
        
        Utils::pauseSystem();
    }
    
    void viewAllQuestions() {
        Utils::clearScreen();
        Utils::printHeader("ALL QUESTIONS");
        
        auto questions = dbManager->getAllQuestions();
        if (questions.empty()) {
            cout << "No questions found!" << endl;
        } else {
            for (size_t i = 0; i < questions.size(); ++i) {
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
    
    void searchQuestions() {
        Utils::clearScreen();
        Utils::printHeader("SEARCH QUESTIONS");
        
        cout << "1. Search by Subject" << endl;
        cout << "2. Search by Difficulty" << endl;
        cout << "3. Search by Keyword" << endl;
        
        cout << "\nEnter choice: ";
        int choice;
        cin >> choice;
        
        vector<Question> results;
        
        switch (choice) {
            case 1: {
                cout << "Enter subject: ";
                string subject;
                cin.ignore();
                getline(cin, subject);
                results = dbManager->getQuestionsBySubject(subject);
                break;
            }
            case 2: {
                cout << "Enter difficulty: ";
                string difficulty;
                cin >> difficulty;
                results = dbManager->getQuestionsByDifficulty(difficulty);
                break;
            }
            case 3: {
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
        for (size_t i = 0; i < results.size(); ++i) {
            cout << "\n--- Result " << (i + 1) << " ---" << endl;
            cout << "ID: " << results[i].getId() << endl;
            results[i].display();
        }
        
        Utils::pauseSystem();
    }
    
    void updateQuestion() {
        Utils::clearScreen();
        Utils::printHeader("UPDATE QUESTION");
        
        cout << "Enter Question ID to update: ";
        int questionId;
        cin >> questionId;
        
        Question question = dbManager->getQuestionById(questionId);
        if (question.getId() == 0) {
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
        if (!input.empty()) question.setSubject(input);
        
        cout << "Question Text [" << question.getQuestionText() << "]: ";
        getline(cin, input);
        if (!input.empty()) question.setQuestionText(input);
        
        cout << "Difficulty [" << question.getDifficulty() << "]: ";
        getline(cin, input);
        if (!input.empty()) question.setDifficulty(input);
        
        if (dbManager->updateQuestion(question)) {
            cout << "\n✓ Question updated successfully!" << endl;
        } else {
            cout << "\n✗ Failed to update question!" << endl;
        }
        
        Utils::pauseSystem();
    }
    
    void deleteQuestion() {
        Utils::clearScreen();
        Utils::printHeader("DELETE QUESTION");
        
        cout << "Enter Question ID to delete: ";
        int questionId;
        cin >> questionId;
        
        Question question = dbManager->getQuestionById(questionId);
        if (question.getId() == 0) {
            cout << "Question not found!" << endl;
            Utils::pauseSystem();
            return;
        }
        
        cout << "\nQuestion to delete:" << endl;
        question.display();
        
        cout << "\nAre you sure you want to delete this question? (y/N): ";
        char confirm;
        cin >> confirm;
        
        if (confirm == 'y' || confirm == 'Y') {
            if (dbManager->deleteQuestion(questionId)) {
                cout << "\n✓ Question deleted successfully!" << endl;
            } else {
                cout << "\n✗ Failed to delete question!" << endl;
            }
        } else {
            cout << "\nDeletion cancelled." << endl;
        }
        
        Utils::pauseSystem();
    }
    
    void userManagement() {
        Utils::clearScreen();
        Utils::printHeader("USER MANAGEMENT");
        
        auto users = dbManager->getAllUsers();
        cout << "Total Users: " << users.size() << endl;
        cout << "\nUser List:" << endl;
        cout << string(80, '-') << endl;
        cout << "ID\tUsername\t\tRole\t\tStatus" << endl;
        cout << string(80, '-') << endl;
        
        for (const auto& user : users) {
            cout << user.getId() << "\t" 
                     << user.getUsername() << "\t\t"
                     << user.roleToString() << "\t\t"
                     << user.statusToString() << endl;
        }
        
        Utils::pauseSystem();
    }
    
    void showStatistics() {
        Utils::clearScreen();
        Utils::printHeader("SYSTEM STATISTICS");
        
        cout << "Total Users: " << dbManager->getTotalUsers() << endl;
        cout << "Total Questions: " << dbManager->getTotalQuestions() << endl;
        cout << "Total Exam Results: " << dbManager->getTotalExamResults() << endl;
        
        // Show question distribution by subject
        auto questions = dbManager->getAllQuestions();
        map<string, int> subjectCount;
        for (const auto& q : questions) {
            subjectCount[q.getSubject()]++;
        }
        
        cout << "\nQuestions by Subject:" << endl;
        for (const auto& pair : subjectCount) {
            cout << "  " << pair.first << ": " << pair.second << endl;
        }
        
        Utils::pauseSystem();
    }
    
    void viewAllResults() {
        Utils::clearScreen();
        Utils::printHeader("ALL EXAM RESULTS");
        
        auto results = dbManager->getAllExamResults();
        if (results.empty()) {
            cout << "No exam results found!" << endl;
        } else {
            cout << "Total Results: " << results.size() << endl;
            cout << string(80, '-') << endl;
            cout << "User\t\tScore\t\tPercentage\tDate" << endl;
            cout << string(80, '-') << endl;
            
            for (const auto& result : results) {
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