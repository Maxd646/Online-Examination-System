#ifndef EXAM_CREATOR_H
#define EXAM_CREATOR_H

#include "../authentication/user.h"
#include "../database/database.h"
#include "../structure/utils.h"
#include "exam_template.h"
#include <iostream>
#include <vector>
#include <string>

using namespace std;

// Exam Question class for direct exam creation
class ExamQuestion {
private:
    int id;
    int examTemplateId;
    int questionNumber;
    string questionText;
    vector<string> options;
    int correctAnswer;
    string explanation;

public:
    ExamQuestion() : id(0), examTemplateId(0), questionNumber(0), correctAnswer(0) {
        options.resize(4);
    }
    
    ExamQuestion(int examTemplateId, int questionNumber, const string& questionText,
                 const vector<string>& options, int correctAnswer, const string& explanation = "")
        : id(0), examTemplateId(examTemplateId), questionNumber(questionNumber),
          questionText(questionText), options(options), correctAnswer(correctAnswer),
          explanation(explanation) {}

    // Getters
    int getId() const { return id; }
    int getExamTemplateId() const { return examTemplateId; }
    int getQuestionNumber() const { return questionNumber; }
    string getQuestionText() const { return questionText; }
    vector<string> getOptions() const { return options; }
    int getCorrectAnswer() const { return correctAnswer; }
    string getExplanation() const { return explanation; }

    // Setters
    void setId(int id) { this->id = id; }
    void setExamTemplateId(int examTemplateId) { this->examTemplateId = examTemplateId; }
    void setQuestionNumber(int questionNumber) { this->questionNumber = questionNumber; }
    void setQuestionText(const string& questionText) { this->questionText = questionText; }
    void setOptions(const vector<string>& options) { this->options = options; }
    void setCorrectAnswer(int correctAnswer) { this->correctAnswer = correctAnswer; }
    void setExplanation(const string& explanation) { this->explanation = explanation; }

    // Validation
    bool isValid() const {
        return !questionText.empty() && options.size() == 4 &&
               correctAnswer >= 0 && correctAnswer <= 3 &&
               !options[0].empty() && !options[1].empty() &&
               !options[2].empty() && !options[3].empty();
    }

    // Display
    void display() const {
        cout << "Q" << questionNumber << ": " << questionText << endl;
        for (size_t i = 0; i < options.size(); ++i) {
            cout << "   " << (i + 1) << ". " << options[i] << endl;
        }
    }

    void displayWithAnswer() const {
        display();
        cout << "   Correct Answer: " << char('a' + correctAnswer) << endl;
        if (!explanation.empty()) {
            cout << "   Explanation: " << explanation << endl;
        }
    }
};

// Exam Creator class for teachers to create complete exams
class ExamCreator {
private:
    DatabaseManager* dbManager;
    User currentTeacher;

public:
    ExamCreator(DatabaseManager* db, const User& teacher) 
        : dbManager(db), currentTeacher(teacher) {}

    void createCompleteExam() {
        Utils::clearScreen();
        Utils::printHeader("CREATE COMPLETE EXAM");
        
        cout << " Create a complete exam with questions for your students" << endl;
        cout << string(60, '=') << endl;

        // Step 1: Create exam template
        ExamTemplate examTemplate = createExamTemplate();
        if (examTemplate.getId() == 0) {
            cout << " Failed to create exam template!" << endl;
            Utils::pauseSystem();
            return;
        }

        cout << "\n Exam template created successfully!" << endl;
        cout << " Template ID: " << examTemplate.getId() << endl;
        cout << " Now let's add questions to your exam..." << endl;
        Utils::pauseSystem();

        // Step 2: Add questions to the exam
        addQuestionsToExam(examTemplate);
    }

