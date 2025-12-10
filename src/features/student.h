#ifndef STUDENT_H
#define STUDENT_H

#include "../authentication/user.h"
#include "../database/database.h"
#include "../components/queue.h"
#include "../components/stack.h"
#include "../structure/utils.h"
#include <vector>
#include <string>
#include <chrono>

// Exam configuration
struct ExamConfig {
    int questionCount;
    string subject;
    string difficulty;
    int timeLimit; // in minutes
    bool shuffleQuestions;
    bool shuffleOptions;
    bool allowReview;
    bool showResults;
    
    ExamConfig() : questionCount(10), subject(""), difficulty("Mixed"), 
                  timeLimit(30), shuffleQuestions(true), shuffleOptions(false),
                  allowReview(false), showResults(true) {}
};

// Question state during exam
struct ExamQuestion {
    Question question;
    int userAnswer;
    bool isAnswered;
    bool isMarkedForReview;
    chrono::steady_clock::time_point startTime;
    chrono::steady_clock::time_point endTime;
    int timeSpent; // in seconds
    
    ExamQuestion(const Question& q) : question(q), userAnswer(-1), 
                                     isAnswered(false), isMarkedForReview(false), 
                                     timeSpent(0) {}
};

// Exam session management
class ExamSession {
private:
    int sessionId;
    int userId;
    string username;
    ExamConfig config;
    Queue<ExamQuestion> questionQueue;
    Stack<int> navigationHistory;
    vector<ExamQuestion> allQuestions;
    int currentQuestionIndex;
    chrono::steady_clock::time_point examStartTime;
    chrono::steady_clock::time_point examEndTime;
    bool isActive;
    bool isCompleted;
    bool isTimedOut;
    
public:
    ExamSession(int userId, const string& username, const ExamConfig& config);
    ~ExamSession();
    
    // Session management
    bool startExam(const vector<Question>& questions);
    bool endExam();
    bool pauseExam();
    bool resumeExam();
    bool submitExam();
    
    // Navigation
    bool nextQuestion();
    bool previousQuestion();
    bool goToQuestion(int index);
    bool markForReview();
    bool unmarkForReview();
    
    // Answer management
    bool answerCurrentQuestion(int answer);
    bool changeAnswer(int questionIndex, int newAnswer);
    bool clearAnswer(int questionIndex);
    
    // Time management
    int getRemainingTime() const; // in seconds
    int getElapsedTime() const;   // in seconds
    bool isTimeUp() const;
    void updateTimer();
    
    // Question access
    ExamQuestion* getCurrentQuestion();
    const ExamQuestion* getCurrentQuestion() const;
    ExamQuestion* getQuestion(int index);
    vector<ExamQuestion> getAllQuestions() const { return allQuestions; }
    
    // Progress tracking
    int getAnsweredCount() const;
    int getUnansweredCount() const;
    int getMarkedForReviewCount() const;
    double getProgressPercentage() const;
    
    // Results calculation
    ExamResult calculateResult();
    vector<bool> getCorrectAnswers() const;
    int getScore() const;
    
    // Getters
    int getSessionId() const { return sessionId; }
    int getUserId() const { return userId; }
    string getUsername() const { return username; }
    ExamConfig getConfig() const { return config; }
    int getCurrentQuestionIndex() const { return currentQuestionIndex; }
    bool getIsActive() const { return isActive; }
    bool getIsCompleted() const { return isCompleted; }
    bool getIsTimedOut() const { return isTimedOut; }
    int getTotalQuestions() const { return allQuestions.size(); }
    
    // Validation
    bool isValidSession() const;
    bool canNavigateToQuestion(int index) const;
    bool canSubmit() const;
    
private:
    void shuffleQuestions();
    void shuffleOptions();
    int generateSessionId();
    void recordQuestionTime();
};

class StudentPanel {
private:
    DatabaseManager* dbManager;
    User currentStudent;
    ExamSession* currentExamSession;
    Stack<ExamResult> recentResults;
    
    // Student preferences
    struct StudentPreferences {
        bool autoSave;
        bool showTimer;
        bool confirmSubmission;
        string preferredSubject;
        string preferredDifficulty;
        int defaultQuestionCount;
        
        StudentPreferences() : autoSave(true), showTimer(true), confirmSubmission(true),
                              preferredSubject(""), preferredDifficulty("Medium"), 
                              defaultQuestionCount(10) {}
    } preferences;
    
public:
    StudentPanel(DatabaseManager* db, const User& student);
    ~StudentPanel();
    
    // Main interface
    void showMainMenu();
    void run();
    
    // Exam management
    void examMenu();
    bool startNewExam();
    bool resumeExam();
    void takeExam();
    void reviewExam();
    bool submitCurrentExam();
    
    // Exam configuration
    ExamConfig configureExam();
    void selectSubject(ExamConfig& config);
    void selectDifficulty(ExamConfig& config);
    void setQuestionCount(ExamConfig& config);
    void setTimeLimit(ExamConfig& config);
    void setExamOptions(ExamConfig& config);
    
