-- Online Examination System Database Schema
-- SQLite Database Schema for comprehensive exam management

-- Enable foreign key constraints
PRAGMA foreign_keys = ON;

-- Users table for authentication and user management
CREATE TABLE IF NOT EXISTS users (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    username TEXT UNIQUE NOT NULL,
    password TEXT NOT NULL,
    email TEXT UNIQUE NOT NULL,
    full_name TEXT NOT NULL,
    role INTEGER NOT NULL DEFAULT 2, -- 1=Admin, 2=Student, 3=Instructor
    status INTEGER NOT NULL DEFAULT 1, -- 1=Active, 2=Inactive, 3=Suspended, 4=Pending
    created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    last_login TEXT,
    login_attempts INTEGER DEFAULT 0,
    is_locked BOOLEAN DEFAULT 0,
    lock_time TEXT,
    profile_image TEXT,
    phone TEXT,
    address TEXT,
    date_of_birth TEXT,
    gender TEXT,
    updated_at TEXT DEFAULT CURRENT_TIMESTAMP
);

-- Questions table for storing exam questions
CREATE TABLE IF NOT EXISTS questions (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    subject TEXT NOT NULL,
    question_text TEXT NOT NULL,
    option1 TEXT NOT NULL,
    option2 TEXT NOT NULL,
    option3 TEXT NOT NULL,
    option4 TEXT NOT NULL,
    correct_answer INTEGER NOT NULL CHECK(correct_answer >= 0 AND correct_answer <= 3),
    difficulty TEXT NOT NULL DEFAULT 'Medium', -- Easy, Medium, Hard
    explanation TEXT,
    created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    created_by INTEGER,
    is_active BOOLEAN DEFAULT 1,
    times_asked INTEGER DEFAULT 0,
    times_correct INTEGER DEFAULT 0,
    average_time INTEGER DEFAULT 0, -- in seconds
    tags TEXT, -- comma-separated tags
    FOREIGN KEY(created_by) REFERENCES users(id) ON DELETE SET NULL
);

-- Exam results table for storing exam outcomes
CREATE TABLE IF NOT EXISTS exam_results (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id INTEGER NOT NULL,
    username TEXT NOT NULL,
    score INTEGER NOT NULL,
    total_questions INTEGER NOT NULL,
    percentage REAL NOT NULL,
    exam_date TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    start_time TEXT,
    end_time TEXT,
    duration INTEGER, -- in minutes
    subject TEXT,
    difficulty TEXT,
    exam_type TEXT DEFAULT 'Practice', -- Practice, Mock, Official, Timed, Adaptive
    exam_mode TEXT DEFAULT 'Flexible', -- Sequential, Flexible, Review, Instant_Feedback
    is_passed BOOLEAN DEFAULT 0,
    grade TEXT,
    negative_marking BOOLEAN DEFAULT 0,
    negative_marks REAL DEFAULT 0,
    FOREIGN KEY(user_id) REFERENCES users(id) ON DELETE CASCADE
);

-- Detailed exam answers for analysis
CREATE TABLE IF NOT EXISTS exam_answers (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    result_id INTEGER NOT NULL,
    question_id INTEGER NOT NULL,
    user_answer INTEGER, -- -1 for unanswered
    is_correct BOOLEAN NOT NULL,
    time_taken INTEGER, -- in seconds
    attempt_count INTEGER DEFAULT 1,
    marked_for_review BOOLEAN DEFAULT 0,
    FOREIGN KEY(result_id) REFERENCES exam_results(id) ON DELETE CASCADE,
    FOREIGN KEY(question_id) REFERENCES questions(id) ON DELETE CASCADE
);

-- User sessions for security and tracking
CREATE TABLE IF NOT EXISTS user_sessions (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id INTEGER NOT NULL,
    session_id TEXT UNIQUE NOT NULL,
    start_time TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    last_activity TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    ip_address TEXT,
    user_agent TEXT,
    is_active BOOLEAN DEFAULT 1,
    end_time TEXT,
    FOREIGN KEY(user_id) REFERENCES users(id) ON DELETE CASCADE
);

-- Exam sessions for tracking active exams
CREATE TABLE IF NOT EXISTS exam_sessions (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id INTEGER NOT NULL,
    exam_id TEXT UNIQUE NOT NULL,
    settings TEXT, -- JSON string of exam settings
    current_question INTEGER DEFAULT 0,
    start_time TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    end_time TEXT,
    is_active BOOLEAN DEFAULT 1,
    is_completed BOOLEAN DEFAULT 0,
    is_submitted BOOLEAN DEFAULT 0,
    auto_submit_time TEXT,
    FOREIGN KEY(user_id) REFERENCES users(id) ON DELETE CASCADE
);

-- Question categories/subjects for organization
CREATE TABLE IF NOT EXISTS subjects (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT UNIQUE NOT NULL,
    description TEXT,
    is_active BOOLEAN DEFAULT 1,
    created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    question_count INTEGER DEFAULT 0
);

