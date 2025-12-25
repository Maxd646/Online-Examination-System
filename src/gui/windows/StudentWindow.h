#ifndef STUDENTWINDOW_H
#define STUDENTWINDOW_H

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
#include <QListWidget>
#include <QRadioButton>
#include <QButtonGroup>
#include <QProgressBar>
#include <QTimer>
#include <QGroupBox>
#include <QScrollArea>
#include <QMessageBox>
#include <QFrame>
#include "../../database/database.h"
#include "../../components/queue.h"
#include "../../components/stack.h"
#include "../../components/hash_table.h"
#include "../../database/exam_types.h"
#include "../../authentication/user.h"

class StudentWindow : public QMainWindow
{
    Q_OBJECT

public:
    StudentWindow(DatabaseManager* dbManager, const User& student, QWidget *parent = nullptr);
    ~StudentWindow();

private slots:
    void startExam();
    void startPractice();
    void viewResults();
    void viewProfile();
    void logout();
    
    // Exam interface
    void nextQuestion();
    void previousQuestion();
    void submitExam();
    void updateTimer();
    void selectAnswer(int optionIndex);
    void markForReview();
    
    // Practice mode
    void startQuickPractice();
    void startSubjectPractice();
    void refreshAvailableExams();

private:
    void setupUI();
    void setupDashboard();
    void setupExamInterface();
    void setupPracticeMode();
    void setupResultsView();
    void setupProfileView();
    void applyStyles();
    
    void loadAvailableExams();
    void loadExamQuestions();
    void displayCurrentQuestion();
    void calculateAndSaveResults();
    void showExamResults();
    void updateDashboardStats();
    void populateResultsTable();
    
    // Database and user
    DatabaseManager* dbManager;
    User currentStudent;
    
    // Main UI
    QTabWidget* tabWidget;
    
    // Dashboard Tab
    QWidget* dashboardTab;
    QLabel* welcomeLabel;
    QLabel* totalExamsLabel;
    QLabel* averageScoreLabel;
    QLabel* lastExamLabel;
    QGroupBox* quickActionsGroup;
    QPushButton* quickExamBtn;
    QPushButton* quickPracticeBtn;
    QPushButton* viewResultsBtn;
    
    // Available Exams
    QGroupBox* availableExamsGroup;
    QListWidget* examsList;
    QPushButton* startSelectedExamBtn;
    QPushButton* refreshExamsBtn;
    
    // Exam Interface Tab
    QWidget* examTab;
    QLabel* examTitleLabel;
    QLabel* timerLabel;
    QProgressBar* progressBar;
    QLabel* questionNumberLabel;
    QLabel* questionTextLabel;
    QButtonGroup* answerGroup;
    QRadioButton* option1Radio;
    QRadioButton* option2Radio;
    QRadioButton* option3Radio;
    QRadioButton* option4Radio;
    QPushButton* previousBtn;
    QPushButton* nextBtn;
    QPushButton* markReviewBtn;
    QPushButton* submitExamBtn;
    QLabel* reviewStatusLabel;
    
    // Practice Mode Tab
    QWidget* practiceTab;
    QGroupBox* practiceOptionsGroup;
    QPushButton* quickPracticeBtn2;
    QPushButton* subjectPracticeBtn;
    QComboBox* practiceSubjectCombo;
    QSpinBox* practiceQuestionsSpin;
    
    // Results Tab
    QWidget* resultsTab;
    QTableWidget* resultsTable;
    QPushButton* refreshResultsBtn;
    QLabel* statsLabel;
    
    // Profile Tab
    QWidget* profileTab;
    QLabel* profileInfoLabel;
    QPushButton* changePasswordBtn;
    
    // Exam state
    bool isExamActive;
    vector<Question> currentExamQuestions;
    vector<int> userAnswers;
    vector<bool> markedForReview;
    int currentQuestionIndex;
    int examTimeLimit; // in seconds
    int remainingTime;
    QTimer* examTimer;
    string currentExamTitle;
    ExamType currentExamType;
    
    // Practice state
    bool isPracticeMode;
    vector<Question> practiceQuestions;
    int practiceScore;
    
    // DSA Components for enhanced functionality
    Queue<int> questionQueue;                    // Queue for question flow
    Stack<int> navigationHistory;                // Navigation back/forward
    HashTable<int, int> answerCache;            // Cache user answers for quick access
};

#endif // STUDENTWINDOW_H