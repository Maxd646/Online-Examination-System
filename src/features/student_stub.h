#ifndef STUDENT_STUB_H
#define STUDENT_STUB_H

#include "../authentication/user.h"
#include "../database/database.h"
#include "../structure/utils.h"
#include "../features/exam_template.h"
#include "../components/stack.h"
#include "../components/sorting.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <chrono>
#include <map>
#include <random>

// State enum for student navigation
enum class StudentState
{
    MAIN_MENU,
    TAKE_EXAM,
    PRACTICE,
    RESULTS,
    PROFILE,
    ANALYTICS,
    LOGOUT
};

// Enhanced StudentPanel with comprehensive exam functionality
class StudentPanel
{
private:
    DatabaseManager *dbManager;
    User currentStudent;
    UndoRedoStack<StudentState> navigationHistory; // DSA: UndoRedoStack for navigation history
    StudentState currentState;                     // Current screen state

public:
    StudentPanel(DatabaseManager *db, const User &student)
        : dbManager(db), currentStudent(student), currentState(StudentState::MAIN_MENU) {}
    void run()
    {
        while (currentState != StudentState::LOGOUT)
        {
            render();
        }
    }

private:
    // Central render function - 
    void render()
    {
        Utils::clearScreen();
        Utils::printHeader("STUDENT PANEL");
        cout << "Welcome " << currentStudent.getFullName() << endl;

        switch (currentState)
        {
        case StudentState::MAIN_MENU:
            showMainMenu();
            break;
        case StudentState::TAKE_EXAM:
            takeExam();
            navigationHistory.execute(currentState); // Save TAKE_EXAM state before returning
            currentState = StudentState::MAIN_MENU;  // Return to main menu after exam
            break;
        case StudentState::PRACTICE:
            practiceMode();
            navigationHistory.execute(currentState); // Save PRACTICE state before returning
            currentState = StudentState::MAIN_MENU;  // Return to main menu after practice
            break;
        case StudentState::RESULTS:
            viewMyResults();
            navigationHistory.execute(currentState); // Save RESULTS state before returning
            currentState = StudentState::MAIN_MENU;  // Return to main menu after viewing results
            break;
        case StudentState::PROFILE:
            viewProfile();
            navigationHistory.execute(currentState); // Save PROFILE state before returning
            currentState = StudentState::MAIN_MENU;  // Return to main menu after viewing profile
            break;
        case StudentState::ANALYTICS:
            performanceAnalytics();
            navigationHistory.execute(currentState); // Save ANALYTICS state before returning
            currentState = StudentState::MAIN_MENU;  // Return to main menu after analytics
            break;
        case StudentState::LOGOUT:
            cout << "\nLogging out..." << endl;
            return;
        }
    }

    // Main menu display and input handling
    void showMainMenu()
    {
        cout << "\nStudent Menu:" << endl;
        cout << " 1. Take Exam (Quiz/Worksheet/Final)" << endl;
        cout << " 2. Practice Mode" << endl;
        cout << " 3. View My Results" << endl;
        cout << " 4. View Profile" << endl;
        cout << " 5. Performance Analytics" << endl;
        if (navigationHistory.canUndo())
        {
            cout << " 0. ← Back" << endl;
        }
        if (navigationHistory.canRedo())
        {
            cout << " 9. Next →" << endl;
        }
        cout << " 6. Logout" << endl;

        cout << "\nEnter your choice: ";
        int choice;
        cin >> choice;

        switch (choice)
        {
        case 0:
            if (navigationHistory.canUndo())
            {
                currentState = navigationHistory.undo();
            }
            else
            {
                cout << "\n✗ Cannot go back!" << endl;
                Utils::pauseSystem();
            }
            break;
        case 1:
            navigationHistory.execute(currentState); // Save current state before navigation
            currentState = StudentState::TAKE_EXAM;
            break;
        case 2:
            navigationHistory.execute(currentState);
            currentState = StudentState::PRACTICE;
            break;
        case 3:
            navigationHistory.execute(currentState);
            currentState = StudentState::RESULTS;
            break;
        case 4:
            navigationHistory.execute(currentState);
            currentState = StudentState::PROFILE;
            break;
        case 5:
            navigationHistory.execute(currentState);
            currentState = StudentState::ANALYTICS;
            break;
        case 9:
            if (navigationHistory.canRedo())
            {
                currentState = navigationHistory.redo();
            }
            else
            {
                cout << "\n✗ Cannot go forward!" << endl;
                Utils::pauseSystem();
            }
            break;
        case 6:
            currentState = StudentState::LOGOUT;
            break;
        default:
            cout << "Invalid choice! Please try again." << endl;
            Utils::pauseSystem();
        }
    }

private:
    // Helper function to convert answer input (supports both a-d and 1-4)
    int parseAnswerInput(const string &input)
    {
        if (input.length() == 1)
        {
            char c = tolower(input[0]);
            if (c >= 'a' && c <= 'd')
            {
                return c - 'a' + 1; // Convert a-d to 1-4
            }
            if (c >= '1' && c <= '4')
            {
                return c - '0'; // Convert '1'-'4' to 1-4
            }
        }
        // Try to parse as number for special commands
        try
        {
            return stoi(input);
        }
        catch (...)
        {
            return -999; // Invalid input
        }
    }

