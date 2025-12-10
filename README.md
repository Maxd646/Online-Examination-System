# 🎓 Online Examination System

A comprehensive C++ examination system with advanced Data Structures & Algorithms implementation, SQLite database integration, and full-featured admin/student panels.

##  Features

###  **Authentication System**

-  User registration with validation
-  Secure login/logout functionality
-  Role-based access control (Admin/Student)
-  Session management
-  Password strength validation
-  Account security features

###  **Admin Panel**

-  **Complete Question Management**
  - Add new questions with multiple choice options
  - Edit existing questions
  - Delete questions with confirmation
  - Search by subject, difficulty, or keyword
  - View all questions with detailed information
-  **User Management**
  - View all registered users
  - Monitor user roles and status
-  **System Analytics**
  - Total users, questions, and results statistics
  - Question distribution by subject
  - Performance monitoring
-  **Results Management**
  - View all student exam results
  - Performance tracking and analysis

###  **Student Panel**

-  **Full Exam System**
  - Configure exams (subject, question count, time limits)
  - Interactive exam interface with navigation
  - Mark questions for review
  - Submit with confirmation
  - Detailed results with explanations
-  **Practice Modes**
  - Quick practice (5 questions)
  - Subject-wise practice
  - Difficulty-based practice
  - Instant feedback with explanations
-  **Performance Analytics**
  - Personal exam history
  - Subject-wise performance analysis
  - Strengths and weaknesses identification
  - Grade calculation and trends
-  **Profile Management**
  - View personal information
  - Exam statistics and metrics

###  **Database Features**

-  SQLite integration with comprehensive schema
-  **20+ Sample Questions** across multiple subjects
-  Persistent data storage for users, questions, and results
-  Advanced search and filtering capabilities
-  Data integrity and transaction support

###  **Advanced Data Structures & Algorithms**

-  **Hash Table**: Fast user/question lookups and caching
-  **Linked List**: Dynamic data management and user lists
-  **Queue**: Exam question flow and processing queues
-  **Stack**: Navigation history and undo operations
-  **Sorting Algorithms**: Result ranking and question organization

##  Project Structure

```
OnlineExaminationSystem/
├── 📁 .git/                       # Git repository data
├── 📁 .vscode/                    # VS Code configuration
│   ├── 📄 c_cpp_properties.json  # IntelliSense configuration
│   ├── 📄 tasks.json              # Build tasks
│   └──  loaunch.json             # Debug configuration
├── 📁 build/                      # Compiled object files and executable
│   ├──  authenticdation/         # Compiled auth objects
│   │   ├── 📄 simple_auth.o       # Authentication object file
│   │   └── 📄 user.o              # User management object file
│   ├──  databoase/               # Compiled database objects
│   │   └── 📄 database.o          # Database object file
│   ├──  strudcture/              # Compiled utility objects
│   │   └── 📄 utils.o             # Utilities object file
│   ├──  exatm_system.exe         # Main executable
│   └──  maina.o                  # Main object file
├──  database/ u                  # SQLite database files
│   └── 📄 exam.db                 # Main database file
├── 📁 docs/                       # Documentation
│   ├── 📄 .gitkeep                # Keep directory in git
│   └──   README.md               # Additional documentation
├── 📁 sql/                        # Database schema and data
│   ├── 📄 schema.sql              # Complete database schema
│   └── 📄 sample_data.sql         # 20+ sample questions
├── 📁 src/                        # Source code directory
│   ├──  sauthentication/         # User authentication system
│   │   ├── 📄 simple_auth.cpp     # Authentication implementation
│   │   ├── 📄 simple_auth.h       # Authentication header
│   │   ├── 📄 user.cpp            # User class implementation
│   │   └── 📄 user.h              # User class header
│   ├── 📁 components/             # Advanced DSA implementations
│   │   ├── 📄 hash_table.h        # Hash table with collision handling
│   │   ├── 📄 linked_list.h       # Dynamic linked list
│   │   ├── 📄 queue.h             # Queue with optimization
│   │   ├── 📄 sorting.h           # Multiple sorting algorithms
│   │   └── 📄 stack.h             # Stack with advanced operations
│   ├── 📁 database/               # Database layer
│   │   ├── 📄 database.cpp        # SQLite implementation
│   │   └── 📄 database.h          # Database interface
│   ├── 📁 features/               # Admin and student panels
│   │   ├── 📄 admin.h             # Advanced admin features (header)
│   │   ├── 📄 admin_stub.h        # Complete admin functionality
│   │   ├── 📄 exam.h              # Exam engine (header)
│   │   ├── 📄 exam_stub.h         # Exam engine stub
│   │   ├── 📄 student.h           # Advanced student features (header)
│   │   └── 📄 student_stub.h      # Complete student functionality
│   ├── 📁 structure/              # Utility functions
│   │   ├── 📄 utils.cpp           # Utility implementations
│   │   └── 📄 utils.h             # System utilities
│   └── 📄 main.cpp                # Main application entry point
├── 📄 .gitignore                  # Git ignore rules
├── 📄 build.ps1                   # Build script
├── 📄 clean.ps1                   # Clean script
├── 📄 LICENSE                     # MIT License
├── 📄 PROJECT_OVERVIEW.md         # Detailed project overview
├── 📄 README.md                   # This file
├── 📄 run.ps1                     # Run script
└── 📄 test.ps1                    # Test script
```

