#include "LoginWindow.h"
#include "TeacherWindow.h"
#include "StudentWindow.h"
#include <QApplication>
#include <QScreen>

LoginWindow::LoginWindow(DatabaseManager* dbManager, QWidget *parent)
    : QMainWindow(parent), dbManager(dbManager), isRegisterMode(false)
{
    setupUI();
    applyStyles();
    
    // Center window on screen
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);
}

LoginWindow::~LoginWindow()
{
}

void LoginWindow::setupUI()
{
    setWindowTitle("Online Examination System - Login");
    setFixedSize(450, 600);
    
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    
    // Header
    headerFrame = new QFrame();
    headerFrame->setFixedHeight(120);
    headerFrame->setObjectName("headerFrame");
    
    QVBoxLayout* headerLayout = new QVBoxLayout(headerFrame);
    titleLabel = new QLabel("🎓 Examination System");
    titleLabel->setObjectName("titleLabel");
    titleLabel->setAlignment(Qt::AlignCenter);
    
    subtitleLabel = new QLabel("Advanced Learning Platform");
    subtitleLabel->setObjectName("subtitleLabel");
    subtitleLabel->setAlignment(Qt::AlignCenter);
    
    headerLayout->addWidget(titleLabel);
    headerLayout->addWidget(subtitleLabel);
    
    // Stacked Widget for forms
    stackedWidget = new QStackedWidget();
    stackedWidget->setObjectName("formFrame");
    
    setupLoginForm();
    setupRegisterForm();
    
    // Add forms to stacked widget
    stackedWidget->addWidget(loginWidget);
    stackedWidget->addWidget(registerWidget);
    
    // Footer
    footerFrame = new QFrame();
    footerFrame->setFixedHeight(60);
    footerFrame->setObjectName("footerFrame");
    
    QVBoxLayout* footerLayout = new QVBoxLayout(footerFrame);
    statusLabel = new QLabel("");
    statusLabel->setObjectName("statusLabel");
    statusLabel->setAlignment(Qt::AlignCenter);
    footerLayout->addWidget(statusLabel);
    
    mainLayout->addWidget(headerFrame);
    mainLayout->addWidget(stackedWidget, 1);
    mainLayout->addWidget(footerFrame);
    
    showLoginForm();
}

void LoginWindow::setupLoginForm()
{
    loginWidget = new QWidget();
    loginWidget->setObjectName("loginWidget");
    loginLayout = new QGridLayout(loginWidget);
    loginLayout->setSpacing(15);
    loginLayout->setContentsMargins(40, 60, 40, 60);
    
    // Username
    loginLayout->addWidget(new QLabel("Username:"), 0, 0);
    loginUsernameEdit = new QLineEdit();
    loginUsernameEdit->setPlaceholderText("Enter your username");
    loginLayout->addWidget(loginUsernameEdit, 0, 1);
    
    // Password
    loginLayout->addWidget(new QLabel("Password:"), 1, 0);
    loginPasswordEdit = new QLineEdit();
    loginPasswordEdit->setEchoMode(QLineEdit::Password);
    loginPasswordEdit->setPlaceholderText("Enter your password");
    loginLayout->addWidget(loginPasswordEdit, 1, 1);
    
    // Buttons
    loginButton = new QPushButton(" Login");
    loginButton->setObjectName("primaryButton");
    loginLayout->addWidget(loginButton, 2, 0, 1, 2);
    
    showRegisterButton = new QPushButton("👤 New User? Register Here");
    showRegisterButton->setObjectName("secondaryButton");
    loginLayout->addWidget(showRegisterButton, 3, 0, 1, 2);
    
    // Connect signals
    connect(loginButton, &QPushButton::clicked, this, &LoginWindow::handleLogin);
    connect(showRegisterButton, &QPushButton::clicked, this, &LoginWindow::showRegisterForm);
    connect(loginPasswordEdit, &QLineEdit::returnPressed, this, &LoginWindow::handleLogin);
}

