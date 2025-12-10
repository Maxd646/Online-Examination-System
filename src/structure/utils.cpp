#include "utils.h"
#include <iostream>
#include <iomanip>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
using namespace std;

void Utils::printWelcomeMessage()
{
    clearScreen();
    cout << R"(
    ╔══════════════════════════════════════════════════════════════╗
    ║                                                              ║
    ║              ONLINE EXAMINATION SYSTEM                       ║
    ║                                                              ║
    ║              Advanced C++ with DSA Implementation            ║
    ║                                                              ║
    ╚══════════════════════════════════════════════════════════════╝
    )" << endl;
}

void Utils::printGoodbye()
{
    clearScreen();
    cout << R"(
    ╔══════════════════════════════════════════════════════════════╗
    ║                                                              ║
    ║                    Thank You!                                ║
    ║                                                              ║
    ║            For using Online Examination System               ║
    ║                                                              ║
    ╚══════════════════════════════════════════════════════════════╝
    )" << endl;
}

void Utils::printLoadingAnimation(const string &message)
{
    cout << message;
    for (int i = 0; i < 3; ++i)
    {
        cout << ".";
        cout.flush();
#ifdef _WIN32
        Sleep(500);
#else
        usleep(500000);
#endif
    }
    cout << " Done!" << endl;
}

void Utils::printProgressBar(int current, int total, const string &prefix)
{
    const int barWidth = 50;
    float progress = static_cast<float>(current) / total;
    int pos = static_cast<int>(barWidth * progress);

    cout << "\r" << prefix << " [";
    for (int i = 0; i < barWidth; ++i)
    {
        if (i < pos)
            cout << "█";
        else if (i == pos)
            cout << ">";
        else
            cout << " ";
    }
    cout << "] " << int(progress * 100.0) << "% (" << current << "/" << total << ")";
    cout.flush();

    if (current == total)
    {
        cout << endl;
    }
}

bool Utils::confirmAction(const string &message)
{
    cout << message << " (y/n): ";
    char choice;
    cin >> choice;
    return (choice == 'y' || choice == 'Y');
}

void Utils::printTableHeader(const vector<string> &headers, const vector<int> &widths)
{
    cout << "|";
    for (size_t i = 0; i < headers.size(); ++i)
    {
        cout << setw(widths[i]) << left << headers[i] << "|";
    }
    cout << endl;

    cout << "|";
    for (int width : widths)
    {
        cout << string(width, '-') << "|";
    }
    cout << endl;
}

void Utils::printTableRow(const vector<string> &data, const vector<int> &widths)
{
    cout << "|";
    for (size_t i = 0; i < data.size() && i < widths.size(); ++i)
    {
        string cellData = data[i];
        if (cellData.length() > static_cast<size_t>(widths[i] - 1))
        {
            cellData = cellData.substr(0, widths[i] - 4) + "...";
        }
        cout << setw(widths[i]) << left << cellData << "|";
    }
    cout << endl;
}