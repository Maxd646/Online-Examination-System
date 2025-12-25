#include "database.h"
#include "../structure/utils.h"
#include "../components/sorting.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
using namespace std;

// DatabaseManager implementation
DatabaseManager::DatabaseManager(const string &databasePath)
    : db(nullptr), dbPath(databasePath), isConnected(false)
{
    connectionPool.resize(MAX_CONNECTIONS, nullptr);
    connectionInUse.resize(MAX_CONNECTIONS, false);
}

DatabaseManager::~DatabaseManager()
{
    disconnect();

    // Clean up connection pool
    for (auto &conn : connectionPool)
    {
        if (conn)
        {
            sqlite3_close(conn);
        }
    }

    // Clean up prepared statements
    auto statements = preparedStatements.getValues();
    for (auto stmt : statements)
    {
        if (stmt)
        {
            sqlite3_finalize(stmt);
        }
    }
}

bool DatabaseManager::connect()
{
    if (isConnected)
        return true;

    int rc = sqlite3_open(dbPath.c_str(), &db);
    if (rc != SQLITE_OK)
    {
        logError("connect", sqlite3_errmsg(db));
        return false;
    }

    isConnected = true;

    // Enable foreign keys
    executeSQL("PRAGMA foreign_keys = ON;");

    // Set journal mode to WAL for better performance
    executeSQL("PRAGMA journal_mode = WAL;");

    return true;
}

void DatabaseManager::disconnect()
{
    if (db)
    {
        sqlite3_close(db);
        db = nullptr;
        isConnected = false;
    }
}

bool DatabaseManager::initializeDatabase()
{
    if (!connect())
    {
        return false;
    }

    return createTables() && insertDefaultData();
}

bool DatabaseManager::createTables()
{
    vector<string> createTableQueries = {
        // Users table
        R"(
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT UNIQUE NOT NULL,
            password TEXT NOT NULL,
            email TEXT UNIQUE NOT NULL,
            full_name TEXT NOT NULL,
            role INTEGER NOT NULL DEFAULT 2,
            status INTEGER NOT NULL DEFAULT 1,
            created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
            last_login TEXT,
            login_attempts INTEGER DEFAULT 0,
            is_locked BOOLEAN DEFAULT 0,
            lock_time TEXT
        );
        )",

        // Questions table
        R"(
        CREATE TABLE IF NOT EXISTS questions (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            subject TEXT NOT NULL,
            question_text TEXT NOT NULL,
            option1 TEXT NOT NULL,
            option2 TEXT NOT NULL,
            option3 TEXT NOT NULL,
            option4 TEXT NOT NULL,
            correct_answer INTEGER NOT NULL CHECK(correct_answer >= 0 AND correct_answer <= 3),
            difficulty TEXT NOT NULL DEFAULT 'Medium',
            explanation TEXT,
            created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
            updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
            created_by INTEGER,
            is_active BOOLEAN DEFAULT 1,
            FOREIGN KEY(created_by) REFERENCES users(id)
        );
        )",

        // Exam configurations table
        R"(
        CREATE TABLE IF NOT EXISTS exam_configs (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            title TEXT NOT NULL,
            exam_type INTEGER NOT NULL,
            subject TEXT NOT NULL,
            time_limit INTEGER NOT NULL,
            question_count INTEGER NOT NULL,
            difficulty TEXT NOT NULL DEFAULT 'Mixed',
            is_active BOOLEAN DEFAULT 1,
            created_by TEXT NOT NULL,
            created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
        );
        )",

        // Exam results table (updated)
        R"(
        CREATE TABLE IF NOT EXISTS exam_results (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id INTEGER NOT NULL,
            username TEXT NOT NULL,
            exam_config_id INTEGER,
            score INTEGER NOT NULL,
            total_questions INTEGER NOT NULL,
            percentage REAL NOT NULL,
            exam_date TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
            start_time TEXT,
            end_time TEXT,
            duration INTEGER,
            subject TEXT,
            difficulty TEXT,
            exam_type INTEGER,
            FOREIGN KEY(user_id) REFERENCES users(id),
            FOREIGN KEY(exam_config_id) REFERENCES exam_configs(id)
        );
        )",

        // Exam answers table (for detailed analysis)
        R"(
        CREATE TABLE IF NOT EXISTS exam_answers (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            result_id INTEGER NOT NULL,
            question_id INTEGER NOT NULL,
            user_answer INTEGER,
            is_correct BOOLEAN NOT NULL,
            time_taken INTEGER,
            FOREIGN KEY(result_id) REFERENCES exam_results(id),
            FOREIGN KEY(question_id) REFERENCES questions(id)
        );
        )",

        // User sessions table
        R"(
        CREATE TABLE IF NOT EXISTS user_sessions (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id INTEGER NOT NULL,
            session_id TEXT UNIQUE NOT NULL,
            start_time TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
            last_activity TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
            ip_address TEXT,
            is_active BOOLEAN DEFAULT 1,
            FOREIGN KEY(user_id) REFERENCES users(id)
        );
        )"};

    for (const auto &query : createTableQueries)
    {
        if (!executeSQL(query))
        {
            return false;
        }
    }

    // Create indexes for better performance
    vector<string> indexQueries = {
        "CREATE INDEX IF NOT EXISTS idx_users_username ON users(username);",
        "CREATE INDEX IF NOT EXISTS idx_users_email ON users(email);",
        "CREATE INDEX IF NOT EXISTS idx_questions_subject ON questions(subject);",
        "CREATE INDEX IF NOT EXISTS idx_questions_difficulty ON questions(difficulty);",
        "CREATE INDEX IF NOT EXISTS idx_exam_configs_type ON exam_configs(exam_type);",
        "CREATE INDEX IF NOT EXISTS idx_exam_configs_subject ON exam_configs(subject);",
        "CREATE INDEX IF NOT EXISTS idx_exam_results_user_id ON exam_results(user_id);",
        "CREATE INDEX IF NOT EXISTS idx_exam_results_date ON exam_results(exam_date);",
        "CREATE INDEX IF NOT EXISTS idx_exam_answers_result_id ON exam_answers(result_id);",
        "CREATE INDEX IF NOT EXISTS idx_sessions_user_id ON user_sessions(user_id);",
        "CREATE INDEX IF NOT EXISTS idx_sessions_session_id ON user_sessions(session_id);"};

    for (const auto &query : indexQueries)
    {
        executeSQL(query);
    }

    return true;
}

