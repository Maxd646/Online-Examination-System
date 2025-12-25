#ifndef DATABASE_H
#define DATABASE_H
#include <string>
#include <vector>
#include <sqlite3.h>
#include "../authentication/user.h"
#include "../components/hash_table.h"

// Forward declarations
class Question;
class ExamResult;
class ExamTemplate;
class ExamQuestion;

// Database connection and management
class DatabaseManager {
private:
    sqlite3* db;
    string dbPath;
    bool isConnected;
    
    // Connection pool for better performance
    static const int MAX_CONNECTIONS = 10;
    vector<sqlite3*> connectionPool;
    vector<bool> connectionInUse;
    
    // Prepared statements cache
    HashTable<string, sqlite3_stmt*> preparedStatements;
    
    // Last inserted IDs for retrieval
    int lastInsertedExamTemplateId;
    
public:
    DatabaseManager(const string& databasePath = "database/exam.db");
    ~DatabaseManager();
    
    // Connection management
    bool connect();
    void disconnect();
    bool isConnectionActive() const { return isConnected; }
    
    // Database initialization
    bool initializeDatabase();
    bool createTables();
    bool insertDefaultData();
    
    // Transaction management
    bool beginTransaction();
    bool commitTransaction();
    bool rollbackTransaction();
    
    // User operations
    bool insertUser(const User& user);
    bool updateUser(const User& user);
    bool deleteUser(int userId);
    User getUserById(int userId);
    User getUserByUsername(const string& username);
    vector<User> getAllUsers();
    vector<User> getUsersByRole(UserRole role);
    bool authenticateUser(const string& username, const string& password, User& user);
    
    // Question operations
    bool insertQuestion(const Question& question);
    bool updateQuestion(const Question& question);
    bool deleteQuestion(int questionId);
    Question getQuestionById(int questionId);
    vector<Question> getAllQuestions();
    vector<Question> getQuestionsBySubject(const string& subject);
    vector<Question> getQuestionsByDifficulty(const string& difficulty);
    vector<Question> getRandomQuestions(int count, const string& subject = "");
    
    // Exam result operations
    bool insertExamResult(const ExamResult& result);
    bool updateExamResult(const ExamResult& result);
    bool deleteExamResult(int resultId);
    ExamResult getExamResultById(int resultId);
    vector<ExamResult> getExamResultsByUser(int userId);
    vector<ExamResult> getAllExamResults();
    vector<ExamResult> getExamResultsByDateRange(const string& startDate, const string& endDate);
    
    // Exam template operations
    bool insertExamTemplate(const ExamTemplate& examTemplate);
    int getLastInsertedExamTemplateId() const { return lastInsertedExamTemplateId; }
    bool updateExamTemplate(const ExamTemplate& examTemplate);
    bool deleteExamTemplate(int templateId);
    ExamTemplate getExamTemplateById(int templateId);
    vector<ExamTemplate> getAllExamTemplates();
    vector<ExamTemplate> getExamTemplatesByType(const string& examType);
    vector<ExamTemplate> getExamTemplatesBySubject(const string& subject);
    vector<ExamTemplate> getActiveExamTemplates();
    bool activateExamTemplate(int templateId);
    bool deactivateExamTemplate(int templateId);
    
    // Exam question operations (for direct exam creation)
    bool insertExamQuestion(const ExamQuestion& question);
    bool updateExamQuestion(const ExamQuestion& question);
    bool deleteExamQuestion(int questionId);
    vector<ExamQuestion> getExamQuestions(int examTemplateId);
    ExamQuestion getExamQuestionById(int questionId);
    int getExamQuestionCount(int examTemplateId);
    
    // Statistics and analytics
    int getTotalUsers();
    int getTotalQuestions();
    int getTotalExamResults();
    double getAverageScore();
    vector<pair<string, int>> getSubjectStatistics();
    vector<pair<string, double>> getUserPerformanceStats();
    
