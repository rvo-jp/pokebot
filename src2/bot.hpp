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
    ~Bot();

    bool isInside(const POINT point);
    bool isForground();
    POINT getRelativePos(const POINT point);
    
    void tap(const POINT pos);
    void swipe(const POINT startPos, const POINT endPos, DWORD time);
    void keyevent(const DWORD key);
    
private:
    string port;
    HWND window;
    int width;
    int height;

    void inputShell(const string& input);

    string runCommand(const string& command);
    void runCommandAsync(const string& command);
};