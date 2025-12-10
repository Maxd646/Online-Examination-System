#ifndef STUDENT_STUB_H
#define STUDENT_STUB_H

#include "../authentication/user.h"
#include "../database/database.h"
#include "../structure/utils.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <chrono>
#include <map>
#include <random>

// Enhanced StudentPanel with comprehensive exam functionality
class StudentPanel {
private:
    DatabaseManager* dbManager;
    User currentStudent;
    
public:
    StudentPanel(DatabaseManager* db, const User& student) 
        : dbManager(db), currentStudent(student) {}
    
    void run() {
        while (true) {
            Utils::clearScreen();
            Utils::printHeader("STUDENT PANEL");
            std::cout << "Welcome " << currentStudent.getFullName() << std::endl;
            std::cout << "\nStudent Menu:" << std::endl;
            std::cout << "1. Take Exam" << std::endl;
            std::cout << "2. Practice Mode" << std::endl;
            std::cout << "3. View My Results" << std::endl;
            std::cout << "4. View Profile" << std::endl;
            std::cout << "5. Performance Analytics" << std::endl;
            std::cout << "6. Logout" << std::endl;
            
            std::cout << "\nEnter your choice: ";
            int choice;
            std::cin >> choice;
            
            switch (choice) {
                case 1:
                    takeExam();
                    break;
                case 2:
                    practiceMode();
                    break;
                case 3:
                    viewMyResults();
                    break;
                case 4:
                    viewProfile();
                    break;
                case 5:
                    performanceAnalytics();
                    break;
                case 6:
                    std::cout << "\nLogging out..." << std::endl;
                    return;
                default:
                    std::cout << "Invalid choice! Please try again." << std::endl;
                    Utils::pauseSystem();
            }
        }
    }
    
private:
    void takeExam() {
        Utils::clearScreen();
        Utils::printHeader("TAKE EXAM");
        
        // Exam configuration
        std::cout << "Configure your exam:" << std::endl;
        
        std::cout << "1. Select Subject:" << std::endl;
        std::cout << "   1. Mathematics" << std::endl;
        std::cout << "   2. Computer Science" << std::endl;
        std::cout << "   3. Science" << std::endl;
        std::cout << "   4. General Knowledge" << std::endl;
        std::cout << "   5. Mixed (All subjects)" << std::endl;
        
        std::cout << "\nEnter choice (1-5): ";
        int subjectChoice;
        std::cin >> subjectChoice;
        
        std::string subject = "";
        switch (subjectChoice) {
            case 1: subject = "Mathematics"; break;
            case 2: subject = "Computer Science"; break;
            case 3: subject = "Science"; break;
            case 4: subject = "General Knowledge"; break;
            case 5: subject = ""; break; // Mixed
            default: 
                std::cout << "Invalid choice!" << std::endl;
                Utils::pauseSystem();
                return;
        }
        
        std::cout << "\nNumber of questions (5-20): ";
        int questionCount;
        std::cin >> questionCount;
        if (questionCount < 5) questionCount = 5;
        if (questionCount > 20) questionCount = 20;
        
        std::cout << "Time limit in minutes (0 for no limit): ";
        int timeLimit;
        std::cin >> timeLimit;
        
        // Get questions
        std::vector<Question> questions;
        if (subject.empty()) {
            questions = dbManager->getRandomQuestions(questionCount);
        } else {
            questions = dbManager->getRandomQuestions(questionCount, subject);
        }
        
        if (questions.empty()) {
            std::cout << "\nNo questions available for the selected criteria!" << std::endl;
            Utils::pauseSystem();
            return;
        }
        
        // Limit to requested count
        if (questions.size() > static_cast<size_t>(questionCount)) {
            questions.resize(questionCount);
        }
        
        // Start exam
        conductExam(questions, timeLimit, subject);
    }
    