-- User preferences and settings
CREATE TABLE IF NOT EXISTS user_preferences (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id INTEGER UNIQUE NOT NULL,
    auto_save BOOLEAN DEFAULT 1,
    show_timer BOOLEAN DEFAULT 1,
    confirm_submission BOOLEAN DEFAULT 1,
    preferred_subject TEXT,
    preferred_difficulty TEXT DEFAULT 'Medium',
    default_question_count INTEGER DEFAULT 10,
    theme TEXT DEFAULT 'default',
    notifications BOOLEAN DEFAULT 1,
    FOREIGN KEY(user_id) REFERENCES users(id) ON DELETE CASCADE
);

-- System audit log for security and tracking
CREATE TABLE IF NOT EXISTS audit_log (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id INTEGER,
    action TEXT NOT NULL,
    table_name TEXT,
    record_id INTEGER,
    old_values TEXT, -- JSON
    new_values TEXT, -- JSON
    ip_address TEXT,
    user_agent TEXT,
    timestamp TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY(user_id) REFERENCES users(id) ON DELETE SET NULL
);

-- System settings and configuration
CREATE TABLE IF NOT EXISTS system_settings (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    setting_key TEXT UNIQUE NOT NULL,
    setting_value TEXT NOT NULL,
    description TEXT,
    updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_by INTEGER,
    FOREIGN KEY(updated_by) REFERENCES users(id) ON DELETE SET NULL
);

-- User achievements and badges
CREATE TABLE IF NOT EXISTS user_achievements (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id INTEGER NOT NULL,
    achievement_type TEXT NOT NULL, -- first_exam, perfect_score, streak_5, etc.
    achievement_name TEXT NOT NULL,
    description TEXT,
    earned_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    points INTEGER DEFAULT 0,
    FOREIGN KEY(user_id) REFERENCES users(id) ON DELETE CASCADE
);

-- Question feedback and ratings
CREATE TABLE IF NOT EXISTS question_feedback (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    question_id INTEGER NOT NULL,
    user_id INTEGER NOT NULL,
    rating INTEGER CHECK(rating >= 1 AND rating <= 5),
    feedback_text TEXT,
    is_reported BOOLEAN DEFAULT 0,
    report_reason TEXT,
    created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY(question_id) REFERENCES questions(id) ON DELETE CASCADE,
    FOREIGN KEY(user_id) REFERENCES users(id) ON DELETE CASCADE,
    UNIQUE(question_id, user_id)
);

-- Create indexes for better performance
CREATE INDEX IF NOT EXISTS idx_users_username ON users(username);
CREATE INDEX IF NOT EXISTS idx_users_email ON users(email);
CREATE INDEX IF NOT EXISTS idx_users_role ON users(role);
CREATE INDEX IF NOT EXISTS idx_users_status ON users(status);

CREATE INDEX IF NOT EXISTS idx_questions_subject ON questions(subject);
CREATE INDEX IF NOT EXISTS idx_questions_difficulty ON questions(difficulty);
CREATE INDEX IF NOT EXISTS idx_questions_active ON questions(is_active);
CREATE INDEX IF NOT EXISTS idx_questions_created_by ON questions(created_by);

CREATE INDEX IF NOT EXISTS idx_exam_results_user_id ON exam_results(user_id);
CREATE INDEX IF NOT EXISTS idx_exam_results_date ON exam_results(exam_date);
CREATE INDEX IF NOT EXISTS idx_exam_results_subject ON exam_results(subject);
CREATE INDEX IF NOT EXISTS idx_exam_results_type ON exam_results(exam_type);

CREATE INDEX IF NOT EXISTS idx_exam_answers_result_id ON exam_answers(result_id);
CREATE INDEX IF NOT EXISTS idx_exam_answers_question_id ON exam_answers(question_id);

CREATE INDEX IF NOT EXISTS idx_sessions_user_id ON user_sessions(user_id);
CREATE INDEX IF NOT EXISTS idx_sessions_session_id ON user_sessions(session_id);
CREATE INDEX IF NOT EXISTS idx_sessions_active ON user_sessions(is_active);

CREATE INDEX IF NOT EXISTS idx_exam_sessions_user_id ON exam_sessions(user_id);
CREATE INDEX IF NOT EXISTS idx_exam_sessions_active ON exam_sessions(is_active);

CREATE INDEX IF NOT EXISTS idx_audit_log_user_id ON audit_log(user_id);
CREATE INDEX IF NOT EXISTS idx_audit_log_timestamp ON audit_log(timestamp);
CREATE INDEX IF NOT EXISTS idx_audit_log_action ON audit_log(action);

-- Create triggers for automatic updates
CREATE TRIGGER IF NOT EXISTS update_user_timestamp 
    AFTER UPDATE ON users
    BEGIN
        UPDATE users SET updated_at = CURRENT_TIMESTAMP WHERE id = NEW.id;
    END;

CREATE TRIGGER IF NOT EXISTS update_question_timestamp 
    AFTER UPDATE ON questions
    BEGIN
        UPDATE questions SET updated_at = CURRENT_TIMESTAMP WHERE id = NEW.id;
    END;

