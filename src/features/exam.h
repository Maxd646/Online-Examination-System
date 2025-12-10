#ifndef EXAM_H
#define EXAM_H

#include "../authentication/user.h"
#include "../database/database.h"
#include "../components/queue.h"
#include "../components/stack.h"
#include "../components/hash_table.h"
#include "../structure/utils.h"
#include <vector>
#include <string>
#include <chrono>
#include <random>

// Exam types and modes
enum class ExamType {
    PRACTICE = 1,
    MOCK = 2,
    OFFICIAL = 3,
    TIMED = 4,
    ADAPTIVE = 5
};

enum class ExamMode {
    SEQUENTIAL = 1,    // Questions in order, no going back
    FLEXIBLE = 2,      // Can navigate freely
    REVIEW = 3,        // Can review and change answers
    INSTANT_FEEDBACK = 4  // Show correct answer immediately
};

enum class QuestionDifficulty {
    EASY = 1,
    MEDIUM = 2,
    HARD = 3,
    MIXED = 4
};

// Exam configuration and settings
struct ExamSettings {
    ExamType type;
    ExamMode mode;
    int questionCount;
    int timeLimit; // in minutes, 0 = no limit
    string subject;
    QuestionDifficulty difficulty;
    bool shuffleQuestions;
    bool shuffleOptions;
    bool allowReview;
    bool showTimer;
    bool autoSubmit;
    bool negativeMarking;
    double negativeMarkValue;
    double passingPercentage;
    
    ExamSettings() : type(ExamType::PRACTICE), mode(ExamMode::FLEXIBLE),
                    questionCount(10), timeLimit(30), subject(""),
                    difficulty(QuestionDifficulty::MIXED), shuffleQuestions(true),
                    shuffleOptions(false), allowReview(true), showTimer(true),
                    autoSubmit(true), negativeMarking(false), negativeMarkValue(0.25),
                    passingPercentage(60.0) {}
};

// Individual question state in exam
class ExamQuestionState {
private:
    Question question;
    int selectedAnswer;
    bool isAnswered;
    bool isMarkedForReview;
    bool isVisited;
    chrono::steady_clock::time_point startTime;
    chrono::steady_clock::time_point endTime;
    int timeSpent; // in seconds
    int attemptCount;
    
public:
    ExamQuestionState(const Question& q);
    
    // Answer management
    void setAnswer(int answer);
    void clearAnswer();
    int getAnswer() const { return selectedAnswer; }
    bool hasAnswer() const { return isAnswered; }
    
    // Review management
    void markForReview() { isMarkedForReview = true; }
    void unmarkForReview() { isMarkedForReview = false; }
    bool isMarked() const { return isMarkedForReview; }
    
    // Time tracking
    void startTiming();
    void stopTiming();
    int getTimeSpent() const { return timeSpent; }
    
    // State management
    void visit() { isVisited = true; }
    bool wasVisited() const { return isVisited; }
    void incrementAttempts() { attemptCount++; }
    int getAttempts() const { return attemptCount; }
    
    // Question access
    const Question& getQuestion() const { return question; }
    Question& getQuestion() { return question; }
    
    // Validation
    bool isCorrect() const;
    double getScore(bool useNegativeMarking = false, double negativeValue = 0.25) const;
};

// Main exam engine
class ExamEngine {
private:
    int examId;
    User student;
    ExamSettings settings;
    vector<ExamQuestionState> questions;
    Queue<int> questionSequence;
    Stack<int> navigationHistory;
    HashTable<int, int> questionIndexMap; // questionId -> index
    
    int currentQuestionIndex;
    chrono::steady_clock::time_point examStartTime;
    chrono::steady_clock::time_point examEndTime;
    chrono::steady_clock::time_point lastActivityTime;
    
    bool isExamActive;
    bool isExamCompleted;
    bool isExamSubmitted;
    bool isTimedOut;
    bool isPaused;
    
    // Statistics
    int totalAnswered;
    int totalCorrect;
    int totalMarkedForReview;
    double currentScore;
    
public:
    ExamEngine(const User& student, const ExamSettings& settings);
    ~ExamEngine();
    
    // Exam lifecycle
    bool initializeExam(const vector<Question>& questionPool);
    bool startExam();
    bool pauseExam();
    bool resumeExam();
    bool endExam();
    bool submitExam();
    bool cancelExam();
    
    // Question navigation
    bool nextQuestion();
    bool previousQuestion();
    bool goToQuestion(int index);
    bool goToFirstUnanswered();
    bool goToFirstMarked();
    
    // Answer management
    bool answerQuestion(int answer);
    bool changeAnswer(int questionIndex, int newAnswer);
    bool clearAnswer(int questionIndex = -1); // -1 for current question
    bool markCurrentForReview();
    bool unmarkCurrentForReview();
    
    // Time management
    int getRemainingTime() const; // in seconds
    int getElapsedTime() const;
    bool isTimeExpired() const;
    void updateActivity();
    bool isInactivityTimeout() const;
    
    // Progress and statistics
    int getAnsweredCount() const { return totalAnswered; }
    int getUnansweredCount() const { return questions.size() - totalAnswered; }
    int getMarkedCount() const { return totalMarkedForReview; }
    double getProgressPercentage() const;
    double getCurrentScore() const { return currentScore; }
    
    // Question access
    ExamQuestionState* getCurrentQuestion();
    const ExamQuestionState* getCurrentQuestion() const;
    ExamQuestionState* getQuestion(int index);
    const vector<ExamQuestionState>& getAllQuestions() const { return questions; }
    
    // Results and analysis
    ExamResult generateResult();
    vector<bool> getAnswerCorrectness() const;
    vector<int> getTimeSpentPerQuestion() const;
    double calculateAccuracy() const;
    double calculateSpeed() const; // questions per minute
    
