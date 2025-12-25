#include "utils.h"
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <limits>
#include <cctype>
#include <stdexcept>
#ifdef _WIN32
#include <thread>
#include <chrono>
#else
#include <unistd.h>
#endif
using namespace std;

void Utils::printWelcomeMessage()
{
    clearScreen();
    cout << "\n";
    cout << "    +==============================================================+" << endl;
    cout << "    |                                                              |" << endl;
    cout << "    |              ONLINE EXAMINATION SYSTEM                       |" << endl;
    cout << "    |                                                              |" << endl;
    cout << "    |              Advanced C++ with DSA Implementation            |" << endl;
    cout << "    |                                                              |" << endl;
    cout << "    +==============================================================+" << endl;
    cout << endl;
}

void Utils::printGoodbye()
{
    clearScreen();
    cout << "\n";
    cout << "    +==============================================================+" << endl;
    cout << "    |                                                              |" << endl;
    cout << "    |                    Thank You!                                |" << endl;
    cout << "    |                                                              |" << endl;
    cout << "    |            For using Online Examination System               |" << endl;
    cout << "    |                                                              |" << endl;
    cout << "    +==============================================================+" << endl;
    cout << endl;
}

string Utils::getSafeString(const string &prompt, size_t maxLength, bool allowSpaces)
{
    string input;
    while (true)
    {
        cout << prompt;
        getline(cin, input);
        
        // Trim whitespace
        input = trim(input);
        
        // Check if empty
        if (input.empty())
        {
            cout << "Error: Input cannot be empty. Please try again." << endl;
            continue;
        }
        
        // Check length
        if (input.length() > maxLength)
        {
            cout << "Error: Input too long (maximum " << maxLength << " characters). Please try again." << endl;
            continue;
        }
        
        // Check for spaces if not allowed
        if (!allowSpaces && input.find(' ') != string::npos)
        {
            cout << "Error: Spaces are not allowed. Please try again." << endl;
            continue;
        }
        
        // Check for control characters
        bool hasControlChar = false;
        for (char c : input)
        {
            if (iscntrl(c))
            {
                hasControlChar = true;
                break;
            }
        }
        if (hasControlChar)
        {
            cout << "Error: Invalid characters detected. Please try again." << endl;
            continue;
        }
        
        return input;
    }
}

string Utils::getSafeStringNoSpaces(const string &prompt, size_t maxLength)
{
    return getSafeString(prompt, maxLength, false);
}

int Utils::getSafeInt(const string &prompt, int min, int max)
{
    int value;
    while (true)
    {
        cout << prompt;
        cin >> value;
        
        // Check if input failed
        if (cin.fail())
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Error: Invalid input. Please enter a valid number." << endl;
            continue;
        }
        
        // Clear the input buffer
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        // Check range
        if (value < min || value > max)
        {
            cout << "Error: Value must be between " << min << " and " << max << ". Please try again." << endl;
            continue;
        }
        
        return value;
    }
}

double Utils::getSafeDouble(const string &prompt, double min, double max)
{
    double value;
    while (true)
    {
        cout << prompt;
        cin >> value;
        
        // Check if input failed
        if (cin.fail())
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Error: Invalid input. Please enter a valid number." << endl;
            continue;
        }
        
        // Clear the input buffer
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        // Check range
        if (value < min || value > max)
        {
            cout << "Error: Value must be between " << min << " and " << max << ". Please try again." << endl;
            continue;
        }
        
        return value;
    }
}

char Utils::getSafeChar(const string &prompt, const string &validChars)
{
    string input;
    while (true)
    {
        cout << prompt;
        getline(cin, input);
        
        if (input.empty())
        {
            cout << "Error: Input cannot be empty. Please try again." << endl;
            continue;
        }
        
        char ch = tolower(input[0]);
        
        // If validChars is specified, check if char is in the list
        if (!validChars.empty())
        {
            bool isValid = false;
            for (char valid : validChars)
            {
                if (tolower(valid) == ch)
                {
                    isValid = true;
                    break;
                }
            }
            if (!isValid)
            {
                cout << "Error: Invalid choice. Valid options: " << validChars << ". Please try again." << endl;
                continue;
            }
        }
        
        return ch;
    }
}

int Utils::safeStoi(const string &str, int defaultValue)
{
    if (str.empty())
        return defaultValue;
    
    try
    {
        size_t pos;
        int value = stoi(str, &pos);
        // Check if entire string was converted
        if (pos != str.length())
            return defaultValue;
        return value;
    }
    catch (const invalid_argument &)
    {
        return defaultValue;
    }
    catch (const out_of_range &)
    {
        return defaultValue;
    }
}

double Utils::safeStod(const string &str, double defaultValue)
{
    if (str.empty())
        return defaultValue;
    
    try
    {
        size_t pos;
        double value = stod(str, &pos);
        // Check if entire string was converted
        if (pos != str.length())
            return defaultValue;
        return value;
    }
    catch (const invalid_argument &)
    {
        return defaultValue;
    }
    catch (const out_of_range &)
    {
        return defaultValue;
    }
}