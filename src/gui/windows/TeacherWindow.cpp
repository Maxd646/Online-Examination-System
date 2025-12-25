#include "TeacherWindow.h"
#include "LoginWindow.h"
#include <QApplication>
#include <QScreen>
#include <QMenuBar>
#include <QStatusBar>

TeacherWindow::TeacherWindow(DatabaseManager *dbManager, const User &teacher, QWidget *parent)
    : QMainWindow(parent), dbManager(dbManager), currentTeacher(teacher),
      selectedQuestionId(-1), selectedExamId(-1),
      subjectQuestionCache(32), questionSearchCache(64)
{
    setupUI();
    applyStyles();
    updateDashboardStats();

    // Center window on screen
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);
}

TeacherWindow::~TeacherWindow()
{
}

void TeacherWindow::setupUI()
{
    setWindowTitle("Teacher Panel - " + QString::fromStdString(currentTeacher.getFullName()));
    setMinimumSize(1200, 800);
    resize(1400, 900);

    // Menu bar
    QMenuBar *menuBar = this->menuBar();
    QMenu *fileMenu = menuBar->addMenu("&File");
    QAction *logoutAction = fileMenu->addAction("&Logout");
    connect(logoutAction, &QAction::triggered, this, &TeacherWindow::logout);

    // Status bar
    statusBar()->showMessage("Welcome, " + QString::fromStdString(currentTeacher.getFullName()));

    // Central widget with tabs
    tabWidget = new QTabWidget(this);
    setCentralWidget(tabWidget);

    setupDashboard();
    setupQuestionManagement();
    setupExamManagement();
    setupResultsManagement();
    setupStudentManagement();

    // Add tabs
    tabWidget->addTab(dashboardTab, " Dashboard");
    tabWidget->addTab(questionTab, " Questions");
    tabWidget->addTab(examTab, " Exams");
    tabWidget->addTab(resultsTab, " Results");
    tabWidget->addTab(studentsTab, " Students");
}

void TeacherWindow::setupDashboard()
{
    dashboardTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(dashboardTab);

    // Welcome section
    QLabel *welcomeLabel = new QLabel("Welcome, " + QString::fromStdString(currentTeacher.getFullName()));
    welcomeLabel->setObjectName("welcomeLabel");
    layout->addWidget(welcomeLabel);

    // Statistics grid
    QGridLayout *statsLayout = new QGridLayout();

    // Stats cards
    QGroupBox *questionsCard = new QGroupBox("📚 Total Questions");
    questionsCard->setObjectName("statsCard");
    QVBoxLayout *questionsLayout = new QVBoxLayout(questionsCard);
    totalQuestionsLabel = new QLabel("0");
    totalQuestionsLabel->setObjectName("statsNumber");
    questionsLayout->addWidget(totalQuestionsLabel);

    QGroupBox *examsCard = new QGroupBox("📝 Active Exams");
    examsCard->setObjectName("statsCard");
    QVBoxLayout *examsLayout = new QVBoxLayout(examsCard);
    totalExamsLabel = new QLabel("0");
    totalExamsLabel->setObjectName("statsNumber");
    examsLayout->addWidget(totalExamsLabel);

    QGroupBox *studentsCard = new QGroupBox("👥 Total Students");
    studentsCard->setObjectName("statsCard");
    QVBoxLayout *studentsLayout = new QVBoxLayout(studentsCard);
    totalStudentsLabel = new QLabel("0");
    totalStudentsLabel->setObjectName("statsNumber");
    studentsLayout->addWidget(totalStudentsLabel);

    QGroupBox *resultsCard = new QGroupBox("📊 Exam Results");
    resultsCard->setObjectName("statsCard");
    QVBoxLayout *resultsLayout = new QVBoxLayout(resultsCard);
    totalResultsLabel = new QLabel("0");
    totalResultsLabel->setObjectName("statsNumber");
    resultsLayout->addWidget(totalResultsLabel);

    statsLayout->addWidget(questionsCard, 0, 0);
    statsLayout->addWidget(examsCard, 0, 1);
    statsLayout->addWidget(studentsCard, 1, 0);
    statsLayout->addWidget(resultsCard, 1, 1);

    layout->addLayout(statsLayout);
    layout->addStretch();
}