    // Exam state
    bool isActive() const { return isExamActive; }
    bool isCompleted() const { return isExamCompleted; }
    bool isSubmitted() const { return isExamSubmitted; }
    bool canNavigate() const;
    bool canSubmit() const;
    
    // Settings and configuration
    const ExamSettings& getSettings() const { return settings; }
    void updateSettings(const ExamSettings& newSettings);
    
    // Validation
    bool isValidExam() const;
    bool isValidQuestionIndex(int index) const;
    bool isValidAnswer(int answer) const;
    
    // Adaptive difficulty (for adaptive exams)
    void adjustDifficulty();
    QuestionDifficulty getCurrentDifficulty() const;
    
private:
    // Helper methods
    void shuffleQuestions();
    void shuffleQuestionOptions();
    void calculateScore();
    void updateStatistics();
    int generateExamId();
    
    // Question selection algorithms
    vector<Question> selectQuestionsByDifficulty(const vector<Question>& pool, 
                                                     QuestionDifficulty difficulty, int count);
    vector<Question> selectAdaptiveQuestions(const vector<Question>& pool, int count);
    vector<Question> selectRandomQuestions(const vector<Question>& pool, int count);
    
    // Scoring algorithms
    double calculateBasicScore() const;
    double calculateWeightedScore() const;
    double calculateAdaptiveScore() const;
    
    // Time management helpers
    void startTimer();
    void stopTimer();
    bool checkTimeLimit() const;
    void handleTimeout();
};

// Exam factory for creating different types of exams
class ExamFactory {
public:
    static ExamEngine* createPracticeExam(const User& student, const string& subject = "");
    static ExamEngine* createMockExam(const User& student, const ExamSettings& settings);
    static ExamEngine* createOfficialExam(const User& student, const ExamSettings& settings);
    static ExamEngine* createTimedExam(const User& student, int timeLimit, int questionCount);
    static ExamEngine* createAdaptiveExam(const User& student, const string& subject = "");
    
    // Predefined exam configurations
    static ExamSettings getQuickPracticeSettings();
    static ExamSettings getStandardExamSettings();
    static ExamSettings getCompetitiveExamSettings();
    static ExamSettings getAdaptiveExamSettings();
};

// Exam scheduler and management
class ExamScheduler {
private:
    DatabaseManager* dbManager;
    vector<ExamEngine*> activeExams;
    HashTable<int, ExamEngine*> examMap; // userId -> exam
    
public:
    ExamScheduler(DatabaseManager* db);
    ~ExamScheduler();
    
    // Exam session management
    int scheduleExam(const User& student, const ExamSettings& settings);
    bool startScheduledExam(int examId);
    bool endExam(int examId);
    bool cancelExam(int examId);
    
    // Active exam management
    ExamEngine* getActiveExam(int userId);
    vector<ExamEngine*> getAllActiveExams();
    int getActiveExamCount() const;
    
    // Cleanup and maintenance
    void cleanupCompletedExams();
    void handleTimeouts();
    void forceEndAllExams();
    
    // Statistics
    int getTotalExamsScheduled() const;
    int getTotalExamsCompleted() const;
    double getAverageExamDuration() const;
};

// Exam analytics and insights
class ExamAnalytics {
private:
    DatabaseManager* dbManager;
    
public:
    ExamAnalytics(DatabaseManager* db) : dbManager(db) {}
    
    // Question analytics
    struct QuestionStats {
        int questionId;
        int timesAsked;
        int timesCorrect;
        double successRate;
        double averageTimeSpent;
        QuestionDifficulty perceivedDifficulty;
    };
    
    vector<QuestionStats> getQuestionStatistics();
    vector<Question> getMostDifficultQuestions(int count = 10);
    vector<Question> getEasiestQuestions(int count = 10);
    vector<Question> getMostTimeConsumingQuestions(int count = 10);
    
    // Exam performance analytics
    struct ExamPerformanceStats {
        double averageScore;
        double averageTime;
        double passRate;
        int totalExams;
        string mostPopularSubject;
        string mostDifficultSubject;
    };
    
    ExamPerformanceStats getOverallPerformance();
    vector<pair<string, double>> getSubjectPerformance();
    vector<pair<QuestionDifficulty, double>> getDifficultyPerformance();
    
    // Trend analysis
    vector<pair<string, double>> getPerformanceTrend(int days = 30);
    vector<pair<string, int>> getExamFrequencyTrend(int days = 30);
    
    // Predictive analytics
    double predictExamScore(int userId, const string& subject);
    vector<Question> recommendQuestionsForUser(int userId);
    QuestionDifficulty recommendDifficultyForUser(int userId, const string& subject);
};

// Exam security and integrity
class ExamSecurity {
private:
    HashTable<int, string> examTokens; // examId -> security token
    HashTable<int, chrono::steady_clock::time_point> lastActivity; // userId -> timestamp
    
public:
    // Security validation
    bool validateExamSession(int examId, const string& token);
    bool checkForCheating(int userId, const ExamEngine& exam);
    bool validateExamIntegrity(const ExamEngine& exam);
    
    // Activity monitoring
    void recordActivity(int userId);
    bool detectSuspiciousActivity(int userId);
    bool isSessionValid(int userId, int maxInactiveMinutes = 5);
    
    // Token management
    string generateExamToken(int examId);
    bool validateToken(int examId, const string& token);
    void invalidateToken(int examId);
    
    // Audit logging
    void logExamStart(int userId, int examId);
    void logExamEnd(int userId, int examId, const ExamResult& result);
    void logSuspiciousActivity(int userId, const string& activity);
    void logSecurityViolation(int userId, const string& violation);
};

#endif // EXAM_H