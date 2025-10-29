#include <windows.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

char *name = "BlueStacks App Player 1";
int X, Y;



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
        COLORREF color = GetPixel(hdc, X + sc.point_color[i].point.x, Y + sc.point_color[i].point.y + 33); // 上の縁の分たす+33px
        if (color == CLR_INVALID) {
            puts("GetPixel failed");
            exit(1);
        }

        printf("color: (%d, %d, %d) (%d, %d, %d)\n", sc.point_color[i].color.r, sc.point_color[i].color.g, sc.point_color[i].color.b, GetRValue(color), GetGValue(color), GetBValue(color));
        if (sc.point_color[i].color.r != GetRValue(color) || sc.point_color[i].color.g != GetGValue(color) || sc.point_color[i].color.b != GetBValue(color)) {
            ReleaseDC(NULL, hdc);
            return false;
        }
    }

    ReleaseDC(NULL, hdc);
    return true;
}

bool testpixel(POINT point, COLOR color) {
    HDC hdc = GetDC(NULL);
    if (!hdc) {
        puts("GetDC failed");
        exit(1);
    }

    COLORREF pixel = GetPixel(hdc, X + point.x, Y + point.y + 33); // 上の縁の分たす+33px
    if (pixel == CLR_INVALID) {
        puts("GetPixel failed");
        exit(1);
    }

    printf("color: (%d, %d, %d) (%d, %d, %d)\n", color.r, color.g, color.b, GetRValue(pixel), GetGValue(pixel), GetBValue(pixel));

    bool result = color.r == GetRValue(pixel) && color.g == GetGValue(pixel) && color.b == GetBValue(pixel);
    ReleaseDC(NULL, hdc);
    return result;
}

void movecursor(int x, int y) {
    SetCursorPos(X + x, Y + y + 33);
}

void leftdown() {
    INPUT input[1] = {
        {.type = INPUT_MOUSE, .mi.dwFlags = MOUSEEVENTF_LEFTDOWN}
    };
    SendInput(1, input, sizeof(INPUT));
}

void leftup() {
    INPUT input[1] = {
        {.type = INPUT_MOUSE, .mi.dwFlags = MOUSEEVENTF_LEFTUP}
    };
    SendInput(1, input, sizeof(INPUT));
}

void leftclick(int x, int y) {
    SetCursorPos(X + x, Y + y + 33);
    INPUT input[2] = {
        {.type = INPUT_MOUSE, .mi.dwFlags = MOUSEEVENTF_LEFTDOWN},
        {.type = INPUT_MOUSE, .mi.dwFlags = MOUSEEVENTF_LEFTUP}
    };
    SendInput(2, input, sizeof(INPUT));
}

void inputkey(char key) {
    WORD wVk = VkKeyScan(key) & 0xFF;
    INPUT input[2] = {
        {.type = INPUT_KEYBOARD, .ki.wVk = wVk},
        {.type = INPUT_KEYBOARD, .ki.wVk = wVk, .ki.dwFlags = KEYEVENTF_KEYUP}
    };
    SendInput(2, input, sizeof(INPUT));
}

void s1();
void s2();
void s3();
void s4();
void s5();
void s6();
void s7();
void s8();
void s9();
void s10();
void s11();
void s12();
void s13();
void s14();
void s15();
void s16();
void s17();
void s18();
void s19();
void s20();
void s21();
void s22();
void s23();
void s24();
void s25();
void s26();
void s27();
void s28();
void s29();
void s30();
void s31();
void s32();
void s33();
void s34();
void s35();
void s36();
void s37();
void s38();
void s39();
void s40();
void s41();
void s42();
void (*func)() = s21;

// スタートスクリーン
void s1() {
    if (testscreen((SCREEN){
        .point_color = (POINT_COLER[]){
            {.point = {140, 445}, .color = {238, 28, 33}},
            {.point = {383, 445}, .color = {238, 28, 33}}
        },
        .len = 2
    })) {
        leftclick(262, 731);
        func = s2;
    }
}

// 国・地域選択
void s2() {
    puts("s2");
    if (testpixel((POINT){208, 842}, (COLOR){169, 191, 219})) {
        puts("set local");
        leftclick(180, 678);
        func = s3;
    }
}

// 国・地域選択 年
void s3() {
    puts("s3");
    if (testpixel((POINT){190, 723}, (COLOR){73, 92, 115})) {
        leftclick(190, 459);
        func = s4;
    }
}

// 国・地域選択 月
void s4() {
    puts("s4");
    if (testpixel((POINT){180, 678}, (COLOR){75, 92, 116})) {
        leftclick(400, 678);
        func = s5;
    }
}

// 国・地域選択 月
void s5() {
    puts("s5");
    if (testpixel((POINT){400, 435}, (COLOR){74, 93, 115})) {
        leftclick(400, 473);
        func = s6;
    }
}