bool DatabaseManager::insertDefaultData()
{
    // Insert default admin user
    const char *checkAdmin = "SELECT COUNT(*) FROM users WHERE role = 1;";
    sqlite3_stmt *stmt = prepareStatement(checkAdmin);

    if (stmt && sqlite3_step(stmt) == SQLITE_ROW)
    {
        int count = sqlite3_column_int(stmt, 0);
        finalizeStatement(stmt);

        if (count == 0)
        {
            User admin(1, "admin", "admin123", "admin@exam.com", "System Administrator", UserRole::TEACHER);
            if (!insertUser(admin))
            {
                return false;
            }
        }
    }

    // Insert sample questions if none exist
    const char *checkQuestions = "SELECT COUNT(*) FROM questions;";
    stmt = prepareStatement(checkQuestions);

    if (stmt && sqlite3_step(stmt) == SQLITE_ROW)
    {
        int count = sqlite3_column_int(stmt, 0);
        finalizeStatement(stmt);

        if (count == 0)
        {
            // Sample questions can be added later via admin panel
        }
    }

    return true;
}

bool DatabaseManager::insertUser(const User &user)
{
    const char *sql = R"(
        INSERT INTO users (username, password, email, full_name, role, status, created_at) 
        VALUES (?, ?, ?, ?, ?, ?, ?);
    )";

    sqlite3_stmt *stmt = prepareStatement(sql);
    if (!stmt) {
        logError("insertUser", "Failed to prepare statement");
        return false;
    }

    sqlite3_bind_text(stmt, 1, user.getUsername().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, user.getPassword().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, user.getEmail().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, user.getFullName().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 5, static_cast<int>(user.getRole()));
    sqlite3_bind_int(stmt, 6, static_cast<int>(user.getStatus()));
    string currentTime = Utils::getCurrentDateTime();
    sqlite3_bind_text(stmt, 7, currentTime.c_str(), -1, SQLITE_TRANSIENT);

    int result = sqlite3_step(stmt);
    
    if (result != SQLITE_DONE) {
        logError("insertUser", sqlite3_errmsg(db));
    }
    
    finalizeStatement(stmt);

    return result == SQLITE_DONE;
}

bool DatabaseManager::updateUser(const User &user)
{
    const char *sql = R"(
        UPDATE users SET username=?, password=?, email=?, full_name=?, role=?, status=?, 
               last_login=?, login_attempts=?, is_locked=? 
        WHERE id=?;
    )";

    sqlite3_stmt *stmt = prepareStatement(sql);
    if (!stmt)
        return false;

    sqlite3_bind_text(stmt, 1, user.getUsername().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, user.getPassword().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, user.getEmail().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, user.getFullName().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 5, static_cast<int>(user.getRole()));
    sqlite3_bind_int(stmt, 6, static_cast<int>(user.getStatus()));
    sqlite3_bind_text(stmt, 7, user.getLastLogin().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 8, user.getLoginAttempts());
    sqlite3_bind_int(stmt, 9, user.getIsLocked() ? 1 : 0);
    sqlite3_bind_int(stmt, 10, user.getId());

    int result = sqlite3_step(stmt);
    finalizeStatement(stmt);

    return result == SQLITE_DONE;
}

