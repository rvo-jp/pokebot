#pragma once
#include <string>
#include <windows.h>
#include <stdexcept>
#include <vector>
#include <iostream>
#include <regex>

using namespace std;

class Bot {
public:
    Bot(const string& label, const string& port);

    bool isInside(const POINT point);
    bool isForground();
    POINT getRelativePos(const POINT point, int devW, int devH);
    
    void tap(const POINT pos);
    void swipe(const POINT startPos, const POINT endPos, DWORD time);
    void keyevent(const DWORD key);
    void disconnect();

    string& getPort();
    
private:
    string port;
    HWND window;
    int width;
    int height;

    void inputShell(const string& input);

    string runCommand(const string& command);
    void runCommandAsync(const string& command);
};