// 国・地域選択 OK
void s6() {
    puts("s6");
    if (testpixel((POINT){400, 678}, (COLOR){75, 91, 116})) {
        leftclick(208, 842);
        func = s7;
    }
}

// 確認
void s7() {
    puts("s7");
    if (testpixel((POINT){256, 558}, (COLOR){250, 253, 253})) {
        leftclick(325, 620);
        puts("privacy policy");
        func = s8;
    }
}

// 利用規約
void s8() {
    puts("s8");
    if (testpixel((POINT){82, 620}, (COLOR){182, 204, 231})) {
        leftclick(200, 484);
        func = s9;
    }
}

// 利用規約 ×ボタン
void s9() {
    puts("s9");
    if (testpixel((POINT){262, 838}, (COLOR){109, 124, 145})) {
        leftclick(262, 838);
        func = s10;
    }
}

// 利用規約 同意
void s10() {
    puts("s10");
    if (testpixel((POINT){82, 620}, (COLOR){182, 204, 231})) {
        leftclick(82, 620);
        func = s11;
    }
}

// プライバシーポリシー
void s11() {
    puts("s11");
    if (testpixel((POINT){82, 620}, (COLOR){0, 210, 199})) {
        leftclick(200, 557);
        func = s12;
    }
}

// プライバシーポリシー ×ボタン
void s12() {
    puts("s12");
    if (testpixel((POINT){262, 838}, (COLOR){109, 124, 145})) {
        leftclick(262, 838);
        func = s13;
    }
}

// プライバシーポリシー 同意
void s13() {
    puts("s13");
    if (testpixel((POINT){82, 680}, (COLOR){180, 203, 230})) {
        leftclick(82, 680);
        func = s14;
    }
}

// OK
void s14() {
    puts("s14");
    if (testpixel((POINT){82, 680}, (COLOR){0, 210, 197})) {
        leftclick(222, 838);
        puts("create save date");
        func = s15;
    }
}

// セーブデータ作成　はじめから
void s15() {
    puts("s15");
    if (testpixel((POINT){260, 580}, (COLOR){114, 128, 148})) {
        leftclick(260, 580);
        puts("connect account");
        func = s16;
    }
}

// アカウント連携　しない
void s16() {
    puts("s16");
    if (testpixel((POINT){260, 790}, (COLOR){145, 159, 175})) {
        leftclick(260, 790);
        func = s17;
    }
}

// ここ待機時間ながめ

// データのダウンロード
void s17() {
    puts("s17");
    if (testpixel((POINT){260, 590}, (COLOR){238, 245, 249})) {
        leftclick(333, 620);
        func = s18;
    }
}

// ここ待機時間　ダウンロードサイズで可変

// データのダウンロードが完了　OK x
void s18() {
    puts("s18");
    // 要修正
    if (testpixel((POINT){140, 620}, (COLOR){239, 247, 250})) {
        leftclick(225, 620);
        puts("movie");
        func = s19;
    }
}

// ビデオスキップ 端っこタップ x
void s19() {
    puts("s19");
    if (!testpixel((POINT){482, 890}, (COLOR){219, 232, 242})) {
        leftclick(482, 890);
        func = s20;
    }
}

// ビデオスキップ スキップボタン
void s20() {
    puts("s20");
    if (testpixel((POINT){482, 890}, (COLOR){239, 247, 250})) {
        leftclick(482, 890);
        puts("start game");
        func = s21;
    }
}

// ようこそ
void s21() {
    puts("s21");
    if (testpixel((POINT){140, 347}, (COLOR){236, 28, 36})) {
        leftclick(230, 754);
        func = s22;
    }
}

// ようこそ 2
void s22() {
    puts("s22");
    if (testpixel((POINT){260, 454}, (COLOR){232, 127, 55})) {
        leftclick(340, 744);
        puts("set name");
        func = s23;
    }
}

// プレイヤー名
void s23() {
    puts("s23");
    if (testpixel((POINT){160, 454}, (COLOR){75, 85, 98})) {
        leftclick(266, 399);
        func = s24;
    }
}

// プレイヤー名 入力欄を開く
void s24() {
    puts("s24");
    if (testpixel((POINT){82, 500}, (COLOR){241, 66, 72})) {
        leftclick(260, 414);
        func = s25;
    }
}

// プレイヤー名 入力
void s25() {
    puts("s25");
    if (testpixel((POINT){260, 900}, (COLOR){255, 255, 255})) {
        inputkey('i');
        inputkey('n');
        inputkey('i');
        inputkey('b');
        inputkey('o');
        inputkey('t');
        func = s26;
    }
}

