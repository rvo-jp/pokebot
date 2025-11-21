#pragma once
#include <string>
#include <windows.h>
#include <stdexcept>
#include <vector>
#include <iostream>
#include <regex>
#include <gdiplus.h>
#include <thread>

#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;
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

    void startWatch();
    void endWatch();
    
private:
    string port;
    HWND window;
    int width;
    int height;

    thread watchWoaker = nullptr;
    bool watch = false;

    void inputShell(const string& input);
    string runCommand(const string& command);
    void runCommandAsync(const string& command);
    vector<unsigned char> execAdbScreencap();
    
    void watchLoop();
};