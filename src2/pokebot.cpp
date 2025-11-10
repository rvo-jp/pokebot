#include <thread>
#include <vector>
#include "bot.hpp"

using namespace std;

int devW, devH;

static HHOOK mouseHook, keyboardHook;
static thread hThread;

static vector<Bot> bots;
static bool isLastClickInside = false;
static Bot *lastBot;
static POINT lastPoint;
static DWORD lastTime;

// クリックダウン
static void leftClickDown(const POINT point) {

    isLastClickInside = false;
    for (auto& bot : bots) {
        if (bot.isInside(point)) {
            isLastClickInside = true;
            lastBot = &bot;
            lastPoint = bot.getRelativePos(point, devW, devH);
            lastTime = GetTickCount();
            break;
        }
    }
}

// マウス
static LRESULT CALLBACK mouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        PMSLLHOOKSTRUCT p = (PMSLLHOOKSTRUCT)lParam;

        if (wParam == WM_LBUTTONDOWN) leftClickDown(p->pt);
        else if (wParam == WM_LBUTTONUP) leftClickUp(p->pt);
    }

    return CallNextHookEx(mouseHook, nCode, wParam, lParam);
}

static void hookThread() {
    mouseHook = SetWindowsHookEx(WH_MOUSE_LL, mouseProc, NULL, 0);
    keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);
    
    if (!mouseHook || !keyboardHook) {
        printf("SetWindowsHookEx failed: %lu\n", GetLastError());
    }

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(mouseHook);
    UnhookWindowsHookEx(keyboardHook);
}

void startPokebot() {
    hThread = thread(hookThread);
}

void endPokebot() {
    DWORD threadId = GetThreadId(hThread.native_handle());
    PostThreadMessage(threadId, WM_QUIT, 0, 0);

    if (hThread.joinable()) {
        hThread.join();
    }
}