void TeacherWindow::setupQuestionManagement()
{
    questionTab = new QWidget();
    QHBoxLayout *mainLayout = new QHBoxLayout(questionTab);

    questionSplitter = new QSplitter(Qt::Horizontal);

    // Question Form (Left side)
    questionFormGroup = new QGroupBox("Add/Edit Question");
    QVBoxLayout *formLayout = new QVBoxLayout(questionFormGroup);

    QGridLayout *formGrid = new QGridLayout();

    // Subject
    formGrid->addWidget(new QLabel("Subject:"), 0, 0);
    questionSubjectEdit = new QLineEdit();
    questionSubjectEdit->setPlaceholderText("e.g., Mathematics, Computer Science");
    formGrid->addWidget(questionSubjectEdit, 0, 1);

    // Question Text
    formGrid->addWidget(new QLabel("Question:"), 1, 0);
    questionTextEdit = new QTextEdit();
    questionTextEdit->setMaximumHeight(100);
    questionTextEdit->setPlaceholderText("Enter the question text...");
    formGrid->addWidget(questionTextEdit, 1, 1);

    // Options
    formGrid->addWidget(new QLabel("Option 1:"), 2, 0);
    option1Edit = new QLineEdit();
    formGrid->addWidget(option1Edit, 2, 1);

    formGrid->addWidget(new QLabel("Option 2:"), 3, 0);
    option2Edit = new QLineEdit();
    formGrid->addWidget(option2Edit, 3, 1);

    formGrid->addWidget(new QLabel("Option 3:"), 4, 0);
    option3Edit = new QLineEdit();
    formGrid->addWidget(option3Edit, 4, 1);

    formGrid->addWidget(new QLabel("Option 4:"), 5, 0);
    option4Edit = new QLineEdit();
    formGrid->addWidget(option4Edit, 5, 1);

    // Correct Answer
    formGrid->addWidget(new QLabel("Correct Answer:"), 6, 0);
    correctAnswerCombo = new QComboBox();
    correctAnswerCombo->addItems({"Option 1", "Option 2", "Option 3", "Option 4"});
    formGrid->addWidget(correctAnswerCombo, 6, 1);

    // Difficulty
    formGrid->addWidget(new QLabel("Difficulty:"), 7, 0);
    difficultyCombo = new QComboBox();
    difficultyCombo->addItems({"Easy", "Medium", "Hard"});
    difficultyCombo->setCurrentText("Medium");
    formGrid->addWidget(difficultyCombo, 7, 1);

    // Explanation
    formGrid->addWidget(new QLabel("Explanation:"), 8, 0);
    explanationEdit = new QTextEdit();
    explanationEdit->setMaximumHeight(80);
    explanationEdit->setPlaceholderText("Optional explanation for the answer...");
    formGrid->addWidget(explanationEdit, 8, 1);

    formLayout->addLayout(formGrid);

    // Form buttons
    QHBoxLayout *formButtonsLayout = new QHBoxLayout();
    addQuestionBtn = new QPushButton("Add Question");
    addQuestionBtn->setObjectName("primaryButton");
    updateQuestionBtn = new QPushButton(" Update Question");
    updateQuestionBtn->setObjectName("primaryButton");
    updateQuestionBtn->setVisible(false);
    clearQuestionBtn = new QPushButton(" Clear Form");
    clearQuestionBtn->setObjectName("secondaryButton");

    formButtonsLayout->addWidget(addQuestionBtn);
    formButtonsLayout->addWidget(updateQuestionBtn);
    formButtonsLayout->addWidget(clearQuestionBtn);
    formLayout->addLayout(formButtonsLayout);

    // Question List (Right side)
    questionListGroup = new QGroupBox("Questions List");
    QVBoxLayout *listLayout = new QVBoxLayout(questionListGroup);

    // Search
    QHBoxLayout *searchLayout = new QHBoxLayout();
    questionSearchEdit = new QLineEdit();
    questionSearchEdit->setPlaceholderText("Search questions...");
    searchQuestionBtn = new QPushButton("🔍 Search");
    refreshQuestionBtn = new QPushButton("🔄 Refresh");

    searchLayout->addWidget(questionSearchEdit);
    searchLayout->addWidget(searchQuestionBtn);
    searchLayout->addWidget(refreshQuestionBtn);
    listLayout->addLayout(searchLayout);

    // Questions table
    questionTable = new QTableWidget();
    questionTable->setColumnCount(5);
    QStringList headers = {"ID", "Subject", "Question", "Difficulty", "Created"};
    questionTable->setHorizontalHeaderLabels(headers);
    questionTable->horizontalHeader()->setStretchLastSection(true);
    questionTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    listLayout->addWidget(questionTable);

    // List buttons
    QHBoxLayout *listButtonsLayout = new QHBoxLayout();
    editQuestionBtn = new QPushButton("✏️ Edit");
    deleteQuestionBtn = new QPushButton("🗑️ Delete");
    editQuestionBtn->setObjectName("primaryButton");
    deleteQuestionBtn->setObjectName("dangerButton");

    listButtonsLayout->addWidget(editQuestionBtn);
    listButtonsLayout->addWidget(deleteQuestionBtn);
    listButtonsLayout->addStretch();
    listLayout->addLayout(listButtonsLayout);

    // Add to splitter
    questionSplitter->addWidget(questionFormGroup);
    questionSplitter->addWidget(questionListGroup);
    questionSplitter->setSizes({400, 800});

    mainLayout->addWidget(questionSplitter);

    // Connect signals
    connect(addQuestionBtn, &QPushButton::clicked, this, &TeacherWindow::addQuestion);
    connect(updateQuestionBtn, &QPushButton::clicked, this, &TeacherWindow::editQuestion);
    connect(clearQuestionBtn, &QPushButton::clicked, this, &TeacherWindow::clearQuestionForm);
    connect(searchQuestionBtn, &QPushButton::clicked, this, &TeacherWindow::searchQuestions);
    connect(refreshQuestionBtn, &QPushButton::clicked, this, &TeacherWindow::refreshQuestionList);
    connect(editQuestionBtn, &QPushButton::clicked, this, &TeacherWindow::editQuestion);
    connect(deleteQuestionBtn, &QPushButton::clicked, this, &TeacherWindow::deleteQuestion);
    connect(questionTable, &QTableWidget::itemSelectionChanged, this, &TeacherWindow::onQuestionSelectionChanged);

    // Load questions
    populateQuestionTable();
}

