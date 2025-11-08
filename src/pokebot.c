#include "pokebot.h"

char mainName[256];
char subAddress[16][16];
int subLen = 0;
int devW  = 0;
int devH = 0;

static HHOOK mouseHook, keyboardHook;
static HWND mainWindow;


// コマンド非同期実行
static void RunCommandAsync(const char* cmd) {
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));
    si.cb = sizeof(si);

    // コンソールを出さずに起動
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    BOOL success = CreateProcessA(
        NULL,               // 実行ファイル名（cmdで指定するのでNULL）
        (LPSTR)cmd,         // コマンドライン
        NULL,               // セキュリティ属性
        NULL,               // スレッド属性
        FALSE,              // ハンドル継承しない
        CREATE_NO_WINDOW,   // ウィンドウなし
        NULL,               // 環境変数なし
        NULL,               // カレントディレクトリ
        &si,
        &pi
    );

    if (success) {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    else {
        printf("CreateProcess failed: %lu\n", GetLastError());
    }
}

// 全デバイスに非同期タスク割り当て
static void SendInputForEachDevices(const char* input) {
    for (int i = 0; i < subLen; i ++) {
        char cmd[256];
        sprintf(cmd, ".\\platform-tools\\adb -s 127.0.0.1:%s shell input %s", subAddress[i], input);
        RunCommandAsync(cmd);
    }
}

static BOOL innerWindow = FALSE;
static POINT lastPoint;
static DWORD lastTime;

// クリックダウン
static void leftClickDown(const POINT point) {
    innerWindow = GetAncestor(WindowFromPoint(point), GA_ROOT) == mainWindow;

    if (innerWindow) {
        RECT rect;
        GetWindowRect(mainWindow, &rect);
        rect.top += 33; // 上の縁

        lastPoint.x = point.x - rect.left;
        lastPoint.y = point.y - rect.top;
        lastTime = GetTickCount();
    }
}

// クリックアップ
static void leftClickUp(const POINT point) {
    if (innerWindow) {
        innerWindow = FALSE;

        RECT rect;
        GetWindowRect(mainWindow, &rect);
        rect.top += 33; // 上の縁

        int winW = rect.right - rect.left;
        int winH = rect.bottom - rect.top;

        POINT newPoint;
        newPoint.x = point.x - rect.left;
        newPoint.y = point.y - rect.top;

        char buf[256];

        // タップ
        if (lastPoint.x == newPoint.x && lastPoint.y == newPoint.y) {
            int tapX = newPoint.x * devW  / winW;
            int tapY = newPoint.y * devH / winH;
            sprintf(buf, "tap %ld %ld", tapX, tapY);
        }

        // スワイプ
        else {
            DWORD newTime = GetTickCount();
            int startX = lastPoint.x * devW  / winW;
            int startY = lastPoint.y * devH / winH;
            int endX = newPoint.x * devW  / winW;
            int endY = newPoint.y * devH / winH;
            sprintf(buf, "swipe %ld %ld %ld %ld %ld", startX, startY, endX, endY, newTime - lastTime);
        }

        SendInputForEachDevices(buf);
    }
}

// マウス
static LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        PMSLLHOOKSTRUCT p = (PMSLLHOOKSTRUCT)lParam;

        if (wParam == WM_LBUTTONDOWN) leftClickDown(p->pt);
        else if (wParam == WM_LBUTTONUP) leftClickUp(p->pt);
    }

    return CallNextHookEx(mouseHook, nCode, wParam, lParam);
}

