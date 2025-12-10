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

// Database connection and management
class DatabaseManager {
private:
    sqlite3* db;
    std::string dbPath;
    bool isConnected;
    
    // Connection pool for better performance
    static const int MAX_CONNECTIONS = 10;
    std::vector<sqlite3*> connectionPool;
    std::vector<bool> connectionInUse;
    
    // Prepared statements cache
    HashTable<std::string, sqlite3_stmt*> preparedStatements;
    
public:
    DatabaseManager(const std::string& databasePath = "database/exam.db");
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
    User getUserByUsername(const std::string& username);
    std::vector<User> getAllUsers();
    std::vector<User> getUsersByRole(UserRole role);
    bool authenticateUser(const std::string& username, const std::string& password, User& user);
    
    // Question operations
    bool insertQuestion(const Question& question);
    bool updateQuestion(const Question& question);
    bool deleteQuestion(int questionId);
    Question getQuestionById(int questionId);
    std::vector<Question> getAllQuestions();
    std::vector<Question> getQuestionsBySubject(const std::string& subject);
    std::vector<Question> getQuestionsByDifficulty(const std::string& difficulty);
    std::vector<Question> getRandomQuestions(int count, const std::string& subject = "");
    
    // Exam result operations
    bool insertExamResult(const ExamResult& result);
    bool updateExamResult(const ExamResult& result);
    bool deleteExamResult(int resultId);
    ExamResult getExamResultById(int resultId);
    std::vector<ExamResult> getExamResultsByUser(int userId);
    std::vector<ExamResult> getAllExamResults();
    std::vector<ExamResult> getExamResultsByDateRange(const std::string& startDate, const std::string& endDate);
    
    // Statistics and analytics
    int getTotalUsers();
    int getTotalQuestions();
    int getTotalExamResults();
    double getAverageScore();
    std::vector<std::pair<std::string, int>> getSubjectStatistics();
    std::vector<std::pair<std::string, double>> getUserPerformanceStats();
    
    // Backup and restore
    bool createBackup(const std::string& backupPath);
    bool restoreFromBackup(const std::string& backupPath);
    bool exportToCSV(const std::string& tableName, const std::string& filePath);
    bool importFromCSV(const std::string& tableName, const std::string& filePath);
    
    // Database maintenance
    bool vacuum();
    bool reindex();
    bool analyze();
    std::vector<std::string> checkIntegrity();
    
    // Advanced queries
    std::vector<Question> searchQuestions(const std::string& keyword);
    std::vector<User> searchUsers(const std::string& keyword);
    std::vector<ExamResult> getTopPerformers(int limit = 10);
    std::vector<ExamResult> getRecentResults(int limit = 20);
    
    // Error handling
    std::string getLastError() const;
    int getLastErrorCode() const;
    
private:
    // Helper methods
    bool executeSQL(const std::string& sql);
    sqlite3_stmt* prepareStatement(const std::string& sql);
    void finalizeStatement(sqlite3_stmt* stmt);
    std::string escapeString(const std::string& str);
    
    // Connection pool management
    sqlite3* getConnection();
    void releaseConnection(sqlite3* conn);
    
    // Schema validation
    bool validateSchema();
    bool updateSchema();
    
    // Logging
    void logError(const std::string& operation, const std::string& error);
    void logQuery(const std::string& query);
};

// Question entity for database operations
class Question {
private:
    int id;
    std::string subject;
    std::string questionText;
    std::vector<std::string> options;
    int correctAnswer;
    std::string difficulty;
    std::string explanation;
    std::string createdAt;
    std::string updatedAt;
    int createdBy;
    bool isActive;
    
public:
    Question();
    Question(int id, const std::string& subject, const std::string& questionText,
             const std::vector<std::string>& options, int correctAnswer,
             const std::string& difficulty = "Medium", const std::string& explanation = "");
    
    // Getters
    int getId() const { return id; }
    std::string getSubject() const { return subject; }
    std::string getQuestionText() const { return questionText; }
    std::vector<std::string> getOptions() const { return options; }
    int getCorrectAnswer() const { return correctAnswer; }
    std::string getDifficulty() const { return difficulty; }
    std::string getExplanation() const { return explanation; }
    std::string getCreatedAt() const { return createdAt; }
    std::string getUpdatedAt() const { return updatedAt; }
    int getCreatedBy() const { return createdBy; }
    bool getIsActive() const { return isActive; }
    
