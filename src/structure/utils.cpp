#include "utils.h"
#include <iostream>
#include <iomanip>
#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif

void Utils::printWelcomeMessage() {
    clearScreen();
    std::cout << R"(
    ╔══════════════════════════════════════════════════════════════╗
    ║                                                              ║
    ║              ONLINE EXAMINATION SYSTEM                       ║
    ║                                                              ║
    ║              Advanced C++ with DSA Implementation            ║
    ║                                                              ║
    ╚══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void Utils::printGoodbye() {
    clearScreen();
    std::cout << R"(
    ╔══════════════════════════════════════════════════════════════╗
    ║                                                              ║
    ║                    Thank You!                                ║
    ║                                                              ║
    ║            For using Online Examination System               ║
    ║                                                              ║
    ╚══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void Utils::printLoadingAnimation(const std::string& message) {
    std::cout << message;
    for (int i = 0; i < 3; ++i) {
        std::cout << ".";
        std::cout.flush();
        #ifdef _WIN32
            Sleep(500);
        #else
            usleep(500000);
        #endif
    }
    std::cout << " Done!" << std::endl;
}

void Utils::printProgressBar(int current, int total, const std::string& prefix) {
    const int barWidth = 50;
    float progress = static_cast<float>(current) / total;
    int pos = static_cast<int>(barWidth * progress);
    
    std::cout << "\r" << prefix << " [";
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "█";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << int(progress * 100.0) << "% (" << current << "/" << total << ")";
    std::cout.flush();
    
    if (current == total) {
        std::cout << std::endl;
    }
}

bool Utils::confirmAction(const std::string& message) {
    std::cout << message << " (y/n): ";
    char choice;
    std::cin >> choice;
    return (choice == 'y' || choice == 'Y');
}

void Utils::printTableHeader(const std::vector<std::string>& headers, const std::vector<int>& widths) {
    std::cout << "|";
    for (size_t i = 0; i < headers.size(); ++i) {
        std::cout << std::setw(widths[i]) << std::left << headers[i] << "|";
    }
    std::cout << std::endl;
    
    std::cout << "|";
    for (int width : widths) {
        std::cout << std::string(width, '-') << "|";
    }
    std::cout << std::endl;
}

void Utils::printTableRow(const std::vector<std::string>& data, const std::vector<int>& widths) {
    std::cout << "|";
    for (size_t i = 0; i < data.size() && i < widths.size(); ++i) {
        std::string cellData = data[i];
        if (cellData.length() > static_cast<size_t>(widths[i] - 1)) {
            cellData = cellData.substr(0, widths[i] - 4) + "...";
        }
        std::cout << std::setw(widths[i]) << std::left << cellData << "|";
    }
    std::cout << std::endl;
}