    // DSA: Custom randomization using Merge Sort
    void randomizeQuestions(vector<Question> &questions)
    {
        if (questions.empty())
            return;

        // Create pairs of (random_value, question) for sorting
        vector<pair<int, Question>> randomPairs;
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dis(1, 100000);

        for (const auto &q : questions)
        {
            randomPairs.push_back({dis(gen), q});
        }

        // DSA: Use Merge Sort with custom comparator to sort by random values
        SortingAlgorithms<pair<int, Question>>::mergeSortCustom(
            randomPairs, 0, randomPairs.size() - 1,
            [](const pair<int, Question> &a, const pair<int, Question> &b)
            {
                return a.first < b.first; // Sort by random value
            });

        // Extract sorted questions (now randomized)
        questions.clear();
        for (const auto &pair : randomPairs)
        {
            questions.push_back(pair.second);
        }
    }

    void takeExam()
    {
        Utils::clearScreen();
        Utils::printHeader("TAKE EXAM");

        cout << " Available Exam Templates:" << endl;
        cout << string(80, '=') << endl;

        // Get all active exam templates
        auto templates = dbManager->getActiveExamTemplates();
        if (templates.empty())
        {
            cout << " No exam templates available!" << endl;
            cout << "Please contact your instructor to createtes." << endl;
            Utils::pauseSystem();
            return;
        }

        // Display templates by type
        map<string, vector<ExamTemplate>> templatesByType;
        for (const auto &tmpl : templates)
        {
            templatesByType[tmpl.getExamTypeString()].push_back(tmpl);
        }

        cout << "\n Select Exam Type:" << endl;
        cout << "1.  QUIZ (Quick assessments)" << endl;
        cout << "2.  WORKSHEET (Practice exercises)" << endl;
        cout << "3.  FINAL (Comprehensive exams)" << endl;
        cout << "4.  Custom Exam (Configure yourself)" << endl;
        cout << "5.   Back to Main Menu" << endl;

        cout << "\nEnter your choice: ";
        int typeChoice;
        cin >> typeChoice;

        switch (typeChoice)
        {
        case 1:
            selectAndTakeTemplateExam("QUIZ", templatesByType["QUIZ"]);
            break;
        case 2:
            selectAndTakeTemplateExam("WORKSHEET", templatesByType["WORKSHEET"]);
            break;
        case 3:
            selectAndTakeTemplateExam("FINAL", templatesByType["FINAL"]);
            break;
        case 4:
            takeCustomExam();
            break;
        case 5:
            return;
        default:
            cout << "Invalid choice!" << endl;
            Utils::pauseSystem();
        }
    }

    void selectAndTakeTemplateExam(const string &examType, const vector<ExamTemplate> &templates)
    {
        Utils::clearScreen();
        Utils::printHeader(examType + " EXAMS");

        if (templates.empty())
        {
            cout << " No " << examType << " templates available!" << endl;
            Utils::pauseSystem();
            return;
        }

        cout << " Available " << examType << " Templates:" << endl;
        cout << string(80, '-') << endl;

        for (size_t i = 0; i < templates.size(); ++i)
        {
            cout << "\n[" << (i + 1) << "] " << templates[i].getTemplateName() << endl;
            cout << "     Subject: " << templates[i].getSubject() << endl;
            cout << "     Questions: " << templates[i].getQuestionCount() << endl;
            cout << "     Time Limit: " << templates[i].getTimeLimit() << " minutes" << endl;
            cout << "     Passing: " << templates[i].getPassingPercentage() << "%" << endl;
            cout << "     Difficulty: " << templates[i].getDifficulty() << endl;
            if (!templates[i].getInstructions().empty())
            {
                cout << "     Instructions: " << templates[i].getInstructions() << endl;
            }
            cout << string(80, '-') << endl;
        }

        cout << "\nSelect template (1-" << templates.size() << ") or 0 to go back: ";
        int choice;
        cin >> choice;

        if (choice == 0)
            return;

        if (choice < 1 || choice > static_cast<int>(templates.size()))
        {
            cout << "Invalid choice!" << endl;
            Utils::pauseSystem();
            return;
        }

        const ExamTemplate &selectedTemplate = templates[choice - 1];

        // Show exam preview and confirmation
        Utils::clearScreen();
        Utils::printHeader("EXAM CONFIRMATION");

        cout << " You are about to start:" << endl;
        cout << string(50, '=') << endl;
        selectedTemplate.display();
        cout << string(50, '=') << endl;

        cout << "\n  Important Notes:" << endl;
        cout << "• Once started, the timer cannot be paused" << endl;
        cout << "• Auto-submit is " << (selectedTemplate.isAutoSubmit() ? "ENABLED" : "DISABLED") << endl;
        cout << "• Review is " << (selectedTemplate.isReviewAllowed() ? "ALLOWED" : "NOT ALLOWED") << endl;
        if (selectedTemplate.hasNegativeMarking())
        {
            cout << "• Negative marking: -" << selectedTemplate.getNegativeMarkValue() << " for wrong answers" << endl;
        }

        cout << "\n Ready to start? (y/N): ";
        char confirm;
        cin >> confirm;

        if (confirm == 'y' || confirm == 'Y')
        {
            startTemplateExam(selectedTemplate);
        }
    }