User DatabaseManager::getUserByUsername(const string &username)
{
    // Return empty user if username is empty
    if (username.empty()) {
        return User();
    }
    
    const char *sql = R"(
        SELECT id, username, password, email, full_name, role, status, created_at, 
               last_login, login_attempts, is_locked 
        FROM users WHERE username = ? AND username != '';
    )";

    sqlite3_stmt *stmt = prepareStatement(sql);
    User user;

    if (stmt)
    {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);

        if (sqlite3_step(stmt) == SQLITE_ROW)
        {
            user.setId(sqlite3_column_int(stmt, 0));
            
            const char* usernamePtr = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
            const char* passwordPtr = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
            const char* emailPtr = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
            const char* fullNamePtr = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 4));
            
            if (usernamePtr) user.setUsername(usernamePtr);
            if (passwordPtr) user.setPassword(passwordPtr);
            if (emailPtr) user.setEmail(emailPtr);
            if (fullNamePtr) user.setFullName(fullNamePtr);
            
            user.setRole(static_cast<UserRole>(sqlite3_column_int(stmt, 5)));
            user.setStatus(static_cast<UserStatus>(sqlite3_column_int(stmt, 6)));

            const char *lastLogin = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 8));
            if (lastLogin)
            {
                user.setLastLogin(lastLogin);
            }
        }

        finalizeStatement(stmt);
    }

    return user;
}

vector<User> DatabaseManager::getAllUsers()
{
    const char *sql = R"(
        SELECT id, username, password, email, full_name, role, status, created_at, 
               last_login, login_attempts, is_locked 
        FROM users ORDER BY id;
    )";

    sqlite3_stmt *stmt = prepareStatement(sql);
    vector<User> users;

    if (stmt)
    {
        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            User user;
            user.setId(sqlite3_column_int(stmt, 0));
            user.setUsername(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1)));
            user.setPassword(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2)));
            user.setEmail(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3)));
            user.setFullName(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 4)));
            user.setRole(static_cast<UserRole>(sqlite3_column_int(stmt, 5)));
            user.setStatus(static_cast<UserStatus>(sqlite3_column_int(stmt, 6)));

            const char *lastLogin = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 8));
            if (lastLogin)
            {
                user.setLastLogin(lastLogin);
            }

            users.push_back(user);
        }

        finalizeStatement(stmt);
    }

    return users;
}

vector<User> DatabaseManager::getUsersByRole(UserRole role)
{
    const char *sql = R"(
        SELECT id, username, password, email, full_name, role, status, created_at, 
               last_login, login_attempts, is_locked 
        FROM users WHERE role = ? ORDER BY id;
    )";

    sqlite3_stmt *stmt = prepareStatement(sql);
    vector<User> users;

    if (stmt)
    {
        sqlite3_bind_int(stmt, 1, static_cast<int>(role));
        
        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            User user;
            user.setId(sqlite3_column_int(stmt, 0));
            user.setUsername(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1)));
            user.setPassword(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2)));
            user.setEmail(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3)));
            user.setFullName(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 4)));
            user.setRole(static_cast<UserRole>(sqlite3_column_int(stmt, 5)));
            user.setStatus(static_cast<UserStatus>(sqlite3_column_int(stmt, 6)));

            const char *lastLogin = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 8));
            if (lastLogin)
            {
                user.setLastLogin(lastLogin);
            }

            users.push_back(user);
        }

        finalizeStatement(stmt);
    }

    return users;
}

int DatabaseManager::getUserCountByRole(UserRole role)
{
    const char *sql = "SELECT COUNT(*) FROM users WHERE role = ?;";
    sqlite3_stmt *stmt = prepareStatement(sql);

    if (stmt)
    {
        sqlite3_bind_int(stmt, 1, static_cast<int>(role));
        
        if (sqlite3_step(stmt) == SQLITE_ROW)
        {
            int count = sqlite3_column_int(stmt, 0);
            finalizeStatement(stmt);
            return count;
        }
        
        finalizeStatement(stmt);
    }

    return 0;
}

bool DatabaseManager::authenticateUser(const string &username, const string &password, User &user)
{
    user = getUserByUsername(username);

    if (user.getId() == 0)
    {
        return false; // User not found
    }

    if (user.isAccountLocked())
    {
        return false; // Account is locked
    }

    if (user.verifyPassword(password))
    {
        user.resetLoginAttempts();
        user.updateLastLogin();
        updateUser(user);
        return true;
    }
    else
    {
        user.incrementLoginAttempts();
        updateUser(user);
        return false;
    }
}

