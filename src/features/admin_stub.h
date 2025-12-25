#ifndef ADMIN_STUB_H
#define ADMIN_STUB_H

#include "../authentication/user.h"
#include "../database/database.h"
#include "../structure/utils.h"
#include "../features/exam_template.h"
#include "../features/exam_creator.h"
#include "../components/queue.h"
#include "../components/stack.h"
#include "../components/linked_list.h"
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <iomanip>
#include <algorithm>
#include <iomanip>

// Enhanced AdminPanel with question management functionality
class AdminPanel
{
private:
    DatabaseManager *dbManager;
    User currentAdmin;
    Queue<string> pendingOperations;  // DSA: Queue for operation processing
    UndoRedoStack<string> operationHistory;   // DSA: UndoRedoStack for undo/redo functionality
    LinkedList<string> recentActions; // DSA: Linked List for recent actions

public:
    AdminPanel(DatabaseManager *db, const User &admin)
    {
        dbManager = db;
        currentAdmin = admin;
    }

    void run()
    {
        pendingOperations.push("System Started"); // DSA: Queue operation
        operationHistory.execute("Admin Login");     // DSA: UndoRedoStack operation

        while (true)
        {
            Utils::clearScreen();

            // DSA: Process pending operations using Queue
            if (!pendingOperations.empty())
            {
                cout << "Processing: " << pendingOperations.front() << endl;
                pendingOperations.pop();
            }

            // Bright admin panel header
            cout << "\n+" << string(58, '=') << "+" << endl;
            cout << "|" << setw(35) << "ADMIN PANEL" << setw(23) << "|" << endl;
            cout << "+" << string(58, '=') << "+" << endl;

            cout << "\n Welcome " << currentAdmin.getFullName() << "!" << endl;
            
            cout << "\n Admin Menu:" << endl;
            cout << " 1. Create Complete Exam (Quiz/Worksheet/Final)" << endl;
            cout << " 2. Manage Existing Exams" << endl;
            cout << " 3. User Management" << endl;
            cout << " 4. View System Statistics" << endl;
            cout << " 5. View All Results" << endl;
            if (operationHistory.canUndo()) {
                cout << " 0. Back" << endl;
            }
            if (operationHistory.canRedo()) {
                cout << " 9. Next" << endl;
            }
            cout << " 6. Logout" << endl;

            cout << "\n Enter your choice: ";
            int choice;
            cin >> choice;

            switch (choice)
            {
            case 0:
                if (operationHistory.canUndo()) {
                    string location = operationHistory.undo();
                    cout << "\n← Going back to: " << location << endl;
                    Utils::pauseSystem();
                    // Continue loop to show previous menu state
                } else {
                    cout << "\n✗ Cannot go back!" << endl;
                    Utils::pauseSystem();
                }
                break;
            case 1:
                pendingOperations.push("Create Exam Operation"); // DSA: Queue
                operationHistory.execute("Create Exam");            // DSA: UndoRedoStack
                createCompleteExam();
                break;
            case 2:
                pendingOperations.push("Manage Exams Operation"); // DSA: Queue
                operationHistory.execute("Manage Exams");            // DSA: UndoRedoStack
                manageExistingExams();
                break;
            case 3:
                operationHistory.execute("User Management");
                userManagement();
                break;
            case 4:
                operationHistory.execute("View Statistics");
                showStatistics();
                break;
            case 5:
                operationHistory.execute("View All Results");
                viewAllResults();
                break;
            case 9:
                if (operationHistory.canRedo()) {
                    string location = operationHistory.redo();
                    cout << "\n→ Going forward to: " << location << endl;
                    Utils::pauseSystem();
                    // Continue loop to show next menu state
                } else {
                    cout << "\n✗ Cannot go forward!" << endl;
                    Utils::pauseSystem();
                }
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
    void createCompleteExam()
    {
        recentActions.push_back("Started creating exam"); // DSA: Linked List
        ExamCreator examCreator(dbManager, currentAdmin);
        examCreator.createCompleteExam();
        recentActions.push_back("Completed exam creation"); // DSA: Linked List
    }

    void manageExistingExams()
    {
        while (true)
        {
            Utils::clearScreen();
            Utils::printHeader("MANAGE EXISTING EXAMS");
            cout << " Manage your created exams" << endl;
            cout << string(60, '-') << endl;
            cout << "1. View All Exams" << endl;
            cout << "2. View Exam Questions" << endl;
            cout << "3. Edit Exam" << endl;
            cout << "4. Delete Exam" << endl;
            cout << "5. Activate/Deactivate Exam" << endl;
            cout << "6. View Exam Results" << endl;
            cout << "7. Back to Main Menu" << endl;

            cout << "\nEnter your choice: ";
            int choice;
            cin >> choice;

            switch (choice)
            {
            case 1:
                viewAllExams();
                break;
            case 2:
                viewExamQuestions();
                break;
            case 3:
                editExam();
                break;
            case 4:
                deleteExam();
                break;
            case 5:
                toggleExamStatus();
                break;
            case 6:
                viewExamResults();
                break;
            case 7:
                return;
            default:
                cout << "Invalid choice!" << endl;
                Utils::pauseSystem();
            }
        }
    }

    void viewAllExams()
    {
        Utils::clearScreen();
        Utils::printHeader("ALL CREATED EXAMS");

        auto templates = dbManager->getAllExamTemplates();
        if (templates.empty())
        {
            cout << " No exams found!" << endl;
            cout << "Create your first exam to get started." << endl;
        }
        else
        {
            cout << " Total Exams: " << templates.size() << endl;
            cout << string(90, '=') << endl;

            for (size_t i = 0; i < templates.size(); ++i)
            {
                cout << "\n[" << (i + 1) << "] ";
                cout << (templates[i].getIsActive() ? "correct " : "erro ");
                cout << templates[i].getTemplateName() << endl;
                cout << string(90, '-') << endl;
                cout << " Type: " << templates[i].getExamTypeString() << endl;
                cout << " Subject: " << templates[i].getSubject() << endl;
                cout << " Time Limit: " << templates[i].getTimeLimit() << " minutes" << endl;
                cout << " Passing: " << templates[i].getPassingPercentage() << "%" << endl;

                // Show question count
                int questionCount = dbManager->getExamQuestionCount(templates[i].getId());
                cout << " Questions: " << questionCount << endl;

                cout << string(90, '=') << endl;
            }
        }

        Utils::pauseSystem();
    }

    void viewExamQuestions()
    {
        Utils::clearScreen();
        Utils::printHeader("VIEW EXAM QUESTIONS");

        cout << "Enter Exam ID to view questions: ";
        int examId;
        cin >> examId;

        ExamTemplate examTemplate = dbManager->getExamTemplateById(examId);
        if (examTemplate.getId() == 0)
        {
            cout << " Exam not found!" << endl;
            Utils::pauseSystem();
            return;
        }

        auto questions = dbManager->getExamQuestions(examId);

        cout << "\n Exam: " << examTemplate.getTemplateName() << endl;
        cout << "Subject: " << examTemplate.getSubject() << endl;
        cout << " Total Questions: " << questions.size() << endl;
        cout << string(80, '=') << endl;

        if (questions.empty())
        {
            cout << " No questions found for this exam!" << endl;
        }
        else
        {
            for (const auto &question : questions)
            {
                cout << "\n";
                question.displayWithAnswer();
                cout << string(80, '-') << endl;
            }
        }

        Utils::pauseSystem();
    }

    void editExam()
    {
        Utils::clearScreen();
        Utils::printHeader("EDIT EXAM");

        cout << "Enter Exam ID to edit: ";
        int examId;
        cin >> examId;

        ExamTemplate examTemplate = dbManager->getExamTemplateById(examId);
        if (examTemplate.getId() == 0)
        {
            cout << " Exam not found!" << endl;
            Utils::pauseSystem();
            return;
        }

        while (true)
        {
            Utils::clearScreen();
            Utils::printHeader("EDIT EXAM - " + examTemplate.getTemplateName());

            cout << "\nCurrent Exam Details:" << endl;
            cout << string(50, '-') << endl;
            examTemplate.display();

            cout << "\nWhat would you like to edit?" << endl;
            cout << "1. Edit Exam Template Details" << endl;
            cout << "2. Edit Exam Questions" << endl;
            cout << "3. Add New Questions to Exam" << endl;
            cout << "4. Remove Questions from Exam" << endl;
            cout << "5. Back to Manage Exams" << endl;

            cout << "\nEnter your choice: ";
            int choice;
            cin >> choice;

            switch (choice)
            {
            case 1:
                editExamTemplateDetails(examTemplate);
                break;
            case 2:
                editExamQuestions(examId);
                break;
            case 3:
                addQuestionsToExam(examId);
                break;
            case 4:
                removeQuestionsFromExam(examId);
                break;
            case 5:
                return;
            default:
                cout << "Invalid choice!" << endl;
                Utils::pauseSystem();
            }
        }
    }

    void editExamTemplateDetails(ExamTemplate &examTemplate)
    {
        Utils::clearScreen();
        Utils::printHeader("EDIT EXAM TEMPLATE DETAILS");

        cout << "\nCurrent Template:" << endl;
        cout << string(50, '-') << endl;
        examTemplate.display();

        cout << "\n\nEnter new values (press Enter to keep current):" << endl;

        string input;
        cin.ignore();

        cout << "Template Name [" << examTemplate.getTemplateName() << "]: ";
        getline(cin, input);
        if (!input.empty())
            examTemplate.setTemplateName(input);

        cout << "Subject [" << examTemplate.getSubject() << "]: ";
        getline(cin, input);
        if (!input.empty())
            examTemplate.setSubject(input);

        cout << "Question Count [" << examTemplate.getQuestionCount() << "]: ";
        getline(cin, input);
        if (!input.empty()) {
            int qCount = Utils::safeStoi(input, examTemplate.getQuestionCount());
            if (qCount < 1 || qCount > 200) {
                cout << "Warning: Question count must be between 1-200. Keeping current value." << endl;
            } else {
                examTemplate.setQuestionCount(qCount);
            }
        }

        cout << "Time Limit [" << examTemplate.getTimeLimit() << "]: ";
        getline(cin, input);
        if (!input.empty()) {
            int tLimit = Utils::safeStoi(input, examTemplate.getTimeLimit());
            if (tLimit < 1 || tLimit > 600) {
                cout << "Warning: Time limit must be between 1-600 minutes. Keeping current value." << endl;
            } else {
                examTemplate.setTimeLimit(tLimit);
            }
        }

        cout << "Passing Percentage [" << examTemplate.getPassingPercentage() << "]: ";
        getline(cin, input);
        if (!input.empty()) {
            double pPercent = Utils::safeStod(input, examTemplate.getPassingPercentage());
            if (pPercent < 0.0 || pPercent > 100.0) {
                cout << "Warning: Passing percentage must be between 0-100. Keeping current value." << endl;
            } else {
                examTemplate.setPassingPercentage(pPercent);
            }
        }

        if (dbManager->updateExamTemplate(examTemplate))
        {
            cout << "\n✓ Template updated successfully!" << endl;
        }
        else
        {
            cout << "\n✗ Failed to update template!" << endl;
        }

        Utils::pauseSystem();
    }

    void editExamQuestions(int examId)
    {
        Utils::clearScreen();
        Utils::printHeader("EDIT EXAM QUESTIONS");

        auto questions = dbManager->getExamQuestions(examId);
        if (questions.empty())
        {
            cout << " No questions found for this exam!" << endl;
            Utils::pauseSystem();
            return;
        }

        cout << " Select question to edit:" << endl;
        cout << string(60, '-') << endl;

        for (size_t i = 0; i < questions.size(); ++i)
        {
            cout << "\n[" << (i + 1) << "] ";
            cout << questions[i].getQuestionText().substr(0, 50);
            if (questions[i].getQuestionText().length() > 50)
                cout << "...";
            cout << endl;
        }

        cout << "\nEnter question number to edit (0 to cancel): ";
        int choice;
        cin >> choice;

        if (choice == 0 || choice > static_cast<int>(questions.size()))
        {
            return;
        }

        editSingleExamQuestion(questions[choice - 1]);
    }

    void editSingleExamQuestion(ExamQuestion &question)
    {
        Utils::clearScreen();
        Utils::printHeader("EDIT EXAM QUESTION");

        cout << "\nCurrent Question:" << endl;
        cout << string(50, '-') << endl;
        question.displayWithAnswer();

        cout << "\n\nEnter new values (press Enter to keep current):" << endl;

        string input;
        cin.ignore();

        cout << "Question Text [" << question.getQuestionText() << "]: ";
        getline(cin, input);
        if (!input.empty())
            question.setQuestionText(input);

        // Edit options
        auto options = question.getOptions();
        char optionLabels[] = {'a', 'b', 'c', 'd'};
        for (int i = 0; i < 4; ++i)
        {
            cout << " " << optionLabels[i] << " [" << options[i] << "]: ";
            getline(cin, input);
            if (!input.empty())
                options[i] = input;
        }
        question.setOptions(options);

        cout << "Correct Answer (a-d) [" << char('a' + question.getCorrectAnswer()) << "]: ";
        getline(cin, input);
        if (!input.empty())
        {
            char c = tolower(input[0]);
            int correctAnswer = -1;
            if (c >= 'a' && c <= 'd') {
                correctAnswer = c - 'a'; // Convert a-d to 0-3
            } else if (c >= '1' && c <= '4') {
                correctAnswer = c - '1'; // Convert 1-4 to 0-3
            }
            if (correctAnswer >= 0 && correctAnswer <= 3)
            {
                question.setCorrectAnswer(correctAnswer);
            }
        }

        cout << "Explanation [" << question.getExplanation() << "]: ";
        getline(cin, input);
        if (!input.empty())
            question.setExplanation(input);

        if (dbManager->updateExamQuestion(question))
        {
            cout << "\n✓ Question updated successfully!" << endl;
        }
        else
        {
            cout << "\n✗ Failed to update question!" << endl;
        }

        Utils::pauseSystem();
    }

    void editSingleQuestion(Question &question)
    {
        Utils::clearScreen();
        Utils::printHeader("EDIT QUESTION");

        cout << "\nCurrent Question:" << endl;
        cout << string(50, '-') << endl;
        question.displayWithAnswer();

        cout << "\n\nEnter new values (press Enter to keep current):" << endl;

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

        // Edit options
        auto options = question.getOptions();
        char optionLabels[] = {'a', 'b', 'c', 'd'};
        for (int i = 0; i < 4; ++i)
        {
            cout << "Option " << optionLabels[i] << " [" << options[i] << "]: ";
            getline(cin, input);
            if (!input.empty())
                options[i] = input;
        }
        question.setOptions(options);

        cout << "Correct Answer (a-d) [" << char('a' + question.getCorrectAnswer()) << "]: ";
        getline(cin, input);
        if (!input.empty())
        {
            char c = tolower(input[0]);
            int correctAnswer = -1;
            if (c >= 'a' && c <= 'd') {
                correctAnswer = c - 'a'; // Convert a-d to 0-3
            } else if (c >= '1' && c <= '4') {
                correctAnswer = c - '1'; // Convert 1-4 to 0-3
            }
            if (correctAnswer >= 0 && correctAnswer <= 3)
            {
                question.setCorrectAnswer(correctAnswer);
            }
        }

        cout << "Difficulty [" << question.getDifficulty() << "]: ";
        getline(cin, input);
        if (!input.empty())
            question.setDifficulty(input);

        cout << "Explanation [" << question.getExplanation() << "]: ";
        getline(cin, input);
        if (!input.empty())
            question.setExplanation(input);

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

    void addQuestionsToExam(int examId)
    {
        Utils::clearScreen();
        Utils::printHeader("ADD QUESTIONS TO EXAM");

        ExamTemplate examTemplate = dbManager->getExamTemplateById(examId);
        cout << " Adding questions to: " << examTemplate.getTemplateName() << endl;
        cout << " Subject: " << examTemplate.getSubject() << endl;

        // Use the existing ExamCreator to add questions
        ExamCreator examCreator(dbManager, currentAdmin);
        examCreator.addQuestionsToExistingExam(examTemplate);
    }

    void removeQuestionsFromExam(int examId)
    {
        Utils::clearScreen();
        Utils::printHeader("REMOVE QUESTIONS FROM EXAM");

        auto questions = dbManager->getExamQuestions(examId);
        if (questions.empty())
        {
            cout << " No questions found for this exam!" << endl;
            Utils::pauseSystem();
            return;
        }

        cout << " Select questions to remove:" << endl;
        cout << string(60, '-') << endl;

        for (size_t i = 0; i < questions.size(); ++i)
        {
            cout << "\n[" << (i + 1) << "] ";
            cout << questions[i].getQuestionText().substr(0, 50);
            if (questions[i].getQuestionText().length() > 50)
                cout << "...";
            cout << endl;
        }

        cout << "\nEnter question number to remove (0 to cancel): ";
        int choice;
        cin >> choice;

        if (choice == 0 || choice > static_cast<int>(questions.size()))
        {
            return;
        }

        cout << "\nAre you sure you want to remove this question? (y/N): ";
        char confirm;
        cin >> confirm;

        if (confirm == 'y' || confirm == 'Y')
        {
            if (dbManager->deleteExamQuestion(questions[choice - 1].getId()))
            {
                cout << "\n✓ Question removed successfully!" << endl;
            }
            else
            {
                cout << "\n✗ Failed to remove question!" << endl;
            }
        }

        Utils::pauseSystem();
    }

    void deleteExam()
    {
        Utils::clearScreen();
        Utils::printHeader("DELETE EXAM");

        cout << "Enter Exam ID to delete: ";
        int examId;
        cin >> examId;

        ExamTemplate examTemplate = dbManager->getExamTemplateById(examId);
        if (examTemplate.getId() == 0)
        {
            cout << " Exam not found!" << endl;
            Utils::pauseSystem();
            return;
        }

        cout << "\n Exam to delete:" << endl;
        cout << string(50, '-') << endl;
        cout << "Name: " << examTemplate.getTemplateName() << endl;
        cout << "Type: " << examTemplate.getExamTypeString() << endl;
        cout << "Subject: " << examTemplate.getSubject() << endl;

        int questionCount = dbManager->getExamQuestionCount(examId);
        cout << "Questions: " << questionCount << endl;

        cout << "\n  WARNING: This will permanently delete the exam and all its questions!" << endl;
        cout << "Are you sure you want to delete this exam? (y/N): ";
        char confirm;
        cin >> confirm;

        if (confirm == 'y' || confirm == 'Y')
        {
            if (dbManager->deleteExamTemplate(examId))
            {
                cout << "\n Exam deleted successfully!" << endl;
            }
            else
            {
                cout << "\n Failed to delete exam!" << endl;
            }
        }
        else
        {
            cout << "\nDeletion cancelled." << endl;
        }

        Utils::pauseSystem();
    }

    void toggleExamStatus()
    {
        Utils::clearScreen();
        Utils::printHeader("ACTIVATE/DEACTIVATE EXAM");

        cout << "Enter Exam ID: ";
        int examId;
        cin >> examId;

        ExamTemplate examTemplate = dbManager->getExamTemplateById(examId);
        if (examTemplate.getId() == 0)
        {
            cout << " Exam not found!" << endl;
            Utils::pauseSystem();
            return;
        }

        cout << "\n Current Status: " << (examTemplate.getIsActive() ? "✅ Active" : "❌ Inactive") << endl;
        cout << " Exam: " << examTemplate.getTemplateName() << endl;

        cout << "\nChange status to " << (examTemplate.getIsActive() ? "Inactive" : "Active") << "? (y/N): ";
        char confirm;
        cin >> confirm;

        if (confirm == 'y' || confirm == 'Y')
        {
            bool success;
            if (examTemplate.getIsActive())
            {
                success = dbManager->deactivateExamTemplate(examId);
            }
            else
            {
                success = dbManager->activateExamTemplate(examId);
            }

            if (success)
            {
                cout << "\n Exam status updated successfully!" << endl;
            }
            else
            {
                cout << "\n Failed to update exam status!" << endl;
            }
        }
        else
        {
            cout << "\nOperation cancelled." << endl;
        }

        Utils::pauseSystem();
    }

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

    void examTemplateManagement()
    {
        while (true)
        {
            Utils::clearScreen();
            Utils::printHeader("EXAM TEMPLATE MANAGEMENT");
            cout << " Manage Quiz, Worksheet, and Final Exam Templates" << endl;
            cout << string(60, '-') << endl;
            cout << "1. Create New Template" << endl;
            cout << "2. View All Templates" << endl;
            cout << "3. View Templates by Type (Quiz/Worksheet/Final)" << endl;
            cout << "4. Edit Template" << endl;
            cout << "5. Delete Template" << endl;
            cout << "6. Activate/Deactivate Template" << endl;
            cout << "7. Back to Main Menu" << endl;

            cout << "\nEnter your choice: ";
            int choice;
            cin >> choice;

            switch (choice)
            {
            case 1:
                createExamTemplate();
                break;
            case 2:
                viewAllTemplates();
                break;
            case 3:
                viewTemplatesByType();
                break;
            case 4:
                editExamTemplate();
                break;
            case 5:
                deleteExamTemplate();
                break;
            case 6:
                toggleTemplateStatus();
                break;
            case 7:
                return;
            default:
                cout << "Invalid choice!" << endl;
                Utils::pauseSystem();
            }
        }
    }

    void createExamTemplate()
    {
        Utils::clearScreen();
        Utils::printHeader("CREATE EXAM TEMPLATE");

        ExamTemplate newTemplate;
        string templateName, subject, difficulty, instructions;
        int examTypeChoice, questionCount, timeLimit;
        double passingPercentage;

        cout << "Template Name: ";
        cin.ignore();
        getline(cin, templateName);
        newTemplate.setTemplateName(templateName);

        cout << "\nSelect Exam Type:" << endl;
        cout << "1. QUIZ (Short assessment, 10-20 questions)" << endl;
        cout << "2. WORKSHEET (Practice/homework, 20-30 questions)" << endl;
        cout << "3. FINAL (Comprehensive exam, 40-100 questions)" << endl;
        examTypeChoice = Utils::getSafeInt("Enter choice (1-3): ", 1, 3);

        switch (examTypeChoice)
        {
        case 1:
            newTemplate.setExamType(ExamType::QUIZ);
            break;
        case 2:
            newTemplate.setExamType(ExamType::WORKSHEET);
            break;
        case 3:
            newTemplate.setExamType(ExamType::FINAL);
            break;
        default:
            cout << "Invalid choice! Defaulting to QUIZ." << endl;
            newTemplate.setExamType(ExamType::QUIZ);
        }

        cout << "\nSubject: ";
        cin.ignore();
        getline(cin, subject);
        newTemplate.setSubject(subject);

        questionCount = Utils::getSafeInt("Number of Questions (1-200): ", 1, 200);
        newTemplate.setQuestionCount(questionCount);

        timeLimit = Utils::getSafeInt("Time Limit (in minutes, 1-600): ", 1, 600);
        newTemplate.setTimeLimit(timeLimit);

        cout << "\nDifficulty Level:" << endl;
        cout << "1. Easy" << endl;
        cout << "2. Medium" << endl;
        cout << "3. Hard" << endl;
        cout << "4. Mixed" << endl;
        int diffChoice = Utils::getSafeInt("Enter choice (1-4): ", 1, 4);
        switch (diffChoice)
        {
        case 1:
            difficulty = "Easy";
            break;
        case 2:
            difficulty = "Medium";
            break;
        case 3:
            difficulty = "Hard";
            break;
        case 4:
            difficulty = "Mixed";
            break;
        default:
            difficulty = "Medium";
        }
        newTemplate.setDifficulty(difficulty);

        passingPercentage = Utils::getSafeDouble("Passing Percentage (0-100): ", 0.0, 100.0);
        newTemplate.setPassingPercentage(passingPercentage);

        cout << "\nAdditional Settings:" << endl;
        char choice;

        cout << "Enable Negative Marking? (y/N): ";
        cin >> choice;
        newTemplate.setNegativeMarking(choice == 'y' || choice == 'Y');

        if (newTemplate.hasNegativeMarking())
        {
            double negValue = Utils::getSafeDouble("Negative Mark Value (0.0-1.0, e.g., 0.25): ", 0.0, 1.0);
            newTemplate.setNegativeMarkValue(negValue);
        }

        cout << "Shuffle Questions? (Y/n): ";
        cin >> choice;
        newTemplate.setShuffleQuestions(choice != 'n' && choice != 'N');

        cout << "Shuffle Options? (y/N): ";
        cin >> choice;
        newTemplate.setShuffleOptions(choice == 'y' || choice == 'Y');

        cout << "Allow Review? (Y/n): ";
        cin >> choice;
        newTemplate.setAllowReview(choice != 'n' && choice != 'N');

        cout << "Auto Submit on Time Expiry? (Y/n): ";
        cin >> choice;
        newTemplate.setAutoSubmit(choice != 'n' && choice != 'N');

        cout << "\nInstructions for students (optional): ";
        cin.ignore();
        getline(cin, instructions);
        newTemplate.setInstructions(instructions);

        newTemplate.setCreatedBy(currentAdmin.getId());
        newTemplate.setIsActive(true);

        // Validate and save
        if (!newTemplate.isValid())
        {
            cout << "\n✗ Invalid template data! Please check all required fields." << endl;
            Utils::pauseSystem();
            return;
        }

        if (dbManager->insertExamTemplate(newTemplate))
        {
            cout << "\n✓ Exam template created successfully!" << endl;
            cout << "\nTemplate Summary:" << endl;
            cout << string(50, '-') << endl;
            newTemplate.display();
        }
        else
        {
            cout << "\n✗ Failed to create exam template!" << endl;
        }

        Utils::pauseSystem();
    }

    void viewAllTemplates()
    {
        Utils::clearScreen();
        Utils::printHeader("ALL EXAM TEMPLATES");

        auto templates = dbManager->getAllExamTemplates();
        if (templates.empty())
        {
            cout << "No exam templates found!" << endl;
            cout << "Create your first template to get started." << endl;
        }
        else
        {
            cout << "Total Templates: " << templates.size() << endl;
            cout << string(90, '=') << endl;

            for (size_t i = 0; i < templates.size(); ++i)
            {
                cout << "\n[" << (i + 1) << "] ";
                cout << (templates[i].getIsActive() ? "✓ " : "✗ ");
                cout << templates[i].getTemplateName() << endl;
                cout << string(90, '-') << endl;
                templates[i].display();
                cout << string(90, '=') << endl;
            }
        }

        Utils::pauseSystem();
    }

    void viewTemplatesByType()
    {
        Utils::clearScreen();
        Utils::printHeader("VIEW TEMPLATES BY TYPE");

        cout << "Select Exam Type:" << endl;
        cout << "1. QUIZ" << endl;
        cout << "2. WORKSHEET" << endl;
        cout << "3. FINAL" << endl;
        cout << "\nEnter choice: ";
        int choice;
        cin >> choice;

        string examType;
        switch (choice)
        {
        case 1:
            examType = "QUIZ";
            break;
        case 2:
            examType = "WORKSHEET";
            break;
        case 3:
            examType = "FINAL";
            break;
        default:
            cout << "Invalid choice!" << endl;
            Utils::pauseSystem();
            return;
        }

        auto templates = dbManager->getExamTemplatesByType(examType);

        cout << "\n"
             << examType << " Templates (" << templates.size() << " found):" << endl;
        cout << string(80, '=') << endl;

        if (templates.empty())
        {
            cout << "No " << examType << " templates found!" << endl;
        }
        else
        {
            for (size_t i = 0; i < templates.size(); ++i)
            {
                cout << "\n[" << (i + 1) << "] " << templates[i].getTemplateName() << endl;
                cout << string(80, '-') << endl;
                templates[i].display();
                cout << string(80, '=') << endl;
            }
        }

        Utils::pauseSystem();
    }

    void editExamTemplate()
    {
        Utils::clearScreen();
        Utils::printHeader("EDIT EXAM TEMPLATE");

        cout << "Enter Template ID to edit: ";
        int templateId;
        cin >> templateId;

        ExamTemplate examTemplate = dbManager->getExamTemplateById(templateId);
        if (examTemplate.getId() == 0)
        {
            cout << "Template not found!" << endl;
            Utils::pauseSystem();
            return;
        }

        cout << "\nCurrent Template:" << endl;
        cout << string(50, '-') << endl;
        examTemplate.displayDetailed();

        cout << "\n\nEnter new values (press Enter to keep current):" << endl;

        string input;
        cin.ignore();

        cout << "Template Name [" << examTemplate.getTemplateName() << "]: ";
        getline(cin, input);
        if (!input.empty())
            examTemplate.setTemplateName(input);

        cout << "Subject [" << examTemplate.getSubject() << "]: ";
        getline(cin, input);
        if (!input.empty())
            examTemplate.setSubject(input);

        cout << "Question Count [" << examTemplate.getQuestionCount() << "]: ";
        getline(cin, input);
        if (!input.empty())
            examTemplate.setQuestionCount(stoi(input));

        cout << "Time Limit [" << examTemplate.getTimeLimit() << "]: ";
        getline(cin, input);
        if (!input.empty())
            examTemplate.setTimeLimit(stoi(input));

        cout << "Passing Percentage [" << examTemplate.getPassingPercentage() << "]: ";
        getline(cin, input);
        if (!input.empty())
            examTemplate.setPassingPercentage(stod(input));

        if (dbManager->updateExamTemplate(examTemplate))
        {
            cout << "\n✓ Template updated successfully!" << endl;
        }
        else
        {
            cout << "\n✗ Failed to update template!" << endl;
        }

        Utils::pauseSystem();
    }

    void deleteExamTemplate()
    {
        Utils::clearScreen();
        Utils::printHeader("DELETE EXAM TEMPLATE");

        cout << "Enter Template ID to delete: ";
        int templateId;
        cin >> templateId;

        ExamTemplate examTemplate = dbManager->getExamTemplateById(templateId);
        if (examTemplate.getId() == 0)
        {
            cout << "Template not found!" << endl;
            Utils::pauseSystem();
            return;
        }

        cout << "\nTemplate to delete:" << endl;
        cout << string(50, '-') << endl;
        examTemplate.display();

        cout << "\n  WARNING: This will permanently delete the template!" << endl;
        cout << "Are you sure you want to delete this template? (y/N): ";
        char confirm;
        cin >> confirm;

        if (confirm == 'y' || confirm == 'Y')
        {
            if (dbManager->deleteExamTemplate(templateId))
            {
                cout << "\n Template deleted successfully!" << endl;
            }
            else
            {
                cout << "\n Failed to delete template!" << endl;
            }
        }
        else
        {
            cout << "\nDeletion cancelled." << endl;
        }

        Utils::pauseSystem();
    }

    void toggleTemplateStatus()
    {
        Utils::clearScreen();
        Utils::printHeader("ACTIVATE/DEACTIVATE TEMPLATE");

        cout << "Enter Template ID: ";
        int templateId;
        cin >> templateId;

        ExamTemplate examTemplate = dbManager->getExamTemplateById(templateId);
        if (examTemplate.getId() == 0)
        {
            cout << "Template not found!" << endl;
            Utils::pauseSystem();
            return;
        }

        cout << "\nCurrent Status: " << (examTemplate.getIsActive() ? "Active" : "Inactive") << endl;
        cout << "Template: " << examTemplate.getTemplateName() << endl;

        cout << "\nChange status to " << (examTemplate.getIsActive() ? "Inactive" : "Active") << "? (y/N): ";
        char confirm;
        cin >> confirm;

        if (confirm == 'y' || confirm == 'Y')
        {
            bool success;
            if (examTemplate.getIsActive())
            {
                success = dbManager->deactivateExamTemplate(templateId);
            }
            else
            {
                success = dbManager->activateExamTemplate(templateId);
            }

            if (success)
            {
                cout << "\n Template status updated successfully!" << endl;
            }
            else
            {
                cout << "\n Failed to update template status!" << endl;
            }
        }
        else
        {
            cout << "\nOperation cancelled." << endl;
        }

        Utils::pauseSystem();
    }

    void addQuestion()
    {
        Utils::clearScreen();
        Utils::printHeader("ADD NEW QUESTION");

        string subject, questionText, option1, option2, option3, option4, difficulty, explanation;

        cout << "Subject: ";
        cin.ignore();
        getline(cin, subject);

        cout << "Question Text: ";
        getline(cin, questionText);

        cout << " a: ";
        getline(cin, option1);

        cout << "b: ";
        getline(cin, option2);

        cout << " c: ";
        getline(cin, option3);

        cout << " d: ";
        getline(cin, option4);

        cout << "Correct Answer (a-d): ";
        string correctAnswerInput;
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

        cout << "Difficulty (Easy/Medium/Hard): ";
        cin >> difficulty;

        cout << "Explanation (optional): ";
        cin.ignore();
        getline(cin, explanation);

        // Validate input
        if (correctAnswer < 0 || correctAnswer > 3)
        {
            cout << "\n Invalid correct answer! Must be a-d." << endl;
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
            cout << "\n Invalid question data! Please check all fields are filled." << endl;
            Utils::pauseSystem();
            return;
        }

        if (dbManager->insertQuestion(newQuestion))
        {
            cout << "\n Question added successfully!" << endl;
        }
        else
        {
            cout << "\n Failed to add question!" << endl;
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
                cout << "\n Question deleted successfully!" << endl;
            }
            else
            {
                cout << "\n Failed to delete question!" << endl;
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
        while (true)
        {
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

            switch (choice)
            {
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
        cout << "\n"
             << string(88, '=') << "=" << endl;
        cout << "=" << setw(44) << "ALL USERS" << setw(44) << "=" << endl;
        cout << "=" << string(88, '=') << "=" << endl;

        auto users = dbManager->getAllUsers();
        cout << "\n Total Users Found: " << users.size() << endl;

        if (users.empty())
        {
            cout << "\n No users found in database!" << endl;
            Utils::pauseSystem();
            return;
        }

        cout << "\n User List:" << endl;
        cout << "" << endl;
        cout << " " << left << setw(3) << "ID"
             << "  " << setw(13) << "Username"
             << "  " << setw(23) << "Full Name"
             << "  " << setw(18) << "Email"
             << "  " << setw(10) << "Role"
             << "  " << setw(8) << "Status" << " │" << endl;
        cout << "" << endl;

        for (const auto &user : users)
        {
            string username = user.getUsername();
            string fullName = user.getFullName();
            string email = user.getEmail();

            // Truncate long strings to fit columns
            if (username.length() > 13)
                username = username.substr(0, 10) + "...";
            if (fullName.length() > 23)
                fullName = fullName.substr(0, 20) + "...";
            if (email.length() > 18)
                email = email.substr(0, 15) + "...";

            cout << " " << left << setw(3) << user.getId()
                 << "  " << setw(13) << username
                 << "  " << setw(23) << fullName
                 << "  " << setw(18) << email
                 << "  " << setw(10) << user.roleToString()
                 << "  " << setw(8) << user.statusToString() << " │" << endl;
        }

        cout << " " << endl;
        cout << "\n User list displayed successfully!" << endl;
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

        if (user.getId() == 0)
        {
            cout << "\nUser '" << username << "' not found!" << endl;
            cout << "\nAvailable users:" << endl;
            auto allUsers = dbManager->getAllUsers();
            for (const auto &u : allUsers)
            {
                cout << "  - " << u.getUsername() << " (" << u.roleToString() << ")" << endl;
            }
        }
        else
        {
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
            if (user.getRole() == UserRole::STUDENT)
            {
                auto results = dbManager->getExamResultsByUser(user.getId());
                cout << "\nExam Statistics:" << endl;
                cout << "Total Exams: " << results.size() << endl;

                if (!results.empty())
                {
                    double totalScore = 0;
                    for (const auto &result : results)
                    {
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

        for (const auto &user : users)
        {
            if (user.getRole() == UserRole::ADMIN)
                adminCount++;
            else if (user.getRole() == UserRole::STUDENT)
                studentCount++;

            if (user.getStatus() == UserStatus::ACTIVE)
                activeCount++;
            else
                inactiveCount++;
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
        if (user.getId() == 0)
        {
            cout << "\nUser not found!" << endl;
            Utils::pauseSystem();
            return;
        }

        if (user.getRole() != UserRole::STUDENT)
        {
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
        switch (choice)
        {
        case 1:
            newStatus = UserStatus::ACTIVE;
            break;
        case 2:
            newStatus = UserStatus::INACTIVE;
            break;
        case 3:
            newStatus = UserStatus::SUSPENDED;
            break;
        case 4:
            return;
        default:
            cout << "Invalid choice!" << endl;
            Utils::pauseSystem();
            return;
        }

        user.setStatus(newStatus);
        if (dbManager->updateUser(user))
        {
            cout << "\nUser status updated successfully!" << endl;
        }
        else
        {
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

    void viewExamResults()
    {
        Utils::clearScreen();
        Utils::printHeader("VIEW EXAM RESULTS");

        // First, show available exams
        auto templates = dbManager->getAllExamTemplates();
        if (templates.empty())
        {
            cout << "No exams found!" << endl;
            cout << "Create an exam first to view results." << endl;
            Utils::pauseSystem();
            return;
        }

        cout << "Available Exams:" << endl;
        cout << string(80, '=') << endl;
        for (size_t i = 0; i < templates.size(); ++i)
        {
            cout << "[" << templates[i].getId() << "] " << templates[i].getTemplateName() << endl;
            cout << "    Type: " << templates[i].getExamTypeString() << " | Subject: " << templates[i].getSubject() << endl;
            cout << string(80, '-') << endl;
        }

        cout << "\nEnter Exam ID to view results (0 to cancel): ";
        int examId;
        cin >> examId;

        if (examId == 0) return;

        // Find the selected exam template
        ExamTemplate selectedTemplate;
        bool found = false;
        for (const auto& tmpl : templates) {
            if (tmpl.getId() == examId) {
                selectedTemplate = tmpl;
                found = true;
                break;
            }
        }

        if (!found) {
            cout << "Invalid Exam ID!" << endl;
            Utils::pauseSystem();
            return;
        }

        // Get all exam results and filter by exam template
        auto allResults = dbManager->getAllExamResults();
        vector<ExamResult> examResults;
        
        for (const auto& result : allResults) {
            if (result.getTemplateName() == selectedTemplate.getTemplateName()) {
                examResults.push_back(result);
            }
        }

        Utils::clearScreen();
        Utils::printHeader("EXAM RESULTS - " + selectedTemplate.getTemplateName());

        cout << "Exam Details:" << endl;
        cout << string(80, '=') << endl;
        cout << "Template: " << selectedTemplate.getTemplateName() << endl;
        cout << "Type: " << selectedTemplate.getExamTypeString() << endl;
        cout << "Subject: " << selectedTemplate.getSubject() << endl;
        cout << "Time Limit: " << selectedTemplate.getTimeLimit() << " minutes" << endl;
        cout << "Passing Score: " << selectedTemplate.getPassingPercentage() << "%" << endl;
        cout << string(80, '=') << endl;

        if (examResults.empty())
        {
            cout << "\nNo students have taken this exam yet." << endl;
            cout << "Results will appear here once students complete the exam." << endl;
        }
        else
        {
            cout << "\nStudent Results (" << examResults.size() << " submissions):" << endl;
            cout << string(100, '-') << endl;
            cout << left << setw(15) << "Student" << setw(10) << "Score" << setw(12) << "Percentage" 
                 << setw(8) << "Grade" << setw(8) << "Status" << setw(20) << "Date" << setw(10) << "Duration" << endl;
            cout << string(100, '-') << endl;

            // Calculate statistics
            double totalPercentage = 0;
            int passedCount = 0;
            double highestScore = 0;
            double lowestScore = 100;
            string topStudent = "";

            for (const auto &result : examResults)
            {
                double percentage = result.getPercentage();
                totalPercentage += percentage;
                
                if (percentage >= selectedTemplate.getPassingPercentage()) {
                    passedCount++;
                }
                
                if (percentage > highestScore) {
                    highestScore = percentage;
                    topStudent = result.getUsername();
                }
                
                if (percentage < lowestScore) {
                    lowestScore = percentage;
                }

                string grade = getGradeFromPercentage(percentage);
                string status = (percentage >= selectedTemplate.getPassingPercentage()) ? "PASS" : "FAIL";

                cout << left << setw(15) << result.getUsername() 
                     << setw(10) << (to_string(result.getScore()) + "/" + to_string(result.getTotalQuestions()))
                     << setw(12) << (to_string((int)percentage) + "%")
                     << setw(8) << grade
                     << setw(8) << status
                     << setw(20) << result.getExamDate().substr(0, 16)
                     << setw(10) << (to_string(result.getDuration()) + "min") << endl;
            }

            cout << string(100, '-') << endl;

            // Show statistics
            cout << "\nExam Statistics:" << endl;
            cout << string(50, '=') << endl;
            cout << "Total Submissions: " << examResults.size() << endl;
            cout << "Average Score: " << fixed << setprecision(1) << (totalPercentage / examResults.size()) << "%" << endl;
            cout << "Pass Rate: " << fixed << setprecision(1) << ((double)passedCount / examResults.size() * 100) << "%" << endl;
            cout << "Highest Score: " << fixed << setprecision(1) << highestScore << "% (" << topStudent << ")" << endl;
            cout << "Lowest Score: " << fixed << setprecision(1) << lowestScore << "%" << endl;
            cout << "Students Passed: " << passedCount << "/" << examResults.size() << endl;
        }

        Utils::pauseSystem();
    }

    // Helper function to get grade from percentage
    string getGradeFromPercentage(double percentage) {
        if (percentage >= 97) return "A+";      // 97-100%
        else if (percentage >= 93) return "A";  // 93-96%
        else if (percentage >= 90) return "A-"; // 90-92%
        else if (percentage >= 87) return "B+"; // 87-89%
        else if (percentage >= 83) return "B";  // 83-86%
        else if (percentage >= 80) return "B-"; // 80-82%
        else if (percentage >= 77) return "C+"; // 77-79%
        else if (percentage >= 73) return "C";  // 73-76%
        else if (percentage >= 70) return "C-"; // 70-72%
        else if (percentage >= 67) return "D+"; // 67-69%
        else if (percentage >= 63) return "D";  // 63-66%
        else if (percentage >= 60) return "D-"; // 60-62%
        else return "F";                         // Below 60%
    }
};

#endif // ADMIN_STUB_H