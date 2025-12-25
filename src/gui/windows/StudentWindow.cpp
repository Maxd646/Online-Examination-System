#include "StudentWindow.h"
#include "LoginWindow.h"
#include <QApplication>
#include <QScreen>

StudentWindow::StudentWindow(DatabaseManager* dbManager, const User& student, QWidget *parent)
    : QMainWindow(parent), dbManager(dbManager), currentStudent(student),
      currentQuestionIndex(0), examTimeLimit(0), remainingTime(0),
      isPracticeMode(false), practiceScore(0),
      answerCache(32),  // DSA: Initialize hash table for answer caching
      isExamActive(false), examTimer(nullptr),
      tabWidget(nullptr), dashboardTab(nullptr), examTab(nullptr),
      practiceTab(nullptr), resultsTab(nullptr), profileTab(nullptr),
      welcomeLabel(nullptr), totalExamsLabel(nullptr), averageScoreLabel(nullptr),
      lastExamLabel(nullptr), quickActionsGroup(nullptr), quickExamBtn(nullptr),
      quickPracticeBtn(nullptr), viewResultsBtn(nullptr), availableExamsGroup(nullptr),
      examsList(nullptr), startSelectedExamBtn(nullptr), refreshExamsBtn(nullptr),
      examTitleLabel(nullptr), timerLabel(nullptr), progressBar(nullptr),
      questionNumberLabel(nullptr), questionTextLabel(nullptr), answerGroup(nullptr),
      option1Radio(nullptr), option2Radio(nullptr), option3Radio(nullptr),
      option4Radio(nullptr), previousBtn(nullptr), nextBtn(nullptr),
      markReviewBtn(nullptr), submitExamBtn(nullptr), reviewStatusLabel(nullptr),
      practiceOptionsGroup(nullptr), quickPracticeBtn2(nullptr), subjectPracticeBtn(nullptr),
      practiceSubjectCombo(nullptr), practiceQuestionsSpin(nullptr),
      resultsTable(nullptr), refreshResultsBtn(nullptr), statsLabel(nullptr),
      profileInfoLabel(nullptr), changePasswordBtn(nullptr)
{
    setupUI();
    applyStyles();
    updateDashboardStats();
    
    // DSA: Initialize question queue for exam flow
    // This will be populated when exam starts
    
    // Center window on screen
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);
}

void StudentWindow::showExamResults()
{
    // Calculate results
    int score = 0;
    for (size_t i = 0; i < currentExamQuestions.size(); ++i) {
        if (i < userAnswers.size() && userAnswers[i] == currentExamQuestions[i].getCorrectAnswer()) {
            score++;
        }
    }
    
    double percentage = (static_cast<double>(score) / currentExamQuestions.size()) * 100.0;
    
    QString resultMsg = QString(
        "Exam Results:\n\n"
        "Score: %1/%2\n"
        "Percentage: %3%\n"
        "Grade: %4\n\n"
        "%5"
    ).arg(score)
     .arg(currentExamQuestions.size())
     .arg(percentage, 0, 'f', 1)
     .arg(percentage >= 60 ? "PASS" : "FAIL")
     .arg(percentage >= 60 ? "Congratulations! You passed the exam." : "Keep studying and try again!");
    
    QMessageBox::information(this, "Exam Results", resultMsg);
}

// Practice mode implementations
void StudentWindow::startQuickPractice()
{
    practiceQuestions = dbManager->getRandomQuestions(5);
    
    if (practiceQuestions.empty()) {
        QMessageBox::warning(this, "No Questions", "No questions available for practice.");
        return;
    }
    
    isPracticeMode = true;
    practiceScore = 0;
    
    // Start practice with first question
    currentQuestionIndex = 0;
    userAnswers.clear();
    userAnswers.resize(practiceQuestions.size(), -1);
    
    QMessageBox::information(this, "Practice Started", 
        "Quick practice mode started!\n5 random questions\nNo time limit - take your time to learn!");
    
    // Switch to exam tab for practice
    tabWidget->setTabEnabled(1, true);
    tabWidget->setCurrentIndex(1);
    
    // Update UI for practice mode
    examTitleLabel->setText(" Practice Mode - Quick Practice");
    timerLabel->setText("No Limit");
    timerLabel->setStyleSheet("color: green; font-weight: bold;");
    progressBar->setMaximum(practiceQuestions.size());
    
    // Use practice questions instead of exam questions
    currentExamQuestions = practiceQuestions;
    displayCurrentQuestion();
}

