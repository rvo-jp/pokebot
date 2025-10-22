#include <windows.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

INPUT INPUT_LEFTCLICK[2] = {
    {.type = INPUT_MOUSE, .mi.dwFlags = MOUSEEVENTF_LEFTDOWN},
    {.type = INPUT_MOUSE, .mi.dwFlags = MOUSEEVENTF_LEFTUP}
};

typedef struct coler {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} COLOR;

typedef struct point_coler {
    POINT point;
    COLOR color;
} POINT_COLER;

typedef struct screen {
    POINT_COLER *point_color;
    int len;
} SCREEN;

bool testscreen(SCREEN sc) {
    HDC hdc = GetDC(NULL);
    if (!hdc) {
        puts("GetDC failed");
        exit(1);
    }

    for (int i = 0; i < sc.len; i ++) {
        COLORREF color = GetPixel(hdc, sc.point_color[i].point.x, sc.point_color[i].point.y);
        if (color == CLR_INVALID) {
            puts("GetPixel failed");
            exit(1);
        }

        if (sc.point_color[i].color.r != GetRValue(color) || sc.point_color[i].color.g != GetGValue(color) || sc.point_color[i].color.b != GetBValue(color)) {
            ReleaseDC(NULL, hdc);
            return false;
        }
    }

    ReleaseDC(NULL, hdc);
    return true;
}


// 国・地域選択
void s1() {
    if (testscreen((SCREEN){
        .point_color = (POINT_COLER[]){
            {.point = {1000, 975}, .color = {169, 192, 219}} // OKボタン
        },
        .len = 1
    })) {
        SetCursorPos(1000, 975);
        SendInput(2, INPUT_LEFTCLICK, sizeof(INPUT));
    }
}


int main() {
    HWND hwnd = FindWindow(NULL, "BlueStacks App Player"); // 例: メモ帳
    if (!hwnd) {
        printf("error: BlueStacks App Player not found\n");
        return 1;
    }
    SetForegroundWindow(hwnd);

    RECT rect;
    GetClientRect(hwnd, &rect);
    int width  = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    printf("Client size: %d x %d\n", width, height);

    Sleep(1000);



    return 0;
}