bool DatabaseManager::insertQuestion(const Question &question)
{
    const char *sql = R"(
        INSERT INTO questions (subject, question_text, option1, option2, option3, option4, 
                             correct_answer, difficulty, explanation, created_by) 
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?);
    )";

    sqlite3_stmt *stmt = prepareStatement(sql);
    if (!stmt)
    {
        logError("insertQuestion", "Failed to prepare statement");
        return false;
    }

    auto options = question.getOptions();

    sqlite3_bind_text(stmt, 1, question.getSubject().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, question.getQuestionText().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, options[0].c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, options[1].c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, options[2].c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, options[3].c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 7, question.getCorrectAnswer());
    sqlite3_bind_text(stmt, 8, question.getDifficulty().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 9, question.getExplanation().c_str(), -1, SQLITE_TRANSIENT);
    
    // Handle created_by field - use NULL if 0
    if (question.getCreatedBy() > 0)
    {
        sqlite3_bind_int(stmt, 10, question.getCreatedBy());
    }
    else
    {
        sqlite3_bind_null(stmt, 10);
    }

    int result = sqlite3_step(stmt);
    
    if (result != SQLITE_DONE)
    {
        logError("insertQuestion", sqlite3_errmsg(db));
    }
    
    finalizeStatement(stmt);

    return result == SQLITE_DONE;
}

vector<Question> DatabaseManager::getAllQuestions()
{
    vector<Question> questions;
    const char *sql = R"(
        SELECT id, subject, question_text, option1, option2, option3, option4, 
               correct_answer, difficulty, explanation, created_by, is_active
        FROM questions WHERE is_active = 1 ORDER BY id;
    )";

    sqlite3_stmt *stmt = prepareStatement(sql);

    if (stmt)
    {
        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            Question question;
            question.setId(sqlite3_column_int(stmt, 0));
            question.setSubject(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1)));
            question.setQuestionText(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2)));

            vector<string> options = {
                reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3)),
                reinterpret_cast<const char *>(sqlite3_column_text(stmt, 4)),
                reinterpret_cast<const char *>(sqlite3_column_text(stmt, 5)),
                reinterpret_cast<const char *>(sqlite3_column_text(stmt, 6))};
            question.setOptions(options);
            question.setCorrectAnswer(sqlite3_column_int(stmt, 7));
            question.setDifficulty(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 8)));

            const char *explanation = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 9));
            if (explanation)
            {
                question.setExplanation(explanation);
            }

            const char *createdBy = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 10));
            if (createdBy) {
                question.setCreatedBy(sqlite3_column_int(stmt, 10));
            }
            question.setIsActive(sqlite3_column_int(stmt, 11) == 1);

            questions.push_back(question);
        }

        finalizeStatement(stmt);
    }

    return questions;
}

vector<Question> DatabaseManager::getRandomQuestions(int count, const string &subject)
{
    string sql = R"(
        SELECT id, subject, question_text, option1, option2, option3, option4, 
               correct_answer, difficulty, explanation, is_active
        FROM questions WHERE is_active = 1
    )";

    if (!subject.empty())
    {
        sql += " AND subject = '" + escapeString(subject) + "'";
    }

    sql += " ORDER BY RANDOM() LIMIT " + to_string(count) + ";";

    vector<Question> questions;
    sqlite3_stmt *stmt = prepareStatement(sql);

    if (stmt)
    {
        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            Question question;
            question.setId(sqlite3_column_int(stmt, 0));
            question.setSubject(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1)));
            question.setQuestionText(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2)));

            vector<string> options = {
                reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3)),
                reinterpret_cast<const char *>(sqlite3_column_text(stmt, 4)),
                reinterpret_cast<const char *>(sqlite3_column_text(stmt, 5)),
                reinterpret_cast<const char *>(sqlite3_column_text(stmt, 6))};
            question.setOptions(options);
            question.setCorrectAnswer(sqlite3_column_int(stmt, 7));
            question.setDifficulty(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 8)));
            
            const char* explanation = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 9));
            if (explanation) {
                question.setExplanation(explanation);
            }
            
            question.setIsActive(sqlite3_column_int(stmt, 10) == 1);

            questions.push_back(question);
        }

        finalizeStatement(stmt);
    }

    return questions;
}

