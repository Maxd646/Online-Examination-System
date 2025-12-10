# Online Examination System

A comprehensive C++ examination system with advanced Data Structures & Algorithms implementation and SQLite database integration.

## Features

- **User Authentication**: Simple registration and login system
- **Role-based Access**: Admin and Student roles with different permissions
- **Question Management**: CRUD operations for exam questions
- **Exam System**: Randomized questions with timer and scoring
- **Database Integration**: SQLite database for persistent storage
- **Advanced DSA**: Implementation of various data structures and algorithms

## Project Structure

```
OnlineExaminationSystem/
├── src/
│   ├── authentication/
│   │   ├── user.h/cpp           # User management
│   │   └── simple_auth.h/cpp    # Authentication system
│   ├── components/
│   │   ├── hash_table.h         # Hash table implementation
│   │   ├── linked_list.h        # Linked list implementation
│   │   ├── queue.h              # Queue implementation
│   │   ├── stack.h              # Stack implementation
│   │   └── sorting.h            # Sorting algorithms
│   ├── database/
│   │   ├── database.h           # Database interface
│   │   └── database.cpp         # Database implementation
│   ├── features/
│   │   ├── admin.h              # Admin panel interface
│   │   ├── student.h            # Student panel interface
│   │   └── exam.h               # Exam interface
│   ├── structure/
│   │   ├── utils.h              # Utility functions
│   │   └── utils.cpp            # Utility implementations
│   └── main.cpp                 # Main application
├── sql/
│   ├── schema.sql               # Database schema
│   └── sample_data.sql          # Sample questions
├── database/                    # SQLite database files
├── build/                       # Compiled object files
├── docs/                        # Documentation
├── build.ps1                    # Build script
├── clean.ps1                    # Clean script
├── run.ps1                      # Run script
├── test.ps1                     # Test script
└── README.md                    # This file
```

## Requirements

- **Compiler**: MinGW-w64 with g++ (C++17 support)
- **Database**: SQLite3 development libraries
- **OS**: Windows (PowerShell scripts)

## Quick Start

### 1. Build the Project
```powershell
.\build.ps1
```

### 2. Test the System
```powershell
.\test.ps1
```

### 3. Run the Application
```powershell
.\run.ps1
```
or
```powershell
.\build\exam_system.exe
```

### 4. Clean Build Files
```powershell
.\clean.ps1
```

## Default Login

- **Username**: admin
- **Password**: admin123

## System Features

### Authentication System
- User registration and login
- Password validation
- Session management
- Role-based access control

### Question Management (Admin)
- Add new questions
- Edit existing questions
- Delete questions
- Organize by subject and difficulty

### Exam System (Student)
- Take randomized exams
- Timer functionality
- Automatic scoring
- Result tracking

### Database Features
- SQLite integration
- Persistent data storage
- Transaction support
- Data integrity

### Data Structures Used
- **Hash Table**: Fast user lookup and caching
- **Linked List**: Dynamic question storage
- **Queue**: Exam question flow management
- **Stack**: Navigation and undo operations
- **Sorting Algorithms**: Result ranking and question ordering

## Development

### Building from Source
1. Ensure g++ and SQLite3 are installed
2. Run `.\build.ps1` to compile
3. The executable will be created in `build\exam_system.exe`

### Adding New Features
1. Create new source files in appropriate directories
2. Update the build script to include new files
3. Follow the existing code structure and patterns

### Database Schema
The system uses SQLite with the following main tables:
- `users`: User accounts and authentication
- `questions`: Exam questions and answers
- `exam_results`: Exam scores and history

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

## Support

For issues and questions, please check the documentation in the `docs/` directory or create an issue in the project repository.