    void conductExam(const std::vector<Question>& questions, int timeLimit, const std::string& subject) {
        Utils::clearScreen();
        Utils::printHeader("EXAM IN PROGRESS");
        
        std::vector<int> userAnswers(questions.size(), -1);
        std::vector<bool> answered(questions.size(), false);
        auto startTime = std::chrono::steady_clock::now();
        
        std::cout << "Exam Started!" << std::endl;
        std::cout << "Questions: " << questions.size() << std::endl;
        if (timeLimit > 0) {
            std::cout << "Time Limit: " << timeLimit << " minutes" << std::endl;
        }
        std::cout << "Instructions:" << std::endl;
        std::cout << "- Enter 1-4 for your answer" << std::endl;
        std::cout << "- Enter 0 to skip question" << std::endl;
        std::cout << "- Enter -1 to go back to previous question" << std::endl;
        std::cout << "- Enter -2 to submit exam" << std::endl;
        std::cout << "\nPress Enter to start...";
        std::cin.ignore();
        std::cin.get();
        
        int currentQuestion = 0;
        bool examCompleted = false;
        
        while (!examCompleted && currentQuestion < static_cast<int>(questions.size())) {
            // Check time limit
            if (timeLimit > 0) {
                auto currentTime = std::chrono::steady_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::minutes>(currentTime - startTime);
                if (elapsed.count() >= timeLimit) {
                    std::cout << "\n⏰ Time's up! Exam will be submitted automatically." << std::endl;
                    break;
                }
            }
            
            Utils::clearScreen();
            
            // Show progress
            int answeredCount = 0;
            for (bool ans : answered) {
                if (ans) answeredCount++;
            }
            
            std::cout << "Question " << (currentQuestion + 1) << " of " << questions.size() 
                     << " | Answered: " << answeredCount << "/" << questions.size() << std::endl;
            
            if (timeLimit > 0) {
                auto currentTime = std::chrono::steady_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::minutes>(currentTime - startTime);
                int remaining = timeLimit - elapsed.count();
                std::cout << "Time Remaining: " << remaining << " minutes" << std::endl;
            }
            
            std::cout << std::string(60, '=') << std::endl;
            
            // Display question
            questions[currentQuestion].display();
            
            if (answered[currentQuestion]) {
                std::cout << "\nCurrent Answer: " << (userAnswers[currentQuestion] + 1) << std::endl;
            }
            
            std::cout << "\nYour answer (1-4, 0=skip, -1=previous, -2=submit): ";
            int answer;
            std::cin >> answer;
            
            if (answer == -2) {
                // Submit exam
                std::cout << "\nAre you sure you want to submit? (y/N): ";
                char confirm;
                std::cin >> confirm;
                if (confirm == 'y' || confirm == 'Y') {
                    examCompleted = true;
                }
            } else if (answer == -1) {
                // Previous question
                if (currentQuestion > 0) {
                    currentQuestion--;
                }
            } else if (answer == 0) {
                // Skip question
                currentQuestion++;
            } else if (answer >= 1 && answer <= 4) {
                // Answer question
                userAnswers[currentQuestion] = answer - 1;
                answered[currentQuestion] = true;
                currentQuestion++;
            } else {
                std::cout << "Invalid input! Please try again." << std::endl;
                Utils::pauseSystem();
            }
        }
        
        // Calculate results
        auto endTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::minutes>(endTime - startTime);
        
        int score = 0;
        for (size_t i = 0; i < questions.size(); ++i) {
            if (answered[i] && userAnswers[i] == questions[i].getCorrectAnswer()) {
                score++;
            }
        }
        
        double percentage = (score * 100.0) / questions.size();
        
        // Save result to database
        ExamResult result(currentStudent.getId(), currentStudent.getUsername(), 
                         score, questions.size(), subject);
        result.setDuration(duration.count());
        dbManager->insertExamResult(result);
        
        // Display results
        showExamResults(questions, userAnswers, answered, score, percentage, duration.count());
    }
    
    void showExamResults(const std::vector<Question>& questions, 
                        const std::vector<int>& userAnswers,
                        const std::vector<bool>& answered,
                        int score, double percentage, int duration) {
        Utils::clearScreen();
        Utils::printHeader("EXAM RESULTS");
        
        std::cout << "Exam Completed!" << std::endl;
        std::cout << std::string(50, '=') << std::endl;
        std::cout << "Score: " << score << "/" << questions.size() << std::endl;
        std::cout << "Percentage: " << percentage << "%" << std::endl;
        std::cout << "Duration: " << duration << " minutes" << std::endl;
        std::cout << "Grade: " << getGrade(percentage) << std::endl;
        std::cout << "Status: " << (percentage >= 60 ? "PASSED" : "FAILED") << std::endl;
        std::cout << std::string(50, '=') << std::endl;
        
        std::cout << "\nDetailed Review:" << std::endl;
        for (size_t i = 0; i < questions.size(); ++i) {
            std::cout << "\nQ" << (i + 1) << ": " << questions[i].getQuestionText() << std::endl;
            
            if (!answered[i]) {
                std::cout << "   Your Answer: Not answered" << std::endl;
                std::cout << "   Status: ❌ Incorrect" << std::endl;
            } else {
                std::cout << "   Your Answer: " << (userAnswers[i] + 1) << std::endl;
                if (userAnswers[i] == questions[i].getCorrectAnswer()) {
                    std::cout << "   Status: ✅ Correct" << std::endl;
                } else {
                    std::cout << "   Status: ❌ Incorrect" << std::endl;
                }
            }
            std::cout << "   Correct Answer: " << (questions[i].getCorrectAnswer() + 1) << std::endl;
            
            if (!questions[i].getExplanation().empty()) {
                std::cout << "   Explanation: " << questions[i].getExplanation() << std::endl;
            }
        }
        
        Utils::pauseSystem();
    }
    
