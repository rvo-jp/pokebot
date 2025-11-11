#include "bot.hpp"

Bot::Bot(const string& label, const string& _port) {
    port = _port;
    window = FindWindowA(NULL, label.c_str());
    if (!window) {
        throw runtime_error("Window '" + label + "'not found");
    }

    // runCommandAsync(".\\platform-tools\\adb connect 127.0.0.1:" + port);
    runCommand(".\\platform-tools\\adb connect 127.0.0.1:" + port);
    string output = runCommand(".\\platform-tools\\adb -s 127.0.0.1:" + port + " shell wm size");
    
    // Physical size: 1080x1920
    regex re(R"((\d+)x(\d+))");
    smatch match;

    if (regex_search(output, match, re)) {
        width  = stoi(match[1].str());
        height = stoi(match[2].str());
    }
    else {
        throw runtime_error("Failed to parse screen size from output: " + output);
    }

    // デバッグ表示
    cout << "[DEBUG] Connected to " << label << " (" << width << "x" << height << ")" << endl;
}

void Bot::disconnect() {
    runCommandAsync(".\\platform-tools\\adb disconnect 127.0.0.1:" + port);
}

string& Bot::getPort() {
    return port;
}

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




// adbタスク
void Bot::inputShell(const string& input) {
    runCommandAsync(".\\platform-tools\\adb -s 127.0.0.1:" + port + " shell input " + input);
}



// コマンド非同期実行
void Bot::runCommandAsync(const string& command) {
    STARTUPINFOA si{};
    PROCESS_INFORMATION pi{};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    // commandは書き換えられる可能性があるためコピーする
    vector<char> cmdBuffer(command.begin(), command.end());
    cmdBuffer.push_back('\0'); // null終端を追加

    // cout << "[DEBUG] Cmd: " << command << endl;

    BOOL success = CreateProcessA(
        nullptr,                // 実行ファイル名（commandで指定するのでNULL）
        cmdBuffer.data(),       // コマンドライン（コピーを渡す）
        nullptr,                // セキュリティ属性
        nullptr,                // スレッド属性
        FALSE,                  // ハンドル継承しない
        CREATE_NO_WINDOW,       // ウィンドウなし
        nullptr,                // 環境変数なし
        nullptr,                // カレントディレクトリ
        &si,
        &pi
    );

    if (!success) {
        DWORD err = GetLastError();
        throw runtime_error("CreateProcessA failed: error code " + to_string(err));
        return;
    }

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
}

// コマンド同期実行（完了を待って出力を返す）
string Bot::runCommand(const string& command) {
    SECURITY_ATTRIBUTES sa{};
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE; // パイプを子プロセスが継承できるように
    sa.lpSecurityDescriptor = nullptr;

    // 標準出力用パイプ作成
    HANDLE hRead = nullptr;
    HANDLE hWrite = nullptr;
    if (!CreatePipe(&hRead, &hWrite, &sa, 0)) {
        throw runtime_error("CreatePipe failed: " + to_string(GetLastError()));
    }

    // 書き込みハンドルを継承しない設定
    SetHandleInformation(hRead, HANDLE_FLAG_INHERIT, 0);

    STARTUPINFOA si{};
    PROCESS_INFORMATION pi{};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.hStdOutput = hWrite;
    si.hStdError  = hWrite;  // 標準エラーもまとめて取得
    si.wShowWindow = SW_HIDE;

    // CreateProcessA に渡すためにコマンドをコピー
    vector<char> cmdBuffer(command.begin(), command.end());
    cmdBuffer.push_back('\0');

    BOOL success = CreateProcessA(
        nullptr,
        cmdBuffer.data(),
        nullptr,
        nullptr,
        TRUE,               // 標準ハンドル継承を有効に
        CREATE_NO_WINDOW,   // コンソール非表示
        nullptr,
        nullptr,
        &si,
        &pi
    );

    CloseHandle(hWrite); // 親側の書き込みハンドルは不要

    if (!success) {
        DWORD err = GetLastError();
        CloseHandle(hRead);
        throw runtime_error("CreateProcessA failed: error code " + to_string(err));
    }

    // 標準出力を読み取る
    string output;
    char buffer[4096];
    DWORD bytesRead = 0;

    while (ReadFile(hRead, buffer, sizeof(buffer) - 1, &bytesRead, nullptr) && bytesRead > 0) {
        buffer[bytesRead] = '\0';
        output += buffer;
    }

    // プロセス終了を待つ
    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD exitCode = 0;
    GetExitCodeProcess(pi.hProcess, &exitCode);

    // リソースを開放
    CloseHandle(hRead);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    if (exitCode != 0) {
        throw runtime_error("Process exited with code " + to_string(exitCode) + ": " + output);
    }

    return output;
}