    void startTemplateExam(const ExamTemplate &examTemplate)
    {
        // Get questions specific to this exam template
        auto examQuestions = dbManager->getExamQuestions(examTemplate.getId());

        // Convert ExamQuestion to Question for compatibility
        vector<Question> questions;
        for (const auto &eq : examQuestions)
        {
            Question q;
            q.setId(eq.getId());
            q.setQuestionText(eq.getQuestionText());
            q.setOptions(eq.getOptions());
            q.setCorrectAnswer(eq.getCorrectAnswer());
            q.setExplanation(eq.getExplanation());
            q.setSubject(examTemplate.getSubject());
            q.setDifficulty(examTemplate.getDifficulty());
            questions.push_back(q);
        }

        if (questions.empty())
        {
            cout << "\n No questions available for this exam template!" << endl;
            cout << "Please contact your instructor." << endl;
            Utils::pauseSystem();
            return;
        }

        // DSA: Randomize questions using Merge Sort if enabled
        if (examTemplate.shouldShuffleQuestions())
        {
            randomizeQuestions(questions); // DSA: Merge Sort randomization
        }

        // Start the exam with template settings
        conductTemplateExam(questions, examTemplate);
    }

    void conductTemplateExam(const vector<Question> &questions, const ExamTemplate &examTemplate)
    {
        Utils::clearScreen();
        Utils::printHeader("EXAM IN PROGRESS - " + examTemplate.getTemplateName());

        vector<int> userAnswers(questions.size(), -1);
        vector<bool> answered(questions.size(), false);
        vector<bool> markedForReview(questions.size(), false);
        auto startTime = chrono::steady_clock::now();

        cout << " Exam Started!" << endl;
        cout << " Template: " << examTemplate.getTemplateName() << endl;
        cout << " Subject: " << examTemplate.getSubject() << endl;
        cout << " Questions: " << questions.size() << endl;
        cout << " Time Limit: " << examTemplate.getTimeLimit() << " minutes" << endl;
        cout << " Passing Score: " << examTemplate.getPassingPercentage() << "%" << endl;

        if (examTemplate.hasNegativeMarking())
        {
            cout << "  Negative Marking: -" << examTemplate.getNegativeMarkValue() << " per wrong answer" << endl;
        }

        cout << "\n Instructions:" << endl;
        cout << "• Enter a-d or 1-4 for your answer" << endl;
        cout << "• Enter 0 to skip question" << endl;
        cout << "• Enter -1 to go back to previous question" << endl;
        if (examTemplate.isReviewAllowed())
        {
            cout << "• Enter -3 to mark/unmark for review" << endl;
        }
        cout << "• Enter -2 to submit exam" << endl;

        if (!examTemplate.getInstructions().empty())
        {
            cout << "\n Special Instructions:" << endl;
            cout << examTemplate.getInstructions() << endl;
        }

        cout << "\nPress Enter to start the timer...";
        cin.ignore();
        cin.get();

        int currentQuestion = 0;
        bool examCompleted = false;

        while (!examCompleted && currentQuestion < static_cast<int>(questions.size()))
        {
            // Check time limit
            auto currentTime = chrono::steady_clock::now();
            auto elapsed = chrono::duration_cast<chrono::minutes>(currentTime - startTime);
            if (elapsed.count() >= examTemplate.getTimeLimit())
            {
                cout << "\n Time's up! ";
                if (examTemplate.isAutoSubmit())
                {
                    cout << "Exam will be submitted automatically." << endl;
                    break;
                }
                else
                {
                    cout << "Please submit your exam." << endl;
                }
            }

            Utils::clearScreen();

            // Show progress and status
            int answeredCount = 0, reviewCount = 0;
            for (size_t i = 0; i < answered.size(); ++i)
            {
                if (answered[i])
                    answeredCount++;
                if (markedForReview[i])
                    reviewCount++;
            }

            cout << " " << examTemplate.getTemplateName() << " | ";
            cout << "" << (currentQuestion + 1) << "/" << questions.size() << " | ";
            cout << " " << answeredCount << " | ";
            if (examTemplate.isReviewAllowed())
            {
                cout << "🔍 " << reviewCount << " | ";
            }

            // Show remaining time
            int remaining = examTemplate.getTimeLimit() - elapsed.count();
            cout << " " << remaining << "min" << endl;

            cout << string(80, '=') << endl;

            // Display question
            questions[currentQuestion].display();

            // Show current answer and review status
            if (answered[currentQuestion])
            {
                char optionLabels[] = {'a', 'b', 'c', 'd'};
                cout << "\n Current Answer: " << optionLabels[userAnswers[currentQuestion]] << ". " << questions[currentQuestion].getOptions()[userAnswers[currentQuestion]] << endl;
            }
            if (examTemplate.isReviewAllowed() && markedForReview[currentQuestion])
            {
                cout << " Marked for Review" << endl;
            }

            cout << "\nYour choice (a-d, 0=skip, -1=previous, -2=submit): ";
            string inputStr;
            cin >> inputStr;
            int answer = parseAnswerInput(inputStr);

            if (answer == -2)
            {
                // Submit exam
                cout << "\n Exam Summary:" << endl;
                cout << "Answered: " << answeredCount << "/" << questions.size() << endl;
                if (examTemplate.isReviewAllowed())
                {
                    cout << "Marked for Review: " << reviewCount << endl;
                }
                cout << "\n Submit exam? (y/N): ";
                char confirm;
                cin >> confirm;
                if (confirm == 'y' || confirm == 'Y')
                {
                    examCompleted = true;
                }
            }
            else if (answer == -3 && examTemplate.isReviewAllowed())
            {
                // Toggle review mark
                markedForReview[currentQuestion] = !markedForReview[currentQuestion];
                cout << (markedForReview[currentQuestion] ? "🔍 Marked for review" : "✅ Review mark removed") << endl;
                Utils::pauseSystem();
            }
            else if (answer == -1)
            {
                // Previous question
                if (currentQuestion > 0)
                {
                    currentQuestion--;
                }
            }
            else if (answer == 0)
            {
                // Skip question
                currentQuestion++;
            }
            else if (answer >= 1 && answer <= 4)
            {
                // Answer question
                userAnswers[currentQuestion] = answer - 1;
                answered[currentQuestion] = true;
                currentQuestion++;
            }
            else
            {
                cout << " Invalid input! Please try again." << endl;
                Utils::pauseSystem();
            }
        }

        // Calculate results with template settings
        auto endTime = chrono::steady_clock::now();
        auto duration = chrono::duration_cast<chrono::minutes>(endTime - startTime);

        double score = 0;
        int correctCount = 0;

        for (size_t i = 0; i < questions.size(); ++i)
        {
            if (answered[i] && userAnswers[i] == questions[i].getCorrectAnswer())
            {
                score += 1.0;
                correctCount++;
            }
            else if (answered[i] && examTemplate.hasNegativeMarking())
            {
                score -= examTemplate.getNegativeMarkValue();
            }
        }

        // Ensure score is not negative
        if (score < 0)
            score = 0;

        double percentage = (score * 100.0) / questions.size();
        bool passed = percentage >= examTemplate.getPassingPercentage();

        // Save result to database with template information
        ExamResult result(currentStudent.getId(), currentStudent.getUsername(),
                          static_cast<int>(score), questions.size(), examTemplate.getSubject());
        result.setDuration(duration.count());
        result.setExamType(examTemplate.getExamTypeString());
        result.setTemplateName(examTemplate.getTemplateName());
        result.setExamTemplateId(examTemplate.getId());
        result.setTimeLimit(examTemplate.getTimeLimit());
        result.setNegativeMarking(examTemplate.hasNegativeMarking());
        if (examTemplate.hasNegativeMarking())
        {
            result.setNegativeMarks((correctCount - score) * examTemplate.getNegativeMarkValue());
        }
        dbManager->insertExamResult(result);

        // Display results
        showTemplateExamResults(questions, userAnswers, answered, examTemplate,
                                score, percentage, duration.count(), passed);
    }

