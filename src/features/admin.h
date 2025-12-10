#ifndef ADMIN_H
#define ADMIN_H

#include "../authentication/user.h"
#include "../database/database.h"
#include "../components/linked_list.h"
#include "../components/stack.h"
#include "../structure/utils.h"
#include <vector>
#include <string>

// Admin operation types for undo/redo functionality
enum class AdminOperation {
    ADD_QUESTION,
    UPDATE_QUESTION,
    DELETE_QUESTION,
    ADD_USER,
    UPDATE_USER,
    DELETE_USER,
    BULK_IMPORT
};

// Operation data for undo/redo
struct OperationData {
    AdminOperation type;
    int targetId;
    string data; // JSON or serialized data
    string timestamp;
    
    OperationData(AdminOperation t, int id, const string& d) 
        : type(t), targetId(id), data(d), timestamp(Utils::getCurrentDateTime()) {}
};

class AdminPanel {
private:
    DatabaseManager* dbManager;
    User currentAdmin;
    UndoRedoStack<OperationData> operationHistory;
    LinkedList<string> recentActions;
    
    // Statistics cache
    mutable bool statsNeedUpdate;
    mutable int cachedTotalUsers;
    mutable int cachedTotalQuestions;
    mutable int cachedTotalResults;
    
public:
    AdminPanel(DatabaseManager* db, const User& admin);
    ~AdminPanel();
    
    // Main menu and navigation
    void showMainMenu();
    void run();
    
    // Question management
    void questionManagementMenu();
    bool addQuestion();
    bool updateQuestion();
    bool deleteQuestion();
    void viewAllQuestions();
    void viewQuestionsBySubject();
    void searchQuestions();
    bool importQuestionsFromFile(const string& filename);
    bool exportQuestionsToFile(const string& filename);
    
    // User management
    void userManagementMenu();
    bool addUser();
    bool updateUser();
    bool deleteUser();
    void viewAllUsers();
    void viewUsersByRole();
    void searchUsers();
    bool lockUser(int userId);
    bool unlockUser(int userId);
    bool resetUserPassword(int userId);
    
    // Result management and analytics
    void resultManagementMenu();
    void viewAllResults();
    void viewResultsByUser();
    void viewResultsByDateRange();
    void generatePerformanceReport();
    void exportResultsToFile(const string& filename);
    
    // System statistics and analytics
    void showSystemStatistics();
    void showDetailedAnalytics();
    void showUserActivityReport();
    void showQuestionAnalytics();
    void showPerformanceTrends();
    
    // Database management
    void databaseManagementMenu();
    bool createDatabaseBackup();
    bool restoreDatabaseBackup();
    void optimizeDatabase();
    void checkDatabaseIntegrity();
    void viewDatabaseInfo();
    
    // System settings
    void systemSettingsMenu();
    void configureExamSettings();
    void manageSystemUsers();
    void viewSystemLogs();
    void clearSystemCache();
    
    // Bulk operations
    void bulkOperationsMenu();
    bool bulkAddQuestions();
    bool bulkUpdateQuestions();
    bool bulkDeleteQuestions();
    bool bulkUserOperations();
    
    // Advanced features
    void advancedFeaturesMenu();
    void questionDifficultyAnalysis();
    void userPerformanceAnalysis();
    void examTrendAnalysis();
    void generateCustomReports();
    
    // Undo/Redo operations
    bool undoLastOperation();
    bool redoLastOperation();
    void showOperationHistory();
    void clearOperationHistory();
    
    // Validation and helpers
    bool validateQuestionData(const Question& question);
    bool validateUserData(const User& user);
    void logAdminAction(const string& action);
    void updateStatisticsCache();
    
    // Display helpers
    void displayQuestionTable(const vector<Question>& questions);
    void displayUserTable(const vector<User>& users);
    void displayResultTable(const vector<ExamResult>& results);
    void displayStatisticsSummary();
    
    // Input helpers
    Question inputQuestionData();
    User inputUserData();
    string selectSubject();
    string selectDifficulty();
    UserRole selectUserRole();
    
    // File operations
    bool exportData(const string& dataType, const string& filename);
    bool importData(const string& dataType, const string& filename);
    
    // Security and audit
    void viewAuditLog();
    void generateSecurityReport();
    bool changeAdminPassword();
    void viewLoginAttempts();
    
private:
    // Helper methods
    void clearScreen() { Utils::clearScreen(); }
    void pauseSystem() { Utils::pauseSystem(); }
    bool confirmAction(const string& message) { return Utils::confirmAction(message); }
    
    // Menu helpers
    int getMenuChoice(int minChoice, int maxChoice);
    void displayMenuHeader(const string& title);
    void displayMenuFooter();
    
    // Data processing
    vector<pair<string, int>> calculateSubjectDistribution();
    vector<pair<string, double>> calculateDifficultyDistribution();
    vector<pair<string, double>> calculatePerformanceBySubject();
    vector<pair<string, int>> calculateMonthlyExamCounts();
    
    // Report generation
    void generatePDFReport(const string& reportType, const string& filename);
    void generateCSVReport(const string& reportType, const string& filename);
    void generateHTMLReport(const string& reportType, const string& filename);
    
    // Validation helpers
    bool isValidQuestionInput(const string& input);
    bool isValidUserInput(const string& input);
    bool isValidEmailFormat(const string& email);
    bool isValidPasswordStrength(const string& password);
    
    // Cache management
    void invalidateCache();
    void refreshCache();
    
    // Error handling
    void handleDatabaseError(const string& operation);
    void handleFileError(const string& operation, const string& filename);
    void handleValidationError(const string& field, const string& value);
};

// Question import/export formats
class QuestionImporter {
public:
    static vector<Question> importFromCSV(const string& filename);
    static vector<Question> importFromJSON(const string& filename);
    static vector<Question> importFromXML(const string& filename);
    static bool validateImportFormat(const string& filename, const string& format);
};

class QuestionExporter {
public:
    static bool exportToCSV(const vector<Question>& questions, const string& filename);
    static bool exportToJSON(const vector<Question>& questions, const string& filename);
    static bool exportToXML(const vector<Question>& questions, const string& filename);
    static bool exportToPDF(const vector<Question>& questions, const string& filename);
};

// Analytics and reporting
class AdminAnalytics {
private:
    DatabaseManager* dbManager;
    
public:
    AdminAnalytics(DatabaseManager* db) : dbManager(db) {}
    
    // Performance analytics
    struct PerformanceMetrics {
        double averageScore;
        double passRate;
        int totalExams;
        string topPerformingSubject;
        string weakestSubject;
        double improvementTrend;
    };
    
    PerformanceMetrics calculateOverallPerformance();
    vector<pair<string, double>> getSubjectPerformance();
    vector<pair<string, int>> getUserActivityStats();
    vector<pair<string, double>> getDifficultyAnalysis();
    
    // Trend analysis
    vector<pair<string, double>> getMonthlyPerformanceTrend();
    vector<pair<string, int>> getExamFrequencyTrend();
    vector<pair<string, double>> getUserEngagementTrend();
    
    // Predictive analytics
    double predictUserPerformance(int userId, const string& subject);
    vector<Question> recommendQuestionsForImprovement(int userId);
    double calculateQuestionDifficulty(int questionId);
    
    // Report generation
    string generatePerformanceReport();
    string generateUserActivityReport();
    string generateSystemHealthReport();
    string generateCustomReport(const vector<string>& metrics);
};

#endif // ADMIN_H