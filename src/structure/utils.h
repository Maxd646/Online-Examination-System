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

using namespace std;

// Forward declarations for DSA components
template <typename T>
class LinkedList;
template <typename T>
class Stack;
template <typename T>
class Queue;
template <typename K, typename V>
class HashTable;

// Utility Functions
class Utils
{
public:
    static string getCurrentDateTime()
    {
        auto now = chrono::system_clock::now();
        time_t time = chrono::system_clock::to_time_t(now);
        stringstream ss;
        ss << put_time(localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }

    static void clearScreen()
    {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
    }

    static void pauseSystem()
    {
        cout << "\nPress Enter to continue...";
        cin.ignore();
        cin.get();
    }

    static string trim(const string &str)
    {
        size_t first = str.find_first_not_of(' ');
        if (string::npos == first)
        {
            return str;
        }
        size_t last = str.find_last_not_of(' ');
        return str.substr(first, (last - first + 1));
    }

    static vector<string> split(const string &str, char delimiter)
    {
        vector<string> tokens;
        stringstream ss(str);
        string token;

        while (getline(ss, token, delimiter))
        {
            tokens.push_back(trim(token));
        }

        return tokens;
    }

    static string generateRandomString(int length)
    {
        const string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dis(0, charset.size() - 1);

        string result;
        for (int i = 0; i < length; ++i)
        {
            result += charset[dis(gen)];
        }

        return result;
    }

    static double calculatePercentage(int score, int total)
    {
        if (total == 0)
            return 0.0;
        return (static_cast<double>(score) / total) * 100.0;
    }

    static void printHeader(const string &title)
    {
        string border(50, '=');
        cout << "\n"
             << border << endl;
        cout << setw(25 + title.length() / 2) << title << endl;
        cout << border << endl;
    }

    static void printWelcomeMessage();
    static void printGoodbye();
};

#endif // STRUCTURE_UTILS_H