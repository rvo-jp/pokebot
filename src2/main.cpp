#include "pokebot.hpp"

#include <windows.h>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <fstream>

HINSTANCE hInst;
HWND hwndMain;
HWND hwndScroll;

const int scrollAreaHeight = 300;
const int elementHeight = 30;
int scrollPos = 0;

// + / - ボタンID
const int ID_BTN_ADD = 1001;
const int ID_BTN_REMOVE = 1002;

// Element 構造体
struct Element {
    HWND hEdit;
    HWND hButton;
    int y;
    int id;
};

vector<Element> elements;
int nextElementId = 2000;

// --- 関数 ---
void UpdateScrollArea(HWND hwnd) {
    for (auto& el : elements) {
        MoveWindow(el.hEdit, 10, el.y - scrollPos, 200, 25, TRUE);
        MoveWindow(el.hButton, 220, el.y - scrollPos, 60, 25, TRUE);
    }

    int scrollMax = max(0, (int)elements.size() * elementHeight - scrollAreaHeight);
    SetScrollRange(hwnd, SB_VERT, 0, scrollMax, TRUE);
    SetScrollPos(hwnd, SB_VERT, scrollPos, TRUE);
}

void AddElement(HWND hwnd, const std::string& label, const std::string& port) {
    int y = elements.size() * elementHeight;
    int id = nextElementId++;

    HWND hEdit = CreateWindow("EDIT", label.c_str(),
        WS_CHILD | WS_VISIBLE | ES_LEFT | WS_BORDER,
        0, y, 200, 25,
        hwnd, NULL, hInst, NULL);

    HWND hBtn = CreateWindow("BUTTON", "CN",
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        220, y, 60, 25,
        hwnd, (HMENU)MAKEINTRESOURCE(id), hInst, NULL);

    elements.push_back({ hEdit, hBtn, y, id });
    UpdateScrollArea(hwnd);
}

void RemoveElement() {
    if (elements.empty()) return;
    auto el = elements.back();
    DestroyWindow(el.hEdit);
    DestroyWindow(el.hButton);
    elements.pop_back();
    UpdateScrollArea(hwndScroll);
}

// --- ScrollArea ウィンドウプロシージャ ---
LRESULT CALLBACK ScrollWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE: {
        ifstream file("config.txt");
        if (!file.is_open()) {
            throw runtime_error("Failed to open config.txt");
        }

        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;

            stringstream ss(line);
            string label, port;

            if (getline(ss, label, ':') && getline(ss, port)) {
                AddElement(hwnd, label, port);
            }
            else {
                throw runtime_error("Invalid line format: " + line);
            }
        }
        break;
    }

    case WM_COMMAND: {
        int id = LOWORD(wParam);
        for (auto& el : elements) {
            if (el.id == id) {
                char buf[128];
                GetWindowTextA(el.hEdit, buf, sizeof(buf));
                MessageBoxA(hwnd, buf, "Element Button", MB_OK);
                break;
            }
        }
        break;
    }

    case WM_VSCROLL: {
        int action = LOWORD(wParam);
        int pos = HIWORD(wParam);
        int maxScroll = max(0, (int)elements.size() * elementHeight - scrollAreaHeight);
        switch (action) {
        case SB_LINEUP:    scrollPos -= 20; break;
        case SB_LINEDOWN:  scrollPos += 20; break;
        case SB_PAGEUP:    scrollPos -= 100; break;
        case SB_PAGEDOWN:  scrollPos += 100; break;
        case SB_THUMBTRACK: scrollPos = pos; break;
        }
        if (scrollPos < 0) scrollPos = 0;
        if (scrollPos > maxScroll) scrollPos = maxScroll;
        UpdateScrollArea(hwndScroll);
        break;
    }

    case WM_MOUSEWHEEL: {
        int delta = GET_WHEEL_DELTA_WPARAM(wParam);
        scrollPos -= (delta / WHEEL_DELTA) * 20;
        int maxScroll = max(0, (int)elements.size() * elementHeight - scrollAreaHeight);
        if (scrollPos < 0) scrollPos = 0;
        if (scrollPos > maxScroll) scrollPos = maxScroll;
        UpdateScrollArea(hwndScroll);
        InvalidateRect(hwnd, NULL, TRUE); // 画面ガビガビ防止
        break;
    }

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

// --- Main ウィンドウプロシージャ ---
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE:
        // + ボタン
        CreateWindowEx(0, "BUTTON", "+", WS_CHILD | WS_VISIBLE,
            10, 10, 30, 30, hwnd, (HMENU)ID_BTN_ADD, hInst, NULL);
        // - ボタン
        CreateWindowEx(0, "BUTTON", "-", WS_CHILD | WS_VISIBLE,
            50, 10, 30, 30, hwnd, (HMENU)ID_BTN_REMOVE, hInst, NULL);
        break;

    case WM_COMMAND: {
        int id = LOWORD(wParam);
        if (id == ID_BTN_ADD) AddElement(hwndScroll, "", "");
        else if (id == ID_BTN_REMOVE) RemoveElement();
        break;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

// --- WinMain ---
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    hInst = hInstance;

    // Main ウィンドウ
    const char CLASS_NAME[] = "MainWnd";
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClass(&wc);

    hwndMain = CreateWindowEx(0, CLASS_NAME, "Pokebot v2.0.0",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 450,
        NULL, NULL, hInstance, NULL);

    // ScrollArea ウィンドウクラス
    WNDCLASS wcScroll = {};
    wcScroll.lpfnWndProc = ScrollWndProc;
    wcScroll.hInstance = hInstance;
    wcScroll.lpszClassName = "ScrollArea";
    wcScroll.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcScroll.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClass(&wcScroll);

    // ScrollArea 作成
    hwndScroll = CreateWindowEx(0, "ScrollArea", "",
        WS_CHILD | WS_VISIBLE | WS_VSCROLL,
        10, 50, 370, scrollAreaHeight,
        hwndMain, NULL, hInstance, NULL);

    ShowWindow(hwndMain, nCmdShow);
    UpdateWindow(hwndMain);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
