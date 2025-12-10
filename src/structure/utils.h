#ifndef STRUCTURE_UTILS_H
#define STRUCTURE_UTILS_H

#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <random>

// Forward declarations for DSA components
template<typename T> class LinkedList;
template<typename T> class Stack;
template<typename T> class Queue;
template<typename K, typename V> class HashTable;

// Utility Functions
class Utils {
public:
    static std::string getCurrentDateTime() {
        auto now = std::chrono::system_clock::now();
        std::time_t time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    static void clearScreen() {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
    }
    
    static void pauseSystem() {
        std::cout << "\nPress Enter to continue...";
        std::cin.ignore();
        std::cin.get();
    }
    
    static std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(' ');
        if (std::string::npos == first) {
            return str;
        }
        size_t last = str.find_last_not_of(' ');
        return str.substr(first, (last - first + 1));
    }
    
    static std::vector<std::string> split(const std::string& str, char delimiter) {
        std::vector<std::string> tokens;
        std::stringstream ss(str);
        std::string token;
        
        while (std::getline(ss, token, delimiter)) {
            tokens.push_back(trim(token));
        }
        
        return tokens;
    }
    
    static std::string generateRandomString(int length) {
        const std::string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, charset.size() - 1);
        
        std::string result;
        for (int i = 0; i < length; ++i) {
            result += charset[dis(gen)];
        }
        
        return result;
    }
    
    template<typename T>
    static void shuffleVector(std::vector<T>& vec) {
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(vec.begin(), vec.end(), g);
    }
    
    static double calculatePercentage(int score, int total) {
        if (total == 0) return 0.0;
        return (static_cast<double>(score) / total) * 100.0;
    }
    
    static void printHeader(const std::string& title) {
        std::string border(50, '=');
        std::cout << "\n" << border << std::endl;
        std::cout << std::setw(25 + title.length()/2) << title << std::endl;
        std::cout << border << std::endl;
    }
    
    static void printSubHeader(const std::string& subtitle) {
        std::string border(40, '-');
        std::cout << "\n" << border << std::endl;
        std::cout << subtitle << std::endl;
        std::cout << border << std::endl;
    }
    
    static void printWelcomeMessage();
    static void printGoodbye();
    static void printLoadingAnimation(const std::string& message);
    static void printProgressBar(int current, int total, const std::string& prefix = "Progress");
    static bool confirmAction(const std::string& message);
    static void printTableHeader(const std::vector<std::string>& headers, const std::vector<int>& widths);
    static void printTableRow(const std::vector<std::string>& data, const std::vector<int>& widths);
};

#endif // STRUCTURE_UTILS_H