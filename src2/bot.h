#ifndef POKEBOT_BOT_H
#define POKEBOT_BOT_H

#include <string>
#include <windows.h>

using namespace std;

class Bot {
private:
    string port;
    HWND window;
    bool getWindow(const string label);
    void runCommandAsync(const string command);
};

#endif