    // Backup and restore
    bool createBackup(const string& backupPath);
    bool restoreFromBackup(const string& backupPath);
    bool exportToCSV(const string& tableName, const string& filePath);
    bool importFromCSV(const string& tableName, const string& filePath);
    
    // Database maintenance
    bool vacuum();
    bool reindex();
    bool analyze();
    vector<string> checkIntegrity();
    
    // Advanced queries
    vector<Question> searchQuestions(const string& keyword);
    vector<User> searchUsers(const string& keyword);
    vector<ExamResult> getTopPerformers(int limit = 10);
    vector<ExamResult> getRecentResults(int limit = 20);
    
    // Error handling
    string getLastError() const;
    int getLastErrorCode() const;
    
private:
    // Helper methods
    bool executeSQL(const string& sql);
    sqlite3_stmt* prepareStatement(const string& sql);
    void finalizeStatement(sqlite3_stmt* stmt);
    string escapeString(const string& str);
    
    // Connection pool management
    sqlite3* getConnection();
    void releaseConnection(sqlite3* conn);
    
    // Schema validation
    bool validateSchema();
    bool updateSchema();
    
    // Logging
    void logError(const string& operation, const string& error);
    void logQuery(const string& query);
};

// Question entity for database operations
class Question {
private:
    int id;
    string subject;
    string questionText;
    vector<string> options;
    int correctAnswer;
    string difficulty;
    string explanation;
    string createdAt;
    string updatedAt;
    int createdBy;
    bool isActive;
    
public:
    Question();
    Question(int id, const string& subject, const string& questionText,
             const vector<string>& options, int correctAnswer,
             const string& difficulty = "Medium", const string& explanation = "");
    
    // Getters
    int getId() const { return id; }
    string getSubject() const { return subject; }
    string getQuestionText() const { return questionText; }
    vector<string> getOptions() const { return options; }
    int getCorrectAnswer() const { return correctAnswer; }
    string getDifficulty() const { return difficulty; }
    string getExplanation() const { return explanation; }
    string getCreatedAt() const { return createdAt; }
    string getUpdatedAt() const { return updatedAt; }
    int getCreatedBy() const { return createdBy; }
    bool getIsActive() const { return isActive; }
    
    // Setters
    void setId(int id) { this->id = id; }
    void setSubject(const string& subject) { this->subject = subject; }
    void setQuestionText(const string& questionText) { this->questionText = questionText; }
    void setOptions(const vector<string>& options) { this->options = options; }
    void setCorrectAnswer(int correctAnswer) { this->correctAnswer = correctAnswer; }
    void setDifficulty(const string& difficulty) { this->difficulty = difficulty; }
    void setExplanation(const string& explanation) { this->explanation = explanation; }
    void setCreatedBy(int createdBy) { this->createdBy = createdBy; }
    void setIsActive(bool isActive) { this->isActive = isActive; }
    void setUpdatedAt(const string& updatedAt) { this->updatedAt = updatedAt; }
    
    // Validation
    bool isValid() const;
    
    // Display
    void display() const;
    void displayWithAnswer() const;
    
    // Operators
    bool operator==(const Question& other) const;
};

// Exam result entity
class ExamResult {
private:
    int id;
    int userId;
    string username;
    int score;
    int totalQuestions;
    double percentage;
    string examDate;
    string startTime;
    string endTime;
    int duration; // in minutes
    string subject;
    string difficulty;
    string examType; // Quiz, Worksheet, Final
    string templateName; // Name of the template used
    int examTemplateId; // ID of the exam template
    int timeLimit; // Original time limit
    bool negativeMarking; // Whether negative marking was used
    double negativeMarks; // Negative marks deducted
    vector<int> questionIds;
    vector<int> userAnswers;
    vector<bool> correctAnswers;
    
public:
    ExamResult();
    ExamResult(int userId, const string& username, int score, int totalQuestions,
               const string& subject = "", const string& difficulty = "Mixed");
    