    void showTemplateExamResults(const vector<Question> &questions,
                                 const vector<int> &userAnswers,
                                 const vector<bool> &answered,
                                 const ExamTemplate &examTemplate,
                                 double score, double percentage,
                                 int duration, bool passed)
    {
        Utils::clearScreen();
        Utils::printHeader("EXAM RESULTS - " + examTemplate.getTemplateName());

        cout << " Exam Completed!" << endl;
        cout << string(80, '=') << endl;
        cout << " Template: " << examTemplate.getTemplateName() << endl;
        cout << " Subject: " << examTemplate.getSubject() << endl;
        cout << " Type: " << examTemplate.getExamTypeString() << endl;
        cout << " Score: " << score << "/" << questions.size() << endl;
        cout << " Percentage: " << percentage << "%" << endl;
        cout << " Duration: " << duration << " minutes" << endl;
        cout << " Required: " << examTemplate.getPassingPercentage() << "%" << endl;
        cout << " Grade: " << getGrade(percentage) << endl;
        cout << " Status: " << (passed ? " PASSED" : " FAILED") << endl;

        if (examTemplate.hasNegativeMarking())
        {
            int correctCount = 0;
            for (size_t i = 0; i < questions.size(); ++i)
            {
                if (answered[i] && userAnswers[i] == questions[i].getCorrectAnswer())
                {
                    correctCount++;
                }
            }
            double negativeMarks = (correctCount - score) * examTemplate.getNegativeMarkValue();
            cout << "  Negative Marks: " << negativeMarks << endl;
        }

        cout << string(80, '=') << endl;

        // ALWAYS show detailed review for ALL exam types (Quiz, Worksheet, Final)
        cout << "\n Detailed Answer Review:" << endl;
        cout << " All answers are displayed below for your learning:" << endl;
        cout << string(80, '-') << endl;

        for (size_t i = 0; i < questions.size(); ++i)
        {
            cout << "\nQ" << (i + 1) << ": " << questions[i].getQuestionText() << endl;

            // Display all options for reference
            auto options = questions[i].getOptions();
            char optionLabels[] = {'a', 'b', 'c', 'd'};
            for (size_t j = 0; j < options.size(); ++j)
            {
                string marker = "";
                if (j == static_cast<size_t>(questions[i].getCorrectAnswer()))
                {
                    marker = "  (Correct Answer)";
                }
                else if (answered[i] && j == static_cast<size_t>(userAnswers[i]))
                {
                    marker = "  (Your Answer)";
                }
                cout << "   " << optionLabels[j] << ". " << options[j] << marker << endl;
            }

            if (!answered[i])
            {
                cout << "\n    Your Answer:  Not answered" << endl;
                cout << "    Status:  Incorrect (0 points)" << endl;
            }
            else
            {
                cout << "\n    Your Answer: " << optionLabels[userAnswers[i]] << ". " << options[userAnswers[i]] << endl;
                if (userAnswers[i] == questions[i].getCorrectAnswer())
                {
                    cout << "    Status:  Correct (+1 point)" << endl;
                }
                else
                {
                    cout << "    Status:  Incorrect (0 points)" << endl;
                    if (examTemplate.hasNegativeMarking())
                    {
                        cout << "     Negative marking: -" << examTemplate.getNegativeMarkValue() << " points" << endl;
                    }
                }
            }
            cout << "    Correct Answer: " << optionLabels[questions[i].getCorrectAnswer()] << ". " << options[questions[i].getCorrectAnswer()] << endl;

            if (!questions[i].getExplanation().empty())
            {
                cout << "    Explanation: " << questions[i].getExplanation() << endl;
            }
            cout << string(80, '-') << endl;
        }

        cout << "\n Study Tip: Review the explanations above to improve your understanding!" << endl;

        Utils::pauseSystem();
    }