void LoginWindow::setupRegisterForm()
{
    registerWidget = new QWidget();
    registerWidget->setObjectName("registerWidget");
    registerLayout = new QGridLayout(registerWidget);
    registerLayout->setSpacing(12);
    registerLayout->setContentsMargins(40, 30, 40, 30);
    
    // Full Name
    registerLayout->addWidget(new QLabel("Full Name:"), 0, 0);
    regFullNameEdit = new QLineEdit();
    regFullNameEdit->setPlaceholderText("Enter your full name");
    registerLayout->addWidget(regFullNameEdit, 0, 1);
    
    // Username
    registerLayout->addWidget(new QLabel("Username:"), 1, 0);
    regUsernameEdit = new QLineEdit();
    regUsernameEdit->setPlaceholderText("Choose a username");
    registerLayout->addWidget(regUsernameEdit, 1, 1);
    
    // Email
    registerLayout->addWidget(new QLabel("Email:"), 2, 0);
    regEmailEdit = new QLineEdit();
    regEmailEdit->setPlaceholderText("Enter your email");
    registerLayout->addWidget(regEmailEdit, 2, 1);
    
    // Role
    registerLayout->addWidget(new QLabel("Role:"), 3, 0);
    regRoleCombo = new QComboBox();
    regRoleCombo->addItem(" Student", static_cast<int>(UserRole::STUDENT));
    regRoleCombo->addItem(" Teacher", static_cast<int>(UserRole::TEACHER));
    registerLayout->addWidget(regRoleCombo, 3, 1);
    
    // Password
    registerLayout->addWidget(new QLabel("Password:"), 4, 0);
    regPasswordEdit = new QLineEdit();
    regPasswordEdit->setEchoMode(QLineEdit::Password);
    regPasswordEdit->setPlaceholderText("Create a password (min 6 chars)");
    registerLayout->addWidget(regPasswordEdit, 4, 1);
    
    // Confirm Password
    registerLayout->addWidget(new QLabel("Confirm:"), 5, 0);
    regConfirmPasswordEdit = new QLineEdit();
    regConfirmPasswordEdit->setEchoMode(QLineEdit::Password);
    regConfirmPasswordEdit->setPlaceholderText("Confirm password");
    registerLayout->addWidget(regConfirmPasswordEdit, 5, 1);
    
    // Buttons
    registerButton = new QPushButton(" Create Account");
    registerButton->setObjectName("primaryButton");
    registerLayout->addWidget(registerButton, 6, 0, 1, 2);
    
    showLoginButton = new QPushButton("← Back to Login");
    showLoginButton->setObjectName("secondaryButton");
    registerLayout->addWidget(showLoginButton, 7, 0, 1, 2);
    
    // Connect signals
    connect(registerButton, &QPushButton::clicked, this, &LoginWindow::handleRegister);
    connect(showLoginButton, &QPushButton::clicked, this, &LoginWindow::showLoginForm);
    connect(regConfirmPasswordEdit, &QLineEdit::returnPressed, this, &LoginWindow::handleRegister);
}

void LoginWindow::showLoginForm()
{
    stackedWidget->setCurrentWidget(loginWidget);
    isRegisterMode = false;
    clearForms();
    statusLabel->setText("Welcome! Please login to continue.");
}

void LoginWindow::showRegisterForm()
{
    stackedWidget->setCurrentWidget(registerWidget);
    isRegisterMode = true;
    clearForms();
    statusLabel->setText("Create your account to get started.");
}

void LoginWindow::handleLogin()
{
    QString username = loginUsernameEdit->text().trimmed();
    QString password = loginPasswordEdit->text();
    
    if (username.isEmpty() || password.isEmpty()) {
        statusLabel->setText(" Please enter both username and password.");
        return;
    }
    
    // Authenticate user
    User user;
    if (dbManager->authenticateUser(username.toStdString(), password.toStdString(), user)) {
        statusLabel->setText(" Login successful! Welcome " + QString::fromStdString(user.getFullName()));
        
        // Open appropriate window based on role
        if (user.getRole() == UserRole::TEACHER) {
            TeacherWindow* teacherWindow = new TeacherWindow(dbManager, user);
            teacherWindow->show();
        } else {
            StudentWindow* studentWindow = new StudentWindow(dbManager, user);
            studentWindow->show();
        }
        
        this->close();
    } else {
        statusLabel->setText(" Invalid username or password.");
    }
}

