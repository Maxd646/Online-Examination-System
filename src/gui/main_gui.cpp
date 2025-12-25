#include <QApplication>
#include <QStyleFactory>
#include <QDir>
#include <QMessageBox>
#include <iostream>
#include "windows/LoginWindow.h"
#include "../database/database.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    try {
        // Set application properties
        app.setApplicationName("Online Examination System");
        app.setApplicationVersion("2.0");
        app.setOrganizationName("Educational Software");
        
        // Set modern style
        app.setStyle(QStyleFactory::create("Fusion"));
        
        // Apply dark theme
        QPalette darkPalette;
        darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
        darkPalette.setColor(QPalette::WindowText, Qt::white);
        darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
        darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
        darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
        darkPalette.setColor(QPalette::ToolTipText, Qt::white);
        darkPalette.setColor(QPalette::Text, Qt::white);
        darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
        darkPalette.setColor(QPalette::ButtonText, Qt::white);
        darkPalette.setColor(QPalette::BrightText, Qt::red);
        darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
        darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
        darkPalette.setColor(QPalette::HighlightedText, Qt::black);
        app.setPalette(darkPalette);
        
        // Initialize database
        std::cout << "Initializing database..." << std::endl;
        DatabaseManager dbManager("database/exam.db");
        if (!dbManager.initializeDatabase()) {
            QMessageBox::critical(nullptr, "Database Error", 
                "Failed to initialize database. Please check if database/exam.db exists and is accessible.");
            return -1;
        }
        std::cout << "Database initialized successfully!" << std::endl;
        
        // Create and show login window
        std::cout << "Creating login window..." << std::endl;
        LoginWindow loginWindow(&dbManager);
        loginWindow.show();
        std::cout << "Login window created and shown!" << std::endl;
        
        return app.exec();
    }
    catch (const std::exception& e) {
        QMessageBox::critical(nullptr, "Application Error", 
            QString("An error occurred: %1").arg(e.what()));
        return -1;
    }
}