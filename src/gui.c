#include "pokebot.h"

#define ID_MAIN_EDIT 1001
#define ID_TEXT_AREA 1002
#define ID_LAUNCH_BTN 1003
#define ID_SCROLLAREA 1004
#define ID_ADD_BUTTON 1005
#define ID_DEL_BUTTON 1006
#define IDC_EDIT_BASE   2000


static HWND hEditMain, hWidth, hHeight, hAdd, hDel, hButton;

typedef struct {
    HWND use;
    HWND name;
    HWND addr;
    HWND text;
} Subwindow;

static Subwindow hSub[64];
static int subCount = 0;

// Pokebot thread
static DWORD threadId;
static HANDLE hThread = NULL;


static void AddSub(HWND hwnd, BOOL use, char *name, char *addr) {
    int h = 130 + subCount * 30;

    hSub[subCount].use = CreateWindow(
        "BUTTON", "",
        WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
        20, h, 20, 25,
        hwnd, (HMENU)IDC_EDIT_BASE, NULL, NULL
    );
    if (use) SendMessage(hSub[subCount].use, BM_SETCHECK, BST_CHECKED, 0);

    hSub[subCount].name = CreateWindow(
        "EDIT", name,
        WS_VISIBLE | WS_CHILD | WS_BORDER,
        40, h, 100, 25,
        hwnd, (HMENU)ID_MAIN_EDIT, NULL, NULL
    );

    hSub[subCount].text = CreateWindow("STATIC", " 127.0.0.1:",
        WS_VISIBLE | WS_CHILD,
        140, h, 80, 25,
        hwnd, NULL, NULL, NULL
    );
    hSub[subCount].addr = CreateWindow(
        "EDIT", addr,
        WS_VISIBLE | WS_CHILD | WS_BORDER,
        220, h, 50, 25,
        hwnd, (HMENU)ID_MAIN_EDIT, NULL, NULL
    );
    subCount ++;
}

static void RemoveSub(HWND hwnd) {
    subCount--;
    DestroyWindow(hSub[subCount].use);
    DestroyWindow(hSub[subCount].name);
    DestroyWindow(hSub[subCount].text);
    DestroyWindow(hSub[subCount].addr);
}

static void UpdatePos(HWND hwnd) {
    SetWindowPos(hButton, NULL, 190, 140 + subCount * 30, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    SetWindowPos(hwnd, NULL, 0, 0, 300, 220 + subCount * 30, SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW);
    RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN | RDW_UPDATENOW);
}

static void CreateGUI(HWND hwnd) {
    char line[256];
    FILE *fp = fopen("config.txt", "r");
    if (!fp) {
        fp = fopen("config.txt", "w");
        fprintf(fp, "BlueStacks App Player;1080:1920;\nBlueStacks App Player 1=5555\n");
        fclose(fp);
        fp = fopen("config.txt", "r");
    }

    char name[64], width[64], height[64];
    if (fgets(line, sizeof(line), fp)) {
        sscanf(line, "%63[^;];%63[^x]x%63s", name, width, height);
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
        if (subCount < 16) {
            BOOL use;
            char name[64] = "", addr[64] = "";
            sscanf(line, "%d;%63[^;];%63s", &use, name, addr);
            AddSub(hwnd, use, name, addr);
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

static void LaunchPokebot() {
    GetWindowTextA(hEditMain, mainName, sizeof(mainName));

    char buf[256];
    GetWindowTextA(hWidth, buf, sizeof(buf));
    devW = atoi(buf);
    GetWindowTextA(hHeight, buf, sizeof(buf));
    devH = atoi(buf);

    subLen = 0;
    for (int i = 0; i < subCount; i ++) {
        Subwindow sub = hSub[i];
        if (SendMessage(sub.use, BM_GETCHECK, 0, 0) == BST_CHECKED) {
            GetWindowTextA(sub.addr, subAddress[subLen ++], 16);
        }
    }

    hThread = CreateThread(NULL, 0, Pokebot, NULL, 0, &threadId);
    SetWindowText(hButton, "Stop");
}

static void StopPokebot() {
    PostThreadMessage(threadId, WM_QUIT, 0, 0);
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);

    hThread = NULL;
    SetWindowText(hButton, "Launch");
}

static void Destroy() {
    if (hThread) StopPokebot();
    
    // 設定保存
    FILE *fp = fopen("config.txt", "w");
    char buf[256];

    GetWindowTextA(hEditMain, buf, sizeof(buf));
    fprintf(fp, "%s;", buf);
    GetWindowTextA(hWidth, buf, sizeof(buf));
    fprintf(fp, "%sx", buf);
    GetWindowTextA(hHeight, buf, sizeof(buf));
    fprintf(fp, "%s", buf);
    

    for (int i = 0; i < subCount; i ++) {
        Subwindow sub = hSub[i];
        fprintf(fp, "\n%c;", SendMessage(sub.use, BM_GETCHECK, 0, 0) == BST_CHECKED ? '1' : '0');
        GetWindowTextA(sub.name, buf, sizeof(buf));
        fprintf(fp, "%s;", buf);
        GetWindowTextA(sub.addr, buf, sizeof(buf));
        fprintf(fp, "%s", buf);
    }
    fclose(fp);

    PostQuitMessage(0);
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: CreateGUI(hwnd);
        break;

        case WM_COMMAND:
        switch (LOWORD(wParam)) {
            case ID_LAUNCH_BTN: 
            if (hThread) StopPokebot();
            else LaunchPokebot();
            break;
            case ID_ADD_BUTTON:
            if (subCount < 16) {
                AddSub(hwnd, FALSE, "", "");
                UpdatePos(hwnd);
            }
            break;
            case ID_DEL_BUTTON:
            if (subCount > 0) {
                RemoveSub(hwnd);
                UpdatePos(hwnd);
            }
            break;
        }
        break;

        case WM_ERASEBKGND: {
            RECT rc;
            GetClientRect(hwnd, &rc);
            FillRect((HDC)wParam, &rc, (HBRUSH)(COLOR_WINDOW + 1));
            return 1; // 既に塗ったので既定処理しない
        }

        case WM_DESTROY: Destroy();
        break;
        
        default: break;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
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
        "MainWnd", "Pokebot v1.2.1",
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
