#ifndef EXAM_TYPES_H
#define EXAM_TYPES_H

#include <string>
using namespace std;

// Exam types with specific purposes
enum class ExamType {
    MID = 1,        // Mid-term exam
    QUIZ = 2,       // Quiz exam
    FINAL = 3,      // Final exam
    WORKSHEET = 4   // Worksheet (Guest mode)
};

// Exam configuration class
class ExamConfig {
private:
    int id;
    string title;
    ExamType type;
    string subject;
    int timeLimit;      // in minutes (set by teacher)
    int questionCount;
    string difficulty;
    bool isActive;
    string createdBy;   // teacher username
    string createdAt;
    
public:
    ExamConfig();
    ExamConfig(const string& title, ExamType type, const string& subject, 
               int timeLimit, int questionCount, const string& difficulty = "Mixed");
    
    // Getters
    int getId() const { return id; }
    string getTitle() const { return title; }
    ExamType getType() const { return type; }
    string getSubject() const { return subject; }
    int getTimeLimit() const { return timeLimit; }
    int getQuestionCount() const { return questionCount; }
    string getDifficulty() const { return difficulty; }
    bool getIsActive() const { return isActive; }
    string getCreatedBy() const { return createdBy; }
    string getCreatedAt() const { return createdAt; }
    
    // Setters
    void setId(int id) { this->id = id; }
    void setTitle(const string& title) { this->title = title; }
    void setType(ExamType type) { this->type = type; }
    void setSubject(const string& subject) { this->subject = subject; }
    void setTimeLimit(int timeLimit) { this->timeLimit = timeLimit; }
    void setQuestionCount(int questionCount) { this->questionCount = questionCount; }
    void setDifficulty(const string& difficulty) { this->difficulty = difficulty; }
    void setIsActive(bool isActive) { this->isActive = isActive; }
    void setCreatedBy(const string& createdBy) { this->createdBy = createdBy; }
    void setCreatedAt(const string& createdAt) { this->createdAt = createdAt; }
    
    // Utility methods
    string typeToString() const;
    static ExamType stringToType(const string& typeStr);
    static string getDefaultTimeForType(ExamType type);
    
    // Display
    void display() const;
    bool isValidConfig() const;
};

#endif // EXAM_TYPES_H