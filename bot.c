#include "pokebot.h"

HWND getWindow(char *name) {
    HWND hwnd = FindWindow(NULL, name);
    if (!hwnd) {
        printf("Error: window '%s' is not found\n", name);
        exit(1);
    }
    return hwnd;
}

void leftdown(POINT pos) {
    INPUT input[2] = {
        {
            .type = INPUT_MOUSE,
            .mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE,
            .mi.dx = (pos.x * 65535) / GetSystemMetrics(SM_CXSCREEN),
            .mi.dy = (pos.y * 65535) / GetSystemMetrics(SM_CYSCREEN),
            .mi.dwExtraInfo = 0x12345678
        },
        {.type = INPUT_MOUSE, .mi.dwFlags = MOUSEEVENTF_LEFTDOWN, .mi.dwExtraInfo = 0x12345678}
    };
    SendInput(2, input, sizeof(INPUT));
}

void leftup(POINT pos) {
    INPUT input[2] = {
        {
            .type = INPUT_MOUSE,
            .mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE,
            .mi.dx = (pos.x * 65535) / GetSystemMetrics(SM_CXSCREEN),
            .mi.dy = (pos.y * 65535) / GetSystemMetrics(SM_CYSCREEN),
            .mi.dwExtraInfo = 0x12345678
        },
        {.type = INPUT_MOUSE, .mi.dwFlags = MOUSEEVENTF_LEFTUP, .mi.dwExtraInfo = 0x12345678}
    };
    SendInput(2, input, sizeof(INPUT));
}

void leftclick(POINT pos) {
    INPUT input[3] = {
        {
            .type = INPUT_MOUSE,
            .mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE,
            .mi.dx = (pos.x * 65535) / GetSystemMetrics(SM_CXSCREEN),
            .mi.dy = (pos.y * 65535) / GetSystemMetrics(SM_CYSCREEN),
            .mi.dwExtraInfo = 0x12345678
        },
        {.type = INPUT_MOUSE, .mi.dwFlags = MOUSEEVENTF_LEFTDOWN, .mi.dwExtraInfo = 0x12345678},
        {.type = INPUT_MOUSE, .mi.dwFlags = MOUSEEVENTF_LEFTUP, .mi.dwExtraInfo = 0x12345678}
    };
    SendInput(3, input, sizeof(INPUT));
}

void move(POINT pos) {
    INPUT input[1] = {
        {
            .type = INPUT_MOUSE,
            .mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE,
            .mi.dx = (pos.x * 65535) / GetSystemMetrics(SM_CXSCREEN),
            .mi.dy = (pos.y * 65535) / GetSystemMetrics(SM_CYSCREEN),
            .mi.dwExtraInfo = 0x12345678
        }
    };
    SendInput(1, input, sizeof(INPUT));
}

POINT POS;

bool LEFTDOWN = false;
bool RIGHTDOWN = false;
bool MIDDLEDOWN = false;

// マウスフックコールバック
LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        MSLLHOOKSTRUCT *p = (MSLLHOOKSTRUCT *)lParam;

        if (p->dwExtraInfo == 0x12345678) return CallNextHookEx(NULL, nCode, wParam, lParam);

        POS = p->pt;

        if (wParam == WM_LBUTTONUP && !LEFTDOWN) {
            LEFTDOWN = true;
        }
        if (wParam == WM_RBUTTONUP && !RIGHTDOWN) {
            RIGHTDOWN = true;
        }
        if (wParam == WM_MBUTTONUP && !MIDDLEDOWN) {
            MIDDLEDOWN = true;
        }
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

int main() {
    HWND windows[] = {
        getWindow("bot0"),
        getWindow("bot1"),
        getWindow("bot2"),
        getWindow("bot3"),
    };

    // HWND windows[] = {
    //     getWindow("ru"),
    //     getWindow("li"),
    //     getWindow("s1"),
    //     getWindow("s2"),
    // };

    HHOOK mouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, NULL, 0);

    bool resume = true;

    MSG msg;
    while (1) {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (LEFTDOWN) {
            if (resume) {
                LONG X, Y, width, height;

                RECT rect;
                if (GetWindowRect(windows[0], &rect)) {
                    X = POS.x - rect.left;
                    Y = POS.y - rect.top;
                    width  = rect.right - rect.left;
                    height = rect.bottom - rect.top;
                }

                for (int i = 1; i < 4; i ++) {
                    HWND bot = windows[i];
                    if (GetWindowRect(bot, &rect)) {
                        if (width != rect.right - rect.left || height != rect.bottom - rect.top) {
                            SetWindowPos(bot, HWND_TOPMOST, 0, 0, width, height, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSENDCHANGING);
                        }
                        else {
                            SetWindowPos(bot, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOSENDCHANGING);
                        }
                        leftclick((POINT){.x = rect.left + X, .y = rect.top + Y});
                        SetWindowPos(bot, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOSENDCHANGING);
                    }
                }
                move((POINT){.x = POS.x, .y = POS.y});
            }
            LEFTDOWN = false;
        }
        
        if (RIGHTDOWN) {
            resume = !resume;
            if (resume) {
                puts("Resume");
            }
            else {
                puts("Pause");
            }
            RIGHTDOWN = false;
        }
        
        if (MIDDLEDOWN) {
            if (resume) {
                for (int i = 0; i < 4; i ++) {
                    HWND bot = windows[i];
                    RECT rect;
                    if (GetWindowRect(bot, &rect)) {
                        LONG width  = rect.right - rect.left;
                        LONG height = rect.bottom - rect.top;

                        SetWindowPos(bot, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOSENDCHANGING);

                        leftdown((POINT){.x = rect.left + width * 0.1, .y = rect.top + height * 0.6});
                        Sleep(40);
                        move((POINT){.x = rect.left + width * 0.2, .y = rect.top + height * 0.6});
                        Sleep(40);
                        move((POINT){.x = rect.left + width * 0.3, .y = rect.top + height * 0.6});
                        Sleep(40);
                        move((POINT){.x = rect.left + width * 0.4, .y = rect.top + height * 0.6});
                        Sleep(40);
                        move((POINT){.x = rect.left + width * 0.5, .y = rect.top + height * 0.6});
                        Sleep(40);
                        move((POINT){.x = rect.left + width * 0.6, .y = rect.top + height * 0.6});
                        Sleep(40);
                        move((POINT){.x = rect.left + width * 0.7, .y = rect.top + height * 0.6});
                        Sleep(40);
                        move((POINT){.x = rect.left + width * 0.8, .y = rect.top + height * 0.6});
                        Sleep(40);
                        leftup((POINT){.x = rect.left + width * 0.9, .y = rect.top + height * 0.6});

                        SetWindowPos(bot, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOSENDCHANGING);
                    }
                }

                move((POINT){.x = POS.x, .y = POS.y});
            }
            MIDDLEDOWN = false;
        }
    }


    UnhookWindowsHookEx(mouseHook);
    return 0;
}