void TeacherWindow::setupExamManagement()
{
    examTab = new QWidget();
    QHBoxLayout *mainLayout = new QHBoxLayout(examTab);

    examSplitter = new QSplitter(Qt::Horizontal);

    // Exam Form (Left side)
    examFormGroup = new QGroupBox("Create/Edit Exam");
    QVBoxLayout *formLayout = new QVBoxLayout(examFormGroup);

    QGridLayout *formGrid = new QGridLayout();

    // Exam Title
    formGrid->addWidget(new QLabel("Exam Title:"), 0, 0);
    examTitleEdit = new QLineEdit();
    examTitleEdit->setPlaceholderText("e.g., Mathematics Mid-term Exam");
    formGrid->addWidget(examTitleEdit, 0, 1);

    // Exam Type
    formGrid->addWidget(new QLabel("Exam Type:"), 1, 0);
    examTypeCombo = new QComboBox();
    examTypeCombo->addItem(" Quiz", static_cast<int>(ExamType::QUIZ));
    examTypeCombo->addItem(" Mid Exam", static_cast<int>(ExamType::MID));
    examTypeCombo->addItem(" Final Exam", static_cast<int>(ExamType::FINAL));
    examTypeCombo->addItem(" Worksheet", static_cast<int>(ExamType::WORKSHEET));
    formGrid->addWidget(examTypeCombo, 1, 1);

    // Subject
    formGrid->addWidget(new QLabel("Subject:"), 2, 0);
    examSubjectEdit = new QLineEdit();
    examSubjectEdit->setPlaceholderText("e.g., Mathematics, Computer Science");
    formGrid->addWidget(examSubjectEdit, 2, 1);

    // Time Limit
    formGrid->addWidget(new QLabel("Time Limit (minutes):"), 3, 0);
    timeLimitSpin = new QSpinBox();
    timeLimitSpin->setRange(1, 300);
    timeLimitSpin->setValue(30);
    timeLimitSpin->setSuffix(" minutes");
    formGrid->addWidget(timeLimitSpin, 3, 1);

    // Question Count
    formGrid->addWidget(new QLabel("Number of Questions:"), 4, 0);
    questionCountSpin = new QSpinBox();
    questionCountSpin->setRange(1, 100);
    questionCountSpin->setValue(10);
    questionCountSpin->setSuffix(" questions");
    formGrid->addWidget(questionCountSpin, 4, 1);

    // Difficulty
    formGrid->addWidget(new QLabel("Difficulty:"), 5, 0);
    examDifficultyCombo = new QComboBox();
    examDifficultyCombo->addItems({"Mixed", "Easy", "Medium", "Hard"});
    formGrid->addWidget(examDifficultyCombo, 5, 1);

    formLayout->addLayout(formGrid);

    // Form buttons
    QHBoxLayout *formButtonsLayout = new QHBoxLayout();
    createExamBtn = new QPushButton(" Create Exam");
    createExamBtn->setObjectName("primaryButton");
    updateExamBtn = new QPushButton(" Update Exam");
    updateExamBtn->setObjectName("primaryButton");
    updateExamBtn->setVisible(false);
    clearExamBtn = new QPushButton(" Clear Form");
    clearExamBtn->setObjectName("secondaryButton");

    formButtonsLayout->addWidget(createExamBtn);
    formButtonsLayout->addWidget(updateExamBtn);
    formButtonsLayout->addWidget(clearExamBtn);
    formLayout->addLayout(formButtonsLayout);

    // Exam List (Right side)
    examListGroup = new QGroupBox("Exams List");
    QVBoxLayout *listLayout = new QVBoxLayout(examListGroup);

    // Exams table
    examTable = new QTableWidget();
    examTable->setColumnCount(6);
    QStringList headers = {"ID", "Title", "Type", "Subject", "Time Limit", "Questions"};
    examTable->setHorizontalHeaderLabels(headers);
    examTable->horizontalHeader()->setStretchLastSection(true);
    examTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    listLayout->addWidget(examTable);

    // List buttons
    QHBoxLayout *listButtonsLayout = new QHBoxLayout();
    editExamBtn = new QPushButton(" Edit");
    deleteExamBtn = new QPushButton(" Delete");
    refreshExamBtn = new QPushButton(" Refresh");
    editExamBtn->setObjectName("primaryButton");
    deleteExamBtn->setObjectName("dangerButton");

    listButtonsLayout->addWidget(editExamBtn);
    listButtonsLayout->addWidget(deleteExamBtn);
    listButtonsLayout->addWidget(refreshExamBtn);
    listButtonsLayout->addStretch();
    listLayout->addLayout(listButtonsLayout);

    // Add to splitter
    examSplitter->addWidget(examFormGroup);
    examSplitter->addWidget(examListGroup);
    examSplitter->setSizes({400, 800});

    mainLayout->addWidget(examSplitter);

    // Connect signals
    connect(createExamBtn, &QPushButton::clicked, this, &TeacherWindow::createExam);
    connect(updateExamBtn, &QPushButton::clicked, this, &TeacherWindow::editExam);
    connect(clearExamBtn, &QPushButton::clicked, this, &TeacherWindow::clearExamForm);
    connect(refreshExamBtn, &QPushButton::clicked, this, &TeacherWindow::refreshExamList);
    connect(editExamBtn, &QPushButton::clicked, this, &TeacherWindow::editExam);
    connect(deleteExamBtn, &QPushButton::clicked, this, &TeacherWindow::deleteExam);
    connect(examTable, &QTableWidget::itemSelectionChanged, this, &TeacherWindow::onExamSelectionChanged);

    // Auto-update time limit based on exam type
    connect(examTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int index)
            {
        ExamType type = static_cast<ExamType>(examTypeCombo->itemData(index).toInt());
        switch (type) {
            case ExamType::QUIZ: timeLimitSpin->setValue(30); break;
            case ExamType::MID: timeLimitSpin->setValue(90); break;
            case ExamType::FINAL: timeLimitSpin->setValue(120); break;
            case ExamType::WORKSHEET: timeLimitSpin->setValue(0); break;
        } });

    // Load exams
    populateExamTable();
}