bool DatabaseManager::insertExamResult(const ExamResult &result)
{
    const char *sql = R"(
        INSERT INTO exam_results (user_id, username, score, total_questions, percentage, 
                                exam_date, start_time, end_time, duration, subject, difficulty) 
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);
    )";

    sqlite3_stmt *stmt = prepareStatement(sql);
    if (!stmt)
        return false;

    sqlite3_bind_int(stmt, 1, result.getUserId());
    sqlite3_bind_text(stmt, 2, result.getUsername().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, result.getScore());
    sqlite3_bind_int(stmt, 4, result.getTotalQuestions());
    sqlite3_bind_double(stmt, 5, result.getPercentage());
    sqlite3_bind_text(stmt, 6, result.getExamDate().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 7, result.getStartTime().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 8, result.getEndTime().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 9, result.getDuration());
    sqlite3_bind_text(stmt, 10, result.getSubject().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 11, result.getDifficulty().c_str(), -1, SQLITE_TRANSIENT);

    int res = sqlite3_step(stmt);
    finalizeStatement(stmt);

    return res == SQLITE_DONE;
}

vector<ExamResult> DatabaseManager::getExamResultsByUser(int userId)
{
    vector<ExamResult> results;
    const char *sql = R"(
        SELECT id, user_id, username, score, total_questions, percentage, exam_date, 
               start_time, end_time, duration, subject, difficulty
        FROM exam_results WHERE user_id = ? ORDER BY exam_date DESC;
    )";

    sqlite3_stmt *stmt = prepareStatement(sql);

    if (stmt)
    {
        sqlite3_bind_int(stmt, 1, userId);

        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            ExamResult result;
            result.setId(sqlite3_column_int(stmt, 0));
            result.setUserId(sqlite3_column_int(stmt, 1));
            result.setUsername(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2)));
            result.setScore(sqlite3_column_int(stmt, 3));
            result.setTotalQuestions(sqlite3_column_int(stmt, 4));
            result.setExamDate(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 6)));

            const char *startTime = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 7));
            const char *endTime = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 8));
            const char *subject = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 10));
            const char *difficulty = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 11));

            if (startTime)
                result.setStartTime(startTime);
            if (endTime)
                result.setEndTime(endTime);
            if (subject)
                result.setSubject(subject);
            if (difficulty)
                result.setDifficulty(difficulty);

            result.setDuration(sqlite3_column_int(stmt, 9));

            results.push_back(result);
        }

        finalizeStatement(stmt);
    }

    return results;
}

// Helper methods
bool DatabaseManager::executeSQL(const string &sql)
{
    if (!isConnected)
        return false;

    char *errorMsg = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errorMsg);

    if (rc != SQLITE_OK)
    {
        if (errorMsg)
        {
            logError("executeSQL", errorMsg);
            sqlite3_free(errorMsg);
        }
        return false;
    }

    return true;
}

sqlite3_stmt *DatabaseManager::prepareStatement(const string &sql)
{
    if (!isConnected)
        return nullptr;

    // DSA: Check if statement is cached in hash table for O(1) lookup
    sqlite3_stmt** cachedStmt = preparedStatements.find(sql);
    if (cachedStmt && *cachedStmt) {
        sqlite3_reset(*cachedStmt);  // Reset for reuse
        return *cachedStmt;
    }

    // Not cached, prepare new statement
    sqlite3_stmt *stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

    if (rc != SQLITE_OK)
    {
        logError("prepareStatement", sqlite3_errmsg(db));
        return nullptr;
    }

    // DSA: Cache the prepared statement in hash table
    preparedStatements.insert(sql, stmt);

    return stmt;
}

void DatabaseManager::finalizeStatement(sqlite3_stmt *stmt)
{
    if (stmt)
    {
        sqlite3_finalize(stmt);
    }
}

string DatabaseManager::escapeString(const string &str)
{
    string escaped;
    for (char c : str)
    {
        if (c == '\'')
        {
            escaped += "''";
        }
        else
        {
            escaped += c;
        }
    }
    return escaped;
}

void DatabaseManager::logError(const string &operation, const string &error)
{
    cerr << "Database Error in " << operation << ": " << error << endl;
}

// Question class implementation
Question::Question() : id(0), correctAnswer(0), createdBy(0), isActive(true)
{
    options.resize(4);
    createdAt = Utils::getCurrentDateTime();
    updatedAt = createdAt;
}

Question::Question(int id, const string &subject, const string &questionText,
                   const vector<string> &options, int correctAnswer,
                   const string &difficulty, const string &explanation)
    : id(id), subject(subject), questionText(questionText), options(options),
      correctAnswer(correctAnswer), difficulty(difficulty), explanation(explanation),
      createdBy(0), isActive(true)
{

    if (this->options.size() != 4)
    {
        this->options.resize(4);
    }

    createdAt = Utils::getCurrentDateTime();
    updatedAt = createdAt;
}

bool Question::isValid() const
{
    return !subject.empty() && !questionText.empty() &&
           options.size() == 4 && correctAnswer >= 0 && correctAnswer < 4 &&
           all_of(options.begin(), options.end(), [](const string &opt)
                  { return !opt.empty(); });
}