void StudentWindow::startSubjectPractice()
{
    QString subject = practiceSubjectCombo->currentText();
    int questionCount = practiceQuestionsSpin->value();
    
    if (subject == "All Subjects") {
        practiceQuestions = dbManager->getRandomQuestions(questionCount);
    } else {
        practiceQuestions = dbManager->getQuestionsBySubject(subject.toStdString());
        if (practiceQuestions.size() > static_cast<size_t>(questionCount)) {
            practiceQuestions.resize(questionCount);
        }
    }
    
    if (practiceQuestions.empty()) {
        QMessageBox::warning(this, "No Questions", 
            QString("No questions available for subject: %1").arg(subject));
        return;
    }
    
    isPracticeMode = true;
    practiceScore = 0;
    
    currentQuestionIndex = 0;
    userAnswers.clear();
    userAnswers.resize(practiceQuestions.size(), -1);
    
    QMessageBox::information(this, "Practice Started", 
        QString("Subject practice started!\nSubject: %1\nQuestions: %2\nNo time limit")
        .arg(subject).arg(practiceQuestions.size()));
    
    tabWidget->setTabEnabled(1, true);
    tabWidget->setCurrentIndex(1);
    
    examTitleLabel->setText(QString("🎯 Practice Mode - %1").arg(subject));
    timerLabel->setText("No Limit");
    timerLabel->setStyleSheet("color: green; font-weight: bold;");
    progressBar->setMaximum(practiceQuestions.size());
    
    currentExamQuestions = practiceQuestions;
    displayCurrentQuestion();
}

void StudentWindow::startPractice()
{
    startQuickPractice();
}

void StudentWindow::refreshAvailableExams()
{
    loadAvailableExams();
    QMessageBox::information(this, "Refreshed", "Available exams list has been refreshed.");
}

void StudentWindow::viewResults()
{
    populateResultsTable();
    updateDashboardStats();
    tabWidget->setCurrentIndex(3); // Switch to results tab
}

void StudentWindow::viewProfile()
{
    // updateProfileInfo();
    tabWidget->setCurrentIndex(4); // Switch to profile tab
}

void StudentWindow::updateDashboardStats()
{
    // Update dashboard statistics
    // This method should populate the dashboard with user statistics
    // For now, we'll add basic placeholder functionality
    
    if (totalExamsLabel) {
        totalExamsLabel->setText("Total Exams: 0");
    }
    if (averageScoreLabel) {
        averageScoreLabel->setText("Average Score: N/A");
    }
    if (lastExamLabel) {
        lastExamLabel->setText("Last Exam: None");
    }
}

void StudentWindow::logout()
{
    // Stop any active timers
    if (examTimer && examTimer->isActive()) {
        examTimer->stop();
    }
    
    // Confirm logout if exam is active
    if (isExamActive) {
        int reply = QMessageBox::question(this, "Logout Confirmation",
            "You have an active exam. Are you sure you want to logout?\n"
            "Your progress will be lost!",
            QMessageBox::Yes | QMessageBox::No);
        
        if (reply == QMessageBox::No) {
            return;
        }
    } else {
        int reply = QMessageBox::question(this, "Logout Confirmation",
            "Are you sure you want to logout?",
            QMessageBox::Yes | QMessageBox::No);
        
        if (reply == QMessageBox::No) {
            return;
        }
    }
    
    // Create new login window
    LoginWindow* loginWindow = new LoginWindow(dbManager);
    loginWindow->show();
    
    this->close();
}