    // Exam interface
    void displayExamInterface();
    void displayQuestion();
    void displayNavigationPanel();
    void displayProgressBar();
    void displayTimer();
    void displayQuestionPalette();
    
    // Navigation handling
    void handleNavigation();
    void handleAnswerInput();
    void handleReviewMarking();
    void handleExamSubmission();
    
    // Results and history
    void resultsMenu();
    void viewMyResults();
    void viewResultDetails(int resultId);
    void compareResults();
    void exportResults();
    void viewPerformanceAnalytics();
    
    // Performance analytics
    void showPerformanceStatistics();
    void showSubjectWisePerformance();
    void showProgressTrend();
    void showWeakAreas();
    void showStrengths();
    void getRecommendations();
    
    // Study materials and practice
    void studyMenu();
    void practiceMode();
    void reviewMistakes();
    void studyBySubject();
    void flashcardMode();
    void mockExam();
    
    // Profile management
    void profileMenu();
    void viewProfile();
    void updateProfile();
    void changePassword();
    void setPreferences();
    void viewAchievements();
    
    // Help and support
    void helpMenu();
    void viewInstructions();
    void viewFAQ();
    void reportIssue();
    void contactSupport();
    
private:
    // Helper methods
    void clearScreen() { Utils::clearScreen(); }
    void pauseSystem() { Utils::pauseSystem(); }
    bool confirmAction(const string& message) { return Utils::confirmAction(message); }
    
    // Input validation
    bool isValidAnswer(int answer);
    bool isValidQuestionIndex(int index);
    bool isValidTimeLimit(int minutes);
    
    // Display helpers
    void displayExamHeader();
    void displayQuestionHeader(int questionNum, int totalQuestions);
    void displayAnswerOptions(const vector<string>& options, int selectedAnswer = -1);
    void displayExamSummary();
    void displayResultSummary(const ExamResult& result);
    
    // Navigation helpers
    int getNavigationChoice();
    void showNavigationHelp();
    void updateQuestionStatus();
    
    // Time management
    void startTimer();
    void stopTimer();
    void showTimeWarning(int remainingMinutes);
    void handleTimeUp();
    
    // Auto-save functionality
    void autoSaveProgress();
    bool loadSavedProgress();
    void clearSavedProgress();
    
    // Performance calculation
    double calculateAccuracy();
    double calculateSpeed(); // questions per minute
    vector<string> identifyWeakSubjects();
    vector<string> identifyStrongSubjects();
    
    // Recommendations
    vector<Question> getRecommendedQuestions();
    vector<string> getStudyRecommendations();
    string getPerformanceFeedback();
    
    // Statistics helpers
    void updateStudentStatistics(const ExamResult& result);
    void calculatePerformanceMetrics();
    void generateProgressReport();
    
    // Error handling
    void handleExamError(const string& error);
    void handleDatabaseError(const string& operation);
    void handleTimeoutError();
    
    // Preferences management
    void loadPreferences();
    void savePreferences();
    void resetPreferences();
    
    // Achievement system
    void checkAchievements(const ExamResult& result);
    void unlockAchievement(const string& achievement);
    void displayAchievements();
};

// Practice mode for students
class PracticeMode {
private:
    DatabaseManager* dbManager;
    User student;
    vector<Question> practiceQuestions;
    vector<bool> answeredCorrectly;
    
public:
    PracticeMode(DatabaseManager* db, const User& student);
    
    // Practice sessions
    void startPracticeSession(const string& subject = "", const string& difficulty = "");
    void reviewIncorrectAnswers();
    void timedPractice(int timeLimit);
    void subjectSpecificPractice(const string& subject);
    
    // Adaptive learning
    vector<Question> getAdaptiveQuestions();
    void updateDifficultyBasedOnPerformance();
    
    // Progress tracking
    void trackPracticeProgress();
    void generatePracticeReport();
};

// Student analytics and insights
class StudentAnalytics {
private:
    DatabaseManager* dbManager;
    int studentId;
    
public:
    StudentAnalytics(DatabaseManager* db, int studentId);
    
    // Performance metrics
    struct StudentMetrics {
        double overallAccuracy;
        double averageScore;
        int totalExamsTaken;
        int totalQuestionsAnswered;
        string strongestSubject;
        string weakestSubject;
        double improvementRate;
        int currentStreak;
        int longestStreak;
    };
    
    StudentMetrics calculateMetrics();
    vector<pair<string, double>> getSubjectPerformance();
    vector<pair<string, double>> getDifficultyPerformance();
    vector<pair<string, int>> getExamHistory();
    
    // Trend analysis
    vector<pair<string, double>> getPerformanceTrend();
    vector<pair<string, int>> getActivityTrend();
    
    // Recommendations
    vector<string> getImprovementSuggestions();
    vector<Question> getRecommendedPracticeQuestions();
    string getNextGoal();
};

#endif // STUDENT_H