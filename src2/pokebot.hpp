#pragma once
#include <windows.h>
#include <vector>
#include "bot.hpp"

using namespace std;

class Pokebot {
public:
    static Bot *connect(const string& label, const string& port);
    static void disconnect(Bot* botPtr);

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

    vector<unique_ptr<Bot>> bots;
    Bot* lastBotPtr;
    POINT lastPos;
    DWORD lastTime;

    bool pause = false;

    static DWORD WINAPI hookThread(LPVOID lpParam);
    static LRESULT CALLBACK mouseProc(int nCode, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK keyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
};