    // Getters
    int getId() const { return id; }
    int getUserId() const { return userId; }
    string getUsername() const { return username; }
    int getScore() const { return score; }
    int getTotalQuestions() const { return totalQuestions; }
    double getPercentage() const { return percentage; }
    string getExamDate() const { return examDate; }
    string getStartTime() const { return startTime; }
    string getEndTime() const { return endTime; }
    int getDuration() const { return duration; }
    string getSubject() const { return subject; }
    string getDifficulty() const { return difficulty; }
    string getExamType() const { return examType; }
    string getTemplateName() const { return templateName; }
    int getExamTemplateId() const { return examTemplateId; }
    int getTimeLimit() const { return timeLimit; }
    bool hasNegativeMarking() const { return negativeMarking; }
    double getNegativeMarks() const { return negativeMarks; }
    vector<int> getQuestionIds() const { return questionIds; }
    vector<int> getUserAnswers() const { return userAnswers; }
    vector<bool> getCorrectAnswers() const { return correctAnswers; }
    
    // Setters
    void setId(int id) { this->id = id; }
    void setUserId(int userId) { this->userId = userId; }
    void setUsername(const string& username) { this->username = username; }
    void setScore(int score) { this->score = score; calculatePercentage(); }
    void setTotalQuestions(int totalQuestions) { this->totalQuestions = totalQuestions; calculatePercentage(); }
    void setExamDate(const string& examDate) { this->examDate = examDate; }
    void setStartTime(const string& startTime) { this->startTime = startTime; }
    void setEndTime(const string& endTime) { this->endTime = endTime; }
    void setDuration(int duration) { this->duration = duration; }
    void setSubject(const string& subject) { this->subject = subject; }
    void setDifficulty(const string& difficulty) { this->difficulty = difficulty; }
    void setExamType(const string& examType) { this->examType = examType; }
    void setTemplateName(const string& templateName) { this->templateName = templateName; }
    void setExamTemplateId(int examTemplateId) { this->examTemplateId = examTemplateId; }
    void setTimeLimit(int timeLimit) { this->timeLimit = timeLimit; }
    void setNegativeMarking(bool negativeMarking) { this->negativeMarking = negativeMarking; }
    void setNegativeMarks(double negativeMarks) { this->negativeMarks = negativeMarks; }
    void setQuestionIds(const vector<int>& questionIds) { this->questionIds = questionIds; }
    void setUserAnswers(const vector<int>& userAnswers) { this->userAnswers = userAnswers; }
    void setCorrectAnswers(const vector<bool>& correctAnswers) { this->correctAnswers = correctAnswers; }
    
    // Utility methods
    void calculatePercentage();
    string getGrade() const;
    bool isPassed(double passingPercentage = 60.0) const;
    
    // Display
    void display() const;
    void displayDetailed() const;
    
    // Operators
    bool operator==(const ExamResult& other) const;
    bool operator<(const ExamResult& other) const; // For sorting by score
};

// Database query builder for complex queries
class QueryBuilder {
private:
    string tableName;
    vector<string> selectFields;
    vector<string> whereConditions;
    vector<string> joinClauses;
    vector<string> orderByFields;
    vector<string> groupByFields;
    string havingClause;
    int limitValue;
    int offsetValue;
    
public:
    QueryBuilder(const string& table);
    
    QueryBuilder& select(const vector<string>& fields);
    QueryBuilder& where(const string& condition);
    QueryBuilder& join(const string& joinClause);
    QueryBuilder& orderBy(const string& field, bool ascending = true);
    QueryBuilder& groupBy(const string& field);
    QueryBuilder& having(const string& condition);
    QueryBuilder& limit(int count);
    QueryBuilder& offset(int count);
    
    string build() const;
    void reset();
};

#endif // DATABASE_H