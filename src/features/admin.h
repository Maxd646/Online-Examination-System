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
    std::string data; // JSON or serialized data
    std::string timestamp;
    
    OperationData(AdminOperation t, int id, const std::string& d) 
        : type(t), targetId(id), data(d), timestamp(Utils::getCurrentDateTime()) {}
};

class AdminPanel {
private:
    DatabaseManager* dbManager;
    User currentAdmin;
    UndoRedoStack<OperationData> operationHistory;
    LinkedList<std::string> recentActions;
    
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
    bool importQuestionsFromFile(const std::string& filename);
    bool exportQuestionsToFile(const std::string& filename);
    
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
    void exportResultsToFile(const std::string& filename);
    
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
    void logAdminAction(const std::string& action);
    void updateStatisticsCache();
    
    // Display helpers
    void displayQuestionTable(const std::vector<Question>& questions);
    void displayUserTable(const std::vector<User>& users);
    void displayResultTable(const std::vector<ExamResult>& results);
    void displayStatisticsSummary();
    
    // Input helpers
    Question inputQuestionData();
    User inputUserData();
    std::string selectSubject();
    std::string selectDifficulty();
    UserRole selectUserRole();
    
    // File operations
    bool exportData(const std::string& dataType, const std::string& filename);
    bool importData(const std::string& dataType, const std::string& filename);
    
    // Security and audit
    void viewAuditLog();
    void generateSecurityReport();
    bool changeAdminPassword();
    void viewLoginAttempts();
    
private:
    // Helper methods
    void clearScreen() { Utils::clearScreen(); }
    void pauseSystem() { Utils::pauseSystem(); }
    bool confirmAction(const std::string& message) { return Utils::confirmAction(message); }
    
    // Menu helpers
    int getMenuChoice(int minChoice, int maxChoice);
    void displayMenuHeader(const std::string& title);
    void displayMenuFooter();
    
    // Data processing
    std::vector<std::pair<std::string, int>> calculateSubjectDistribution();
    std::vector<std::pair<std::string, double>> calculateDifficultyDistribution();
    std::vector<std::pair<std::string, double>> calculatePerformanceBySubject();
    std::vector<std::pair<std::string, int>> calculateMonthlyExamCounts();
    
    // Report generation
    void generatePDFReport(const std::string& reportType, const std::string& filename);
    void generateCSVReport(const std::string& reportType, const std::string& filename);
    void generateHTMLReport(const std::string& reportType, const std::string& filename);
    
    // Validation helpers
    bool isValidQuestionInput(const std::string& input);
    bool isValidUserInput(const std::string& input);
    bool isValidEmailFormat(const std::string& email);
    bool isValidPasswordStrength(const std::string& password);
    
    // Cache management
    void invalidateCache();
    void refreshCache();
    
    // Error handling
    void handleDatabaseError(const std::string& operation);
    void handleFileError(const std::string& operation, const std::string& filename);
    void handleValidationError(const std::string& field, const std::string& value);
};

// Question import/export formats
class QuestionImporter {
public:
    static std::vector<Question> importFromCSV(const std::string& filename);
    static std::vector<Question> importFromJSON(const std::string& filename);
    static std::vector<Question> importFromXML(const std::string& filename);
    static bool validateImportFormat(const std::string& filename, const std::string& format);
};

class QuestionExporter {
public:
    static bool exportToCSV(const std::vector<Question>& questions, const std::string& filename);
    static bool exportToJSON(const std::vector<Question>& questions, const std::string& filename);
    static bool exportToXML(const std::vector<Question>& questions, const std::string& filename);
    static bool exportToPDF(const std::vector<Question>& questions, const std::string& filename);
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
        std::string topPerformingSubject;
        std::string weakestSubject;
        double improvementTrend;
    };
    
    PerformanceMetrics calculateOverallPerformance();
    std::vector<std::pair<std::string, double>> getSubjectPerformance();
    std::vector<std::pair<std::string, int>> getUserActivityStats();
    std::vector<std::pair<std::string, double>> getDifficultyAnalysis();
    
    // Trend analysis
    std::vector<std::pair<std::string, double>> getMonthlyPerformanceTrend();
    std::vector<std::pair<std::string, int>> getExamFrequencyTrend();
    std::vector<std::pair<std::string, double>> getUserEngagementTrend();
    
    // Predictive analytics
    double predictUserPerformance(int userId, const std::string& subject);
    std::vector<Question> recommendQuestionsForImprovement(int userId);
    double calculateQuestionDifficulty(int questionId);
    
    // Report generation
    std::string generatePerformanceReport();
    std::string generateUserActivityReport();
    std::string generateSystemHealthReport();
    std::string generateCustomReport(const std::vector<std::string>& metrics);
};

#endif // ADMIN_H