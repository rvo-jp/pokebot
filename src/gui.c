#include <windows.h>
#include "pokebot.h"

HWND hEditMain, hEditText, hButton;
#define ID_MAIN_EDIT 1001
#define ID_TEXT_AREA 1002
#define ID_LAUNCH_BTN 1003

BOOL running = FALSE;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE:
            CreateWindow("STATIC", "Main Window:", WS_VISIBLE | WS_CHILD, 20, 20, 100, 25, hwnd, NULL, NULL, NULL);
            hEditMain = CreateWindow("EDIT", "Name", WS_VISIBLE | WS_CHILD | WS_BORDER, 120, 20, 160, 25, hwnd, (HMENU)ID_MAIN_EDIT, NULL, NULL); 

            CreateWindow("STATIC", "Sub Windows Address:", WS_VISIBLE | WS_CHILD, 20, 55, 160, 25, hwnd, NULL, NULL, NULL);
            hEditText = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"120.0.0.1:55555\r\n",
            WS_CHILD | WS_VISIBLE | WS_BORDER |
            ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | WS_VSCROLL,
            20, 80, 260, 100, hwnd, (HMENU)ID_TEXT_AREA, NULL, NULL);

            hButton = CreateWindow("BUTTON", "Launch", WS_VISIBLE | WS_CHILD, 200, 195, 80, 30, hwnd, (HMENU)ID_LAUNCH_BTN, NULL, NULL);
            break;

        case WM_COMMAND:
            if (LOWORD(wParam) == ID_LAUNCH_BTN) {
                if (!running) {
                    GetWindowText(hEditMain, mainName, sizeof(mainName));
                    char Buf[1024];
                    GetWindowText(hEditText, Buf, sizeof(Buf));
                    SetWindowText(hButton, "Stop");
                    hThread = CreateThread(NULL, 0, pokebot, NULL, 0, NULL);
                    running = TRUE;
                }
                else {
                    PostThreadMessage(GetThreadId(hThread), WM_QUIT, 0, 0);
                    WaitForSingleObject(hThread, 1000);
                    CloseHandle(hThread);
                    hThread = NULL;
                    SetWindowText(hButton, "Launch");
                    running = FALSE;
                }
            }
            break;

        case WM_DESTROY:
            running = FALSE;
            if (hThread) {
                PostThreadMessage(GetThreadId(hThread), WM_QUIT, 0, 0);
                WaitForSingleObject(hThread, 1000);
                CloseHandle(hThread);
            }
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow) {
    const char CLASS_NAME[] = "PokebotWindow";

    WNDCLASSEX wc = { sizeof(WNDCLASSEX) };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClassEx(&wc);

    HWND hwnd = CreateWindowEx(
        0, CLASS_NAME, "Pokebot",
        WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME,
        CW_USEDEFAULT, CW_USEDEFAULT, 310, 270,
        NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
