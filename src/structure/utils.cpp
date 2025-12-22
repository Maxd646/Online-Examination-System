#include "utils.h"
#include <iostream>
#include <iomanip>
#include <cstdlib>
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