// プレイヤー名 OK
void s26() {
    puts("s26");
    if (!testpixel((POINT){321, 622}, (COLOR){173, 194, 222})) {
        leftclick(321, 622);
        func = s27;
    }
}

// プレイヤー名 OK(2)
void s27() {
    puts("s27");
    if (!testpixel((POINT){223, 620}, (COLOR){173, 194, 222})) {
        leftclick(223, 620);
        func = s28;
    }
}

// プレイヤー名 OK(3) 30日間変更できない
void s28() {
    puts("s28");
    if (testpixel((POINT){155, 512}, (COLOR){241, 62, 68})) {
        leftclick(325, 622);
        puts("open 1st pack");
        func = s29;
    }
}

// 1回目パック開封
void s29() {
    puts("s29");
    if (testpixel((POINT){191, 161}, (COLOR){57, 67, 81})) {
        leftclick(265, 520);
        func = s30;
    }
}

// これにする
void s30() {
    puts("s30");
    if (!testpixel((POINT){191, 161}, (COLOR){57, 67, 81})) {
        leftclick(191, 726);
        func = s31;
    }
}

// アニメーションにより待機

// スライド開封
void s31() {
    puts("s31");
    if (testpixel((POINT){10, 10}, (COLOR){169, 196, 228})) {
        movecursor(83, 540);
        leftdown();
        Sleep(100);
        movecursor(165, 540);
        Sleep(100);
        movecursor(267, 540);
        Sleep(100);
        movecursor(370, 540);
        Sleep(100);
        movecursor(435, 539);
        leftup();
        func = s32;
    }
}

// めくる 1　フシキダネ確定？
void s32() {
    puts("s32");
    if (testpixel((POINT){122, 760}, (COLOR){69, 71, 70})) {
        leftclick(260, 477);
        func = s33;
    }
}

// めくる 2
void s33() {
    puts("s33");
    if (testpixel((POINT){122, 760}, (COLOR){69, 71, 70})) {
        leftclick(260, 477);
        func = s34;
    }
}

// めくる 3
void s34() {
    puts("s34");
    if (testpixel((POINT){122, 760}, (COLOR){69, 71, 70})) {
        leftclick(260, 477);
        func = s35;
    }
}

// めくる 4
void s35() {
    puts("s35");
    if (testpixel((POINT){122, 760}, (COLOR){69, 71, 70})) {
        leftclick(260, 477);
        func = s36;
    }
}

// めくる 5 ガラガラ確定
void s36() {
    puts("s36");
    if (testpixel((POINT){122, 760}, (COLOR){69, 71, 70})) {
        leftclick(260, 477);
        func = s37;
    }
}

// アニメーション待機

// 図鑑登録 上スライド
void s37() {
    puts("s37");
    if (testpixel((POINT){72, 121}, (COLOR){88, 195, 252})) {
        movecursor(262, 800);
        leftdown();
        Sleep(100);
        movecursor(262, 575);
        Sleep(100);
        movecursor(262, 325);
        leftup();
        func = s38;
    }
}
// アニメーション待機

// ガラガラ観察
void s38() {
    puts("s38");
    if (testpixel((POINT){238, 753}, (COLOR){59, 69, 84})) {
        leftclick(435, 769);
        func = s39;
    }
}

// ガラガラ観察 ゆび
void s39() {
    puts("s39");
    if (testpixel((POINT){262, 642}, (COLOR){246, 249, 248})) {
        leftclick(262, 642);
        func = s40;
    }
}

// カードを動かす OK
void s40() {
    puts("s40");
    if (testpixel((POINT){251, 830}, (COLOR){244, 249, 249})) { // 245, 249, 249も
        leftclick(300, 838);
        func = s41;
    }
}

// 次に進む
void s41() {
    puts("s41");
    if (testpixel((POINT){261, 588}, (COLOR){238, 246, 250})) {
        leftclick(318, 624);
        func = s42;
    }
}

// アニメーション

// ミッション
void s42() {
    puts("s42");
    if (testpixel((POINT){20, 20}, (COLOR){117, 132, 152})) {
        leftclick(467, 812);
        func = s1;
    }
}

int main() {
    while (1) {
        HWND hwnd = FindWindow(NULL, name);
        if (!hwnd) {
            printf("Error: '%s' not found\n", name);
            return 1;
        }

        RECT wrect;
        if (GetWindowRect(hwnd, &wrect)) {
            X = wrect.left;
            Y = wrect.top;
            int width  = wrect.right - wrect.left;
            int height = wrect.bottom - wrect.top;

            printf("from: (%d, %d) size: %d x %d\n", X, Y, width, height);

            SetWindowPos(hwnd, HWND_TOPMOST, X, Y, 557, 964, SWP_SHOWWINDOW);
        }

        func();
        Sleep(1000);
    }

    return 0;
}