    void takeCustomExam()
    {
        Utils::clearScreen();
        Utils::printHeader("CUSTOM EXAM CONFIGURATION");

        cout << " Configure your custom exam:" << endl;

        cout << "\n1. Select Subject:" << endl;
        cout << "   1. Mathematics" << endl;
        cout << "   2. Computer Science (General)" << endl;
        cout << "   3. DSA (Data Structures & Algorithms)" << endl;
        cout << "   4. OOP (Object-Oriented Programming)" << endl;
        cout << "   5. COA (Computer Organization & Architecture)" << endl;
        cout << "   6. SAM (Software Architecture & Modeling)" << endl;
        cout << "   7. Database Systems" << endl;
        cout << "   8. Operating Systems" << endl;
        cout << "   9. Computer Networks" << endl;
        cout << "   10. Science" << endl;
        cout << "   11. General Knowledge" << endl;
        cout << "   12. Mixed (All subjects)" << endl;

        cout << "\nEnter choice (1-12): ";
        int subjectChoice;
        cin >> subjectChoice;

        string subject = "";
        switch (subjectChoice)
        {
        case 1:
            subject = "Mathematics";
            break;
        case 2:
            subject = "Computer Science";
            break;
        case 3:
            subject = "DSA";
            break;
        case 4:
            subject = "OOP";
            break;
        case 5:
            subject = "COA";
            break;
        case 6:
            subject = "SAM";
            break;
        case 7:
            subject = "Database Systems";
            break;
        case 8:
            subject = "Operating Systems";
            break;
        case 9:
            subject = "Computer Networks";
            break;
        case 10:
            subject = "Science";
            break;
        case 11:
            subject = "General Knowledge";
            break;
        case 12:
            subject = "";
            break; // Mixed
        default:
            cout << "Invalid choice!" << endl;
            Utils::pauseSystem();
            return;
        }

        cout << "\nNumber of questions (5-50): ";
        int questionCount;
        cin >> questionCount;
        if (questionCount < 5)
            questionCount = 5;
        if (questionCount > 50)
            questionCount = 50;

        cout << "Time limit in minutes (0 for no limit): ";
        int timeLimit;
        cin >> timeLimit;

        // Get questions
        vector<Question> questions;
        if (subject.empty())
        {
            questions = dbManager->getRandomQuestions(questionCount);
        }
        else
        {
            questions = dbManager->getRandomQuestions(questionCount, subject);
        }

        if (questions.empty())
        {
            cout << "\nNo questions available for the selected criteria!" << endl;
            Utils::pauseSystem();
            return;
        }

        // Create temporary template for custom exam
        ExamTemplate customTemplate;
        customTemplate.setTemplateName("Custom Exam");
        customTemplate.setExamType(ExamType::QUIZ);
        customTemplate.setSubject(subject.empty() ? "Mixed" : subject);
        customTemplate.setQuestionCount(questionCount);
        customTemplate.setTimeLimit(timeLimit > 0 ? timeLimit : 60);
        customTemplate.setPassingPercentage(60.0);
        customTemplate.setNegativeMarking(false);
        customTemplate.setShuffleQuestions(true);
        customTemplate.setAllowReview(true);
        customTemplate.setAutoSubmit(timeLimit > 0);

        // Start exam
        conductTemplateExam(questions, customTemplate);
    }