##  Quick Start

### Prerequisites

- **Compiler**: MinGW-w64 with g++ (C++17 support)
- **Database**: SQLite3 development libraries
- **OS**: Windows with PowerShell

### Installation & Setup

1. **Clone the repository**

   ```bash
   git clone <repository-url>
   cd OnlineExaminationSystem
   ```

2. **Build the project**

   ```powershell
   .\build.ps1
   ```

3. **Test the system**

   ```powershell
   .\test.ps1
   ```

4. **Run the application**

   ```powershell
   .\run.ps1
   ```

   or directly:

   ```powershell
   .\build\exam_system.exe
   ```

5. **Clean build files** (optional)
   ```powershell
   .\clean.ps1
   ```

##  Default Credentials

### Administrator Access

- **Username**: `admin`
- **Password**: `admin123`

### Student Registration

Students can register new accounts through the system interface with the following requirements:

- **Username**: 3-20 characters (letters, numbers, underscore)
- **Password**: Minimum 6 characters
- **Email**: Valid email format
- **Full Name**: Required field

##  Sample Data

The system comes pre-loaded with **20+ sample questions** across multiple subjects:

- **Mathematics**: Arithmetic, algebra, geometry
-  **Computer Science**: Programming concepts, algorithms
-  **Science**: Physics, chemistry, biology
-  **General Knowledge**: Geography, history, current affairs

Each question includes:

- Multiple choice options (4 choices)
- Correct answer indication
- Difficulty level (Easy/Medium/Hard)
- Detailed explanations for learning

##  Usage Guide

### For Administrators

1. **Login** with admin credentials
2. **Question Management**:
   - Add new questions with subjects and difficulty levels
   - Edit existing questions
   - Delete outdated questions
   - Search and filter questions
3. **User Management**: View and monitor all registered students
4. **Analytics**: Monitor system usage and performance statistics
5. **Results**: Review all student exam results and performance

### For Students

1. **Register** a new account or **login** with existing credentials
2. **Take Exams**:
   - Configure exam settings (subject, question count, time limit)
   - Navigate through questions with review options
   - Submit and view detailed results
3. **Practice Modes**:
   - Quick practice sessions
   - Subject-specific practice
   - Difficulty-based practice
4. **Performance Tracking**:
   - View exam history
   - Analyze performance by subject
   - Track improvement over time

##  Development

### Building from Source

```powershell
# Compile individual components
g++ -std=c++17 -Wall -Wextra -O2 -Isrc -c src/main.cpp -o build/main.o

# Link with dependencies
g++ build/*.o build/*/*.o -o build/exam_system.exe -lsqlite3
```

### VS Code Integration

The project includes VS Code configuration files:

- **IntelliSense**: Proper include paths and C++17 support
- **Build Tasks**: Integrated build, run, and test commands
- **Debugging**: GDB integration for debugging

### Adding New Features

1. Create source files in appropriate `src/` subdirectories
2. Update `build.ps1` to include new files in compilation
3. Follow existing code patterns and documentation standards
4. Test thoroughly before committing

## Database Schema

### Core Tables

- **`users`**: User accounts, authentication, and profiles
- **`questions`**: Exam questions with options and metadata
- **`exam_results`**: Student exam scores and performance data

### Advanced Features

- **Indexing**: Optimized queries for usernames, emails, and subjects
- **Relationships**: Foreign key constraints for data integrity
- **Triggers**: Automatic timestamp updates and statistics

##  Testing

### Automated Testing

```powershell
.\test.ps1  # Runs system validation and database checks
```

### Manual Testing Scenarios

1. **User Registration**: Test with various input combinations
2. **Admin Functions**: Add/edit/delete questions and view results
3. **Student Exams**: Take exams with different configurations
4. **Database Integrity**: Verify data persistence and relationships

##  Contributing

1. **Fork** the repository
2. **Create** a feature branch (`git checkout -b feature/amazing-feature`)
3. **Commit** your changes (`git commit -m 'Add amazing feature'`)
4. **Push** to the branch (`git push origin feature/amazing-feature`)
5. **Open** a Pull Request

### Code Standards

- Follow C++17 standards and best practices
- Use meaningful variable and function names
- Add comments for complex algorithms
- Maintain consistent indentation and formatting

##  License

This project is licensed under the **MIT License** - see the [LICENSE](LICENSE) file for details.

##  Support & Documentation

- **Documentation**: Check the `docs/` directory for detailed guides
- **Issues**: Report bugs and request features via GitHub issues
- **Wiki**: Additional documentation and tutorials (if available)

##  Acknowledgments

- Built with modern C++17 features and best practices
- SQLite for reliable and lightweight database management
- Advanced DSA implementations for educational purposes
- Comprehensive testing and validation systems

---

** Star this repository if you find it helpful!**

** Connect**: Feel free to reach out for questions, suggestions, or collaboration opportunities.
