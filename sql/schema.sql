-- Online Examination System Database Schema (Simplified)
-- SQLite Database Schema for direct exam creation with embedded questions

-- Enable foreign key constraints
PRAGMA foreign_keys = ON;

-- Users table for authentication and user management
CREATE TABLE IF NOT EXISTS users (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    username TEXT UNIQUE NOT NULL,
    password TEXT NOT NULL,
    email TEXT UNIQUE NOT NULL,
    full_name TEXT NOT NULL,
    role INTEGER NOT NULL DEFAULT 2, -- 1=Admin, 2=Student
    status INTEGER NOT NULL DEFAULT 1, -- 1=Active, 2=Inactive
    created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    last_login TEXT,
    login_attempts INTEGER DEFAULT 0,
    is_locked BOOLEAN DEFAULT 0,
    updated_at TEXT DEFAULT CURRENT_TIMESTAMP
);

-- Exam templates table - teachers create exams directly
CREATE TABLE IF NOT EXISTS exam_templates (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    template_name TEXT UNIQUE NOT NULL,
    exam_type TEXT NOT NULL CHECK(exam_type IN ('QUIZ', 'WORKSHEET', 'FINAL')),
    subject TEXT NOT NULL CHECK(subject IN ('DSA', 'OOP', 'COA', 'SAM', 'Mathematics')),
    question_count INTEGER NOT NULL DEFAULT 10 CHECK(question_count > 0),
    time_limit INTEGER NOT NULL CHECK(time_limit > 0), -- in minutes
    difficulty TEXT DEFAULT 'Medium' CHECK(difficulty IN ('Easy', 'Medium', 'Hard', 'Mixed')),
    passing_percentage REAL DEFAULT 60.0,
    negative_marking BOOLEAN DEFAULT 0,
    negative_mark_value REAL DEFAULT 0.25,
    shuffle_questions BOOLEAN DEFAULT 1,
    shuffle_options BOOLEAN DEFAULT 0,
    allow_review BOOLEAN DEFAULT 1,
    auto_submit BOOLEAN DEFAULT 1,
    instructions TEXT,
    created_by INTEGER NOT NULL,
    created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    is_active BOOLEAN DEFAULT 1,
    FOREIGN KEY(created_by) REFERENCES users(id) ON DELETE CASCADE
);

-- Exam questions table - questions belong directly to specific exams
CREATE TABLE IF NOT EXISTS exam_questions (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    exam_template_id INTEGER NOT NULL,
    question_number INTEGER NOT NULL,
    question_text TEXT NOT NULL,
    option1 TEXT NOT NULL,
    option2 TEXT NOT NULL,
    option3 TEXT NOT NULL,
    option4 TEXT NOT NULL,
    correct_answer INTEGER NOT NULL CHECK(correct_answer >= 0 AND correct_answer <= 3),
    explanation TEXT,
    created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY(exam_template_id) REFERENCES exam_templates(id) ON DELETE CASCADE,
    UNIQUE(exam_template_id, question_number)
);

-- General questions table - question bank for reuse across exams
CREATE TABLE IF NOT EXISTS questions (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    subject TEXT NOT NULL CHECK(subject IN ('DSA', 'OOP', 'COA', 'SAM', 'Mathematics')),
    question_text TEXT NOT NULL,
    option1 TEXT NOT NULL,
    option2 TEXT NOT NULL,
    option3 TEXT NOT NULL,
    option4 TEXT NOT NULL,
    correct_answer INTEGER NOT NULL CHECK(correct_answer >= 0 AND correct_answer <= 3),
    difficulty TEXT NOT NULL CHECK(difficulty IN ('Easy', 'Medium', 'Hard')),
    explanation TEXT,
    created_by INTEGER NOT NULL,
    created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    is_active BOOLEAN DEFAULT 1,
    FOREIGN KEY(created_by) REFERENCES users(id) ON DELETE CASCADE
);

-- Exam results table
CREATE TABLE IF NOT EXISTS exam_results (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id INTEGER NOT NULL,
    username TEXT NOT NULL,
    exam_template_id INTEGER NOT NULL,
    score INTEGER NOT NULL,
    total_questions INTEGER NOT NULL,
    percentage REAL NOT NULL,
    exam_date TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    start_time TEXT,
    end_time TEXT,
    duration INTEGER, -- in minutes
    time_limit INTEGER, -- original time limit
    subject TEXT,
    exam_type TEXT, -- QUIZ, WORKSHEET, FINAL
    exam_name TEXT, -- Name of the exam taken
    is_passed BOOLEAN DEFAULT 0,
    grade TEXT,
    negative_marking BOOLEAN DEFAULT 0,
    negative_marks REAL DEFAULT 0,
    FOREIGN KEY(user_id) REFERENCES users(id) ON DELETE CASCADE,
    FOREIGN KEY(exam_template_id) REFERENCES exam_templates(id) ON DELETE CASCADE
);