// Missing method implementations
void StudentWindow::setupUI()
{
    // Basic UI setup - placeholder implementation
    tabWidget = new QTabWidget(this);
    setCentralWidget(tabWidget);
    
    // Initialize tabs
    dashboardTab = new QWidget();
    examTab = new QWidget();
    practiceTab = new QWidget();
    resultsTab = new QWidget();
    profileTab = new QWidget();
    
    tabWidget->addTab(dashboardTab, "Dashboard");
    tabWidget->addTab(examTab, "Exam");
    tabWidget->addTab(practiceTab, "Practice");
    tabWidget->addTab(resultsTab, "Results");
    tabWidget->addTab(profileTab, "Profile");
    
    setupDashboard();
    setupExamInterface();
    setupPracticeMode();
    setupResultsView();
    setupProfileView();
}

void StudentWindow::setupDashboard()
{
    // Dashboard setup - placeholder
    QVBoxLayout* layout = new QVBoxLayout(dashboardTab);
    
    welcomeLabel = new QLabel(QString("Welcome, %1!").arg(QString::fromStdString(currentStudent.getUsername())));
    welcomeLabel->setObjectName("welcomeLabel");
    layout->addWidget(welcomeLabel);
    
    // Stats labels
    totalExamsLabel = new QLabel("Total Exams: 0");
    averageScoreLabel = new QLabel("Average Score: N/A");
    lastExamLabel = new QLabel("Last Exam: None");
    
    layout->addWidget(totalExamsLabel);
    layout->addWidget(averageScoreLabel);
    layout->addWidget(lastExamLabel);
    
    // Quick actions
    quickActionsGroup = new QGroupBox("Quick Actions");
    QVBoxLayout* actionsLayout = new QVBoxLayout(quickActionsGroup);
    
    quickPracticeBtn = new QPushButton("Quick Practice");
    quickPracticeBtn->setObjectName("primaryButton");
    connect(quickPracticeBtn, &QPushButton::clicked, this, &StudentWindow::startQuickPractice);
    
    viewResultsBtn = new QPushButton("View Results");
    viewResultsBtn->setObjectName("secondaryButton");
    connect(viewResultsBtn, &QPushButton::clicked, this, &StudentWindow::viewResults);
    
    actionsLayout->addWidget(quickPracticeBtn);
    actionsLayout->addWidget(viewResultsBtn);
    
    layout->addWidget(quickActionsGroup);
}

void StudentWindow::setupExamInterface()
{
    // Exam interface setup - placeholder
    QVBoxLayout* layout = new QVBoxLayout(examTab);
    
    examTitleLabel = new QLabel("No Exam Selected");
    examTitleLabel->setObjectName("examTitle");
    layout->addWidget(examTitleLabel);
    
    timerLabel = new QLabel("00:00");
    timerLabel->setObjectName("timerLabel");
    layout->addWidget(timerLabel);
    
    progressBar = new QProgressBar();
    layout->addWidget(progressBar);
    
    // Question display
    questionNumberLabel = new QLabel("Question 1 of 10");
    questionTextLabel = new QLabel("Question text will appear here");
    
    layout->addWidget(questionNumberLabel);
    layout->addWidget(questionTextLabel);
    
    // Answer options
    answerGroup = new QButtonGroup(this);
    option1Radio = new QRadioButton("Option 1");
    option2Radio = new QRadioButton("Option 2");
    option3Radio = new QRadioButton("Option 3");
    option4Radio = new QRadioButton("Option 4");
    
    answerGroup->addButton(option1Radio, 0);
    answerGroup->addButton(option2Radio, 1);
    answerGroup->addButton(option3Radio, 2);
    answerGroup->addButton(option4Radio, 3);
    
    layout->addWidget(option1Radio);
    layout->addWidget(option2Radio);
    layout->addWidget(option3Radio);
    layout->addWidget(option4Radio);
    
    // Navigation buttons
    QHBoxLayout* navLayout = new QHBoxLayout();
    previousBtn = new QPushButton("Previous");
    nextBtn = new QPushButton("Next");
    submitExamBtn = new QPushButton("Submit Exam");
    
    previousBtn->setObjectName("navButton");
    nextBtn->setObjectName("navButton");
    submitExamBtn->setObjectName("submitButton");
    
    navLayout->addWidget(previousBtn);
    navLayout->addWidget(nextBtn);
    navLayout->addWidget(submitExamBtn);
    
    layout->addLayout(navLayout);
    
    // Initialize timer
    examTimer = new QTimer(this);
    connect(examTimer, &QTimer::timeout, this, &StudentWindow::updateTimer);
}