    void conductExam(const vector<Question> &questions, int timeLimit, const string &subject)
    {
        Utils::clearScreen();
        Utils::printHeader("EXAM IN PROGRESS");

        vector<int> userAnswers(questions.size(), -1);
        vector<bool> answered(questions.size(), false);
        auto startTime = chrono::steady_clock::now();

        cout << "Exam Started!" << endl;
        cout << "Questions: " << questions.size() << endl;
        if (timeLimit > 0)
        {
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

        while (!examCompleted && currentQuestion < static_cast<int>(questions.size()))
        {
            // Check time limit
            if (timeLimit > 0)
            {
                auto currentTime = chrono::steady_clock::now();
                auto elapsed = chrono::duration_cast<chrono::minutes>(currentTime - startTime);
                if (elapsed.count() >= timeLimit)
                {
                    cout << "\n Time's up! Exam will be submitted automatically." << endl;
                    break;
                }
            }

            Utils::clearScreen();

            // Show progress
            int answeredCount = 0;
            for (bool ans : answered)
            {
                if (ans)
                    answeredCount++;
            }

            cout << "Question " << (currentQuestion + 1) << " of " << questions.size()
                 << " | Answered: " << answeredCount << "/" << questions.size() << endl;

            if (timeLimit > 0)
            {
                auto currentTime = chrono::steady_clock::now();
                auto elapsed = chrono::duration_cast<chrono::minutes>(currentTime - startTime);
                int remaining = timeLimit - elapsed.count();
                cout << "Time Remaining: " << remaining << " minutes" << endl;
            }

            cout << string(60, '=') << endl;

            // Display question
            questions[currentQuestion].display();

            if (answered[currentQuestion])
            {
                char optionLabels[] = {'a', 'b', 'c', 'd'};
                cout << "\nCurrent Answer: " << optionLabels[userAnswers[currentQuestion]] << ". " << questions[currentQuestion].getOptions()[userAnswers[currentQuestion]] << endl;
            }

            cout << "\nYour answer (a-d, 0=skip, -1=previous, -2=submit): ";
            string inputStr;
            cin >> inputStr;
            int answer = parseAnswerInput(inputStr);

            if (answer == -2)
            {
                // Submit exam
                cout << "\nAre you sure you want to submit? (y/N): ";
                char confirm;
                cin >> confirm;
                if (confirm == 'y' || confirm == 'Y')
                {
                    examCompleted = true;
                }
            }
            else if (answer == -1)
            {
                // Previous question
                if (currentQuestion > 0)
                {
                    currentQuestion--;
                }
            }
            else if (answer == 0)
            {
                // Skip question
                currentQuestion++;
            }
            else if (answer >= 1 && answer <= 4)
            {
                // Answer question
                userAnswers[currentQuestion] = answer - 1;
                answered[currentQuestion] = true;
                currentQuestion++;
            }
            else
            {
                cout << "Invalid input! Please try again." << endl;
                Utils::pauseSystem();
            }
        }

        // Calculate results
        auto endTime = chrono::steady_clock::now();
        auto duration = chrono::duration_cast<chrono::minutes>(endTime - startTime);

        int score = 0;
        for (size_t i = 0; i < questions.size(); ++i)
        {
            if (answered[i] && userAnswers[i] == questions[i].getCorrectAnswer())
            {
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

    void showExamResults(const vector<Question> &questions,
                         const vector<int> &userAnswers,
                         const vector<bool> &answered,
                         int score, double percentage, int duration)
    {
        Utils::clearScreen();
        Utils::printHeader("EXAM RESULTS");

        cout << " Exam Completed!" << endl;
        cout << string(80, '=') << endl;
        cout << " Score: " << score << "/" << questions.size() << endl;
        cout << " Percentage: " << percentage << "%" << endl;
        cout << " Duration: " << duration << " minutes" << endl;
        cout << " Grade: " << getGrade(percentage) << endl;
        cout << " Status: " << (percentage >= 60 ? " PASSED" : " FAILED") << endl;
        cout << string(80, '=') << endl;

        // ALWAYS show detailed answer review for ALL exams
        cout << "\n Complete Answer Review:" << endl;
        cout << " All answers and explanations are provided below:" << endl;
        cout << string(80, '-') << endl;

        for (size_t i = 0; i < questions.size(); ++i)
        {
            cout << "\nQ" << (i + 1) << ": " << questions[i].getQuestionText() << endl;

            // Display all options for reference
            auto options = questions[i].getOptions();
            for (size_t j = 0; j < options.size(); ++j)
            {
                string marker = "";
                if (j == static_cast<size_t>(questions[i].getCorrectAnswer()))
                {
                    marker = "  (Correct Answer)";
                }
                else if (answered[i] && j == static_cast<size_t>(userAnswers[i]))
                {
                    marker = "  (Your Answer)";
                }
                char optionLabels[] = {'a', 'b', 'c', 'd'};
                cout << "   " << optionLabels[j] << ". " << options[j] << marker << endl;
            }

            if (!answered[i])
            {
                cout << "\n    Your Answer:  Not answered" << endl;
                cout << "    Status:  Incorrect (0 points)" << endl;
            }
            else
            {
                char optionLabels[] = {'a', 'b', 'c', 'd'};
                cout << "\n    Your Answer: " << optionLabels[userAnswers[i]] << ". " << options[userAnswers[i]] << endl;
                if (userAnswers[i] == questions[i].getCorrectAnswer())
                {
                    cout << "    Status:  Correct (+1 point)" << endl;
                }
                else
                {
                    cout << "    Status:  Incorrect (0 points)" << endl;
                }
            }
            char optionLabels[] = {'a', 'b', 'c', 'd'};
            cout << "    Correct Answer: " << optionLabels[questions[i].getCorrectAnswer()] << ". " << options[questions[i].getCorrectAnswer()] << endl;

            if (!questions[i].getExplanation().empty())
            {
                cout << "    Explanation: " << questions[i].getExplanation() << endl;
            }
            cout << string(80, '-') << endl;
        }

        cout << "\n Study Tip: Review the explanations above to improve your understanding!" << endl;

        Utils::pauseSystem();
    }

    void practiceMode()
    {
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

        switch (choice)
        {
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

    void quickPractice()
    {
        auto questions = dbManager->getRandomQuestions(5);
        if (questions.empty())
        {
            cout << "No questions available!" << endl;
            Utils::pauseSystem();
            return;
        }

        Utils::clearScreen();
        Utils::printHeader("QUICK PRACTICE");

        int score = 0;
        for (size_t i = 0; i < questions.size(); ++i)
        {
            cout << "\nQuestion " << (i + 1) << " of " << questions.size() << endl;
            cout << string(40, '-') << endl;
            questions[i].display();

            cout << "\nYour answer (a-d): ";
            string inputStr;
            cin >> inputStr;
            int answer = parseAnswerInput(inputStr);

            if (answer >= 1 && answer <= 4 && (answer - 1) == questions[i].getCorrectAnswer())
            {
                cout << " Correct!" << endl;
                score++;
            }
            else
            {
                char optionLabels[] = {'a', 'b', 'c', 'd'};
                cout << " Wrong! Correct answer: " << optionLabels[questions[i].getCorrectAnswer()] << endl;
                if (!questions[i].getExplanation().empty())
                {
                    cout << "Explanation: " << questions[i].getExplanation() << endl;
                }
            }

            if (i < questions.size() - 1)
            {
                cout << "\nPress Enter for next question...";
                cin.ignore();
                cin.get();
            }
        }

        cout << "\n"
             << string(40, '=') << endl;
        cout << "Practice Complete!" << endl;
        cout << "Score: " << score << "/" << questions.size() << endl;
        cout << "Accuracy: " << (score * 100.0 / questions.size()) << "%" << endl;

        Utils::pauseSystem();
    }

    void subjectPractice()
    {
        Utils::clearScreen();
        Utils::printHeader("SUBJECT-WISE PRACTICE");

        cout << "Select Subject:" << endl;
        cout << "1. Mathematics" << endl;
        cout << "2. Computer Science (General)" << endl;
        cout << "3. DSA (Data Structures & Algorithms)" << endl;
        cout << "4. OOP (Object-Oriented Programming)" << endl;
        cout << "5. COA (Computer Organization & Architecture)" << endl;
        cout << "6. SAM (Software Architecture & Modeling)" << endl;
        cout << "7. Database Systems" << endl;
        cout << "8. Operating Systems" << endl;
        cout << "9. Computer Networks" << endl;
        cout << "10. Science" << endl;
        cout << "11. General Knowledge" << endl;

        cout << "\nEnter choice (1-11): ";
        int choice;
        cin >> choice;

        string subject;
        switch (choice)
        {
        case 1:
            subject = "Mathematics";
            break;
        case 2:
            subject = "Computer Science";
            break;
        case 3:
            subject = "DSA";
            break;
        case 4:
            subject = "OOP";
            break;
        case 5:
            subject = "COA";
            break;
        case 6:
            subject = "SAM";
            break;
        case 7:
            subject = "Database Systems";
            break;
        case 8:
            subject = "Operating Systems";
            break;
        case 9:
            subject = "Computer Networks";
            break;
        case 10:
            subject = "Science";
            break;
        case 11:
            subject = "General Knowledge";
            break;
        default:
            cout << "Invalid choice!" << endl;
            Utils::pauseSystem();
            return;
        }

        auto questions = dbManager->getQuestionsBySubject(subject);
        if (questions.empty())
        {
            cout << "No questions available for " << subject << "!" << endl;
            Utils::pauseSystem();
            return;
        }

        // DSA: Randomize using Merge Sort and limit to 10 questions
        randomizeQuestions(questions); // DSA: Merge Sort randomization
        if (questions.size() > 10)
        {
            questions.resize(10);
        }

        conductPracticeSession(questions, subject + " Practice");
    }

    void difficultyPractice()
    {
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
        switch (choice)
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
        default:
            cout << "Invalid choice!" << endl;
            Utils::pauseSystem();
            return;
        }

        auto questions = dbManager->getQuestionsByDifficulty(difficulty);
        if (questions.empty())
        {
            cout << "No " << difficulty << " questions available!" << endl;
            Utils::pauseSystem();
            return;
        }

        // DSA: Randomize using Merge Sort and limit to 10 questions
        randomizeQuestions(questions); // DSA: Merge Sort randomization
        if (questions.size() > 10)
        {
            questions.resize(10);
        }

        conductPracticeSession(questions, difficulty + " Practice");
    }

    void conductPracticeSession(const vector<Question> &questions, const string &sessionName)
    {
        Utils::clearScreen();
        Utils::printHeader(sessionName);

        int score = 0;
        vector<bool> correct(questions.size(), false);

        for (size_t i = 0; i < questions.size(); ++i)
        {
            cout << "\nQuestion " << (i + 1) << " of " << questions.size() << endl;
            cout << string(50, '-') << endl;
            questions[i].display();

            cout << "\nYour answer (a-d): ";
            string inputStr;
            cin >> inputStr;
            int answer = parseAnswerInput(inputStr);

            if (answer >= 1 && answer <= 4 && (answer - 1) == questions[i].getCorrectAnswer())
            {
                cout << " Correct!" << endl;
                score++;
                correct[i] = true;
            }
            else
            {
                char optionLabels[] = {'a', 'b', 'c', 'd'};
                cout << " Wrong! Correct answer: " << optionLabels[questions[i].getCorrectAnswer()] << endl;
                if (!questions[i].getExplanation().empty())
                {
                    cout << "Explanation: " << questions[i].getExplanation() << endl;
                }
            }

            if (i < questions.size() - 1)
            {
                cout << "\nPress Enter for next question...";
                cin.ignore();
                cin.get();
            }
        }

        // Show summary
        cout << "\n"
             << string(50, '=') << endl;
        cout << sessionName << " Complete!" << endl;
        cout << "Score: " << score << "/" << questions.size() << endl;
        cout << "Accuracy: " << (score * 100.0 / questions.size()) << "%" << endl;

        // Show incorrect questions for review
        bool hasIncorrect = false;
        for (size_t i = 0; i < questions.size(); ++i)
        {
            if (!correct[i])
            {
                if (!hasIncorrect)
                {
                    cout << "\nQuestions to review:" << endl;
                    hasIncorrect = true;
                }
                cout << "Q" << (i + 1) << ": " << questions[i].getSubject()
                     << " - " << questions[i].getDifficulty() << endl;
            }
        }

        Utils::pauseSystem();
    }

    void reviewMistakes()
    {
        Utils::clearScreen();
        Utils::printHeader("REVIEW MISTAKES");

        cout << "This feature will show questions you got wrong in previous exams." << endl;
        cout << "Feature coming soon!" << endl;

        Utils::pauseSystem();
    }

    void viewMyResults()
    {
        Utils::clearScreen();
        Utils::printHeader("MY EXAM RESULTS");

        auto results = dbManager->getExamResultsByUser(currentStudent.getId());
        if (results.empty())
        {
            cout << "No exam results found!" << endl;
            cout << "Take an exam to see your results here." << endl;
        }
        else
        {
            cout << "Total Exams Taken: " << results.size() << endl;
            cout << string(80, '-') << endl;
            cout << "Date\t\tSubject\t\tScore\t\tPercentage\tGrade" << endl;
            cout << string(80, '-') << endl;

            double totalPercentage = 0;
            for (const auto &result : results)
            {
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

    void viewProfile()
    {
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

        if (!results.empty())
        {
            double totalPercentage = 0;
            int passedExams = 0;

            for (const auto &result : results)
            {
                totalPercentage += result.getPercentage();
                if (result.getPercentage() >= 60)
                    passedExams++;
            }

            cout << "Average Score: " << (totalPercentage / results.size()) << "%" << endl;
            cout << "Passed Exams: " << passedExams << "/" << results.size() << endl;
            cout << "Pass Rate: " << (passedExams * 100.0 / results.size()) << "%" << endl;
        }

        Utils::pauseSystem();
    }

    void performanceAnalytics()
    {
        Utils::clearScreen();
        Utils::printHeader("PERFORMANCE ANALYTICS");

        auto results = dbManager->getExamResultsByUser(currentStudent.getId());
        if (results.empty())
        {
            cout << "No exam data available for analysis." << endl;
            cout << "Take some exams to see your performance analytics!" << endl;
            Utils::pauseSystem();
            return;
        }

        // Calculate statistics
        map<string, vector<double>> subjectScores;
        double totalScore = 0;
        int passedExams = 0;

        for (const auto &result : results)
        {
            string subject = result.getSubject().empty() ? "Mixed" : result.getSubject();
            subjectScores[subject].push_back(result.getPercentage());
            totalScore += result.getPercentage();
            if (result.getPercentage() >= 60)
                passedExams++;
        }

        cout << "Overall Performance:" << endl;
        cout << "Total Exams: " << results.size() << endl;
        cout << "Average Score: " << (totalScore / results.size()) << "%" << endl;
        cout << "Pass Rate: " << (passedExams * 100.0 / results.size()) << "%" << endl;

        cout << "\nSubject-wise Performance:" << endl;
        cout << string(50, '-') << endl;
        for (const auto &pair : subjectScores)
        {
            double subjectTotal = 0;
            for (double score : pair.second)
            {
                subjectTotal += score;
            }
            double average = subjectTotal / pair.second.size();
            cout << pair.first << ": " << average << "% ("
                 << pair.second.size() << " exams)" << endl;
        }

        // Find strengths and weaknesses
        string strongestSubject, weakestSubject;
        double highestAvg = 0, lowestAvg = 100;

        for (const auto &pair : subjectScores)
        {
            if (pair.second.size() >= 2)
            { // Only consider subjects with 2+ exams
                double subjectTotal = 0;
                for (double score : pair.second)
                {
                    subjectTotal += score;
                }
                double average = subjectTotal / pair.second.size();

                if (average > highestAvg)
                {
                    highestAvg = average;
                    strongestSubject = pair.first;
                }
                if (average < lowestAvg)
                {
                    lowestAvg = average;
                    weakestSubject = pair.first;
                }
            }
        }

        if (!strongestSubject.empty())
        {
            cout << "\nStrengths & Weaknesses:" << endl;
            cout << "Strongest Subject: " << strongestSubject << " (" << highestAvg << "%)" << endl;
            if (!weakestSubject.empty() && weakestSubject != strongestSubject)
            {
                cout << "Needs Improvement: " << weakestSubject << " (" << lowestAvg << "%)" << endl;
            }
        }

        Utils::pauseSystem();
    }

    string getGrade(double percentage)
    {
        if (percentage >= 97)
            return "A+"; // 97-100%
        else if (percentage >= 93)
            return "A"; // 93-96%
        else if (percentage >= 90)
            return "A-"; // 90-92%
        else if (percentage >= 87)
            return "B+"; // 87-89%
        else if (percentage >= 83)
            return "B"; // 83-86%
        else if (percentage >= 80)
            return "B-"; // 80-82%
        else if (percentage >= 77)
            return "C+"; // 77-79%
        else if (percentage >= 73)
            return "C"; // 73-76%
        else if (percentage >= 70)
            return "C-"; // 70-72%
        else if (percentage >= 67)
            return "D+"; // 67-69%
        else if (percentage >= 63)
            return "D"; // 63-66%
        else if (percentage >= 60)
            return "D-"; // 60-62%
        else
            return "F"; // Below 60%
    }
};

#endif // STUDENT_STUB_H