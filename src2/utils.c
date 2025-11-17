#include <utils.h>

// コマンド非同期実行
void runCommandAsync(const char *cmd) {
    STARTUPINFOA si;
    PROCESS_INFORMATION pi{};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    // commandは書き換えられる可能性があるためコピーする
    vector<char> cmdBuffer(command.begin(), command.end());
    cmdBuffer.push_back('\0'); // null終端を追加

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