void StudentWindow::setupPracticeMode()
{
    // Practice mode setup - placeholder
    QVBoxLayout* layout = new QVBoxLayout(practiceTab);
    
    practiceOptionsGroup = new QGroupBox("Practice Options");
    QVBoxLayout* optionsLayout = new QVBoxLayout(practiceOptionsGroup);
    
    quickPracticeBtn2 = new QPushButton("Quick Practice (5 Questions)");
    quickPracticeBtn2->setObjectName("primaryButton");
    connect(quickPracticeBtn2, &QPushButton::clicked, this, &StudentWindow::startQuickPractice);
    
    subjectPracticeBtn = new QPushButton("Subject Practice");
    subjectPracticeBtn->setObjectName("secondaryButton");
    connect(subjectPracticeBtn, &QPushButton::clicked, this, &StudentWindow::startSubjectPractice);
    
    practiceSubjectCombo = new QComboBox();
    practiceSubjectCombo->addItem("All Subjects");
    practiceSubjectCombo->addItem("Mathematics");
    practiceSubjectCombo->addItem("Science");
    practiceSubjectCombo->addItem("History");
    
    practiceQuestionsSpin = new QSpinBox();
    practiceQuestionsSpin->setRange(5, 50);
    practiceQuestionsSpin->setValue(10);
    
    optionsLayout->addWidget(quickPracticeBtn2);
    optionsLayout->addWidget(subjectPracticeBtn);
    optionsLayout->addWidget(new QLabel("Subject:"));
    optionsLayout->addWidget(practiceSubjectCombo);
    optionsLayout->addWidget(new QLabel("Number of Questions:"));
    optionsLayout->addWidget(practiceQuestionsSpin);
    
    layout->addWidget(practiceOptionsGroup);
}

void StudentWindow::setupResultsView()
{
    // Results view setup - placeholder
    QVBoxLayout* layout = new QVBoxLayout(resultsTab);
    
    statsLabel = new QLabel("Exam Statistics");
    statsLabel->setObjectName("statsLabel");
    layout->addWidget(statsLabel);
    
    resultsTable = new QTableWidget();
    resultsTable->setColumnCount(4);
    QStringList headers = {"Date", "Exam", "Score", "Grade"};
    resultsTable->setHorizontalHeaderLabels(headers);
    layout->addWidget(resultsTable);
    
    refreshResultsBtn = new QPushButton("Refresh Results");
    refreshResultsBtn->setObjectName("secondaryButton");
    connect(refreshResultsBtn, &QPushButton::clicked, this, &StudentWindow::viewResults);
    layout->addWidget(refreshResultsBtn);
}

void StudentWindow::setupProfileView()
{
    // Profile view setup - placeholder
    QVBoxLayout* layout = new QVBoxLayout(profileTab);
    
    profileInfoLabel = new QLabel(QString(
        "Student Profile\n\n"
        "Username: %1\n"
        "Role: Student\n"
        "Status: Active"
    ).arg(QString::fromStdString(currentStudent.getUsername())));
    
    profileInfoLabel->setObjectName("profileInfo");
    layout->addWidget(profileInfoLabel);
    
    changePasswordBtn = new QPushButton("Change Password");
    changePasswordBtn->setObjectName("secondaryButton");
    layout->addWidget(changePasswordBtn);
}

// Additional missing methods
void StudentWindow::loadAvailableExams()
{
    // Placeholder - load available exams from database
}

void StudentWindow::populateResultsTable()
{
    // Placeholder - populate results table with user's exam history
}

