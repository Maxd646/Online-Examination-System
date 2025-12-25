# Online Examination System

## 🚀 Advanced C++ Implementation with Data Structures & Algorithms

A comprehensive digital examination platform built with modern C++ featuring advanced data structures, algorithms, and SQLite database integration.

## 📋 Table of Contents

- [Features](#features)
- [Architecture](#architecture)
- [Installation](#installation)
- [Usage](#usage)
- [Project Structure](#project-structure)
- [DSA Implementation](#dsa-implementation)
- [Database Schema](#database-schema)
- [API Documentation](#api-documentation)
- [Contributing](#contributing)
- [License](#license)

## ✨ Features

### 🔐 Authentication & Authorization
- **Multi-role System**: Admin, Student, Instructor roles
- **Secure Login**: Password hashing and session management
- **Account Security**: Login attempt limits, account locking
- **Session Management**: Timeout handling and security tokens

### 📚 Question Management (CRUD Operations)
- **Create**: Add new questions with multiple choice options
- **Read**: View and search questions by various criteria
- **Update**: Modify existing questions and metadata
- **Delete**: Remove questions with proper validation
- **Advanced Features**: 
  - Subject categorization
  - Difficulty levels (Easy, Medium, Hard)
  - Question statistics and analytics
  - Bulk import/export (CSV, JSON, XML)

### 📝 Exam Engine
- **Multiple Exam Types**: Practice, Mock, Official, Timed, Adaptive
- **Flexible Navigation**: Sequential or free navigation modes
- **Advanced Features**:
  - Random question selection
  - Question and option shuffling
  - Timer management with auto-submit
  - Review and bookmark functionality
  - Negative marking support
  - Real-time progress tracking

### 📊 Result Management & Analytics
- **Comprehensive Results**: Detailed score analysis
- **Performance Tracking**: Historical performance trends
- **Analytics Dashboard**: Subject-wise performance insights
- **Reporting**: Exportable reports in multiple formats
- **Achievements**: Badge and achievement system

### 🗄️ Database Integration
- **SQLite Database**: Reliable and efficient data storage
- **ACID Compliance**: Transaction safety and data integrity
- **Optimized Queries**: Indexed searches and performance optimization
- **Backup & Recovery**: Automated backup and restore functionality

## 🏗️ Architecture

### Modular Design
```
OnlineExaminationSystem/
├── src/
│   ├── authentication/     # User management and security
│   ├── components/         # Reusable DSA components
│   ├── database/          # Database operations and models
│   ├── features/          # Core application features
│   ├── structure/         # Utility functions and helpers
│   └── main.cpp          # Application entry point
├── sql/                   # Database schema and migrations
├── database/             # SQLite database files
├── docs/                 # Documentation and reports
└── build/               # Compiled binaries and objects
```

### Design Patterns Used
- **Factory Pattern**: Exam creation and configuration
- **Observer Pattern**: Real-time updates and notifications
- **Strategy Pattern**: Different exam modes and scoring algorithms
- **Singleton Pattern**: Database connection management
- **Command Pattern**: Undo/redo functionality for admin operations

## 🛠️ Installation

### Prerequisites
- **C++ Compiler**: g++ with C++17 support
- **SQLite3**: Database engine and development libraries
- **Make**: Build automation tool

### Platform-Specific Installation

#### Windows
```bash
# Install MinGW-w64 or Visual Studio
# Download SQLite3 from https://www.sqlite.org/download.html
# Add SQLite3 to system PATH

# Clone and build
git clone <repository-url>
cd OnlineExaminationSystem
make install-deps  # May require manual installation on Windows
make all
```

#### Linux (Ubuntu/Debian)
```bash
# Install dependencies
sudo apt-get update
sudo apt-get install build-essential sqlite3 libsqlite3-dev

# Clone and build
git clone <repository-url>
cd OnlineExaminationSystem
make install-deps
make all
```

#### Linux (CentOS/RHEL)
```bash
# Install dependencies
sudo yum groupinstall "Development Tools"
sudo yum install sqlite-devel

# Clone and build
git clone <repository-url>
cd OnlineExaminationSystem
make all
```

#### macOS
```bash
# Install Xcode command line tools
xcode-select --install

# Install SQLite3 (if not already installed)
brew install sqlite3

# Clone and build
git clone <repository-url>
cd OnlineExaminationSystem
make all
```

### Quick Start
```bash
# Build the project
make all

# Run the application
make run

# Or run directly
./build/exam_system
```

## 🎯 Usage

### Default Credentials
- **Admin**: username: `admin`, password: `admin123`
- **Student**: username: `student`, password: `student123`

### Admin Functions
1. **Question Management**
   - Add new questions with detailed options
   - Edit existing questions and metadata
   - Delete questions with confirmation
   - Import questions from CSV/JSON files
   - Export question banks

2. **User Management**
   - Create and manage user accounts
   - Set user roles and permissions
   - Monitor user activity and performance
   - Generate user reports

3. **System Analytics**
   - View comprehensive system statistics
   - Generate performance reports
   - Monitor exam trends and patterns
   - Export analytics data

### Student Functions
1. **Take Exams**
   - Choose from available subjects
   - Configure exam settings (time, difficulty)
   - Navigate through questions flexibly
   - Review and modify answers
   - Submit exams with confirmation

2. **View Results**
   - Access detailed exam results
   - View performance history
   - Analyze subject-wise performance
   - Track improvement over time

3. **Practice Mode**
   - Take unlimited practice exams
   - No time restrictions
   - Immediate feedback on answers
   - Subject-specific practice sessions

## 📁 Project Structure

### Source Code Organization
```
src/
├── authentication/
│   ├── user.h              # User class and authentication
│   └── user.cpp            # User implementation
├── components/
│   ├── linked_list.h       # Custom linked list implementation
│   ├── queue.h             # Queue and priority queue
│   ├── stack.h             # Stack with undo/redo support
│   ├── hash_table.h        # Hash table for fast lookups
│   └── sorting.h           # Advanced sorting algorithms
├── database/
│   ├── database.h          # Database manager and models
│   └── database.cpp        # Database implementation
├── features/
│   ├── admin.h             # Admin panel functionality
│   ├── student.h           # Student interface
│   └── exam.h              # Exam engine and management
├── structure/
│   ├── utils.h             # Utility functions and helpers
│   └── utils.cpp           # Utility implementations
└── main.cpp                # Application entry point
```

### Database Structure
```
database/
├── exam.db                 # Main SQLite database
└── backups/               # Automatic database backups
```

### Documentation
```
docs/
├── README.md              # This file
├── API_Documentation.md   # API reference
├── Database_Schema.md     # Database design
└── User_Manual.md         # End-user documentation
```

## 🧮 DSA Implementation

### Data Structures Used

#### 1. **Linked List** (`components/linked_list.h`)
- **Usage**: Recent user activity tracking, navigation history
- **Features**: Template-based, iterator support, custom operations
- **Time Complexity**: O(1) insertion/deletion, O(n) search

#### 2. **Stack** (`components/stack.h`)
- **Usage**: Undo/redo operations, navigation history
- **Features**: Template-based, specialized undo/redo stack
- **Time Complexity**: O(1) push/pop operations

#### 3. **Queue** (`components/queue.h`)
- **Usage**: Exam question flow, user request processing
- **Features**: Template-based, priority queue variant
- **Time Complexity**: O(1) enqueue/dequeue operations

#### 4. **Hash Table** (`components/hash_table.h`)
- **Usage**: Fast user lookups, session management, caching
- **Features**: Open addressing, dynamic resizing, collision handling
- **Time Complexity**: O(1) average case for all operations

### Algorithms Implemented

#### 1. **Sorting Algorithms** (`components/sorting.h`)
- **Quick Sort**: Question randomization, result ranking
- **Merge Sort**: Stable sorting for user lists
- **Heap Sort**: Priority-based question selection
- **Radix Sort**: Numerical data sorting
- **Custom Comparators**: Multi-criteria sorting

#### 2. **Search Algorithms**
- **Binary Search**: Sorted data lookups
- **Linear Search**: General purpose searching
- **Hash-based Search**: O(1) user and session lookups

#### 3. **Graph Algorithms** (Future Enhancement)
- **Dependency Tracking**: Question prerequisite management
- **Recommendation System**: Personalized question suggestions

### Performance Optimizations
- **Memory Pool**: Efficient memory management for frequent allocations
- **Caching**: LRU cache for frequently accessed data
- **Indexing**: Database indexes for fast query execution
- **Lazy Loading**: On-demand data loading for large datasets

## 🗃️ Database Schema

### Core Tables

#### Users Table
```sql
CREATE TABLE users (
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
    is_locked BOOLEAN DEFAULT 0
);
```

#### Questions Table
```sql
CREATE TABLE questions (
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
    times_asked INTEGER DEFAULT 0,
    times_correct INTEGER DEFAULT 0
);
```

#### Exam Results Table
```sql
CREATE TABLE exam_results (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id INTEGER NOT NULL,
    username TEXT NOT NULL,
    score INTEGER NOT NULL,
    total_questions INTEGER NOT NULL,
    percentage REAL NOT NULL,
    exam_date TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
    start_time TEXT,
    end_time TEXT,
    duration INTEGER,
    subject TEXT,
    difficulty TEXT,
    exam_type TEXT DEFAULT 'Practice',
    is_passed BOOLEAN DEFAULT 0
);
```

### Advanced Features
- **Foreign Key Constraints**: Data integrity enforcement
- **Triggers**: Automatic timestamp updates and statistics
- **Views**: Optimized queries for common operations
- **Indexes**: Performance optimization for frequent queries

## 📖 API Documentation

### Core Classes

#### DatabaseManager
```cpp
class DatabaseManager {
public:
    bool connect();
    bool initializeDatabase();
    
    // User operations
    bool insertUser(const User& user);
    User getUserByUsername(const std::string& username);
    bool authenticateUser(const std::string& username, const std::string& password, User& user);
    
    // Question operations
    bool insertQuestion(const Question& question);
    std::vector<Question> getAllQuestions();
    std::vector<Question> getRandomQuestions(int count, const std::string& subject = "");
    
    // Result operations
    bool insertExamResult(const ExamResult& result);
    std::vector<ExamResult> getExamResultsByUser(int userId);
};
```

#### ExamEngine
```cpp
class ExamEngine {
public:
    ExamEngine(const User& student, const ExamSettings& settings);
    
    bool initializeExam(const std::vector<Question>& questionPool);
    bool startExam();
    bool nextQuestion();
    bool answerQuestion(int answer);
    bool submitExam();
    
    ExamResult generateResult();
    int getRemainingTime() const;
    double getProgressPercentage() const;
};
```

### Usage Examples

#### Creating and Taking an Exam
```cpp
// Create exam settings
ExamSettings settings;
settings.questionCount = 10;
settings.timeLimit = 30;
settings.subject = "Mathematics";

// Create exam engine
ExamEngine exam(student, settings);

// Initialize with questions
auto questions = dbManager->getRandomQuestions(10, "Mathematics");
exam.initializeExam(questions);

// Start exam
exam.startExam();

// Answer questions
while (exam.isActive() && !exam.isCompleted()) {
    auto currentQ = exam.getCurrentQuestion();
    // Display question and get user input
    int answer = getUserInput();
    exam.answerQuestion(answer);
    exam.nextQuestion();
}

// Submit and get results
exam.submitExam();
ExamResult result = exam.generateResult();
```

## 🧪 Testing

### Unit Tests
```bash
# Build with test configuration
make test

# Run specific test suites
./build/test_runner --suite=database
./build/test_runner --suite=algorithms
./build/test_runner --suite=exam_engine
```

### Performance Testing
```bash
# Memory leak detection
make memcheck

# Performance profiling
make profile

# Load testing
./build/load_test --users=100 --duration=300
```

## 🔧 Configuration

### System Settings
The system can be configured through the database `system_settings` table or configuration files:

```cpp
// Example configuration
{
    "max_login_attempts": 3,
    "session_timeout": 30,
    "default_exam_time": 60,
    "passing_percentage": 60,
    "negative_marking": false,
    "auto_submit": true
}
```

### Environment Variables
```bash
export EXAM_DB_PATH="/path/to/database/exam.db"
export EXAM_LOG_LEVEL="INFO"
export EXAM_MAX_CONNECTIONS=10
```

## 🚀 Advanced Features

### Adaptive Examination
- **Dynamic Difficulty**: Questions adapt based on user performance
- **Personalized Learning**: Customized question recommendations
- **Performance Prediction**: ML-based score prediction

### Security Features
- **Session Security**: Token-based authentication
- **Data Encryption**: Sensitive data protection
- **Audit Logging**: Complete activity tracking
- **Anti-Cheating**: Suspicious activity detection

### Analytics & Reporting
- **Real-time Analytics**: Live performance dashboards
- **Predictive Analytics**: Performance trend analysis
- **Custom Reports**: Flexible report generation
- **Data Export**: Multiple format support (PDF, CSV, JSON)

## 🤝 Contributing

### Development Setup
```bash
# Fork the repository
git clone <your-fork-url>
cd OnlineExaminationSystem

# Create development branch
git checkout -b feature/your-feature-name

# Install development dependencies
make install-deps

# Build and test
make debug
make test
```

### Code Style Guidelines
- **C++ Standard**: C++17 or later
- **Naming Convention**: camelCase for variables, PascalCase for classes
- **Documentation**: Doxygen-style comments for all public APIs
- **Testing**: Unit tests for all new features

### Submission Process
1. Create feature branch from `main`
2. Implement feature with tests
3. Update documentation
4. Submit pull request with detailed description
5. Address review feedback

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](../LICENSE) file for details.

## 🙏 Acknowledgments

- **SQLite Team**: For the excellent embedded database
- **C++ Community**: For continuous language improvements
- **Contributors**: All developers who contributed to this project

## 📞 Support

### Getting Help
- **Documentation**: Check the `docs/` directory
- **Issues**: Report bugs on GitHub Issues
- **Discussions**: Join community discussions
- **Email**: Contact maintainers directly

### Troubleshooting

#### Common Issues
1. **Database Connection Failed**
   - Check SQLite3 installation
   - Verify database file permissions
   - Ensure database directory exists

2. **Compilation Errors**
   - Verify C++17 compiler support
   - Check SQLite3 development headers
   - Update system dependencies

3. **Runtime Errors**
   - Check log files in `logs/` directory
   - Verify database schema version
   - Ensure proper file permissions

---

**Built with ❤️ using Modern C++, Advanced DSA, and SQLite**