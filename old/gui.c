#define UNICODE
#include <windows.h>
#include <tchar.h>

#define MAX_WINDOWS 256

HWND hwndList[MAX_WINDOWS];
int windowCount = 0;

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    TCHAR title[256];
    if (GetWindowText(hwnd, title, sizeof(title) / sizeof(title[0])) && _tcslen(title) > 0) {
        if (IsWindowVisible(hwnd) && windowCount < MAX_WINDOWS) {
            hwndList[windowCount++] = hwnd;
        }
    }
    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HWND *checkboxes;
    static int count = 0;

    switch (msg) {
    case WM_CREATE: {
        // 全ウィンドウ列挙
        EnumWindows(EnumWindowsProc, 0);

        checkboxes = (HWND *)malloc(sizeof(HWND) * windowCount);
        count = windowCount;

        // スクロール可能に
        CreateWindowEx(0, L"STATIC", L"Select Windows:", WS_CHILD | WS_VISIBLE, 10, 10, 300, 20, hwnd, NULL, NULL, NULL);

        int y = 40;
        for (int i = 0; i < count; i++) {
            TCHAR title[256];
            GetWindowText(hwndList[i], title, 256);

            checkboxes[i] = CreateWindowEx(
                0, L"BUTTON", title,
                WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
                20, y, 400, 20,
                hwnd, (HMENU)(1000 + i), NULL, NULL
            );
            y += 25;
        }

        CreateWindowEx(0, L"BUTTON", L"OK", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            180, y + 10, 80, 25, hwnd, (HMENU)1, NULL, NULL);
        break;
    }

    case WM_COMMAND: {
        if (LOWORD(wParam) == 1) { // OKボタン
            for (int i = 0; i < count; i++) {
                LRESULT state = SendMessage(checkboxes[i], BM_GETCHECK, 0, 0);
                if (state == BST_CHECKED) {
                    TCHAR title[256];
                    GetWindowText(hwndList[i], title, 256);
                    wprintf(L"[✔] %s\n", title);
                }
            }
            MessageBox(hwnd, L"選択されたウィンドウをコンソールに出力しました。", L"完了", MB_OK);
        }
        break;
    }

    case WM_DESTROY:
        free(checkboxes);
        PostQuitMessage(0);
        break;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int APIENTRY wWinMain(HINSTANCE hInst, HINSTANCE hPrev, LPWSTR lpCmdLine, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"WindowListApp";

    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0, CLASS_NAME, L"Window List Viewer",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_VSCROLL,
        CW_USEDEFAULT, CW_USEDEFAULT, 500, 600,
        NULL, NULL, hInst, NULL
    );

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}