void TeacherWindow::setupResultsManagement()
{
    resultsTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(resultsTab);

    // Buttons
    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    viewAllResultsBtn = new QPushButton(" View All Results");
    exportResultsBtn = new QPushButton(" Export Results");
    refreshResultsBtn = new QPushButton(" Refresh");

    viewAllResultsBtn->setObjectName("primaryButton");
    exportResultsBtn->setObjectName("secondaryButton");

    buttonsLayout->addWidget(viewAllResultsBtn);
    buttonsLayout->addWidget(exportResultsBtn);
    buttonsLayout->addWidget(refreshResultsBtn);
    buttonsLayout->addStretch();

    layout->addLayout(buttonsLayout);

    // Results table
    resultsTable = new QTableWidget();
    resultsTable->setColumnCount(7);
    QStringList headers = {"ID", "Student", "Exam", "Score", "Total", "Percentage", "Date"};
    resultsTable->setHorizontalHeaderLabels(headers);
    resultsTable->horizontalHeader()->setStretchLastSection(true);
    resultsTable->setAlternatingRowColors(true);
    layout->addWidget(resultsTable);

    // Connect signals
    connect(viewAllResultsBtn, &QPushButton::clicked, this, &TeacherWindow::viewAllResults);
    connect(exportResultsBtn, &QPushButton::clicked, this, &TeacherWindow::exportResults);
    connect(refreshResultsBtn, &QPushButton::clicked, this, &TeacherWindow::refreshResultsList);

    // Load results
    populateResultsTable();
}