CREATE TRIGGER IF NOT EXISTS update_question_stats
    AFTER INSERT ON exam_answers
    BEGIN
        UPDATE questions 
        SET times_asked = times_asked + 1,
            times_correct = times_correct + CASE WHEN NEW.is_correct THEN 1 ELSE 0 END
        WHERE id = NEW.question_id;
    END;

CREATE TRIGGER IF NOT EXISTS update_subject_count
    AFTER INSERT ON questions
    BEGIN
        UPDATE subjects 
        SET question_count = (
            SELECT COUNT(*) FROM questions 
            WHERE subject = NEW.subject AND is_active = 1
        )
        WHERE name = NEW.subject;
    END;

-- Insert default system settings
INSERT OR IGNORE INTO system_settings (setting_key, setting_value, description) VALUES
('system_name', 'Online Examination System', 'Name of the examination system'),
('version', '2.0.0', 'Current system version'),
('max_login_attempts', '3', 'Maximum login attempts before account lock'),
('session_timeout', '30', 'Session timeout in minutes'),
('default_exam_time', '60', 'Default exam time limit in minutes'),
('passing_percentage', '60', 'Default passing percentage'),
('negative_marking', 'false', 'Enable negative marking by default'),
('negative_mark_value', '0.25', 'Negative mark value per wrong answer'),
('auto_submit', 'true', 'Auto submit exam when time expires'),
('allow_review', 'true', 'Allow students to review answers'),
('shuffle_questions', 'true', 'Shuffle questions by default'),
('shuffle_options', 'false', 'Shuffle answer options by default'),
('maintenance_mode', 'false', 'System maintenance mode'),
('registration_enabled', 'true', 'Allow new student registrations'),
('guest_mode_enabled', 'true', 'Allow guest/practice mode');

-- Insert default subjects
INSERT OR IGNORE INTO subjects (name, description) VALUES
('Mathematics', 'Mathematical concepts and problem solving'),
('Science', 'General science topics'),
('Computer Science', 'Programming and computer concepts'),
('English', 'Language and literature'),
('History', 'Historical events and knowledge'),
('Geography', 'World geography and locations'),
('Physics', 'Physics concepts and principles'),
('Chemistry', 'Chemical reactions and properties'),
('Biology', 'Life sciences and biological processes'),
('General Knowledge', 'Mixed topics and current affairs');

-- Insert sample questions (optional - can be loaded separately)
INSERT OR IGNORE INTO questions (subject, question_text, option1, option2, option3, option4, correct_answer, difficulty, explanation) VALUES
('Mathematics', 'What is 2 + 2?', '3', '4', '5', '6', 1, 'Easy', 'Basic addition: 2 + 2 = 4'),
('Mathematics', 'What is the square root of 16?', '2', '4', '6', '8', 1, 'Easy', 'Square root of 16 is 4 because 4 × 4 = 16'),
('Computer Science', 'What does CPU stand for?', 'Central Processing Unit', 'Computer Personal Unit', 'Central Program Unit', 'Computer Processing Unit', 0, 'Easy', 'CPU stands for Central Processing Unit'),
('Science', 'What is the chemical symbol for water?', 'H2O', 'CO2', 'NaCl', 'O2', 0, 'Easy', 'Water is composed of 2 hydrogen atoms and 1 oxygen atom: H2O'),
('English', 'Which is the correct spelling?', 'Recieve', 'Receive', 'Receve', 'Receeve', 1, 'Medium', 'The correct spelling follows the rule "i before e except after c"');

-- Create views for common queries
CREATE VIEW IF NOT EXISTS user_stats AS
SELECT 
    u.id,
    u.username,
    u.full_name,
    u.role,
    COUNT(er.id) as total_exams,
    AVG(er.percentage) as average_score,
    MAX(er.percentage) as best_score,
    COUNT(CASE WHEN er.is_passed THEN 1 END) as passed_exams
FROM users u
LEFT JOIN exam_results er ON u.id = er.user_id
WHERE u.role = 2 -- Students only
GROUP BY u.id, u.username, u.full_name, u.role;

CREATE VIEW IF NOT EXISTS question_stats AS
SELECT 
    q.id,
    q.subject,
    q.difficulty,
    q.times_asked,
    q.times_correct,
    CASE 
        WHEN q.times_asked > 0 THEN ROUND((q.times_correct * 100.0 / q.times_asked), 2)
        ELSE 0 
    END as success_rate,
    q.is_active
FROM questions q;

CREATE VIEW IF NOT EXISTS recent_activity AS
SELECT 
    'exam' as activity_type,
    er.user_id,
    u.username,
    'Completed exam in ' || er.subject as description,
    er.exam_date as timestamp
FROM exam_results er
JOIN users u ON er.user_id = u.id
UNION ALL
SELECT 
    'login' as activity_type,
    u.id as user_id,
    u.username,
    'User logged in' as description,
    u.last_login as timestamp
FROM users u
WHERE u.last_login IS NOT NULL
ORDER BY timestamp DESC
LIMIT 50;