void StudentWindow::startExam()
{
    // Get selected exam from list
    QListWidgetItem* selectedItem = examsList->currentItem();
    if (!selectedItem) {
        QMessageBox::warning(this, "No Selection", "Please select an exam to start.");
        return;
    }
    
    QString examTitle = selectedItem->text();
    currentExamTitle = examTitle.toStdString();
    
    // Load exam questions from database
    currentExamQuestions = dbManager->getAllQuestions();
    
    if (currentExamQuestions.empty()) {
        QMessageBox::warning(this, "No Questions", "This exam has no questions available.");
        return;
    }
    
    // Initialize exam state
    isExamActive = true;
    isPracticeMode = false;
    currentQuestionIndex = 0;
    examTimeLimit = 3600; // 60 minutes default
    remainingTime = examTimeLimit;
    
    // Initialize user answers and review markers
    userAnswers.clear();
    userAnswers.resize(currentExamQuestions.size(), -1);
    markedForReview.clear();
    markedForReview.resize(currentExamQuestions.size(), false);
    
    // DSA: Initialize question queue for sequential access
    questionQueue.clear();
    for (int i = 0; i < static_cast<int>(currentExamQuestions.size()); ++i) {
        questionQueue.push(i);
    }
    
    // Clear navigation history and answer cache
    navigationHistory.clear();
    answerCache.clear();
    
    // Setup exam interface
    examTitleLabel->setText(QString(" %1").arg(examTitle));
    progressBar->setMaximum(currentExamQuestions.size());
    progressBar->setValue(1);
    
    // Start timer
    if (!examTimer) {
        examTimer = new QTimer(this);
        connect(examTimer, &QTimer::timeout, this, &StudentWindow::updateTimer);
    }
    examTimer->start(1000); // Update every second
    
    // Enable exam tab and switch to it
    tabWidget->setTabEnabled(1, true);
    tabWidget->setCurrentIndex(1);
    
    // Display first question
    displayCurrentQuestion();
    
    QMessageBox::information(this, "Exam Started", 
        QString("Exam '%1' has started!\n"
                "Time limit: %2 minutes\n"
                "Total questions: %3\n\n"
                "Good luck!")
        .arg(examTitle)
        .arg(examTimeLimit / 60)
        .arg(currentExamQuestions.size()));
}

void StudentWindow::nextQuestion()
{
    if (!isExamActive && !isPracticeMode) return;
    
    // DSA: Use stack to track navigation history
    navigationHistory.push(currentQuestionIndex);
    
    // Save current answer to cache
    if (answerGroup->checkedButton()) {
        int selectedAnswer = answerGroup->checkedId();
        if (selectedAnswer >= 0 && selectedAnswer < 4) {
            userAnswers[currentQuestionIndex] = selectedAnswer;
            // DSA: Cache the answer using hash table
            answerCache.insert(currentQuestionIndex, selectedAnswer);
        }
    }
    
    // Move to next question
    vector<Question>& questions = isPracticeMode ? practiceQuestions : currentExamQuestions;
    if (currentQuestionIndex < static_cast<int>(questions.size()) - 1) {
        currentQuestionIndex++;
        displayCurrentQuestion();
    } else {
        QMessageBox::information(this, "Last Question", 
            "This is the last question. Click Submit to finish the exam.");
    }
}

void StudentWindow::previousQuestion()
{
    if (!isExamActive && !isPracticeMode) return;
    
    // Save current answer to cache
    if (answerGroup->checkedButton()) {
        int selectedAnswer = answerGroup->checkedId();
        if (selectedAnswer >= 0 && selectedAnswer < 4) {
            userAnswers[currentQuestionIndex] = selectedAnswer;
            // DSA: Cache the answer using hash table
            answerCache.insert(currentQuestionIndex, selectedAnswer);
        }
    }
    
    // DSA: Use stack to go back to previous question
    if (!navigationHistory.empty()) {
        currentQuestionIndex = navigationHistory.top();
        navigationHistory.pop();
        displayCurrentQuestion();
    } else if (currentQuestionIndex > 0) {
        currentQuestionIndex--;
        displayCurrentQuestion();
    } else {
        QMessageBox::information(this, "First Question", 
            "This is the first question.");
    }
}

