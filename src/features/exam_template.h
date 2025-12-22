#ifndef EXAM_TEMPLATE_H
#define EXAM_TEMPLATE_H

#include <string>
#include <vector>

using namespace std;

// Exam type enumeration
enum class ExamType {
    QUIZ = 1,       // Short, quick assessment (10-15 questions, 15-30 min)
    WORKSHEET = 2,  // Practice/homework (20-30 questions, 30-60 min)
    FINAL = 3       // Comprehensive exam (40-100 questions, 90-180 min)
};

// Exam template class for admin-defined exam configurations
class ExamTemplate {
private:
    int id;
    string templateName;
    ExamType examType;
    string subject;
    int questionCount;
    int timeLimit;           // in minutes
    string difficulty;       // Easy, Medium, Hard, Mixed
    double passingPercentage;
    bool negativeMarking;
    double negativeMarkValue;
    bool shuffleQuestions;
    bool shuffleOptions;
    bool allowReview;
    bool autoSubmit;
    string instructions;
    int createdBy;
    string createdAt;
    string updatedAt;
    bool isActive;

public:
    // Constructors
    ExamTemplate() : id(0), examType(ExamType::QUIZ), questionCount(10), 
                     timeLimit(15), difficulty("Medium"), passingPercentage(60.0),
                     negativeMarking(false), negativeMarkValue(0.25),
                     shuffleQuestions(true), shuffleOptions(false),
                     allowReview(true), autoSubmit(true), createdBy(0), isActive(true) {}
    
    ExamTemplate(int id, const string& name, ExamType type, const string& subj,
                 int qCount, int timeLimit, const string& diff = "Medium")
        : id(id), templateName(name), examType(type), subject(subj),
          questionCount(qCount), timeLimit(timeLimit), difficulty(diff),
          passingPercentage(60.0), negativeMarking(false), negativeMarkValue(0.25),
          shuffleQuestions(true), shuffleOptions(false), allowReview(true),
          autoSubmit(true), createdBy(0), isActive(true) {}

    // Getters
    int getId() const { return id; }
    string getTemplateName() const { return templateName; }
    ExamType getExamType() const { return examType; }
    string getExamTypeString() const {
        switch (examType) {
            case ExamType::QUIZ: return "QUIZ";
            case ExamType::WORKSHEET: return "WORKSHEET";
            case ExamType::FINAL: return "FINAL";
            default: return "UNKNOWN";
        }
    }
    string getSubject() const { return subject; }
    int getQuestionCount() const { return questionCount; }
    int getTimeLimit() const { return timeLimit; }
    string getDifficulty() const { return difficulty; }
    double getPassingPercentage() const { return passingPercentage; }
    bool hasNegativeMarking() const { return negativeMarking; }
    double getNegativeMarkValue() const { return negativeMarkValue; }
    bool shouldShuffleQuestions() const { return shuffleQuestions; }
    bool shouldShuffleOptions() const { return shuffleOptions; }
    bool isReviewAllowed() const { return allowReview; }
    bool isAutoSubmit() const { return autoSubmit; }
    string getInstructions() const { return instructions; }
    int getCreatedBy() const { return createdBy; }
    string getCreatedAt() const { return createdAt; }
    string getUpdatedAt() const { return updatedAt; }
    bool getIsActive() const { return isActive; }

    // Setters
    void setId(int id) { this->id = id; }
    void setTemplateName(const string& name) { templateName = name; }
    void setExamType(ExamType type) { examType = type; }
    void setExamTypeFromString(const string& typeStr) {
        if (typeStr == "QUIZ") examType = ExamType::QUIZ;
        else if (typeStr == "WORKSHEET") examType = ExamType::WORKSHEET;
        else if (typeStr == "FINAL") examType = ExamType::FINAL;
    }
    void setSubject(const string& subj) { subject = subj; }
    void setQuestionCount(int count) { questionCount = count; }
    void setTimeLimit(int limit) { timeLimit = limit; }
    void setDifficulty(const string& diff) { difficulty = diff; }
    void setPassingPercentage(double percentage) { passingPercentage = percentage; }
    void setNegativeMarking(bool enabled) { negativeMarking = enabled; }
    void setNegativeMarkValue(double value) { negativeMarkValue = value; }
    void setShuffleQuestions(bool shuffle) { shuffleQuestions = shuffle; }
    void setShuffleOptions(bool shuffle) { shuffleOptions = shuffle; }
    void setAllowReview(bool allow) { allowReview = allow; }
    void setAutoSubmit(bool autoSub) { autoSubmit = autoSub; }
    void setInstructions(const string& instr) { instructions = instr; }
    void setCreatedBy(int userId) { createdBy = userId; }
    void setCreatedAt(const string& timestamp) { createdAt = timestamp; }
    void setUpdatedAt(const string& timestamp) { updatedAt = timestamp; }
    void setIsActive(bool active) { isActive = active; }

    // Validation
    bool isValid() const {
        return !templateName.empty() && !subject.empty() && 
               questionCount > 0 && timeLimit > 0 && 
               passingPercentage >= 0 && passingPercentage <= 100;
    }

    // Display methods
    void display() const {
        cout << "Template: " << templateName << endl;
        cout << "Type: " << getExamTypeString() << endl;
        cout << "Subject: " << subject << endl;
        cout << "Questions: " << questionCount << endl;
        cout << "Time Limit: " << timeLimit << " minutes" << endl;
        cout << "Difficulty: " << difficulty << endl;
        cout << "Passing: " << passingPercentage << "%" << endl;
        if (!instructions.empty()) {
            cout << "Instructions: " << instructions << endl;
        }
    }

    void displayDetailed() const {
        display();
        cout << "Negative Marking: " << (negativeMarking ? "Yes" : "No") << endl;
        if (negativeMarking) {
            cout << "Negative Mark Value: " << negativeMarkValue << endl;
        }
        cout << "Shuffle Questions: " << (shuffleQuestions ? "Yes" : "No") << endl;
        cout << "Shuffle Options: " << (shuffleOptions ? "Yes" : "No") << endl;
        cout << "Allow Review: " << (allowReview ? "Yes" : "No") << endl;
        cout << "Auto Submit: " << (autoSubmit ? "Yes" : "No") << endl;
        cout << "Status: " << (isActive ? "Active" : "Inactive") << endl;
    }
};

#endif // EXAM_TEMPLATE_H