    // Setters
    void setId(int id) { this->id = id; }
    void setSubject(const std::string& subject) { this->subject = subject; }
    void setQuestionText(const std::string& questionText) { this->questionText = questionText; }
    void setOptions(const std::vector<std::string>& options) { this->options = options; }
    void setCorrectAnswer(int correctAnswer) { this->correctAnswer = correctAnswer; }
    void setDifficulty(const std::string& difficulty) { this->difficulty = difficulty; }
    void setExplanation(const std::string& explanation) { this->explanation = explanation; }
    void setCreatedBy(int createdBy) { this->createdBy = createdBy; }
    void setIsActive(bool isActive) { this->isActive = isActive; }
    void setUpdatedAt(const std::string& updatedAt) { this->updatedAt = updatedAt; }
    
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
    std::string username;
    int score;
    int totalQuestions;
    double percentage;
    std::string examDate;
    std::string startTime;
    std::string endTime;
    int duration; // in minutes
    std::string subject;
    std::string difficulty;
    std::vector<int> questionIds;
    std::vector<int> userAnswers;
    std::vector<bool> correctAnswers;
    
public:
    ExamResult();
    ExamResult(int userId, const std::string& username, int score, int totalQuestions,
               const std::string& subject = "", const std::string& difficulty = "Mixed");
    
    // Getters
    int getId() const { return id; }
    int getUserId() const { return userId; }
    std::string getUsername() const { return username; }
    int getScore() const { return score; }
    int getTotalQuestions() const { return totalQuestions; }
    double getPercentage() const { return percentage; }
    std::string getExamDate() const { return examDate; }
    std::string getStartTime() const { return startTime; }
    std::string getEndTime() const { return endTime; }
    int getDuration() const { return duration; }
    std::string getSubject() const { return subject; }
    std::string getDifficulty() const { return difficulty; }
    std::vector<int> getQuestionIds() const { return questionIds; }
    std::vector<int> getUserAnswers() const { return userAnswers; }
    std::vector<bool> getCorrectAnswers() const { return correctAnswers; }
    
    // Setters
    void setId(int id) { this->id = id; }
    void setUserId(int userId) { this->userId = userId; }
    void setUsername(const std::string& username) { this->username = username; }
    void setScore(int score) { this->score = score; calculatePercentage(); }
    void setTotalQuestions(int totalQuestions) { this->totalQuestions = totalQuestions; calculatePercentage(); }
    void setExamDate(const std::string& examDate) { this->examDate = examDate; }
    void setStartTime(const std::string& startTime) { this->startTime = startTime; }
    void setEndTime(const std::string& endTime) { this->endTime = endTime; }
    void setDuration(int duration) { this->duration = duration; }
    void setSubject(const std::string& subject) { this->subject = subject; }
    void setDifficulty(const std::string& difficulty) { this->difficulty = difficulty; }
    void setQuestionIds(const std::vector<int>& questionIds) { this->questionIds = questionIds; }
    void setUserAnswers(const std::vector<int>& userAnswers) { this->userAnswers = userAnswers; }
    void setCorrectAnswers(const std::vector<bool>& correctAnswers) { this->correctAnswers = correctAnswers; }
    
    // Utility methods
    void calculatePercentage();
    std::string getGrade() const;
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
    std::string tableName;
    std::vector<std::string> selectFields;
    std::vector<std::string> whereConditions;
    std::vector<std::string> joinClauses;
    std::vector<std::string> orderByFields;
    std::vector<std::string> groupByFields;
    std::string havingClause;
    int limitValue;
    int offsetValue;
    
public:
    QueryBuilder(const std::string& table);
    
    QueryBuilder& select(const std::vector<std::string>& fields);
    QueryBuilder& where(const std::string& condition);
    QueryBuilder& join(const std::string& joinClause);
    QueryBuilder& orderBy(const std::string& field, bool ascending = true);
    QueryBuilder& groupBy(const std::string& field);
    QueryBuilder& having(const std::string& condition);
    QueryBuilder& limit(int count);
    QueryBuilder& offset(int count);
    
    std::string build() const;
    void reset();
};

#endif // DATABASE_H