void StudentWindow::submitExam()
{
    if (!isExamActive && !isPracticeMode) return;
    
    // Save current answer before submitting
    if (answerGroup->checkedButton()) {
        int selectedAnswer = answerGroup->checkedId();
        if (selectedAnswer >= 0 && selectedAnswer < 4) {
            userAnswers[currentQuestionIndex] = selectedAnswer;
            answerCache.insert(currentQuestionIndex, selectedAnswer);
        }
    }
    
    // Confirm submission
    QMessageBox::StandardButton reply = QMessageBox::question(this, 
        "Submit Exam", 
        "Are you sure you want to submit the exam?\nThis action cannot be undone.",
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply != QMessageBox::Yes) {
        return;
    }
    
    // Stop timer if active
    if (examTimer && examTimer->isActive()) {
        examTimer->stop();
    }
    
    // Calculate results
    vector<Question>& questions = isPracticeMode ? practiceQuestions : currentExamQuestions;
    int score = 0;
    int totalQuestions = questions.size();
    
    for (int i = 0; i < totalQuestions; ++i) {
        if (i < static_cast<int>(userAnswers.size()) && 
            userAnswers[i] == questions[i].getCorrectAnswer()) {
            score++;
        }
    }
    
    double percentage = totalQuestions > 0 ? (static_cast<double>(score) / totalQuestions) * 100.0 : 0.0;
    
    if (!isPracticeMode) {
        // Save exam result to database
        ExamResult result;
        result.setUserId(currentStudent.getId());
        result.setSubject(currentExamTitle);
        result.setScore(score);
        result.setTotalQuestions(totalQuestions);
        result.setScore(score);
        result.setTotalQuestions(currentExamQuestions.size());
        result.setDuration(examTimeLimit - remainingTime);
        
        if (dbManager->insertExamResult(result)) {
            QMessageBox::information(this, "Exam Submitted", 
                "Your exam has been submitted successfully!");
        } else {
            QMessageBox::warning(this, "Submission Error", 
                "There was an error saving your exam results. Please contact your instructor.");
        }
    }
    
    // Show results
    QString resultMsg = QString(
        "%1 Results:\n\n"
        "Score: %2/%3\n"
        "Percentage: %4%\n"
        "Grade: %5\n\n"
        "%6"
    ).arg(isPracticeMode ? "Practice" : "Exam")
     .arg(score)
     .arg(totalQuestions)
     .arg(percentage, 0, 'f', 1)
     .arg(percentage >= 60 ? "PASS" : "FAIL")
     .arg(percentage >= 60 ? "Congratulations! You passed!" : "Keep studying and try again!");
    
    QMessageBox::information(this, "Results", resultMsg);
    
    // Reset exam state
    isExamActive = false;
    isPracticeMode = false;
    currentExamQuestions.clear();
    practiceQuestions.clear();
    userAnswers.clear();
    markedForReview.clear();
    navigationHistory.clear();
    answerCache.clear();
    
    // Return to dashboard
    tabWidget->setCurrentIndex(0);
    tabWidget->setTabEnabled(1, false);
    
    // Update dashboard stats
    updateDashboardStats();
}

void StudentWindow::updateTimer()
{
    if (!isExamActive || remainingTime <= 0) {
        if (examTimer && examTimer->isActive()) {
            examTimer->stop();
        }
        
        if (isExamActive && remainingTime <= 0) {
            QMessageBox::warning(this, "Time's Up!", 
                "Time has expired! Your exam will be submitted automatically.");
            submitExam();
        }
        return;
    }
    
    remainingTime--;
    
    // Update timer display
    int hours = remainingTime / 3600;
    int minutes = (remainingTime % 3600) / 60;
    int seconds = remainingTime % 60;
    
    QString timeText = QString("%1:%2:%3")
        .arg(hours, 2, 10, QChar('0'))
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'));
    
    timerLabel->setText(QString(" Time Remaining: %1").arg(timeText));
    
    // Change color based on remaining time
    if (remainingTime <= 300) { // Last 5 minutes
        timerLabel->setStyleSheet("color: red; font-weight: bold; font-size: 14px;");
    } else if (remainingTime <= 600) { // Last 10 minutes
        timerLabel->setStyleSheet("color: orange; font-weight: bold; font-size: 14px;");
    } else {
        timerLabel->setStyleSheet("color: white; font-weight: bold; font-size: 14px;");
    }
    
    // Auto-submit when time runs out
    if (remainingTime <= 0) {
        examTimer->stop();
        QMessageBox::warning(this, "Time's Up!", 
            "Time has expired! Your exam will be submitted automatically.");
        submitExam();
    }
}

void StudentWindow::selectAnswer(int optionIndex)
{
    // Placeholder - handle answer selection
}

