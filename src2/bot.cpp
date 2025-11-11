#include "bot.hpp"

bool Bot::isInside(POINT point) {
    return GetAncestor(WindowFromPoint(point), GA_ROOT) == window;
}

bool Bot::isForground() {
    return GetForegroundWindow() == window;
}

POINT Bot::getRelativePos(const POINT point, int devW, int devH) {
    RECT rect;
    GetWindowRect(window, &rect);
    rect.top += 33; // 上の縁

    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    
    POINT pos;
    pos.x = (point.x - rect.left) * devW  / width;
    pos.y = (point.y - rect.top) * devH / height;
    return pos;
}

void Bot::tap(const POINT pos) {
    inputShell("tap " + to_string(pos.x) + " " + to_string(pos.y));
}

void Bot::swipe(const POINT startPos, const POINT endPos, DWORD time) {
    inputShell("swipe " + to_string(startPos.x) + " " + to_string(startPos.y) + " " + to_string(endPos.x) + " " + to_string(endPos.y) + to_string(time));
}

// US配列前提
static int vk_to_android(const DWORD vk) {
    switch (vk) {
        case 'A': return 29;
        case 'B': return 30;
        case 'C': return 31;
        case 'D': return 32;
        case 'E': return 33;
        case 'F': return 34;
        case 'G': return 35;
        case 'H': return 36;
        case 'I': return 37;
        case 'J': return 38;
        case 'K': return 39;
        case 'L': return 40;
        case 'M': return 41;
        case 'N': return 42;
        case 'O': return 43;
        case 'P': return 44;
        case 'Q': return 45;
        case 'R': return 46;
        case 'S': return 47;
        case 'T': return 48;
        case 'U': return 49;
        case 'V': return 50;
        case 'W': return 51;
        case 'X': return 52;
        case 'Y': return 53;
        case 'Z': return 54;

        // 数字キー
        case '0': return 7;
        case '1': return 8;
        case '2': return 9;
        case '3': return 10;
        case '4': return 11;
        case '5': return 12;
        case '6': return 13;
        case '7': return 14;
        case '8': return 15;
        case '9': return 16;

        // 記号
        case VK_OEM_MINUS:    return 69;  // '-'
        case VK_OEM_PLUS:     return 70;  // '='
        case VK_OEM_4:        return 71;  // '['
        case VK_OEM_6:        return 72;  // ']'
        case VK_OEM_5:        return 73;  // '\'
        case VK_OEM_1:        return 74;  // ';'
        case VK_OEM_7:        return 75;  // '''
        case VK_OEM_COMMA:    return 55;  // ','
        case VK_OEM_PERIOD:   return 56;  // '.'
        case VK_OEM_2:        return 76;  // '/`?'
        case VK_OEM_3:        return 68;  // '`'

        // 特殊キー
        case VK_SPACE:        return 62;
        case VK_RETURN:       return 66;
        case VK_BACK:         return 67;
        case VK_TAB:          return 61;
        case VK_ESCAPE:       return 111;
        case VK_DELETE:       return 112;
        case VK_LEFT:         return 21;
        case VK_UP:           return 19;
        case VK_RIGHT:        return 22;
        case VK_DOWN:         return 20;
        case VK_HOME:         return 3;
        case VK_END:          return 123;
        case VK_PRIOR:        return 92;  // Page Up
        case VK_NEXT:         return 93;  // Page Down
        case VK_CAPITAL:      return 115; // Caps Lock
        case VK_SHIFT:        return 59;
        case VK_CONTROL:      return 113;
        case VK_MENU:         return 57;  // Alt
        case VK_LWIN:         return 3;   // Homeボタン扱い

        // ファンクションキー
        case VK_F1: return 131;
        case VK_F2: return 132;
        case VK_F3: return 133;
        case VK_F4: return 134;
        case VK_F5: return 135;
        case VK_F6: return 136;
        case VK_F7: return 137;
        case VK_F8: return 138;
        case VK_F9: return 139;
        case VK_F10: return 140;
        case VK_F11: return 141;
        case VK_F12: return 142;

        default: return -1;
    }
}

void Bot::keyevent(const DWORD key) {
    inputShell("keyevent " + to_string(vk_to_android(key)));
}



bool Bot::getWindow(const string label) {
    window = FindWindowA(NULL, label.c_str());
}

// コマンド非同期実行
void Bot::runCommandAsync(const string command) {
    STARTUPINFOA si{};
    PROCESS_INFORMATION pi{};
    si.cb = sizeof(si);

    // コンソールを出さない設定
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    BOOL success = CreateProcessA(
        nullptr,               // 実行ファイル名（commandで指定するのでNULL）
        const_cast<char *>(command.c_str()),         // コマンドライン
        nullptr,                // セキュリティ属性
        nullptr,                // スレッド属性
        FALSE,                  // ハンドル継承しない
        CREATE_NO_WINDOW,       // ウィンドウなし
        nullptr,                // 環境変数なし
        nullptr,                // カレントディレクトリ
        &si, &pi
    );

    if (success) {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    else {
        printf("CreateProcess failed: %lu\n", GetLastError());
    }
}

// adbタスク
void Bot::inputShell(const string input) {
    runCommandAsync(".\\platform-tools\\adb -s 127.0.0.1:" + port + " shell input " + input);
}

