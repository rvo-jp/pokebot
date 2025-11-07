#include "pokebot.h"

#define ID_MAIN_EDIT 1001
#define ID_TEXT_AREA 1002
#define ID_LAUNCH_BTN 1003
#define ID_SCROLLAREA 1004
#define ID_ADD_BUTTON 1005
#define ID_DEL_BUTTON 1006
#define IDC_EDIT_BASE   2000


HWND hEditMain, hWidth, hHeight, hAdd, hDel, hSubName[16], hSubAddr[16], hButton;
int subCount = 0;

static void AddSub(HWND hwnd, char *name, char *addr) {
    char num[16];
    sprintf(num, "%d", subCount + 1);
    int h = 130 + subCount * 30;

    CreateWindow("STATIC", num,
        WS_VISIBLE | WS_CHILD,
        20, h, 20, 25,
        hwnd, NULL, NULL, NULL
    );
    hSubName[subCount] = CreateWindow(
        "EDIT", name,
        WS_VISIBLE | WS_CHILD | WS_BORDER,
        40, h, 100, 25,
        hwnd, (HMENU)ID_MAIN_EDIT, NULL, NULL
    );

    CreateWindow("STATIC", " 127.0.0.1:",
        WS_VISIBLE | WS_CHILD,
        140, h, 80, 25,
        hwnd, NULL, NULL, NULL
    );
    hSubAddr[subCount] = CreateWindow(
        "EDIT", addr,
        WS_VISIBLE | WS_CHILD | WS_BORDER,
        220, h, 50, 25,
        hwnd, (HMENU)ID_MAIN_EDIT, NULL, NULL
    );
    subCount ++;
}

static void CreateGUI(HWND hwnd) {
    char line[256];
    FILE *fp = fopen("config.txt", "r");
    if (!fp) {
        fp = fopen("config.txt", "w");
        fprintf(fp, "900:1600;MainWindow\nabc=127.0.0.1:5555\n");
        fclose(fp);
        fp = fopen("config.txt", "r");
    }

    char name[64], width[64], height[64];
    if (fgets(line, sizeof(line), fp)) {
        sscanf(line, "%63[^:]:%63[^;];%63s", width, height, name);
    }

    CreateWindow(
        "STATIC", "Mainwindow:", WS_VISIBLE | WS_CHILD,
        20, 20, 90, 25, hwnd, NULL, NULL, NULL
    );
    hEditMain = CreateWindow(
        "EDIT", name, WS_VISIBLE | WS_CHILD | WS_BORDER,
        110, 20, 160, 25, hwnd, (HMENU)ID_MAIN_EDIT, NULL, NULL
    );

    CreateWindow(
        "STATIC", "Resolution:",
        WS_VISIBLE | WS_CHILD,
        20, 50, 90, 25,
        hwnd, NULL, NULL, NULL
    );
    hWidth = CreateWindow(
        "EDIT", width,
        WS_VISIBLE | WS_CHILD | WS_BORDER,
        110, 50, 70, 25,
        hwnd, (HMENU)ID_MAIN_EDIT, NULL, NULL
    );
    CreateWindow(
        "STATIC", " x ",
        WS_VISIBLE | WS_CHILD,
        180, 50, 20, 25,
        hwnd, NULL, NULL, NULL
    );
    hHeight = CreateWindow(
        "EDIT", height,
        WS_VISIBLE | WS_CHILD | WS_BORDER,
        200, 50, 70, 25,
        hwnd, (HMENU)ID_MAIN_EDIT, NULL, NULL
    );

    CreateWindow("STATIC", "Subwindows:",
        WS_VISIBLE | WS_CHILD,
        20, 100, 190, 25,
        hwnd, NULL, NULL, NULL
    );
    hAdd = CreateWindow("BUTTON", "+",
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        210, 100, 30, 25, hwnd, (HMENU)ID_ADD_BUTTON, NULL, NULL
    );
    hDel = CreateWindow("BUTTON", "-",
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        240, 100, 30, 25, hwnd, (HMENU)ID_DEL_BUTTON, NULL, NULL
    );

    while (fgets(line, sizeof(line), fp)) {
        char name[64], addr[64];
        if (sscanf(line, "%63[^=]=%63s", name, addr) == 2 && subCount < 16) {
            AddSub(hwnd, name, addr);
        }
    }

    // Launchボタン
    hButton = CreateWindow(
        "BUTTON", "Launch",
        WS_VISIBLE | WS_CHILD | WS_BORDER,
        190, 140 + subCount * 30, 80, 30,
        hwnd, (HMENU)ID_LAUNCH_BTN, NULL, NULL
    );
    fclose(fp);

    SetWindowPos(hwnd, NULL, 0, 0, 300, 220 + subCount * 30, SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW);
}

// Pokebot thread
unsigned threadId;
HANDLE hThread = NULL;

static void LaunchBtn() {
    SetWindowText(hButton, "...");

    if (hThread) {
        // 起動済み
        PostThreadMessage(threadId, WM_QUIT, 0, 0);
        WaitForSingleObject(hThread, INFINITE);
        CloseHandle(hThread);

        hThread = NULL;
        SetWindowText(hButton, "Launch");
    }
    else {
        // 起動
        hThread = (HANDLE)_beginthreadex(NULL, 0, Pokebot, NULL, 0, &threadId);
        SetWindowText(hButton, "Stop");
    }
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: CreateGUI(hwnd);
        break;

        case WM_COMMAND:
        switch (LOWORD(wParam)) {
            case ID_LAUNCH_BTN: LaunchBtn();
            break;
            case ID_ADD_BUTTON:
            if (subCount < 16) {
                AddSub(hwnd, "", "");
            }
            break;
        }
        break;

        case WM_DESTROY:
        if (hThread) {
            PostThreadMessage(threadId, WM_QUIT, 0, 0);
            WaitForSingleObject(hThread, INFINITE);
            CloseHandle(hThread);
        }
        PostQuitMessage(0);
        break;

        default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc = { sizeof(WNDCLASS) };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    // wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = "MainWnd";
    // wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClass(&wc);

    HWND hwnd = CreateWindow(
        "MainWnd", "Pokebot",
        WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME,
        CW_USEDEFAULT, CW_USEDEFAULT, 300, 400,
        NULL, NULL, hInstance, NULL
    );
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