void Question::display() const
{
    cout << "\nQ" << id << ". " << questionText << endl;
    cout << "Subject: " << subject << " | Difficulty: " << difficulty << endl;

    for (size_t i = 0; i < options.size(); ++i)
    {
        cout << (i + 1) << ". " << options[i] << endl;
    }
}

void Question::displayWithAnswer() const
{
    display();
    cout << "Correct Answer: " << (correctAnswer + 1) << ". " << options[correctAnswer] << endl;
    if (!explanation.empty())
    {
        cout << "Explanation: " << explanation << endl;
    }
}

bool Question::operator==(const Question &other) const
{
    return id == other.id;
}

// ExamResult class implementation
ExamResult::ExamResult() : id(0), userId(0), score(0), totalQuestions(0),
                           percentage(0.0), duration(0)
{
    examDate = Utils::getCurrentDateTime();
}

ExamResult::ExamResult(int userId, const string &username, int score, int totalQuestions,
                       const string &subject, const string &difficulty)
    : id(0), userId(userId), username(username), score(score), totalQuestions(totalQuestions),
      duration(0), subject(subject), difficulty(difficulty)
{
    examDate = Utils::getCurrentDateTime();
    calculatePercentage();
}

void ExamResult::calculatePercentage()
{
    if (totalQuestions > 0)
    {
        percentage = (static_cast<double>(score) / totalQuestions) * 100.0;
    }
    else
    {
        percentage = 0.0;
    }
}

string ExamResult::getGrade() const
{
    if (percentage >= 90)
        return "A+";
    if (percentage >= 85)
        return "A";
    if (percentage >= 80)
        return "B+";
    if (percentage >= 75)
        return "B";
    if (percentage >= 70)
        return "C+";
    if (percentage >= 65)
        return "C";
    if (percentage >= 60)
        return "D";
    return "F";
}

bool ExamResult::isPassed(double passingPercentage) const
{
    return percentage >= passingPercentage;
}

void ExamResult::display() const
{
    cout << "Exam Result ID: " << id << endl;
    cout << "Student: " << username << endl;
    cout << "Score: " << score << "/" << totalQuestions << endl;
    cout << "Percentage: " << fixed << setprecision(2) << percentage << "%" << endl;
    cout << "Grade: " << getGrade() << endl;
    cout << "Date: " << examDate << endl;
    cout << "Status: " << (isPassed() ? "PASSED" : "FAILED") << endl;
}

bool ExamResult::operator==(const ExamResult &other) const
{
    return id == other.id;
}

bool ExamResult::operator<(const ExamResult &other) const
{
    return percentage < other.percentage;
}

bool ExamResult::operator>(const ExamResult &other) const
{
    return percentage > other.percentage;
}

bool ExamResult::operator<=(const ExamResult &other) const
{
    return *this < other || *this == other;
}

bool ExamResult::operator>=(const ExamResult &other) const
{
    return *this > other || *this == other;
}
// Add missing DatabaseManager methods
int DatabaseManager::getTotalUsers()
{
    const char *sql = "SELECT COUNT(*) FROM users;";
    sqlite3_stmt *stmt = prepareStatement(sql);

    if (stmt && sqlite3_step(stmt) == SQLITE_ROW)
    {
        int count = sqlite3_column_int(stmt, 0);
        finalizeStatement(stmt);
        return count;
    }

    if (stmt)
        finalizeStatement(stmt);
    return 0;
}

int DatabaseManager::getTotalQuestions()
{
    const char *sql = "SELECT COUNT(*) FROM questions WHERE is_active = 1;";
    sqlite3_stmt *stmt = prepareStatement(sql);

    if (stmt && sqlite3_step(stmt) == SQLITE_ROW)
    {
        int count = sqlite3_column_int(stmt, 0);
        finalizeStatement(stmt);
        return count;
    }

    if (stmt)
        finalizeStatement(stmt);
    return 0;
}

int DatabaseManager::getTotalExamResults()
{
    const char *sql = "SELECT COUNT(*) FROM exam_results;";
    sqlite3_stmt *stmt = prepareStatement(sql);

    if (stmt && sqlite3_step(stmt) == SQLITE_ROW)
    {
        int count = sqlite3_column_int(stmt, 0);
        finalizeStatement(stmt);
        return count;
    }

    if (stmt)
        finalizeStatement(stmt);
    return 0;
}