    void practiceMode() {
        Utils::clearScreen();
        Utils::printHeader("PRACTICE MODE");
        
        std::cout << "Practice Mode Options:" << std::endl;
        std::cout << "1. Quick Practice (5 questions)" << std::endl;
        std::cout << "2. Subject-wise Practice" << std::endl;
        std::cout << "3. Difficulty-based Practice" << std::endl;
        std::cout << "4. Review Mistakes" << std::endl;
        std::cout << "5. Back to Main Menu" << std::endl;
        
        std::cout << "\nEnter choice: ";
        int choice;
        std::cin >> choice;
        
        switch (choice) {
            case 1:
                quickPractice();
                break;
            case 2:
                subjectPractice();
                break;
            case 3:
                difficultyPractice();
                break;
            case 4:
                reviewMistakes();
                break;
            case 5:
                return;
            default:
                std::cout << "Invalid choice!" << std::endl;
                Utils::pauseSystem();
        }
    }
    
    void quickPractice() {
        auto questions = dbManager->getRandomQuestions(5);
        if (questions.empty()) {
            std::cout << "No questions available!" << std::endl;
            Utils::pauseSystem();
            return;
        }
        
        Utils::clearScreen();
        Utils::printHeader("QUICK PRACTICE");
        
        int score = 0;
        for (size_t i = 0; i < questions.size(); ++i) {
            std::cout << "\nQuestion " << (i + 1) << " of " << questions.size() << std::endl;
            std::cout << std::string(40, '-') << std::endl;
            questions[i].display();
            
            std::cout << "\nYour answer (1-4): ";
            int answer;
            std::cin >> answer;
            
            if (answer >= 1 && answer <= 4 && (answer - 1) == questions[i].getCorrectAnswer()) {
                std::cout << "✅ Correct!" << std::endl;
                score++;
            } else {
                std::cout << "❌ Wrong! Correct answer: " << (questions[i].getCorrectAnswer() + 1) << std::endl;
                if (!questions[i].getExplanation().empty()) {
                    std::cout << "Explanation: " << questions[i].getExplanation() << std::endl;
                }
            }
            
            if (i < questions.size() - 1) {
                std::cout << "\nPress Enter for next question...";
                std::cin.ignore();
                std::cin.get();
            }
        }
        
        std::cout << "\n" << std::string(40, '=') << std::endl;
        std::cout << "Practice Complete!" << std::endl;
        std::cout << "Score: " << score << "/" << questions.size() << std::endl;
        std::cout << "Accuracy: " << (score * 100.0 / questions.size()) << "%" << std::endl;
        
        Utils::pauseSystem();
    }
    
    void subjectPractice() {
        Utils::clearScreen();
        Utils::printHeader("SUBJECT-WISE PRACTICE");
        
        std::cout << "Select Subject:" << std::endl;
        std::cout << "1. Mathematics" << std::endl;
        std::cout << "2. Computer Science" << std::endl;
        std::cout << "3. Science" << std::endl;
        std::cout << "4. General Knowledge" << std::endl;
        
        std::cout << "\nEnter choice: ";
        int choice;
        std::cin >> choice;
        
        std::string subject;
        switch (choice) {
            case 1: subject = "Mathematics"; break;
            case 2: subject = "Computer Science"; break;
            case 3: subject = "Science"; break;
            case 4: subject = "General Knowledge"; break;
            default:
                std::cout << "Invalid choice!" << std::endl;
                Utils::pauseSystem();
                return;
        }
        
        auto questions = dbManager->getQuestionsBySubject(subject);
        if (questions.empty()) {
            std::cout << "No questions available for " << subject << "!" << std::endl;
            Utils::pauseSystem();
            return;
        }
        
        // Shuffle and limit to 10 questions
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(questions.begin(), questions.end(), g);
        if (questions.size() > 10) {
            questions.resize(10);
        }
        
        conductPracticeSession(questions, subject + " Practice");
    }
    
