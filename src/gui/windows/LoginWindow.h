#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QStackedWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QMessageBox>
#include <QFrame>
#include <QPixmap>
#include <QMovie>
#include "../../database/database.h"
#include "../../authentication/user.h"

// Forward declarations to avoid circular includes
class TeacherWindow;
class StudentWindow;

class LoginWindow : public QMainWindow
{
    Q_OBJECT

public:
    LoginWindow(DatabaseManager* dbManager, QWidget *parent = nullptr);
    ~LoginWindow();

private slots:
    void handleLogin();
    void handleRegister();
    void showRegisterForm();
    void showLoginForm();

private:
    void setupUI();
    void setupLoginForm();
    void setupRegisterForm();
    void applyStyles();
    bool validateInput();
    void clearForms();
    
    // Database
    DatabaseManager* dbManager;
    
    // UI Components
    QWidget* centralWidget;
    QVBoxLayout* mainLayout;
    QFrame* headerFrame;
    QStackedWidget* stackedWidget;
    QFrame* footerFrame;
    
    // Header
    QLabel* titleLabel;
    QLabel* subtitleLabel;
    
    // Login Form
    QWidget* loginWidget;
    QGridLayout* loginLayout;
    QLineEdit* loginUsernameEdit;
    QLineEdit* loginPasswordEdit;
    QPushButton* loginButton;
    QPushButton* showRegisterButton;
    
    // Register Form
    QWidget* registerWidget;
    QGridLayout* registerLayout;
    QLineEdit* regFullNameEdit;
    QLineEdit* regUsernameEdit;
    QLineEdit* regEmailEdit;
    QLineEdit* regPasswordEdit;
    QLineEdit* regConfirmPasswordEdit;
    QComboBox* regRoleCombo;
    QPushButton* registerButton;
    QPushButton* showLoginButton;
    
    // Status
    QLabel* statusLabel;
    
    bool isRegisterMode;
};

#endif // LOGINWINDOW_H