void LoginWindow::handleRegister()
{
    // Validate input
    if (!validateInput()) {
        return;
    }
    
    QString fullName = regFullNameEdit->text().trimmed();
    QString username = regUsernameEdit->text().trimmed();
    QString email = regEmailEdit->text().trimmed();
    QString password = regPasswordEdit->text();
    UserRole role = static_cast<UserRole>(regRoleCombo->currentData().toInt());
    
    // Check if user exists
    User existingUser = dbManager->getUserByUsername(username.toStdString());
    if (existingUser.getId() != 0) {
        statusLabel->setText(" Username already exists. Please choose another.");
        return;
    }
    
    // Create new user
    User newUser(0, username.toStdString(), password.toStdString(), 
                email.toStdString(), fullName.toStdString(), role);
    
    if (dbManager->insertUser(newUser)) {
        statusLabel->setText(" Registration successful! You can now login.");
        showLoginForm();
    } else {
        statusLabel->setText("Registration failed. Please try again.");
    }
}

bool LoginWindow::validateInput()
{
    if (regFullNameEdit->text().trimmed().isEmpty()) {
        statusLabel->setText(" Please enter your full name.");
        return false;
    }
    
    if (regUsernameEdit->text().trimmed().isEmpty()) {
        statusLabel->setText(" Please enter a username.");
        return false;
    }
    
    if (regEmailEdit->text().trimmed().isEmpty()) {
        statusLabel->setText(" Please enter your email.");
        return false;
    }
    
    if (regPasswordEdit->text().length() < 6) {
        statusLabel->setText(" Password must be at least 6 characters.");
        return false;
    }
    
    if (regPasswordEdit->text() != regConfirmPasswordEdit->text()) {
        statusLabel->setText(" Passwords do not match.");
        return false;
    }
    
    return true;
}

void LoginWindow::clearForms()
{
    // Clear login form
    loginUsernameEdit->clear();
    loginPasswordEdit->clear();
    
    // Clear register form
    regFullNameEdit->clear();
    regUsernameEdit->clear();
    regEmailEdit->clear();
    regPasswordEdit->clear();
    regConfirmPasswordEdit->clear();
    regRoleCombo->setCurrentIndex(0);
}

void LoginWindow::applyStyles()
{
    setStyleSheet(R"(
        QMainWindow {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #2c3e50, stop:1 #34495e);
        }
        
        #headerFrame {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #3498db, stop:1 #2980b9);
            border-bottom: 3px solid #1abc9c;
        }
        
        #titleLabel {
            font-size: 28px;
            font-weight: bold;
            color: white;
            margin: 10px;
        }
        
        #subtitleLabel {
            font-size: 14px;
            color: #ecf0f1;
            margin-bottom: 10px;
        }
        
        #formFrame {
            background: #34495e;
            border-radius: 10px;
            margin: 20px;
        }
        
        #loginWidget, #registerWidget {
            background: transparent;
        }
        
        QLabel {
            color: #ecf0f1;
            font-size: 12px;
            font-weight: bold;
        }
        
        QLineEdit {
            padding: 10px;
            border: 2px solid #7f8c8d;
            border-radius: 5px;
            background: #2c3e50;
            color: white;
            font-size: 12px;
        }
        
        QLineEdit:focus {
            border-color: #3498db;
        }
        
        QComboBox {
            padding: 10px;
            border: 2px solid #7f8c8d;
            border-radius: 5px;
            background: #2c3e50;
            color: white;
            font-size: 12px;
        }
        
        QComboBox:focus {
            border-color: #3498db;
        }
        
        QComboBox::drop-down {
            border: none;
            background: #34495e;
        }
        
        QComboBox::down-arrow {
            image: none;
            border-left: 5px solid transparent;
            border-right: 5px solid transparent;
            border-top: 5px solid #ecf0f1;
            margin-right: 10px;
        }
        
        QComboBox QAbstractItemView {
            background: #2c3e50;
            color: white;
            selection-background-color: #3498db;
            border: 1px solid #7f8c8d;
        }
        
        #primaryButton {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #27ae60, stop:1 #229954);
            color: white;
            border: none;
            padding: 12px;
            border-radius: 6px;
            font-size: 14px;
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
            padding: 10px;
            border-radius: 6px;
            font-size: 12px;
        }
        
        #secondaryButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #7f8c8d, stop:1 #6c7b7d);
        }
        
        #statusLabel {
            color: #e74c3c;
            font-size: 12px;
            font-weight: bold;
            padding: 10px;
        }
        
        #footerFrame {
            background: #2c3e50;
            border-top: 1px solid #7f8c8d;
        }
    )");
}