// US配列前提
static int vk_to_android(int vk) {
    switch (vk) {
        case 'A': return 29;
        case 'B': return 30;
        case 'C': return 31;
        case 'D': return 32;
        case 'E': return 33;
        case 'F': return 34;
        case 'G': return 35;
        case 'H': return 36;
        case 'I': return 37;
        case 'J': return 38;
        case 'K': return 39;
        case 'L': return 40;
        case 'M': return 41;
        case 'N': return 42;
        case 'O': return 43;
        case 'P': return 44;
        case 'Q': return 45;
        case 'R': return 46;
        case 'S': return 47;
        case 'T': return 48;
        case 'U': return 49;
        case 'V': return 50;
        case 'W': return 51;
        case 'X': return 52;
        case 'Y': return 53;
        case 'Z': return 54;

        // 数字キー
        case '0': return 7;
        case '1': return 8;
        case '2': return 9;
        case '3': return 10;
        case '4': return 11;
        case '5': return 12;
        case '6': return 13;
        case '7': return 14;
        case '8': return 15;
        case '9': return 16;

        // 記号
        case VK_OEM_MINUS:    return 69;  // '-'
        case VK_OEM_PLUS:     return 70;  // '='
        case VK_OEM_4:        return 71;  // '['
        case VK_OEM_6:        return 72;  // ']'
        case VK_OEM_5:        return 73;  // '\'
        case VK_OEM_1:        return 74;  // ';'
        case VK_OEM_7:        return 75;  // '''
        case VK_OEM_COMMA:    return 55;  // ','
        case VK_OEM_PERIOD:   return 56;  // '.'
        case VK_OEM_2:        return 76;  // '/`?'
        case VK_OEM_3:        return 68;  // '`'

        // 特殊キー
        case VK_SPACE:        return 62;
        case VK_RETURN:       return 66;
        case VK_BACK:         return 67;
        case VK_TAB:          return 61;
        case VK_ESCAPE:       return 111;
        case VK_DELETE:       return 112;
        case VK_LEFT:         return 21;
        case VK_UP:           return 19;
        case VK_RIGHT:        return 22;
        case VK_DOWN:         return 20;
        case VK_HOME:         return 3;
        case VK_END:          return 123;
        case VK_PRIOR:        return 92;  // Page Up
        case VK_NEXT:         return 93;  // Page Down
        case VK_CAPITAL:      return 115; // Caps Lock
        case VK_SHIFT:        return 59;
        case VK_CONTROL:      return 113;
        case VK_MENU:         return 57;  // Alt
        case VK_LWIN:         return 3;   // Homeボタン扱い

        // ファンクションキー
        case VK_F1: return 131;
        case VK_F2: return 132;
        case VK_F3: return 133;
        case VK_F4: return 134;
        case VK_F5: return 135;
        case VK_F6: return 136;
        case VK_F7: return 137;
        case VK_F8: return 138;
        case VK_F9: return 139;
        case VK_F10: return 140;
        case VK_F11: return 141;
        case VK_F12: return 142;

        default: return -1;
    }
}

// マウス
static LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && GetForegroundWindow() == mainWindow) {
        KBDLLHOOKSTRUCT* kbd = (KBDLLHOOKSTRUCT*)lParam;

        if (wParam == WM_KEYDOWN) {
            char buf[256];
            sprintf(buf, "keyevent %d", vk_to_android(kbd->vkCode));
            SendInputForEachDevices(buf);
        }
    }

    return CallNextHookEx(keyboardHook, nCode, wParam, lParam);
}

DWORD WINAPI Pokebot(LPVOID lpParam) {
    mainWindow = FindWindowA(NULL, mainName);
    if (!mainWindow) {
        printf("Error: %s not found\n", mainName);
        return 1;
    }

    for (int i = 0; i < subLen; i ++) {
        char cmd[256];
        sprintf(cmd, ".\\platform-tools\\adb connect 127.0.0.1:%s", subAddress[i]);
        RunCommandAsync(cmd);
    }

    mouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, NULL, 0);
    keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);
    
    if (!mouseHook || !keyboardHook) {
        printf("SetWindowsHookEx failed: %lu\n", GetLastError());
        return 1;
    }

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(mouseHook);
    UnhookWindowsHookEx(keyboardHook);
    RunCommandAsync(".\\platform-tools\\adb disconnect");
    return 0;
}