// Question operations
Question DatabaseManager::getQuestionById(int questionId)
{
    const char *sql = R"(
        SELECT id, subject, question_text, option1, option2, option3, option4, 
               correct_answer, difficulty, explanation, is_active 
        FROM questions WHERE id = ?;
    )";

    sqlite3_stmt *stmt = prepareStatement(sql);
    Question question;

    if (stmt)
    {
        sqlite3_bind_int(stmt, 1, questionId);

        if (sqlite3_step(stmt) == SQLITE_ROW)
        {
            question.setId(sqlite3_column_int(stmt, 0));
            question.setSubject(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1)));
            question.setQuestionText(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2)));

            vector<string> options = {
                reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3)),
                reinterpret_cast<const char *>(sqlite3_column_text(stmt, 4)),
                reinterpret_cast<const char *>(sqlite3_column_text(stmt, 5)),
                reinterpret_cast<const char *>(sqlite3_column_text(stmt, 6))};
            question.setOptions(options);
            question.setCorrectAnswer(sqlite3_column_int(stmt, 7));
            question.setDifficulty(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 8)));

            const char *explanation = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 9));
            if (explanation)
            {
                question.setExplanation(explanation);
            }

            question.setIsActive(sqlite3_column_int(stmt, 10) == 1);
        }

        finalizeStatement(stmt);
    }

    return question;
}

bool DatabaseManager::updateQuestion(const Question &question)
{
    const char *sql = R"(
        UPDATE questions 
        SET subject = ?, question_text = ?, option1 = ?, option2 = ?, 
            option3 = ?, option4 = ?, correct_answer = ?, difficulty = ?, 
            explanation = ?, is_active = ?, updated_at = CURRENT_TIMESTAMP
        WHERE id = ?;
    )";

    sqlite3_stmt *stmt = prepareStatement(sql);

    if (stmt)
    {
        auto options = question.getOptions();
        sqlite3_bind_text(stmt, 1, question.getSubject().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, question.getQuestionText().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, options[0].c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 4, options[1].c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 5, options[2].c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 6, options[3].c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 7, question.getCorrectAnswer());
        sqlite3_bind_text(stmt, 8, question.getDifficulty().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 9, question.getExplanation().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 10, question.getIsActive() ? 1 : 0);
        sqlite3_bind_int(stmt, 11, question.getId());

        int result = sqlite3_step(stmt);
        finalizeStatement(stmt);

        return result == SQLITE_DONE;
    }

    return false;
}

bool DatabaseManager::deleteQuestion(int questionId)
{
    const char *sql = "DELETE FROM questions WHERE id = ?;";

    sqlite3_stmt *stmt = prepareStatement(sql);

    if (stmt)
    {
        sqlite3_bind_int(stmt, 1, questionId);

        int result = sqlite3_step(stmt);
        finalizeStatement(stmt);

        return result == SQLITE_DONE;
    }

    return false;
}

vector<Question> DatabaseManager::getQuestionsBySubject(const string &subject)
{
    const char *sql = R"(
        SELECT id, subject, question_text, option1, option2, option3, option4, 
               correct_answer, difficulty, explanation, is_active 
        FROM questions WHERE subject = ? AND is_active = 1;
    )";

    sqlite3_stmt *stmt = prepareStatement(sql);
    vector<Question> questions;

    if (stmt)
    {
        sqlite3_bind_text(stmt, 1, subject.c_str(), -1, SQLITE_TRANSIENT);

        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            Question question;
            question.setId(sqlite3_column_int(stmt, 0));
            question.setSubject(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1)));
            question.setQuestionText(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2)));

            vector<string> options = {
                reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3)),
                reinterpret_cast<const char *>(sqlite3_column_text(stmt, 4)),
                reinterpret_cast<const char *>(sqlite3_column_text(stmt, 5)),
                reinterpret_cast<const char *>(sqlite3_column_text(stmt, 6))};
            question.setOptions(options);
            question.setCorrectAnswer(sqlite3_column_int(stmt, 7));
            question.setDifficulty(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 8)));

            const char *explanation = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 9));
            if (explanation)
            {
                question.setExplanation(explanation);
            }

            question.setIsActive(sqlite3_column_int(stmt, 10) == 1);
            questions.push_back(question);
        }

        finalizeStatement(stmt);
    }

    return questions;
}

vector<Question> DatabaseManager::getQuestionsByDifficulty(const string &difficulty)
{
    const char *sql = R"(
        SELECT id, subject, question_text, option1, option2, option3, option4, 
               correct_answer, difficulty, explanation, is_active 
        FROM questions WHERE difficulty = ? AND is_active = 1;
    )";

    sqlite3_stmt *stmt = prepareStatement(sql);
    vector<Question> questions;

    if (stmt)
    {
        sqlite3_bind_text(stmt, 1, difficulty.c_str(), -1, SQLITE_TRANSIENT);

        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            Question question;
            question.setId(sqlite3_column_int(stmt, 0));
            question.setSubject(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1)));
            question.setQuestionText(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2)));

            vector<string> options = {
                reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3)),
                reinterpret_cast<const char *>(sqlite3_column_text(stmt, 4)),
                reinterpret_cast<const char *>(sqlite3_column_text(stmt, 5)),
                reinterpret_cast<const char *>(sqlite3_column_text(stmt, 6))};
            question.setOptions(options);
            question.setCorrectAnswer(sqlite3_column_int(stmt, 7));
            question.setDifficulty(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 8)));

            const char *explanation = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 9));
            if (explanation)
            {
                question.setExplanation(explanation);
            }

            question.setIsActive(sqlite3_column_int(stmt, 10) == 1);
            questions.push_back(question);
        }

        finalizeStatement(stmt);
    }

    return questions;
}

