#include "bot.hpp"

bool Bot::isInside(POINT point) {
    return GetAncestor(WindowFromPoint(point), GA_ROOT) == window;
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