void TeacherWindow::setupStudentManagement()
{
    studentsTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(studentsTab);

    // Buttons
    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    refreshStudentsBtn = new QPushButton(" Refresh Students");
    refreshStudentsBtn->setObjectName("primaryButton");

    buttonsLayout->addWidget(refreshStudentsBtn);
    buttonsLayout->addStretch();

    layout->addLayout(buttonsLayout);

    // Students table
    studentsTable = new QTableWidget();
    studentsTable->setColumnCount(5);
    QStringList headers = {"ID", "Username", "Full Name", "Email", "Last Login"};
    studentsTable->setHorizontalHeaderLabels(headers);
    studentsTable->horizontalHeader()->setStretchLastSection(true);
    studentsTable->setAlternatingRowColors(true);
    layout->addWidget(studentsTable);

    // Connect signals
    connect(refreshStudentsBtn, &QPushButton::clicked, this, &TeacherWindow::refreshStudentsList);

    // Load students
    populateStudentsTable();
}

// Implementation of slot methods will continue in next part...

void TeacherWindow::addQuestion()
{
    // Validate input
    if (questionSubjectEdit->text().trimmed().isEmpty() ||
        questionTextEdit->toPlainText().trimmed().isEmpty() ||
        option1Edit->text().trimmed().isEmpty() ||
        option2Edit->text().trimmed().isEmpty() ||
        option3Edit->text().trimmed().isEmpty() ||
        option4Edit->text().trimmed().isEmpty())
    {

        QMessageBox::warning(this, "Validation Error", "Please fill in all required fields.");
        return;
    }

    // Create question
    vector<string> options = {
        option1Edit->text().toStdString(),
        option2Edit->text().toStdString(),
        option3Edit->text().toStdString(),
        option4Edit->text().toStdString()};

    Question question(0,
                      questionSubjectEdit->text().toStdString(),
                      questionTextEdit->toPlainText().toStdString(),
                      options,
                      correctAnswerCombo->currentIndex(),
                      difficultyCombo->currentText().toStdString(),
                      explanationEdit->toPlainText().toStdString());

    if (dbManager->insertQuestion(question))
    {
        // DSA: Add to operation history for undo functionality
        operationHistory.push("ADD_QUESTION:" + questionSubjectEdit->text().toStdString() + ":" + questionTextEdit->toPlainText().left(50).toStdString());

        // DSA: Update subject cache for faster filtering
        string subject = questionSubjectEdit->text().toStdString();
        vector<int> *subjectQuestions = subjectQuestionCache.find(subject);
        if (!subjectQuestions)
        {
            vector<int> newList;
            subjectQuestionCache.insert(subject, newList);
        }

        QMessageBox::information(this, "Success", "Question added successfully!");
        clearQuestionForm();
        populateQuestionTable();
        updateDashboardStats();
    }
    else
    {
        QMessageBox::critical(this, "Error", "Failed to add question.");
    }
}

