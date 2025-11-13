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

const int scrollAreaHeight = 210;
const int elementHeight = 30;
int scrollPos = 0;



// Element 構造体
struct Element {
    HWND hEditLabel;
    HWND hEditPort;
    HWND hButton;
    int y;
    int id;
    Bot *botPtr;
};

vector<Element> elements;
int nextElementId = 2000;


void ErrorMessage(HWND hwnd, const char *e) {
    MessageBoxA(hwnd, e, "Error", MB_OK);
}

void UpdateScrollArea(HWND hwnd) {
    for (auto& el : elements) {
        MoveWindow(el.hEditLabel, 0, el.y - scrollPos, 170, 25, TRUE);
        MoveWindow(el.hEditPort, 180, el.y - scrollPos, 60, 25, TRUE);
        MoveWindow(el.hButton, 250, el.y - scrollPos, 50, 25, TRUE);
    }

    int scrollMax = max(0, (int)elements.size() * elementHeight - scrollAreaHeight);
    SetScrollRange(hwnd, SB_VERT, 0, scrollMax, TRUE);
    SetScrollPos(hwnd, SB_VERT, scrollPos, TRUE);
    InvalidateRect(hwnd, NULL, TRUE); // 画面ガビガビ防止
}

void AddElement(HWND hwnd, const string& label, const string& port) {
    int y = elements.size() * elementHeight;
    int id = nextElementId++;

    HWND hEditLabel = CreateWindow("EDIT", label.c_str(),
            WS_CHILD | WS_VISIBLE | ES_LEFT | WS_BORDER,
            0, y, 170, 25,
            hwnd, NULL, hInst, NULL);

    HWND hEditPort = CreateWindow("EDIT", port.c_str(),
            WS_CHILD | WS_VISIBLE | ES_LEFT | WS_BORDER,
            180, y, 60, 25,
            hwnd, NULL, hInst, NULL);

    HWND hButton = CreateWindow("BUTTON", "ON",
            WS_CHILD | WS_VISIBLE | WS_BORDER,
            250, y, 50, 25,
            hwnd, (HMENU)MAKEINTRESOURCE(id), hInst, NULL);

    elements.push_back({ hEditLabel, hEditPort, hButton, y, id, nullptr });
    UpdateScrollArea(hwnd);
}

void RemoveElement() {
    if (elements.empty()) return;
    auto el = elements.back();
    DestroyWindow(el.hEditLabel);
    DestroyWindow(el.hEditPort);
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
            ErrorMessage(hwnd, "Failed to open 'config.txt'");
            break;
        }

        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;

            string label, port;
            size_t pos = line.find(':');

            if (pos != string::npos) {
                label = line.substr(0, pos);
                port  = line.substr(pos + 1);

                AddElement(hwnd, label, port);
            }
            else {
                ErrorMessage(hwnd, ("Invalid format: " + line).c_str());
            }
        }
        file.close();

        break;
    }

    case WM_COMMAND: {
        int id = LOWORD(wParam);
        for (auto& el : elements) {
            if (el.id == id) {
                char buf[128];
                GetWindowTextA(el.hEditLabel, buf, sizeof(buf));
                string label(buf);
                GetWindowTextA(el.hEditPort, buf, sizeof(buf));
                string port(buf);

                try {
                    if (el.botPtr == nullptr) {
                        el.botPtr = Pokebot::connect(label, port);
                        SetWindowTextA(el.hButton, "OFF");
                    }
                    else {
                        Pokebot::disconnect(el.botPtr);
                        el.botPtr = nullptr;
                        SetWindowTextA(el.hButton, "ON");
                    }
                }
                catch (const exception& e) {
                    ErrorMessage(hwnd, e.what());
                }
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
        break;
    }

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

// + / - ボタンID
const int ID_BTN_ADD = 1001;
const int ID_BTN_REMOVE = 1002;

// --- Main ウィンドウプロシージャ ---
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE:
        // + ボタン
        CreateWindow(
            "BUTTON", "Add", WS_CHILD | WS_VISIBLE | WS_BORDER,
            10, 230, 50, 30,
            hwnd, (HMENU)ID_BTN_ADD, hInst, NULL
        );
        
        // - ボタン
        CreateWindow(
            "BUTTON", "Remove", WS_CHILD | WS_VISIBLE |WS_BORDER,
            70, 230, 70, 30,
            hwnd, (HMENU)ID_BTN_REMOVE, hInst, NULL
        );

        break;

    case WM_COMMAND: {
        int id = LOWORD(wParam);
        if (id == ID_BTN_ADD) AddElement(hwndScroll, "", "");
        else if (id == ID_BTN_REMOVE) RemoveElement();
        break;
    }

    case WM_DESTROY: {
        ofstream file("config.txt");
        if (!file.is_open()) {
            ErrorMessage(hwnd, "Failed to open 'config.txt'");
            break;;
        }

        for (auto& el : elements) {
            char buf[128];
            GetWindowTextA(el.hEditLabel, buf, sizeof(buf));
            string label(buf);
            GetWindowTextA(el.hEditPort, buf, sizeof(buf));
            string port(buf);

            file << label << ":" << port << "\n";
        }
        file.close();

        PostQuitMessage(0);
        break;
    }


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
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 340, 300,
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
        10, 10, 320, scrollAreaHeight,
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