    void addQuestionsToExistingExam(const ExamTemplate& examTemplate) {
        addQuestionsToExam(examTemplate);
    }

private:
    ExamTemplate createExamTemplate() {
        ExamTemplate examTemplate;
        
        cout << "\n STEP 1: Exam Details" << endl;
        cout << string(40, '-') << endl;

        // Get exam name
        string examName;
        cout << "Exam Name: ";
        cin.ignore();
        getline(cin, examName);
        examTemplate.setTemplateName(examName);

        // Get exam type
        cout << "\nSelect Exam Type:" << endl;
        cout << "1.  QUIZ (Quick assessment)" << endl;
        cout << "2.  WORKSHEET (Practice/homework)" << endl;
        cout << "3.  FINAL (Comprehensive exam)" << endl;
        cout << "Enter choice (1-3): ";
        int typeChoice;
        cin >> typeChoice;

        switch (typeChoice) {
            case 1: examTemplate.setExamType(ExamType::QUIZ); break;
            case 2: examTemplate.setExamType(ExamType::WORKSHEET); break;
            case 3: examTemplate.setExamType(ExamType::FINAL); break;
            default: 
                cout << "Invalid choice! Defaulting to QUIZ." << endl;
                examTemplate.setExamType(ExamType::QUIZ);
        }

        // Get subject
        cout << "\nSelect Subject:" << endl;
        cout << "1.  DSA (Data Structures & Algorithms)" << endl;
        cout << "2.  OOP (Object-Oriented Programming)" << endl;
        cout << "3.  COA (Computer Organization & Architecture)" << endl;
        cout << "4.  SAM (Software Architecture & Modeling)" << endl;
        cout << "5.  Mathematics" << endl;
        cout << "Enter choice (1-5): ";
        int subjectChoice;
        cin >> subjectChoice;

        string subject;
        switch (subjectChoice) {
            case 1: subject = "DSA"; break;
            case 2: subject = "OOP"; break;
            case 3: subject = "COA"; break;
            case 4: subject = "SAM"; break;
            case 5: subject = "Mathematics"; break;
            default:
                cout << "Invalid choice! Defaulting to DSA." << endl;
                subject = "DSA";
        }
        examTemplate.setSubject(subject);

        // Get time limit
        cout << "\nTime Limit (in minutes): ";
        int timeLimit;
        cin >> timeLimit;
        examTemplate.setTimeLimit(timeLimit);

        // Get passing percentage
        cout << "Passing Percentage (0-100): ";
        double passingPercentage;
        cin >> passingPercentage;
        examTemplate.setPassingPercentage(passingPercentage);

        // Additional settings
        cout << "\n Additional Settings:" << endl;
        char choice;
        
        cout << "Enable Negative Marking? (y/N): ";
        cin >> choice;
        examTemplate.setNegativeMarking(choice == 'y' || choice == 'Y');
        
        if (examTemplate.hasNegativeMarking()) {
            double negValue;
            cout << "Negative Mark Value (e.g., 0.25): ";
            cin >> negValue;
            examTemplate.setNegativeMarkValue(negValue);
        }

        cout << "Shuffle Questions? (Y/n): ";
        cin >> choice;
        examTemplate.setShuffleQuestions(choice != 'n' && choice != 'N');

        cout << "Allow Review? (Y/n): ";
        cin >> choice;
        examTemplate.setAllowReview(choice != 'n' && choice != 'N');

        cout << "Auto Submit on Time Expiry? (Y/n): ";
        cin >> choice;
        examTemplate.setAutoSubmit(choice != 'n' && choice != 'N');

        // Get instructions
        cout << "\nInstructions for students (optional): ";
        string instructions;
        cin.ignore();
        getline(cin, instructions);
        examTemplate.setInstructions(instructions);

        examTemplate.setCreatedBy(currentTeacher.getId());
        examTemplate.setIsActive(true);

        // Save to database
        if (dbManager->insertExamTemplate(examTemplate)) {
            // Get the ID of the inserted template
            examTemplate.setId(dbManager->getLastInsertedExamTemplateId());
        }

        return examTemplate;
    }

    void addQuestionsToExam(const ExamTemplate& examTemplate) {
        Utils::clearScreen();
        Utils::printHeader("ADD QUESTIONS TO EXAM");
        
        cout << " Adding questions to: " << examTemplate.getTemplateName() << endl;
        cout << " Subject: " << examTemplate.getSubject() << endl;
        cout << " Type: " << examTemplate.getExamTypeString() << endl;
        cout << string(60, '=') << endl;

        vector<ExamQuestion> questions;
        int questionNumber = 1;
        char continueAdding = 'y';

        while (continueAdding == 'y' || continueAdding == 'Y') {
            cout << "\n Question " << questionNumber << ":" << endl;
            cout << string(30, '-') << endl;

            ExamQuestion question;
            question.setExamTemplateId(examTemplate.getId());
            question.setQuestionNumber(questionNumber);

            // Get question text
            string questionText;
            cout << "Question: ";
            cin.ignore(); // Always clear the input buffer before getline
            getline(cin, questionText);
            question.setQuestionText(questionText);

            cout << endl; // Add space before options

            // Get options
            vector<string> options(4);
            char optionLabels[] = {'a', 'b', 'c', 'd'};
            for (int i = 0; i < 4; ++i) {
                cout << "Option " << optionLabels[i] << ": ";
                getline(cin, options[i]);
            }
            question.setOptions(options);

            cout << endl; // Add space before correct answer

            // Get correct answer
            string correctAnswerInput;
            cout << "Correct Answer (a-d): ";
            cin >> correctAnswerInput;
            
            int correctAnswer = -1;
            if (correctAnswerInput.length() == 1) {
                char c = tolower(correctAnswerInput[0]);
                if (c >= 'a' && c <= 'd') {
                    correctAnswer = c - 'a'; // Convert a-d to 0-3
                } else if (c >= '1' && c <= '4') {
                    correctAnswer = c - '1'; // Convert 1-4 to 0-3
                }
            }
            if (correctAnswer >= 0 && correctAnswer <= 3) {
                question.setCorrectAnswer(correctAnswer);
            } else {
                cout << "Invalid answer! Defaulting to 'a'." << endl;
                question.setCorrectAnswer(0);
            }

            // Get explanation
            string explanation;
            cout << "Explanation (optional): ";
            cin.ignore(); // Clear buffer before getline
            getline(cin, explanation);
            question.setExplanation(explanation);

            // Validate and add question
            if (question.isValid()) {
                questions.push_back(question);
                
                // Save question to database
                if (dbManager->insertExamQuestion(question)) {
                    cout << " Question " << questionNumber << " added successfully!" << endl;
                } else {
                    cout << " Failed to save question " << questionNumber << endl;
                }
                
                questionNumber++;
            } else {
                cout << " Invalid question data! Please check all fields." << endl;
            }

            cout << "\nAdd another question? (y/N): ";
            cin >> continueAdding;
        }

        cout << "\n Exam creation completed!" << endl;
        cout << " Total questions added: " << questions.size() << endl;
        cout << " Exam: " << examTemplate.getTemplateName() << endl;
        cout << " Students can now take this exam!" << endl;
        
        Utils::pauseSystem();
    }
};

#endif // EXAM_CREATOR_H