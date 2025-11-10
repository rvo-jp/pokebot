#ifndef POKEBOT_BOT_H
#define POKEBOT_BOT_H

#include <string>
#include <windows.h>

using namespace std;

class Bot {
public:
    bool isInside(const POINT point);
    POINT getRelativePos(const POINT point, int devW, int devH);

private:
    string port;
    HWND window;
    bool getWindow(const string label);
    void inputShell(const string input);
    void runCommandAsync(const string command);
};

#endif