void StudentWindow::markForReview()
{
    if (!isExamActive && !isPracticeMode) return;
    
    vector<Question>& questions = isPracticeMode ? practiceQuestions : currentExamQuestions;
    if (currentQuestionIndex < 0 || currentQuestionIndex >= static_cast<int>(questions.size())) {
        return;
    }
    
    // Ensure markedForReview vector is properly sized
    if (markedForReview.size() <= static_cast<size_t>(currentQuestionIndex)) {
        markedForReview.resize(questions.size(), false);
    }
    
    // Toggle review status
    markedForReview[currentQuestionIndex] = !markedForReview[currentQuestionIndex];
    
    // Update UI
    if (markedForReview[currentQuestionIndex]) {
        reviewStatusLabel->setText("⭐ Marked for Review");
        reviewStatusLabel->setStyleSheet("color: orange; font-weight: bold;");
        markReviewBtn->setText("Unmark Review");
        QMessageBox::information(this, "Marked for Review", 
            "This question has been marked for review. You can return to it later.");
    } else {
        reviewStatusLabel->setText("");
        markReviewBtn->setText("Mark for Review");
        QMessageBox::information(this, "Review Unmarked", 
            "This question is no longer marked for review.");
    }
}

void StudentWindow::loadExamQuestions()
{
    // Placeholder - load questions for current exam
}

void StudentWindow::displayCurrentQuestion()
{
    vector<Question>& questions = isPracticeMode ? practiceQuestions : currentExamQuestions;
    
    if (currentQuestionIndex < 0 || currentQuestionIndex >= static_cast<int>(questions.size())) {
        return;
    }
    
    const Question& question = questions[currentQuestionIndex];
    
    // Update question display
    questionNumberLabel->setText(QString("Question %1 of %2")
        .arg(currentQuestionIndex + 1)
        .arg(questions.size()));
    
    questionTextLabel->setText(QString::fromStdString(question.getQuestionText()));
    
    // Update options
    vector<string> options = question.getOptions();
    if (options.size() >= 4) {
        option1Radio->setText(QString::fromStdString(options[0]));
        option2Radio->setText(QString::fromStdString(options[1]));
        option3Radio->setText(QString::fromStdString(options[2]));
        option4Radio->setText(QString::fromStdString(options[3]));
    }
    
    // Clear previous selection
    answerGroup->setExclusive(false);
    option1Radio->setChecked(false);
    option2Radio->setChecked(false);
    option3Radio->setChecked(false);
    option4Radio->setChecked(false);
    answerGroup->setExclusive(true);
    
    // DSA: Restore answer from cache if exists
    int* cachedAnswerPtr = answerCache.find(currentQuestionIndex);
    int cachedAnswer = cachedAnswerPtr ? *cachedAnswerPtr : -1;
    if (cachedAnswer != -1 && cachedAnswer < 4) {
        switch (cachedAnswer) {
            case 0: option1Radio->setChecked(true); break;
            case 1: option2Radio->setChecked(true); break;
            case 2: option3Radio->setChecked(true); break;
            case 3: option4Radio->setChecked(true); break;
        }
    } else if (currentQuestionIndex < static_cast<int>(userAnswers.size()) && 
               userAnswers[currentQuestionIndex] != -1) {
        // Fallback to userAnswers array
        int answer = userAnswers[currentQuestionIndex];
        if (answer >= 0 && answer < 4) {
            switch (answer) {
                case 0: option1Radio->setChecked(true); break;
                case 1: option2Radio->setChecked(true); break;
                case 2: option3Radio->setChecked(true); break;
                case 3: option4Radio->setChecked(true); break;
            }
        }
    }
    
    // Update progress bar
    progressBar->setValue(currentQuestionIndex + 1);
    
    // Update review status
    if (currentQuestionIndex < static_cast<int>(markedForReview.size()) && 
        markedForReview[currentQuestionIndex]) {
        reviewStatusLabel->setText("⭐ Marked for Review");
        reviewStatusLabel->setStyleSheet("color: orange; font-weight: bold;");
    } else {
        reviewStatusLabel->setText("");
    }
    
    // Update button states
    previousBtn->setEnabled(currentQuestionIndex > 0 || !navigationHistory.empty());
    nextBtn->setEnabled(currentQuestionIndex < static_cast<int>(questions.size()) - 1);
}