void TeacherWindow::clearQuestionForm()
{
    questionSubjectEdit->clear();
    questionTextEdit->clear();
    option1Edit->clear();
    option2Edit->clear();
    option3Edit->clear();
    option4Edit->clear();
    correctAnswerCombo->setCurrentIndex(0);
    difficultyCombo->setCurrentIndex(1); // Medium
    explanationEdit->clear();

    addQuestionBtn->setVisible(true);
    updateQuestionBtn->setVisible(false);
    selectedQuestionId = -1;
}

void TeacherWindow::populateQuestionTable()
{
    auto questions = dbManager->getAllQuestions();
    questionTable->setRowCount(questions.size());

    for (size_t i = 0; i < questions.size(); ++i)
    {
        questionTable->setItem(i, 0, new QTableWidgetItem(QString::number(questions[i].getId())));
        questionTable->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(questions[i].getSubject())));

        QString questionText = QString::fromStdString(questions[i].getQuestionText());
        if (questionText.length() > 50)
        {
            questionText = questionText.left(47) + "...";
        }
        questionTable->setItem(i, 2, new QTableWidgetItem(questionText));
        questionTable->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(questions[i].getDifficulty())));
        questionTable->setItem(i, 4, new QTableWidgetItem(QString::fromStdString(questions[i].getCreatedAt())));
    }
}

void TeacherWindow::populateExamTable()
{
    // This would populate exam configurations when implemented
    examTable->setRowCount(0);
}

void TeacherWindow::populateResultsTable()
{
    auto results = dbManager->getAllExamResults();
    resultsTable->setRowCount(results.size());

    for (size_t i = 0; i < results.size(); ++i)
    {
        resultsTable->setItem(i, 0, new QTableWidgetItem(QString::number(results[i].getId())));
        resultsTable->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(results[i].getUsername())));
        resultsTable->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(results[i].getSubject())));
        resultsTable->setItem(i, 3, new QTableWidgetItem(QString::number(results[i].getScore())));
        resultsTable->setItem(i, 4, new QTableWidgetItem(QString::number(results[i].getTotalQuestions())));
        resultsTable->setItem(i, 5, new QTableWidgetItem(QString::number(results[i].getPercentage(), 'f', 1) + "%"));
        resultsTable->setItem(i, 6, new QTableWidgetItem(QString::fromStdString(results[i].getExamDate())));
    }
}

void TeacherWindow::populateStudentsTable()
{
    auto students = dbManager->getUsersByRole(UserRole::STUDENT);
    studentsTable->setRowCount(students.size());

    for (size_t i = 0; i < students.size(); ++i)
    {
        studentsTable->setItem(i, 0, new QTableWidgetItem(QString::number(students[i].getId())));
        studentsTable->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(students[i].getUsername())));
        studentsTable->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(students[i].getFullName())));
        studentsTable->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(students[i].getEmail())));
        studentsTable->setItem(i, 4, new QTableWidgetItem(QString::fromStdString(students[i].getLastLogin())));
    }
}

void TeacherWindow::updateDashboardStats()
{
    totalQuestionsLabel->setText(QString::number(dbManager->getTotalQuestions()));
    totalStudentsLabel->setText(QString::number(dbManager->getUserCountByRole(UserRole::STUDENT)));
    totalResultsLabel->setText(QString::number(dbManager->getTotalExamResults()));
    totalExamsLabel->setText("0"); // Will be updated when exam configs are implemented
}