vector<Question> DatabaseManager::searchQuestions(const string &keyword)
{
    const char *sql = R"(
        SELECT id, subject, question_text, option1, option2, option3, option4, 
               correct_answer, difficulty, explanation, is_active 
        FROM questions 
        WHERE (question_text LIKE ? OR subject LIKE ? OR explanation LIKE ?) 
        AND is_active = 1;
    )";

    sqlite3_stmt *stmt = prepareStatement(sql);
    vector<Question> questions;
    if (stmt)
    {
        string searchPattern = "%" + keyword + "%";
        sqlite3_bind_text(stmt, 1, searchPattern.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, searchPattern.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, searchPattern.c_str(), -1, SQLITE_TRANSIENT);

        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            Question question;
            question.setId(sqlite3_column_int(stmt, 0));
            question.setSubject(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1)));
            question.setQuestionText(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2)));

            vector<string> options = {
                reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3)),
                reinterpret_cast<const char *>(sqlite3_column_text(stmt, 4)),
                reinterpret_cast<const char *>(sqlite3_column_text(stmt, 5)),
                reinterpret_cast<const char *>(sqlite3_column_text(stmt, 6))};
            question.setOptions(options);
            question.setCorrectAnswer(sqlite3_column_int(stmt, 7));
            question.setDifficulty(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 8)));

            const char *explanation = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 9));
            if (explanation)
            {
                question.setExplanation(explanation);
            }

            question.setIsActive(sqlite3_column_int(stmt, 10) == 1);
            questions.push_back(question);
        }

        finalizeStatement(stmt);
    }

    return questions;
}

vector<ExamResult> DatabaseManager::getAllExamResults()
{
    const char *sql = R"(
        SELECT id, user_id, username, score, total_questions, percentage, 
               exam_date, start_time, end_time, duration, subject, difficulty 
        FROM exam_results ORDER BY exam_date DESC;
    )";

    sqlite3_stmt *stmt = prepareStatement(sql);
    vector<ExamResult> results;

    if (stmt)
    {
        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            ExamResult result;
            result.setId(sqlite3_column_int(stmt, 0));
            result.setUserId(sqlite3_column_int(stmt, 1));
            result.setUsername(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2)));
            result.setScore(sqlite3_column_int(stmt, 3));
            result.setTotalQuestions(sqlite3_column_int(stmt, 4));
            // percentage is calculated automatically in setScore/setTotalQuestions

            const char *examDate = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 6));
            if (examDate)
                result.setExamDate(examDate);

            const char *startTime = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 7));
            if (startTime)
                result.setStartTime(startTime);

            const char *endTime = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 8));
            if (endTime)
                result.setEndTime(endTime);

            result.setDuration(sqlite3_column_int(stmt, 9));

            const char *subject = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 10));
            if (subject)
                result.setSubject(subject);

            const char *difficulty = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 11));
            if (difficulty)
                result.setDifficulty(difficulty);

            results.push_back(result);
        }

        finalizeStatement(stmt);
    }

    return results;
}

// DSA: Get exam results sorted using custom sorting algorithms
vector<ExamResult> DatabaseManager::getSortedExamResults(const string& sortBy)
{
    vector<ExamResult> results = getAllExamResults();
    
    // Use custom sorting algorithms based on sort criteria
    if (sortBy == "percentage") {
        // Sort by percentage (descending) using quick sort
        SortingAlgorithms<ExamResult>::quickSortCustom(results, 0, results.size() - 1,
            [](const ExamResult& a, const ExamResult& b) {
                return a.getPercentage() > b.getPercentage();
            });
    }
    else if (sortBy == "date") {
        // Sort by exam date (newest first)
        SortingAlgorithms<ExamResult>::mergeSortCustom(results, 0, results.size() - 1,
            [](const ExamResult& a, const ExamResult& b) {
                return a.getExamDate() > b.getExamDate();
            });
    }
    else if (sortBy == "username") {
        // Sort by username alphabetically
        SortingAlgorithms<ExamResult>::heapSort(results);
    }
    
    return results;
}