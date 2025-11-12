#pragma once
#include <windows.h>
#include <vector>
#include "bot.hpp"

using namespace std;

class Pokebot {
public:
    static void connect(const string& label, const string& port);
    static void disconnect(const string& port);

private:
    Pokebot();
    ~Pokebot();
    
    // シングルトンインスタンス取得
    static Pokebot& getInstance();

    // コピー禁止
    Pokebot(const Pokebot&) = delete;
    Pokebot& operator=(const Pokebot&) = delete;

    HANDLE hThread;
    HHOOK mouseHook;
    HHOOK keyboardHook;
    int devW = 540;
    int devH = 960;

    vector<Bot> bots;
    Bot* lastBot;
    POINT lastPos;
    DWORD lastTime;

    static DWORD WINAPI hookThread(LPVOID lpParam);
    static LRESULT CALLBACK mouseProc(int nCode, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK keyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
};
