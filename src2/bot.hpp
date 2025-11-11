#pragma once
#include <string>
#include <windows.h>

using namespace std;

class Bot {
public:
    bool isInside(const POINT point);
    bool isForground();
    POINT getRelativePos(const POINT point, int devW, int devH);
    
    void tap(const POINT pos);
    void swipe(const POINT startPos, const POINT endPos, DWORD time);
    void keyevent(const DWORD key);
    
private:
    string port;
    HWND window;

    bool getWindow(const string label);
    void inputShell(const string input);
    void runCommandAsync(const string command);
};