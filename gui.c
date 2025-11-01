#include <windows.h>
#include <stdio.h>

#define BTN_SUBMIT 1001

HWND hEdit[4];
HWND hButton;
HWND hOutput;

// 入力内容を取得して表示
void onButtonClicked(HWND hwnd) {
    char text[4][256];
    char buffer[1024] = "";

    for (int i = 0; i < 4; i++) {
        GetWindowTextA(hEdit[i], text[i], sizeof(text[i]));
        char line[300];
        sprintf(line, "Input %d: %s\n", i + 1, text[i]);
        strcat(buffer, line);
    }

    SetWindowTextA(hOutput, buffer);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            for (int i = 0; i < 4; i++) {
                hEdit[i] = CreateWindowExA(
                    WS_EX_CLIENTEDGE, "EDIT", "",
                    WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
                    20, 20 + i * 30, 200, 25,
                    hwnd, (HMENU)(100 + i), GetModuleHandle(NULL), NULL
                );
            }

            hButton = CreateWindowA(
                "BUTTON", "Submit",
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                240, 20, 100, 120,
                hwnd, (HMENU)BTN_SUBMIT, GetModuleHandle(NULL), NULL
            );

            hOutput = CreateWindowExA(
                WS_EX_CLIENTEDGE, "EDIT", "",
                WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
                20, 160, 320, 120,
                hwnd, NULL, GetModuleHandle(NULL), NULL
            );
            break;
        }

        case WM_COMMAND:
            if (LOWORD(wParam) == BTN_SUBMIT) {
                onButtonClicked(hwnd);
            }
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSA wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "InputExample";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClassA(&wc);

    HWND hwnd = CreateWindowA(
        "InputExample", "4 Inputs + Button",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, 380, 350,
        NULL, NULL, hInstance, NULL
    );

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
