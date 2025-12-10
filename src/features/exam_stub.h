#ifndef EXAM_STUB_H
#define EXAM_STUB_H

#include "../authentication/user.h"
#include "../database/database.h"

// Temporary exam types and settings for stub
enum class ExamType {
    PRACTICE = 1,
    MOCK = 2,
    OFFICIAL = 3
};

enum class ExamMode {
    SEQUENTIAL = 1,
    FLEXIBLE = 2
};

struct ExamSettings {
    ExamType type;
    ExamMode mode;
    int questionCount;
    int timeLimit;
    bool showTimer;
    
    ExamSettings() : type(ExamType::PRACTICE), mode(ExamMode::FLEXIBLE),
                    questionCount(5), timeLimit(0), showTimer(false) {}
};

// Temporary stub for ExamEngine until full implementation
class ExamEngine {
private:
    User student;
    ExamSettings settings;
    std::vector<Question> examQuestions;
    int currentQuestionIndex;
    bool active;
    bool completed;
    
public:
    ExamEngine(const User& user, const ExamSettings& config) 
        : student(user), settings(config), currentQuestionIndex(0), 
          active(false), completed(false) {}
    
    bool initializeExam(const std::vector<Question>& questions) {
        examQuestions = questions;
        if (examQuestions.size() > static_cast<size_t>(settings.questionCount)) {
            examQuestions.resize(settings.questionCount);
        }
        return !examQuestions.empty();
    }
    
    bool startExam() {
        if (!examQuestions.empty()) {
            active = true;
            return true;
        }
        return false;
    }
    
    bool isActive() const { return active; }
    bool isCompleted() const { return completed; }
    
    Question* getCurrentQuestion() {
        if (currentQuestionIndex < static_cast<int>(examQuestions.size())) {
            return &examQuestions[currentQuestionIndex];
        }
        return nullptr;
    }
    
    bool answerQuestion(int answer) {
        // Simple implementation - just store answer
        return true;
    }
    
    bool nextQuestion() {
        if (currentQuestionIndex < static_cast<int>(examQuestions.size()) - 1) {
            currentQuestionIndex++;
            return true;
        }
        return false;
    }
    
    bool previousQuestion() {
        if (currentQuestionIndex > 0) {
            currentQuestionIndex--;
            return true;
        }
        return false;
    }
    
    bool submitExam() {
        active = false;
        completed = true;
        return true;
    }
    
    ExamResult generateResult() {
        // Simple result generation
        ExamResult result(student.getId(), student.getUsername(), 
                         examQuestions.size() / 2, examQuestions.size());
        return result;
    }
};

// Temporary stub for ExamScheduler
class ExamScheduler {
private:
    DatabaseManager* dbManager;
    
public:
    ExamScheduler(DatabaseManager* db) : dbManager(db) {}
    
    void forceEndAllExams() {
        // Stub implementation
        std::cout << "All exams ended." << std::endl;
    }
};

#endif // EXAM_STUB_H