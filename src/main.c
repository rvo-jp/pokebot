#include <windows.h>
#include <stdio.h>
#include <process.h>

// #include "pokebot.h"
BOOL running = TRUE;
char mainName[256] = "s1";
char subAddress[16][16] = {
    "127.0.0.1:5565",
    "127.0.0.1:5635",
    "127.0.0.1:5575"
};
int subLen = 3;

HHOOK mouseHook, keyboardHook;
HWND mainWindow;
POINT lpt = {.x = 0, .y = 0};
DWORD ltm = 0;

void __cdecl RunSystemAsync(void* arg) {
    const char* cmd = (const char*)arg;
    system(cmd);
    _endthread(); // スレッド終了
}

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        PMSLLHOOKSTRUCT p = (PMSLLHOOKSTRUCT)lParam;

        HWND hClicked = WindowFromPoint(p->pt);
        HWND hRoot = GetAncestor(hClicked, GA_ROOT);

        if (hRoot == mainWindow) {
            POINT pt = p->pt;

            RECT rect;
            GetWindowRect(mainWindow, &rect);
            rect.top += 33;
            int winW = rect.right - rect.left;
            int winH = rect.bottom - rect.top;
            int devW  = 900;
            int devH = 1600;
            
            pt.x -= rect.left;
            pt.y -= rect.top;

            if (wParam == WM_LBUTTONDOWN) {
                lpt = pt;
                ltm = GetTickCount();
                printf("Down: (%ld, %ld)\n", pt.x, pt.y);
            }
            else if (wParam == WM_LBUTTONUP) {
                DWORD tm = GetTickCount();
                printf("Up: (%ld, %ld) ", pt.x, pt.y);

                if (lpt.x == pt.x && lpt.y == pt.y) {
                    printf("tap\n");
                    int tapX = (double)pt.x * devW  / winW;
                    int tapY = (double)pt.y * devH / winH;

                    for (int i = 0; i < subLen; i ++) {
                        char cmd[256];
                        sprintf(cmd, ".\\platform-tools\\adb -s %s shell input tap %ld %ld", subAddress[i], tapX, tapY);
                        _beginthread(RunSystemAsync, 0, cmd);
                    }
                }
                else {
                    printf("swipe\n");
                    int startX = (double)lpt.x * devW  / winW;
                    int startY = (double)lpt.y * devH / winH;
                    int endX = (double)pt.x * devW  / winW;
                    int endY = (double)pt.y * devH / winH;

                    for (int i = 0; i < subLen; i ++) {
                        char cmd[256];
                        sprintf(cmd, ".\\platform-tools\\adb -s %s shell input swipe %ld %ld %ld %ld %ld", subAddress[i], startX, startY, endX, endY, tm - ltm);
                        _beginthread(RunSystemAsync, 0, cmd);
                    }
                }
            }
        }
    }

    return CallNextHookEx(mouseHook, nCode, wParam, lParam);
}

// US配列前提
int vk_to_android(int vk) {
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

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && GetForegroundWindow() == mainWindow) {
        KBDLLHOOKSTRUCT* kbd = (KBDLLHOOKSTRUCT*)lParam;

        if (wParam == WM_KEYDOWN) {
            printf("Key: %c\n", kbd->vkCode);
            for (int i = 0; i < subLen; i ++) {
                char cmd[256];
                sprintf(cmd, ".\\platform-tools\\adb -s %s shell input keyevent %d", subAddress[i], vk_to_android(kbd->vkCode));
                _beginthread(RunSystemAsync, 0, cmd);
            }
        }
    }
    return CallNextHookEx(keyboardHook, nCode, wParam, lParam);
}


int main() {
    mainWindow = FindWindowA(NULL, mainName);
    if (!mainWindow) {
        printf("Error: %s not found\n", mainName);
        return 1;
    }

    for (int i = 0; i < subLen; i ++) {
        char cmd[256];
        sprintf(cmd, ".\\platform-tools\\adb connect %s", subAddress[i]);
        system(cmd);
    }
    system(".\\platform-tools\\adb devices");

    mouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, NULL, 0);
    keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);
    
    if (!mouseHook || !keyboardHook) {
        printf("SetWindowsHookEx failed: %lu\n", GetLastError());
        return 1;
    }

    MSG msg;
    while (running && GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    system(".\\platform-tools\\adb disconnect");
    UnhookWindowsHookEx(mouseHook);
    UnhookWindowsHookEx(keyboardHook);
    return 0;
}