// Placeholder implementations for remaining slots
void TeacherWindow::editQuestion() { /* TODO */ }
void TeacherWindow::deleteQuestion() { /* TODO */ }
void TeacherWindow::searchQuestions() { populateQuestionTable(); }
void TeacherWindow::refreshQuestionList() { populateQuestionTable(); }
void TeacherWindow::onQuestionSelectionChanged() { /* TODO */ }
void TeacherWindow::createExam() { /* TODO */ }
void TeacherWindow::editExam() { /* TODO */ }
void TeacherWindow::deleteExam() { /* TODO */ }
void TeacherWindow::refreshExamList() { populateExamTable(); }
void TeacherWindow::onExamSelectionChanged() { /* TODO */ }
void TeacherWindow::viewAllResults() { populateResultsTable(); }
void TeacherWindow::viewStudentResults() { /* TODO */ }
void TeacherWindow::exportResults() { /* TODO */ }
void TeacherWindow::refreshResultsList() { populateResultsTable(); }
void TeacherWindow::viewAllStudents() { populateStudentsTable(); }
void TeacherWindow::refreshStudentsList() { populateStudentsTable(); }
void TeacherWindow::clearExamForm() { /* TODO */ }

void TeacherWindow::logout()
{
    QMessageBox::StandardButton reply = QMessageBox::question(this,
                                                              "Logout Confirmation",
                                                              "Are you sure you want to logout?",
                                                              QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes)
    {
        // Create new login window
        LoginWindow *loginWindow = new LoginWindow(dbManager);
        loginWindow->show();

        this->close();
    }
}

void TeacherWindow::applyStyles()
{
    setStyleSheet(R"(
        QMainWindow {
            background: #2c3e50;
        }
        
        QTabWidget::pane {
            border: 1px solid #34495e;
            background: #34495e;
        }
        
        QTabBar::tab {
            background: #7f8c8d;
            color: white;
            padding: 10px 20px;
            margin-right: 2px;
            border-top-left-radius: 5px;
            border-top-right-radius: 5px;
        }
        
        QTabBar::tab:selected {
            background: #3498db;
        }
        
        QGroupBox {
            font-weight: bold;
            border: 2px solid #7f8c8d;
            border-radius: 5px;
            margin: 10px;
            padding-top: 10px;
            color: white;
        }
        
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 5px 0 5px;
        }
        
        #statsCard {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #3498db, stop:1 #2980b9);
            border: none;
            border-radius: 10px;
            min-height: 100px;
        }
        
        #statsNumber {
            font-size: 36px;
            font-weight: bold;
            color: white;
            text-align: center;
        }
        
        #welcomeLabel {
            font-size: 24px;
            font-weight: bold;
            color: #3498db;
            padding: 20px;
        }
        
        QLabel {
            color: #ecf0f1;
        }
        
        QLineEdit, QTextEdit, QSpinBox, QComboBox {
            padding: 8px;
            border: 2px solid #7f8c8d;
            border-radius: 4px;
            background: #2c3e50;
            color: white;
        }
        
        QLineEdit:focus, QTextEdit:focus, QSpinBox:focus, QComboBox:focus {
            border-color: #3498db;
        }
        
        #primaryButton {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #27ae60, stop:1 #229954);
            color: white;
            border: none;
            padding: 10px 20px;
            border-radius: 5px;
            font-weight: bold;
        }
        
        #primaryButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #229954, stop:1 #1e8449);
        }
        
        #secondaryButton {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #95a5a6, stop:1 #7f8c8d);
            color: white;
            border: none;
            padding: 10px 20px;
            border-radius: 5px;
        }
        
        #dangerButton {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #e74c3c, stop:1 #c0392b);
            color: white;
            border: none;
            padding: 10px 20px;
            border-radius: 5px;
        }
        
        QTableWidget {
            background: #2c3e50;
            color: white;
            gridline-color: #7f8c8d;
            selection-background-color: #3498db;
        }
        
        QHeaderView::section {
            background: #34495e;
            color: white;
            padding: 8px;
            border: 1px solid #7f8c8d;
            font-weight: bold;
        }
    )");
}