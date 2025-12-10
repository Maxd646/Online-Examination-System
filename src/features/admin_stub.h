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
            std::cout << "Welcome " << currentAdmin.getFullName() << std::endl;
            std::cout << "\nAdmin Menu:" << std::endl;
            std::cout << "1. Question Management" << std::endl;
            std::cout << "2. User Management" << std::endl;
            std::cout << "3. View System Statistics" << std::endl;
            std::cout << "4. View All Results" << std::endl;
            std::cout << "5. Logout" << std::endl;
            
            std::cout << "\nEnter your choice: ";
            int choice;
            std::cin >> choice;
            
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
                    std::cout << "\nLogging out..." << std::endl;
                    return;
                default:
                    std::cout << "Invalid choice! Please try again." << std::endl;
                    Utils::pauseSystem();
            }
        }
    }
    
private:
    void questionManagement() {
        while (true) {
            Utils::clearScreen();
            Utils::printHeader("QUESTION MANAGEMENT");
            std::cout << "1. Add New Question" << std::endl;
            std::cout << "2. View All Questions" << std::endl;
            std::cout << "3. Search Questions" << std::endl;
            std::cout << "4. Update Question" << std::endl;
            std::cout << "5. Delete Question" << std::endl;
            std::cout << "6. Back to Main Menu" << std::endl;
            
            std::cout << "\nEnter your choice: ";
            int choice;
            std::cin >> choice;
            
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
                    std::cout << "Invalid choice!" << std::endl;
                    Utils::pauseSystem();
            }
        }
    }
    
    void addQuestion() {
        Utils::clearScreen();
        Utils::printHeader("ADD NEW QUESTION");
        
        std::string subject, questionText, option1, option2, option3, option4, difficulty, explanation;
        int correctAnswer;
        
        std::cout << "Subject: ";
        std::cin.ignore();
        std::getline(std::cin, subject);
        
        std::cout << "Question Text: ";
        std::getline(std::cin, questionText);
        
        std::cout << "Option 1: ";
        std::getline(std::cin, option1);
        
        std::cout << "Option 2: ";
        std::getline(std::cin, option2);
        
        std::cout << "Option 3: ";
        std::getline(std::cin, option3);
        
        std::cout << "Option 4: ";
        std::getline(std::cin, option4);
        
        std::cout << "Correct Answer (1-4): ";
        std::cin >> correctAnswer;
        correctAnswer--; // Convert to 0-based index
        
        std::cout << "Difficulty (Easy/Medium/Hard): ";
        std::cin >> difficulty;
        
        std::cout << "Explanation (optional): ";
        std::cin.ignore();
        std::getline(std::cin, explanation);
        
        // Create question object
        std::vector<std::string> options = {option1, option2, option3, option4};
        Question newQuestion(0, subject, questionText, options, correctAnswer, difficulty, explanation);
        
        if (dbManager->insertQuestion(newQuestion)) {
            std::cout << "\n✓ Question added successfully!" << std::endl;
        } else {
            std::cout << "\n✗ Failed to add question!" << std::endl;
        }
        
        Utils::pauseSystem();
    }
    
    void viewAllQuestions() {
        Utils::clearScreen();
        Utils::printHeader("ALL QUESTIONS");
        
        auto questions = dbManager->getAllQuestions();
        if (questions.empty()) {
            std::cout << "No questions found!" << std::endl;
        } else {
            for (size_t i = 0; i < questions.size(); ++i) {
                std::cout << "\n--- Question " << (i + 1) << " ---" << std::endl;
                std::cout << "ID: " << questions[i].getId() << std::endl;
                std::cout << "Subject: " << questions[i].getSubject() << std::endl;
                std::cout << "Difficulty: " << questions[i].getDifficulty() << std::endl;
                questions[i].displayWithAnswer();
                std::cout << std::string(50, '-') << std::endl;
            }
        }
        
        Utils::pauseSystem();
    }
    
    void searchQuestions() {
        Utils::clearScreen();
        Utils::printHeader("SEARCH QUESTIONS");
        
        std::cout << "1. Search by Subject" << std::endl;
        std::cout << "2. Search by Difficulty" << std::endl;
        std::cout << "3. Search by Keyword" << std::endl;
        
        std::cout << "\nEnter choice: ";
        int choice;
        std::cin >> choice;
        
        std::vector<Question> results;
        
        switch (choice) {
            case 1: {
                std::cout << "Enter subject: ";
                std::string subject;
                std::cin.ignore();
                std::getline(std::cin, subject);
                results = dbManager->getQuestionsBySubject(subject);
                break;
            }
            case 2: {
                std::cout << "Enter difficulty: ";
                std::string difficulty;
                std::cin >> difficulty;
                results = dbManager->getQuestionsByDifficulty(difficulty);
                break;
            }
            case 3: {
                std::cout << "Enter keyword: ";
                std::string keyword;
                std::cin.ignore();
                std::getline(std::cin, keyword);
                results = dbManager->searchQuestions(keyword);
                break;
            }
            default:
                std::cout << "Invalid choice!" << std::endl;
                Utils::pauseSystem();
                return;
        }
        
        std::cout << "\nSearch Results (" << results.size() << " found):" << std::endl;
        for (size_t i = 0; i < results.size(); ++i) {
            std::cout << "\n--- Result " << (i + 1) << " ---" << std::endl;
            std::cout << "ID: " << results[i].getId() << std::endl;
            results[i].display();
        }
        
        Utils::pauseSystem();
    }
    
    void updateQuestion() {
        Utils::clearScreen();
        Utils::printHeader("UPDATE QUESTION");
        
        std::cout << "Enter Question ID to update: ";
        int questionId;
        std::cin >> questionId;
        
        Question question = dbManager->getQuestionById(questionId);
        if (question.getId() == 0) {
            std::cout << "Question not found!" << std::endl;
            Utils::pauseSystem();
            return;
        }
        
        std::cout << "\nCurrent Question:" << std::endl;
        question.displayWithAnswer();
        
        std::cout << "\nEnter new values (press Enter to keep current):" << std::endl;
        
        std::string input;
        std::cin.ignore();
        
        std::cout << "Subject [" << question.getSubject() << "]: ";
        std::getline(std::cin, input);
        if (!input.empty()) question.setSubject(input);
        
        std::cout << "Question Text [" << question.getQuestionText() << "]: ";
        std::getline(std::cin, input);
        if (!input.empty()) question.setQuestionText(input);
        
        std::cout << "Difficulty [" << question.getDifficulty() << "]: ";
        std::getline(std::cin, input);
        if (!input.empty()) question.setDifficulty(input);
        
        if (dbManager->updateQuestion(question)) {
            std::cout << "\n✓ Question updated successfully!" << std::endl;
        } else {
            std::cout << "\n✗ Failed to update question!" << std::endl;
        }
        
        Utils::pauseSystem();
    }
    
    void deleteQuestion() {
        Utils::clearScreen();
        Utils::printHeader("DELETE QUESTION");
        
        std::cout << "Enter Question ID to delete: ";
        int questionId;
        std::cin >> questionId;
        
        Question question = dbManager->getQuestionById(questionId);
        if (question.getId() == 0) {
            std::cout << "Question not found!" << std::endl;
            Utils::pauseSystem();
            return;
        }
        
        std::cout << "\nQuestion to delete:" << std::endl;
        question.display();
        
        std::cout << "\nAre you sure you want to delete this question? (y/N): ";
        char confirm;
        std::cin >> confirm;
        
        if (confirm == 'y' || confirm == 'Y') {
            if (dbManager->deleteQuestion(questionId)) {
                std::cout << "\n✓ Question deleted successfully!" << std::endl;
            } else {
                std::cout << "\n✗ Failed to delete question!" << std::endl;
            }
        } else {
            std::cout << "\nDeletion cancelled." << std::endl;
        }
        
        Utils::pauseSystem();
    }
    
    void userManagement() {
        Utils::clearScreen();
        Utils::printHeader("USER MANAGEMENT");
        
        auto users = dbManager->getAllUsers();
        std::cout << "Total Users: " << users.size() << std::endl;
        std::cout << "\nUser List:" << std::endl;
        std::cout << std::string(80, '-') << std::endl;
        std::cout << "ID\tUsername\t\tRole\t\tStatus" << std::endl;
        std::cout << std::string(80, '-') << std::endl;
        
        for (const auto& user : users) {
            std::cout << user.getId() << "\t" 
                     << user.getUsername() << "\t\t"
                     << user.roleToString() << "\t\t"
                     << user.statusToString() << std::endl;
        }
        
        Utils::pauseSystem();
    }
    
    void showStatistics() {
        Utils::clearScreen();
        Utils::printHeader("SYSTEM STATISTICS");
        
        std::cout << "Total Users: " << dbManager->getTotalUsers() << std::endl;
        std::cout << "Total Questions: " << dbManager->getTotalQuestions() << std::endl;
        std::cout << "Total Exam Results: " << dbManager->getTotalExamResults() << std::endl;
        
        // Show question distribution by subject
        auto questions = dbManager->getAllQuestions();
        std::map<std::string, int> subjectCount;
        for (const auto& q : questions) {
            subjectCount[q.getSubject()]++;
        }
        
        std::cout << "\nQuestions by Subject:" << std::endl;
        for (const auto& pair : subjectCount) {
            std::cout << "  " << pair.first << ": " << pair.second << std::endl;
        }
        
        Utils::pauseSystem();
    }
    
    void viewAllResults() {
        Utils::clearScreen();
        Utils::printHeader("ALL EXAM RESULTS");
        
        auto results = dbManager->getAllExamResults();
        if (results.empty()) {
            std::cout << "No exam results found!" << std::endl;
        } else {
            std::cout << "Total Results: " << results.size() << std::endl;
            std::cout << std::string(80, '-') << std::endl;
            std::cout << "User\t\tScore\t\tPercentage\tDate" << std::endl;
            std::cout << std::string(80, '-') << std::endl;
            
            for (const auto& result : results) {
                std::cout << result.getUsername() << "\t\t"
                         << result.getScore() << "/" << result.getTotalQuestions() << "\t\t"
                         << result.getPercentage() << "%\t\t"
                         << result.getExamDate() << std::endl;
            }
        }
        
        Utils::pauseSystem();
    }
};

#endif // ADMIN_STUB_H