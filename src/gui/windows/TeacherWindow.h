#ifndef TEACHERWINDOW_H
#define TEACHERWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QSpinBox>
#include <QTextEdit>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QMessageBox>
#include <QGroupBox>
#include <QRadioButton>
#include "../../components/hash_table.h"
#include "../../components/stack.h"
#include "../../components/sorting.h"
#include <QButtonGroup>
#include <QProgressBar>
#include <QTimer>
#include <QSplitter>
#include "../../database/database.h"
#include "../../database/exam_types.h"
#include "../../authentication/user.h"

class TeacherWindow : public QMainWindow
{
    Q_OBJECT

public:
    TeacherWindow(DatabaseManager* dbManager, const User& teacher, QWidget *parent = nullptr);
    ~TeacherWindow();

private slots:
    void addQuestion();
    void editQuestion();
    void deleteQuestion();
    void searchQuestions();
    void refreshQuestionList();
    void onQuestionSelectionChanged();
    
    void createExam();
    void editExam();
    void deleteExam();
    void refreshExamList();
    void onExamSelectionChanged();
    
    void viewAllResults();
    void viewStudentResults();
    void exportResults();
    void refreshResultsList();
    
    void viewAllStudents();
    void refreshStudentsList();
    
    void logout();

private:
    void setupUI();
    void setupQuestionManagement();
    void setupExamManagement();
    void setupResultsManagement();
    void setupStudentManagement();
    void setupDashboard();
    void applyStyles();
    void updateDashboardStats();
    void populateQuestionTable();
    void populateExamTable();
    void populateResultsTable();
    void populateStudentsTable();
    void clearQuestionForm();
    void clearExamForm();
    
    // Database and user
    DatabaseManager* dbManager;
    User currentTeacher;
    
    // Main UI
    QTabWidget* tabWidget;
    
    // Dashboard Tab
    QWidget* dashboardTab;
    QLabel* totalQuestionsLabel;
    QLabel* totalExamsLabel;
    QLabel* totalStudentsLabel;
    QLabel* totalResultsLabel;
    
    // Question Management Tab
    QWidget* questionTab;
    QSplitter* questionSplitter;
    
    // Question Form
    QGroupBox* questionFormGroup;
    QLineEdit* questionSubjectEdit;
    QTextEdit* questionTextEdit;
    QLineEdit* option1Edit;
    QLineEdit* option2Edit;
    QLineEdit* option3Edit;
    QLineEdit* option4Edit;
    QComboBox* correctAnswerCombo;
    QComboBox* difficultyCombo;
    QTextEdit* explanationEdit;
    QPushButton* addQuestionBtn;
    QPushButton* updateQuestionBtn;
    QPushButton* clearQuestionBtn;
    
    // Question List
    QGroupBox* questionListGroup;
    QLineEdit* questionSearchEdit;
    QPushButton* searchQuestionBtn;
    QPushButton* refreshQuestionBtn;
    QTableWidget* questionTable;
    QPushButton* editQuestionBtn;
    QPushButton* deleteQuestionBtn;
    
    // Exam Management Tab
    QWidget* examTab;
    QSplitter* examSplitter;
    
    // Exam Form
    QGroupBox* examFormGroup;
    QLineEdit* examTitleEdit;
    QComboBox* examTypeCombo;
    QLineEdit* examSubjectEdit;
    QSpinBox* timeLimitSpin;
    QSpinBox* questionCountSpin;
    QComboBox* examDifficultyCombo;
    QPushButton* createExamBtn;
    QPushButton* updateExamBtn;
    QPushButton* clearExamBtn;
    
    // Exam List
    QGroupBox* examListGroup;
    QTableWidget* examTable;
    QPushButton* editExamBtn;
    QPushButton* deleteExamBtn;
    QPushButton* refreshExamBtn;
    
    // Results Management Tab
    QWidget* resultsTab;
    QTableWidget* resultsTable;
    QPushButton* viewAllResultsBtn;
    QPushButton* exportResultsBtn;
    QPushButton* refreshResultsBtn;
    
    // Student Management Tab
    QWidget* studentsTab;
    QTableWidget* studentsTable;
    QPushButton* refreshStudentsBtn;
    
    // Current selections
    int selectedQuestionId;
    int selectedExamId;
    
    // DSA Components for enhanced functionality
    HashTable<string, vector<int>> subjectQuestionCache;  // Cache questions by subject
    Stack<string> operationHistory;                       // Undo operations
    HashTable<int, string> questionSearchCache;          // Fast question lookup
};

#endif // TEACHERWINDOW_H