-- Detailed exam answers for analysis
CREATE TABLE IF NOT EXISTS exam_answers (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    result_id INTEGER NOT NULL,
    question_id INTEGER NOT NULL,
    user_answer INTEGER, -- -1 for unanswered
    is_correct BOOLEAN NOT NULL,
    time_taken INTEGER, -- in seconds
    FOREIGN KEY(result_id) REFERENCES exam_results(id) ON DELETE CASCADE,
    FOREIGN KEY(question_id) REFERENCES exam_questions(id) ON DELETE CASCADE
);

-- User sessions for security
CREATE TABLE IF NOT EXISTS user_sessions (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id INTEGER NOT NULL,
    session_id TEXT UNIQUE NOT NULL,
    start_time TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    last_activity TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    is_active BOOLEAN DEFAULT 1,
    FOREIGN KEY(user_id) REFERENCES users(id) ON DELETE CASCADE
);

-- System settings table
CREATE TABLE IF NOT EXISTS system_settings (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    setting_key TEXT UNIQUE NOT NULL,
    setting_value TEXT NOT NULL,
    description TEXT,
    created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
);

-- Subjects table (simplified)
CREATE TABLE IF NOT EXISTS subjects (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT UNIQUE NOT NULL,
    description TEXT,
    is_active BOOLEAN DEFAULT 1,
    created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
);

-- Create indexes for better performance
CREATE INDEX IF NOT EXISTS idx_users_username ON users(username);
CREATE INDEX IF NOT EXISTS idx_users_email ON users(email);
CREATE INDEX IF NOT EXISTS idx_users_role ON users(role);

CREATE INDEX IF NOT EXISTS idx_exam_templates_type ON exam_templates(exam_type);
CREATE INDEX IF NOT EXISTS idx_exam_templates_subject ON exam_templates(subject);
CREATE INDEX IF NOT EXISTS idx_exam_templates_created_by ON exam_templates(created_by);
CREATE INDEX IF NOT EXISTS idx_exam_templates_active ON exam_templates(is_active);

CREATE INDEX IF NOT EXISTS idx_exam_questions_template_id ON exam_questions(exam_template_id);
CREATE INDEX IF NOT EXISTS idx_exam_questions_number ON exam_questions(question_number);

CREATE INDEX IF NOT EXISTS idx_exam_results_user_id ON exam_results(user_id);
CREATE INDEX IF NOT EXISTS idx_exam_results_date ON exam_results(exam_date);
CREATE INDEX IF NOT EXISTS idx_exam_results_template_id ON exam_results(exam_template_id);

CREATE INDEX IF NOT EXISTS idx_exam_answers_result_id ON exam_answers(result_id);
CREATE INDEX IF NOT EXISTS idx_exam_answers_question_id ON exam_answers(question_id);

CREATE INDEX IF NOT EXISTS idx_sessions_user_id ON user_sessions(user_id);

-- Create triggers for automatic updates
CREATE TRIGGER IF NOT EXISTS update_exam_template_timestamp 
    AFTER UPDATE ON exam_templates
    BEGIN
        UPDATE exam_templates SET updated_at = CURRENT_TIMESTAMP WHERE id = NEW.id;
    END;

CREATE TRIGGER IF NOT EXISTS update_user_timestamp 
    AFTER UPDATE ON users
    BEGIN
        UPDATE users SET updated_at = CURRENT_TIMESTAMP WHERE id = NEW.id;
    END;

-- Insert default system settings
INSERT OR IGNORE INTO system_settings (setting_key, setting_value, description) VALUES
('system_name', 'Online Examination System', 'Name of the examination system'),
('version', '3.0.0', 'Current system version'),
('max_login_attempts', '3', 'Maximum login attempts before account lock'),
('session_timeout', '30', 'Session timeout in minutes'),
('passing_percentage', '60', 'Default passing percentage'),
('negative_marking', 'false', 'Enable negative marking by default'),
('auto_submit', 'true', 'Auto submit exam when time expires'),
('allow_review', 'true', 'Allow students to review answers');

-- Insert core subjects
INSERT OR IGNORE INTO subjects (name, description) VALUES
('DSA', 'Data Structures and Algorithms'),
('OOP', 'Object-Oriented Programming'),
('COA', 'Computer Organization and Architecture'),
('SAM', 'Software Architecture and Modeling'),
('Mathematics', 'Mathematical concepts and problem solving');

-- Insert default admin user
INSERT OR IGNORE INTO users (username, password, email, full_name, role, status) VALUES
('admin', 'admin123', 'admin@exam.system', 'System Administrator', 1, 1);