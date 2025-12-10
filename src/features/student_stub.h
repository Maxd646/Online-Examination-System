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
            cout << "Welcome " << currentStudent.getFullName() << endl;
            cout << "\nStudent Menu:" << endl;
            cout << "1. Take Exam" << endl;
            cout << "2. Practice Mode" << endl;
            cout << "3. View My Results" << endl;
            cout << "4. View Profile" << endl;
            cout << "5. Performance Analytics" << endl;
            cout << "6. Logout" << endl;
            
            cout << "\nEnter your choice: ";
            int choice;
            cin >> choice;
            
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
                    cout << "\nLogging out..." << endl;
                    return;
                default:
                    cout << "Invalid choice! Please try again." << endl;
                    Utils::pauseSystem();
            }
        }
    }
    
private:
    void takeExam() {
        Utils::clearScreen();
        Utils::printHeader("TAKE EXAM");
        
        // Exam configuration
        cout << "Configure your exam:" << endl;
        
        cout << "1. Select Subject:" << endl;
        cout << "   1. Mathematics" << endl;
        cout << "   2. Computer Science" << endl;
        cout << "   3. Science" << endl;
        cout << "   4. General Knowledge" << endl;
        cout << "   5. Mixed (All subjects)" << endl;
        
        cout << "\nEnter choice (1-5): ";
        int subjectChoice;
        cin >> subjectChoice;
        
        string subject = "";
        switch (subjectChoice) {
            case 1: subject = "Mathematics"; break;
            case 2: subject = "Computer Science"; break;
            case 3: subject = "Science"; break;
            case 4: subject = "General Knowledge"; break;
            case 5: subject = ""; break; // Mixed
            default: 
                cout << "Invalid choice!" << endl;
                Utils::pauseSystem();
                return;
        }
        
        cout << "\nNumber of questions (5-20): ";
        int questionCount;
        cin >> questionCount;
        if (questionCount < 5) questionCount = 5;
        if (questionCount > 20) questionCount = 20;
        
        cout << "Time limit in minutes (0 for no limit): ";
        int timeLimit;
        cin >> timeLimit;
        
        // Get questions
        vector<Question> questions;
        if (subject.empty()) {
            questions = dbManager->getRandomQuestions(questionCount);
        } else {
            questions = dbManager->getRandomQuestions(questionCount, subject);
        }
        
        if (questions.empty()) {
            cout << "\nNo questions available for the selected criteria!" << endl;
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
    
    void conductExam(const vector<Question>& questions, int timeLimit, const string& subject) {
        Utils::clearScreen();
        Utils::printHeader("EXAM IN PROGRESS");
        
        vector<int> userAnswers(questions.size(), -1);
        vector<bool> answered(questions.size(), false);
        auto startTime = chrono::steady_clock::now();
        
        cout << "Exam Started!" << endl;
        cout << "Questions: " << questions.size() << endl;
        if (timeLimit > 0) {
            cout << "Time Limit: " << timeLimit << " minutes" << endl;
        }
        cout << "Instructions:" << endl;
        cout << "- Enter 1-4 for your answer" << endl;
        cout << "- Enter 0 to skip question" << endl;
        cout << "- Enter -1 to go back to previous question" << endl;
        cout << "- Enter -2 to submit exam" << endl;
        cout << "\nPress Enter to start...";
        cin.ignore();
        cin.get();
        
        int currentQuestion = 0;
        bool examCompleted = false;
        
        while (!examCompleted && currentQuestion < static_cast<int>(questions.size())) {
            // Check time limit
            if (timeLimit > 0) {
                auto currentTime = chrono::steady_clock::now();
                auto elapsed = chrono::duration_cast<chrono::minutes>(currentTime - startTime);
                if (elapsed.count() >= timeLimit) {
                    cout << "\n⏰ Time's up! Exam will be submitted automatically." << endl;
                    break;
                }
            }
            
            Utils::clearScreen();
            
            // Show progress
            int answeredCount = 0;
            for (bool ans : answered) {
                if (ans) answeredCount++;
            }
            
            cout << "Question " << (currentQuestion + 1) << " of " << questions.size() 
                     << " | Answered: " << answeredCount << "/" << questions.size() << endl;
            
            if (timeLimit > 0) {
                auto currentTime = chrono::steady_clock::now();
                auto elapsed = chrono::duration_cast<chrono::minutes>(currentTime - startTime);
                int remaining = timeLimit - elapsed.count();
                cout << "Time Remaining: " << remaining << " minutes" << endl;
            }
            
            cout << string(60, '=') << endl;
            
            // Display question
            questions[currentQuestion].display();
            
            if (answered[currentQuestion]) {
                cout << "\nCurrent Answer: " << (userAnswers[currentQuestion] + 1) << endl;
            }
            
            cout << "\nYour answer (1-4, 0=skip, -1=previous, -2=submit): ";
            int answer;
            cin >> answer;
            
            if (answer == -2) {
                // Submit exam
                cout << "\nAre you sure you want to submit? (y/N): ";
                char confirm;
                cin >> confirm;
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
                cout << "Invalid input! Please try again." << endl;
                Utils::pauseSystem();
            }
        }
        
        // Calculate results
        auto endTime = chrono::steady_clock::now();
        auto duration = chrono::duration_cast<chrono::minutes>(endTime - startTime);
        
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
    
    void showExamResults(const vector<Question>& questions, 
                        const vector<int>& userAnswers,
                        const vector<bool>& answered,
                        int score, double percentage, int duration) {
        Utils::clearScreen();
        Utils::printHeader("EXAM RESULTS");
        
        cout << "Exam Completed!" << endl;
        cout << string(50, '=') << endl;
        cout << "Score: " << score << "/" << questions.size() << endl;
        cout << "Percentage: " << percentage << "%" << endl;
        cout << "Duration: " << duration << " minutes" << endl;
        cout << "Grade: " << getGrade(percentage) << endl;
        cout << "Status: " << (percentage >= 60 ? "PASSED" : "FAILED") << endl;
        cout << string(50, '=') << endl;
        
        cout << "\nDetailed Review:" << endl;
        for (size_t i = 0; i < questions.size(); ++i) {
            cout << "\nQ" << (i + 1) << ": " << questions[i].getQuestionText() << endl;
            
            if (!answered[i]) {
                cout << "   Your Answer: Not answered" << endl;
                cout << "   Status: ❌ Incorrect" << endl;
            } else {
                cout << "   Your Answer: " << (userAnswers[i] + 1) << endl;
                if (userAnswers[i] == questions[i].getCorrectAnswer()) {
                    cout << "   Status: ✅ Correct" << endl;
                } else {
                    cout << "   Status: ❌ Incorrect" << endl;
                }
            }
            cout << "   Correct Answer: " << (questions[i].getCorrectAnswer() + 1) << endl;
            
            if (!questions[i].getExplanation().empty()) {
                cout << "   Explanation: " << questions[i].getExplanation() << endl;
            }
        }
        
        Utils::pauseSystem();
    }
    
    void practiceMode() {
        Utils::clearScreen();
        Utils::printHeader("PRACTICE MODE");
        
        cout << "Practice Mode Options:" << endl;
        cout << "1. Quick Practice (5 questions)" << endl;
        cout << "2. Subject-wise Practice" << endl;
        cout << "3. Difficulty-based Practice" << endl;
        cout << "4. Review Mistakes" << endl;
        cout << "5. Back to Main Menu" << endl;
        
        cout << "\nEnter choice: ";
        int choice;
        cin >> choice;
        
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
                cout << "Invalid choice!" << endl;
                Utils::pauseSystem();
        }
    }
    
    void quickPractice() {
        auto questions = dbManager->getRandomQuestions(5);
        if (questions.empty()) {
            cout << "No questions available!" << endl;
            Utils::pauseSystem();
            return;
        }
        
        Utils::clearScreen();
        Utils::printHeader("QUICK PRACTICE");
        
        int score = 0;
        for (size_t i = 0; i < questions.size(); ++i) {
            cout << "\nQuestion " << (i + 1) << " of " << questions.size() << endl;
            cout << string(40, '-') << endl;
            questions[i].display();
            
            cout << "\nYour answer (1-4): ";
            int answer;
            cin >> answer;
            
            if (answer >= 1 && answer <= 4 && (answer - 1) == questions[i].getCorrectAnswer()) {
                cout << "✅ Correct!" << endl;
                score++;
            } else {
                cout << "❌ Wrong! Correct answer: " << (questions[i].getCorrectAnswer() + 1) << endl;
                if (!questions[i].getExplanation().empty()) {
                    cout << "Explanation: " << questions[i].getExplanation() << endl;
                }
            }
            
            if (i < questions.size() - 1) {
                cout << "\nPress Enter for next question...";
                cin.ignore();
                cin.get();
            }
        }
        
        cout << "\n" << string(40, '=') << endl;
        cout << "Practice Complete!" << endl;
        cout << "Score: " << score << "/" << questions.size() << endl;
        cout << "Accuracy: " << (score * 100.0 / questions.size()) << "%" << endl;
        
        Utils::pauseSystem();
    }
    
    void subjectPractice() {
        Utils::clearScreen();
        Utils::printHeader("SUBJECT-WISE PRACTICE");
        
        cout << "Select Subject:" << endl;
        cout << "1. Mathematics" << endl;
        cout << "2. Computer Science" << endl;
        cout << "3. Science" << endl;
        cout << "4. General Knowledge" << endl;
        
        cout << "\nEnter choice: ";
        int choice;
        cin >> choice;
        
        string subject;
        switch (choice) {
            case 1: subject = "Mathematics"; break;
            case 2: subject = "Computer Science"; break;
            case 3: subject = "Science"; break;
            case 4: subject = "General Knowledge"; break;
            default:
                cout << "Invalid choice!" << endl;
                Utils::pauseSystem();
                return;
        }
        
        auto questions = dbManager->getQuestionsBySubject(subject);
        if (questions.empty()) {
            cout << "No questions available for " << subject << "!" << endl;
            Utils::pauseSystem();
            return;
        }
        
        // Shuffle and limit to 10 questions
        random_device rd;
        mt19937 g(rd());
        shuffle(questions.begin(), questions.end(), g);
        if (questions.size() > 10) {
            questions.resize(10);
        }
        
        conductPracticeSession(questions, subject + " Practice");
    }
    
    void difficultyPractice() {
        Utils::clearScreen();
        Utils::printHeader("DIFFICULTY-BASED PRACTICE");
        
        cout << "Select Difficulty:" << endl;
        cout << "1. Easy" << endl;
        cout << "2. Medium" << endl;
        cout << "3. Hard" << endl;
        
        cout << "\nEnter choice: ";
        int choice;
        cin >> choice;
        
        string difficulty;
        switch (choice) {
            case 1: difficulty = "Easy"; break;
            case 2: difficulty = "Medium"; break;
            case 3: difficulty = "Hard"; break;
            default:
                cout << "Invalid choice!" << endl;
                Utils::pauseSystem();
                return;
        }
        
        auto questions = dbManager->getQuestionsByDifficulty(difficulty);
        if (questions.empty()) {
            cout << "No " << difficulty << " questions available!" << endl;
            Utils::pauseSystem();
            return;
        }
        
        // Shuffle and limit to 10 questions
        random_device rd;
        mt19937 g(rd());
        shuffle(questions.begin(), questions.end(), g);
        if (questions.size() > 10) {
            questions.resize(10);
        }
        
        conductPracticeSession(questions, difficulty + " Practice");
    }
    
    void conductPracticeSession(const vector<Question>& questions, const string& sessionName) {
        Utils::clearScreen();
        Utils::printHeader(sessionName);
        
        int score = 0;
        vector<bool> correct(questions.size(), false);
        
        for (size_t i = 0; i < questions.size(); ++i) {
            cout << "\nQuestion " << (i + 1) << " of " << questions.size() << endl;
            cout << string(50, '-') << endl;
            questions[i].display();
            
            cout << "\nYour answer (1-4): ";
            int answer;
            cin >> answer;
            
            if (answer >= 1 && answer <= 4 && (answer - 1) == questions[i].getCorrectAnswer()) {
                cout << "✅ Correct!" << endl;
                score++;
                correct[i] = true;
            } else {
                cout << "❌ Wrong! Correct answer: " << (questions[i].getCorrectAnswer() + 1) << endl;
                if (!questions[i].getExplanation().empty()) {
                    cout << "Explanation: " << questions[i].getExplanation() << endl;
                }
            }
            
            if (i < questions.size() - 1) {
                cout << "\nPress Enter for next question...";
                cin.ignore();
                cin.get();
            }
        }
        
        // Show summary
        cout << "\n" << string(50, '=') << endl;
        cout << sessionName << " Complete!" << endl;
        cout << "Score: " << score << "/" << questions.size() << endl;
        cout << "Accuracy: " << (score * 100.0 / questions.size()) << "%" << endl;
        
        // Show incorrect questions for review
        bool hasIncorrect = false;
        for (size_t i = 0; i < questions.size(); ++i) {
            if (!correct[i]) {
                if (!hasIncorrect) {
                    cout << "\nQuestions to review:" << endl;
                    hasIncorrect = true;
                }
                cout << "Q" << (i + 1) << ": " << questions[i].getSubject() 
                         << " - " << questions[i].getDifficulty() << endl;
            }
        }
        
        Utils::pauseSystem();
    }
    
    void reviewMistakes() {
        Utils::clearScreen();
        Utils::printHeader("REVIEW MISTAKES");
        
        cout << "This feature will show questions you got wrong in previous exams." << endl;
        cout << "Feature coming soon!" << endl;
        
        Utils::pauseSystem();
    }
    
    void viewMyResults() {
        Utils::clearScreen();
        Utils::printHeader("MY EXAM RESULTS");
        
        auto results = dbManager->getExamResultsByUser(currentStudent.getId());
        if (results.empty()) {
            cout << "No exam results found!" << endl;
            cout << "Take an exam to see your results here." << endl;
        } else {
            cout << "Total Exams Taken: " << results.size() << endl;
            cout << string(80, '-') << endl;
            cout << "Date\t\tSubject\t\tScore\t\tPercentage\tGrade" << endl;
            cout << string(80, '-') << endl;
            
            double totalPercentage = 0;
            for (const auto& result : results) {
                cout << result.getExamDate() << "\t"
                         << (result.getSubject().empty() ? "Mixed" : result.getSubject()) << "\t\t"
                         << result.getScore() << "/" << result.getTotalQuestions() << "\t\t"
                         << result.getPercentage() << "%\t\t"
                         << getGrade(result.getPercentage()) << endl;
                totalPercentage += result.getPercentage();
            }
            
            cout << string(80, '-') << endl;
            cout << "Average Score: " << (totalPercentage / results.size()) << "%" << endl;
        }
        
        Utils::pauseSystem();
    }
    
    void viewProfile() {
        Utils::clearScreen();
        Utils::printHeader("MY PROFILE");
        
        cout << "Username: " << currentStudent.getUsername() << endl;
        cout << "Full Name: " << currentStudent.getFullName() << endl;
        cout << "Email: " << currentStudent.getEmail() << endl;
        cout << "Role: " << currentStudent.roleToString() << endl;
        cout << "Status: " << currentStudent.statusToString() << endl;
        
        // Show exam statistics
        auto results = dbManager->getExamResultsByUser(currentStudent.getId());
        cout << "\nExam Statistics:" << endl;
        cout << "Total Exams: " << results.size() << endl;
        
        if (!results.empty()) {
            double totalPercentage = 0;
            int passedExams = 0;
            
            for (const auto& result : results) {
                totalPercentage += result.getPercentage();
                if (result.getPercentage() >= 60) passedExams++;
            }
            
            cout << "Average Score: " << (totalPercentage / results.size()) << "%" << endl;
            cout << "Passed Exams: " << passedExams << "/" << results.size() << endl;
            cout << "Pass Rate: " << (passedExams * 100.0 / results.size()) << "%" << endl;
        }
        
        Utils::pauseSystem();
    }
    
    void performanceAnalytics() {
        Utils::clearScreen();
        Utils::printHeader("PERFORMANCE ANALYTICS");
        
        auto results = dbManager->getExamResultsByUser(currentStudent.getId());
        if (results.empty()) {
            cout << "No exam data available for analysis." << endl;
            cout << "Take some exams to see your performance analytics!" << endl;
            Utils::pauseSystem();
            return;
        }
        
        // Calculate statistics
        map<string, vector<double>> subjectScores;
        double totalScore = 0;
        int passedExams = 0;
        
        for (const auto& result : results) {
            string subject = result.getSubject().empty() ? "Mixed" : result.getSubject();
            subjectScores[subject].push_back(result.getPercentage());
            totalScore += result.getPercentage();
            if (result.getPercentage() >= 60) passedExams++;
        }
        
        cout << "Overall Performance:" << endl;
        cout << "Total Exams: " << results.size() << endl;
        cout << "Average Score: " << (totalScore / results.size()) << "%" << endl;
        cout << "Pass Rate: " << (passedExams * 100.0 / results.size()) << "%" << endl;
        
        cout << "\nSubject-wise Performance:" << endl;
        cout << string(50, '-') << endl;
        for (const auto& pair : subjectScores) {
            double subjectTotal = 0;
            for (double score : pair.second) {
                subjectTotal += score;
            }
            double average = subjectTotal / pair.second.size();
            cout << pair.first << ": " << average << "% (" 
                     << pair.second.size() << " exams)" << endl;
        }
        
        // Find strengths and weaknesses
        string strongestSubject, weakestSubject;
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
            cout << "\nStrengths & Weaknesses:" << endl;
            cout << "Strongest Subject: " << strongestSubject << " (" << highestAvg << "%)" << endl;
            if (!weakestSubject.empty() && weakestSubject != strongestSubject) {
                cout << "Needs Improvement: " << weakestSubject << " (" << lowestAvg << "%)" << endl;
            }
        }
        
        Utils::pauseSystem();
    }
    
    string getGrade(double percentage) {
        if (percentage >= 90) return "A+";
        else if (percentage >= 80) return "A";
        else if (percentage >= 70) return "B";
        else if (percentage >= 60) return "C";
        else if (percentage >= 50) return "D";
        else return "F";
    }
};

#endif // STUDENT_STUB_H