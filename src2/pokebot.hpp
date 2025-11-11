#pragma once
#include <windows.h>
#include <thread>
#include <vector>
#include "bot.hpp"

using namespace std;

class Pokebot {
public:
    // シングルトンインスタンス取得
    static Pokebot& getInstance();

private:
    Pokebot();
    ~Pokebot();

    // コピー禁止
    Pokebot(const Pokebot&) = delete;
    Pokebot& operator=(const Pokebot&) = delete;

    thread worker;
    HHOOK mouseHook;
    HHOOK keyboardHook;
    int devW;
    int devH;

    vector<Bot> bots;
    Bot* lastBot;
    POINT lastPos;
    DWORD lastTime;

    void hookThread();
    static LRESULT CALLBACK mouseProc(int nCode, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK keyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
};