void StudentWindow::calculateAndSaveResults()
{
    // Placeholder - calculate and save exam results
}

StudentWindow::~StudentWindow()
{
    // Destructor - cleanup if needed
}

void StudentWindow::applyStyles()
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
        
        #welcomeLabel {
            font-size: 24px;
            font-weight: bold;
            color: #3498db;
            padding: 20px;
        }
        
        #statsCard {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #3498db, stop:1 #2980b9);
            border: none;
            border-radius: 10px;
            min-height: 100px;
            margin: 10px;
        }
        
        #statsNumber {
            font-size: 36px;
            font-weight: bold;
            color: white;
            text-align: center;
        }
        
        #statsText {
            font-size: 14px;
            color: white;
            text-align: center;
            padding: 10px;
        }
        
        #examTitle {
            font-size: 20px;
            font-weight: bold;
            color: #3498db;
        }
        
        #timerLabel {
            font-size: 24px;
            font-weight: bold;
            color: #27ae60;
            background: #2c3e50;
            padding: 10px;
            border-radius: 5px;
            border: 2px solid #27ae60;
        }
        
        #questionFrame {
            background: #34495e;
            border: 2px solid #7f8c8d;
            border-radius: 10px;
            padding: 20px;
            margin: 10px;
        }
        
        #questionNumber {
            font-size: 16px;
            font-weight: bold;
            color: #3498db;
            margin-bottom: 10px;
        }
        
        #questionText {
            font-size: 16px;
            color: #ecf0f1;
            background: #2c3e50;
            padding: 15px;
            border-radius: 5px;
            border: 1px solid #7f8c8d;
        }
        
        #answerOption {
            font-size: 14px;
            color: #ecf0f1;
            padding: 10px;
            margin: 5px;
        }
        
        #answerOption:checked {
            color: #3498db;
            font-weight: bold;
        }
        
        #reviewStatus {
            font-size: 14px;
            padding: 5px;
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
            padding: 15px 25px;
            border-radius: 8px;
            font-size: 14px;
            font-weight: bold;
            margin: 5px;
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
            padding: 15px 25px;
            border-radius: 8px;
            font-size: 14px;
            margin: 5px;
        }
        
        #secondaryButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #7f8c8d, stop:1 #6c7b7d);
        }
        
        #navButton {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #3498db, stop:1 #2980b9);
            color: white;
            border: none;
            padding: 12px 20px;
            border-radius: 6px;
            font-size: 14px;
            font-weight: bold;
            margin: 5px;
        }
        
        #navButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #2980b9, stop:1 #21618c);
        }
        
        #submitButton {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #e74c3c, stop:1 #c0392b);
            color: white;
            border: none;
            padding: 12px 20px;
            border-radius: 6px;
            font-size: 14px;
            font-weight: bold;
            margin: 5px;
        }
        
        #submitButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #c0392b, stop:1 #a93226);
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
        
        QListWidget {
            background: #2c3e50;
            color: white;
            border: 2px solid #7f8c8d;
            border-radius: 5px;
            padding: 5px;
        }
        
        QListWidget::item {
            padding: 10px;
            border-bottom: 1px solid #7f8c8d;
        }
        
        QListWidget::item:selected {
            background: #3498db;
        }
        
        QProgressBar {
            border: 2px solid #7f8c8d;
            border-radius: 5px;
            text-align: center;
            font-weight: bold;
        }
        
        QProgressBar::chunk {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #27ae60, stop:1 #229954);
            border-radius: 3px;
        }
        
        #instructionsLabel {
            background: #34495e;
            border: 1px solid #7f8c8d;
            border-radius: 5px;
            padding: 15px;
            color: #ecf0f1;
        }
        
        #profileInfo {
            background: #34495e;
            border: 1px solid #7f8c8d;
            border-radius: 5px;
            padding: 20px;
            color: #ecf0f1;
        }
        
        #statsLabel {
            background: #34495e;
            border: 1px solid #7f8c8d;
            border-radius: 5px;
            padding: 15px;
            color: #ecf0f1;
            margin-bottom: 10px;
        }
    )");
}