#  Online Examination System (C++ | Data Structures & Algorithms)

##  Project Overview
The **Online Examination System** is a C++-based console application designed to manage **exams, questions, students, and results** using efficient **data structures and algorithms**.

This project demonstrates:
- Use of **OOP (classes & objects)**
- **DSA concepts** such as linked lists, stacks, queues, arrays, sorting, and searching
- File handling for data storage
- A clean and interactive console interface

---

##  Features
###  Admin Panel
- Add new questions  
- Update existing questions  
- Delete questions  
- View all questions  
- Create new exams  
- View student results  

###  Student Panel
- Register or Login  
- Take exam  
- Timer-based questions (optional)  
- See results instantly  
- Review correct answers  

---

##  Data Structures Used
| Feature | Data Structure | Reason |
|--------|----------------|--------|
| Storing Questions | Linked List | Fast insertion/deletion |
| Exam Queue | Queue | Sequential exam flow |
| Result History | File Handling + Struct Array | Easy retrieval |
| User Login | Map / Array Search | Quick lookup |
| Answer Evaluation | Stack (optional) | Compare answers |

---

##  Project Structure
```
Online-Examination-System/
│
├── src/
│   ├── main.cpp
│   ├── admin.cpp
│   ├── student.cpp
│   ├── question.cpp
│   ├── exam.cpp
│   ├── utils.cpp
│
├── include/
│   ├── admin.h
│   ├── student.h
│   ├── question.h
│   ├── exam.h
│   ├── utils.h
│
├── data/
│   ├── questions.txt
│   ├── students.txt
│   ├── results.txt
│
├── build/
│
├── README.md
└── Makefile
```

---

## 🔧 How to Compile & Run

###  **Using G++**
```bash
g++ src/main.cpp src/admin.cpp src/student.cpp src/question.cpp src/exam.cpp src/utils.cpp -o exam
./exam
```

###  **Using Makefile**
```bash
make
./exam
```

---

##  Sample Output
```
===============================
     ONLINE EXAMINATION
===============================

1. Admin Login
2. Student Login
3. Exit
Enter your choice:
```

---

## 📘 Learning Outcomes
- Mastery of **DSA + OOP in real applications**
- Experience with **file handling**
- Clean **modular programming**
- Real-world simulation of an exam portal

---

## Contributors


---

## 📄 License
This project is created for **educational purposes (DSA course)**.  
You may modify or improve it freely.