    void difficultyPractice() {
        Utils::clearScreen();
        Utils::printHeader("DIFFICULTY-BASED PRACTICE");
        
        std::cout << "Select Difficulty:" << std::endl;
        std::cout << "1. Easy" << std::endl;
        std::cout << "2. Medium" << std::endl;
        std::cout << "3. Hard" << std::endl;
        
        std::cout << "\nEnter choice: ";
        int choice;
        std::cin >> choice;
        
        std::string difficulty;
        switch (choice) {
            case 1: difficulty = "Easy"; break;
            case 2: difficulty = "Medium"; break;
            case 3: difficulty = "Hard"; break;
            default:
                std::cout << "Invalid choice!" << std::endl;
                Utils::pauseSystem();
                return;
        }
        
        auto questions = dbManager->getQuestionsByDifficulty(difficulty);
        if (questions.empty()) {
            std::cout << "No " << difficulty << " questions available!" << std::endl;
            Utils::pauseSystem();
            return;
        }
        
        // Shuffle and limit to 10 questions
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(questions.begin(), questions.end(), g);
        if (questions.size() > 10) {
            questions.resize(10);
        }
        
        conductPracticeSession(questions, difficulty + " Practice");
    }
    
    void conductPracticeSession(const std::vector<Question>& questions, const std::string& sessionName) {
        Utils::clearScreen();
        Utils::printHeader(sessionName);
        
        int score = 0;
        std::vector<bool> correct(questions.size(), false);
        
        for (size_t i = 0; i < questions.size(); ++i) {
            std::cout << "\nQuestion " << (i + 1) << " of " << questions.size() << std::endl;
            std::cout << std::string(50, '-') << std::endl;
            questions[i].display();
            
            std::cout << "\nYour answer (1-4): ";
            int answer;
            std::cin >> answer;
            
            if (answer >= 1 && answer <= 4 && (answer - 1) == questions[i].getCorrectAnswer()) {
                std::cout << "✅ Correct!" << std::endl;
                score++;
                correct[i] = true;
            } else {
                std::cout << "❌ Wrong! Correct answer: " << (questions[i].getCorrectAnswer() + 1) << std::endl;
                if (!questions[i].getExplanation().empty()) {
                    std::cout << "Explanation: " << questions[i].getExplanation() << std::endl;
                }
            }
            
            if (i < questions.size() - 1) {
                std::cout << "\nPress Enter for next question...";
                std::cin.ignore();
                std::cin.get();
            }
        }
        
        // Show summary
        std::cout << "\n" << std::string(50, '=') << std::endl;
        std::cout << sessionName << " Complete!" << std::endl;
        std::cout << "Score: " << score << "/" << questions.size() << std::endl;
        std::cout << "Accuracy: " << (score * 100.0 / questions.size()) << "%" << std::endl;
        
        // Show incorrect questions for review
        bool hasIncorrect = false;
        for (size_t i = 0; i < questions.size(); ++i) {
            if (!correct[i]) {
                if (!hasIncorrect) {
                    std::cout << "\nQuestions to review:" << std::endl;
                    hasIncorrect = true;
                }
                std::cout << "Q" << (i + 1) << ": " << questions[i].getSubject() 
                         << " - " << questions[i].getDifficulty() << std::endl;
            }
        }
        
        Utils::pauseSystem();
    }
    
    void reviewMistakes() {
        Utils::clearScreen();
        Utils::printHeader("REVIEW MISTAKES");
        
        std::cout << "This feature will show questions you got wrong in previous exams." << std::endl;
        std::cout << "Feature coming soon!" << std::endl;
        
        Utils::pauseSystem();
    }
    
    void viewMyResults() {
        Utils::clearScreen();
        Utils::printHeader("MY EXAM RESULTS");
        
        auto results = dbManager->getExamResultsByUser(currentStudent.getId());
        if (results.empty()) {
            std::cout << "No exam results found!" << std::endl;
            std::cout << "Take an exam to see your results here." << std::endl;
        } else {
            std::cout << "Total Exams Taken: " << results.size() << std::endl;
            std::cout << std::string(80, '-') << std::endl;
            std::cout << "Date\t\tSubject\t\tScore\t\tPercentage\tGrade" << std::endl;
            std::cout << std::string(80, '-') << std::endl;
            
            double totalPercentage = 0;
            for (const auto& result : results) {
                std::cout << result.getExamDate() << "\t"
                         << (result.getSubject().empty() ? "Mixed" : result.getSubject()) << "\t\t"
                         << result.getScore() << "/" << result.getTotalQuestions() << "\t\t"
                         << result.getPercentage() << "%\t\t"
                         << getGrade(result.getPercentage()) << std::endl;
                totalPercentage += result.getPercentage();
            }
            
            std::cout << std::string(80, '-') << std::endl;
            std::cout << "Average Score: " << (totalPercentage / results.size()) << "%" << std::endl;
        }
        
        Utils::pauseSystem();
    }
    
