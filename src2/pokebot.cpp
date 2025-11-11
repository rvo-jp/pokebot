#include "pokebot.hpp"



// シングルインスタンス
Pokebot& Pokebot::getInstance() {
    static Pokebot instance;
    return instance;
}

// 初回生成時に自動起動
Pokebot::Pokebot() {
    worker = thread(&Pokebot::hookThread, this);
}

// 解放時に自動終了
Pokebot::~Pokebot() {
    DWORD threadId = GetThreadId(worker.native_handle());
    PostThreadMessage(threadId, WM_QUIT, 0, 0);
    if (worker.joinable()) {
        worker.join();
    }
}

// スレッド
void Pokebot::hookThread() {
    mouseHook = SetWindowsHookEx(WH_MOUSE_LL, mouseProc, NULL, 0);
    keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, keyboardProc, NULL, 0);
    
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

// マウスプロシージャ
LRESULT CALLBACK Pokebot::mouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    Pokebot& obj = Pokebot::getInstance();

    if (nCode == HC_ACTION) {
        PMSLLHOOKSTRUCT p = (PMSLLHOOKSTRUCT)lParam;

        if (wParam == WM_LBUTTONDOWN) { // クリックダウン
            obj.lastBot = nullptr;
            
            for (auto& bot : obj.bots) {
                if (bot.isInside(p->pt)) {
                    obj.lastBot = const_cast<Bot*>(&bot);
                    obj.lastPos = bot.getRelativePos(p->pt, obj.devW, obj.devH);
                    obj.lastTime = GetTickCount();
                    break;
                }
            }
        }
        else if (wParam == WM_LBUTTONUP) { //　クリックアップ
            if (obj.lastBot == nullptr) return;
            
            POINT pos = obj.lastBot->getRelativePos(p->pt, obj.devW, obj.devH);
            DWORD newTime = GetTickCount();
            
            for (auto& bot : obj.bots) {
                if (&bot == obj.lastBot) continue;
            
                if (obj.lastPos.x == pos.x && obj.lastPos.y == pos.y) {
                    bot.tap(pos);
                }
                else {
                    bot.swipe(obj.lastPos, pos, newTime - obj.lastTime);
                }
            }
        }
    }

    return CallNextHookEx(obj.mouseHook, nCode, wParam, lParam);
}

// キーボードプロシージャ
LRESULT CALLBACK Pokebot::keyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    Pokebot& obj = Pokebot::getInstance();
    
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* kbd = (KBDLLHOOKSTRUCT*)lParam;

        if (wParam == WM_KEYDOWN) {
            for (auto& bot : obj.bots) {
                if (bot.isForground()) continue;
                bot.keyevent(kbd->vkCode);
            }
        }
    }

    return CallNextHookEx(obj.keyboardHook, nCode, wParam, lParam);
}

