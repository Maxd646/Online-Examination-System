#include "exam_types.h"
#include "../structure/utils.h"
#include <iostream>
#include <algorithm>
using namespace std;

ExamConfig::ExamConfig() : id(0), type(ExamType::QUIZ), timeLimit(30), 
                          questionCount(10), difficulty("Mixed"), isActive(true) {
    createdAt = Utils::getCurrentDateTime();
}

ExamConfig::ExamConfig(const string& title, ExamType type, const string& subject, 
                      int timeLimit, int questionCount, const string& difficulty)
    : id(0), title(title), type(type), subject(subject), timeLimit(timeLimit),
      questionCount(questionCount), difficulty(difficulty), isActive(true) {
    createdAt = Utils::getCurrentDateTime();
}

string ExamConfig::typeToString() const {
    switch (type) {
        case ExamType::MID: return "Mid Exam";
        case ExamType::QUIZ: return "Quiz";
        case ExamType::FINAL: return "Final Exam";
        case ExamType::WORKSHEET: return "Worksheet";
        default: return "Unknown";
    }
}

ExamType ExamConfig::stringToType(const string& typeStr) {
    string lower = typeStr;
    transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    
    if (lower == "mid" || lower == "midterm") return ExamType::MID;
    if (lower == "quiz") return ExamType::QUIZ;
    if (lower == "final") return ExamType::FINAL;
    if (lower == "worksheet") return ExamType::WORKSHEET;
    
    return ExamType::QUIZ; // Default
}

string ExamConfig::getDefaultTimeForType(ExamType type) {
    switch (type) {
        case ExamType::MID: return "90 minutes";
        case ExamType::QUIZ: return "30 minutes";
        case ExamType::FINAL: return "120 minutes";
        case ExamType::WORKSHEET: return "No limit";
        default: return "30 minutes";
    }
}

void ExamConfig::display() const {
    cout << "\n=== EXAM CONFIGURATION ===" << endl;
    cout << "Title: " << title << endl;
    cout << "Type: " << typeToString() << endl;
    cout << "Subject: " << subject << endl;
    cout << "Time Limit: " << timeLimit << " minutes" << endl;
    cout << "Questions: " << questionCount << endl;
    cout << "Difficulty: " << difficulty << endl;
    cout << "Status: " << (isActive ? "Active" : "Inactive") << endl;
    cout << "Created by: " << createdBy << endl;
    cout << "Created: " << createdAt << endl;
}

bool ExamConfig::isValidConfig() const {
    return !title.empty() && !subject.empty() && 
           timeLimit > 0 && questionCount > 0;
}