    void viewProfile() {
        Utils::clearScreen();
        Utils::printHeader("MY PROFILE");
        
        std::cout << "Username: " << currentStudent.getUsername() << std::endl;
        std::cout << "Full Name: " << currentStudent.getFullName() << std::endl;
        std::cout << "Email: " << currentStudent.getEmail() << std::endl;
        std::cout << "Role: " << currentStudent.roleToString() << std::endl;
        std::cout << "Status: " << currentStudent.statusToString() << std::endl;
        
        // Show exam statistics
        auto results = dbManager->getExamResultsByUser(currentStudent.getId());
        std::cout << "\nExam Statistics:" << std::endl;
        std::cout << "Total Exams: " << results.size() << std::endl;
        
        if (!results.empty()) {
            double totalPercentage = 0;
            int passedExams = 0;
            
            for (const auto& result : results) {
                totalPercentage += result.getPercentage();
                if (result.getPercentage() >= 60) passedExams++;
            }
            
            std::cout << "Average Score: " << (totalPercentage / results.size()) << "%" << std::endl;
            std::cout << "Passed Exams: " << passedExams << "/" << results.size() << std::endl;
            std::cout << "Pass Rate: " << (passedExams * 100.0 / results.size()) << "%" << std::endl;
        }
        
        Utils::pauseSystem();
    }
    
    void performanceAnalytics() {
        Utils::clearScreen();
        Utils::printHeader("PERFORMANCE ANALYTICS");
        
        auto results = dbManager->getExamResultsByUser(currentStudent.getId());
        if (results.empty()) {
            std::cout << "No exam data available for analysis." << std::endl;
            std::cout << "Take some exams to see your performance analytics!" << std::endl;
            Utils::pauseSystem();
            return;
        }
        
        // Calculate statistics
        std::map<std::string, std::vector<double>> subjectScores;
        double totalScore = 0;
        int passedExams = 0;
        
        for (const auto& result : results) {
            std::string subject = result.getSubject().empty() ? "Mixed" : result.getSubject();
            subjectScores[subject].push_back(result.getPercentage());
            totalScore += result.getPercentage();
            if (result.getPercentage() >= 60) passedExams++;
        }
        
        std::cout << "Overall Performance:" << std::endl;
        std::cout << "Total Exams: " << results.size() << std::endl;
        std::cout << "Average Score: " << (totalScore / results.size()) << "%" << std::endl;
        std::cout << "Pass Rate: " << (passedExams * 100.0 / results.size()) << "%" << std::endl;
        
        std::cout << "\nSubject-wise Performance:" << std::endl;
        std::cout << std::string(50, '-') << std::endl;
        for (const auto& pair : subjectScores) {
            double subjectTotal = 0;
            for (double score : pair.second) {
                subjectTotal += score;
            }
            double average = subjectTotal / pair.second.size();
            std::cout << pair.first << ": " << average << "% (" 
                     << pair.second.size() << " exams)" << std::endl;
        }
        
        // Find strengths and weaknesses
        std::string strongestSubject, weakestSubject;
        double highestAvg = 0, lowestAvg = 100;
        
        for (const auto& pair : subjectScores) {
            if (pair.second.size() >= 2) { // Only consider subjects with 2+ exams
                double subjectTotal = 0;
                for (double score : pair.second) {
                    subjectTotal += score;
                }
                double average = subjectTotal / pair.second.size();
                
                if (average > highestAvg) {
                    highestAvg = average;
                    strongestSubject = pair.first;
                }
                if (average < lowestAvg) {
                    lowestAvg = average;
                    weakestSubject = pair.first;
                }
            }
        }
        
        if (!strongestSubject.empty()) {
            std::cout << "\nStrengths & Weaknesses:" << std::endl;
            std::cout << "Strongest Subject: " << strongestSubject << " (" << highestAvg << "%)" << std::endl;
            if (!weakestSubject.empty() && weakestSubject != strongestSubject) {
                std::cout << "Needs Improvement: " << weakestSubject << " (" << lowestAvg << "%)" << std::endl;
            }
        }
        
        Utils::pauseSystem();
    }
    
    std::string getGrade(double percentage) {
        if (percentage >= 90) return "A+";
        else if (percentage >= 80) return "A";
        else if (percentage >= 70) return "B";
        else if (percentage >= 60) return "C";
        else if (percentage >= 50) return "D";
        else return "F";
    }
};

#endif // STUDENT_STUB_H