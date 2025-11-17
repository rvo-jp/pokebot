#include <windows.h>
#include <gdiplus.h>
#include <vector>
#include <iostream>
#include <fstream>

#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;
using namespace std;

// スクショPNG配列出力
vector<unsigned char> execAdbScreencap() {
    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
    HANDLE readPipe, writePipe;

    CreatePipe(&readPipe, &writePipe, &sa, 0);
    SetHandleInformation(readPipe, HANDLE_FLAG_INHERIT, 0);

    PROCESS_INFORMATION pi;
    STARTUPINFOA si = { sizeof(STARTUPINFOA) };
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdOutput = writePipe;
    si.hStdError = writePipe;

    char cmd[] = ".\\Pokebot2\\platform-tools\\adb -s 127.0.0.1:5555 exec-out screencap -p";

    CreateProcessA(
        NULL, cmd, NULL, NULL, TRUE,
        CREATE_NO_WINDOW, NULL, NULL,
        &si, &pi
    );

    CloseHandle(writePipe);

    vector<unsigned char> buffer;
    unsigned char temp[4096];
    DWORD bytesRead;

    while (ReadFile(readPipe, temp, sizeof(temp), &bytesRead, NULL) && bytesRead > 0) {
        buffer.insert(buffer.end(), temp, temp + bytesRead);
    }

    CloseHandle(readPipe);
    WaitForSingleObject(pi.hProcess, INFINITE);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return buffer;
}

// PNG バイト列から Bitmap を生成
Bitmap* loadPngFromMemory(const vector<unsigned char>& pngData) {
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, pngData.size());
    if (!hMem) return nullptr;

    void* pMem = GlobalLock(hMem);
    memcpy(pMem, pngData.data(), pngData.size());
    GlobalUnlock(hMem);

    IStream* pStream = nullptr;
    if (CreateStreamOnHGlobal(hMem, TRUE, &pStream) != S_OK) {
        GlobalFree(hMem);
        return nullptr;
    }

    Bitmap* bmp = Bitmap::FromStream(pStream);
    pStream->Release();
    return bmp;
}

// Bitmap の一部がテンプレに一致するか調べる
bool MatchTemplate(Bitmap* src, Bitmap* tpl, int startX, int startY) {
    const UINT tplW = tpl->GetWidth();
    const UINT tplH = tpl->GetHeight();

    // 範囲チェック
    if (startX + tplW > src->GetWidth()) return false;
    if (startY + tplH > src->GetHeight()) return false;

    Color cSrc, cTpl;

    for (UINT y = 0; y < tplH; y++) {
        for (UINT x = 0; x < tplW; x++) {
            tpl->GetPixel(x, y, &cTpl);
            src->GetPixel(startX + x, startY + y, &cSrc);

            // 完全一致判定（誤差許容したいならここで調整）
            if (cTpl.GetValue() != cSrc.GetValue()) {
                return false; // 不一致
            }
        }
    }
    return true; // 全ピクセル一致
}

void putsColer(Bitmap* bmp, int x, int y) {
    Gdiplus::Color color;
    if (bmp->GetPixel(x, y, &color) == Gdiplus::Ok) {
        std::cout << "Pixel (" << x << "," << y << "): "
                  << "Value=" << to_string(color.GetValue()) << " "
                  << "R=" << to_string(color.GetR()) << " "
                  << "G=" << to_string(color.GetG()) << " "
                  << "B=" << to_string(color.GetB()) << std::endl;
    }
}

void putsPixel() {
    // --- ここで PNG バイト列を取得 ---
    std::vector<unsigned char> pngBuffer;
    pngBuffer = execAdbScreencap();

    // ofstream ofs("test.png", ios::binary);
    // if (!ofs.is_open()) {
    //     return;
    // }
    // ofs.write(reinterpret_cast<const char*>(pngBuffer.data()), pngBuffer.size());
    // ofs.close();

    // テスト用に pngBuffer が空でないことを確認
    if (pngBuffer.empty()) {
        std::cerr << "pngBuffer is empty!\n";
        return;
    }

    Gdiplus::Bitmap* bmp = loadPngFromMemory(pngBuffer);
    if (!bmp || bmp->GetLastStatus() != Gdiplus::Ok) {
        std::cerr << "Failed to load PNG\n";
        return;
    }

    putsColer(bmp, 470, 893); // R=107 G=123 B=145
    putsColer(bmp, 470, 917); // R=106 G=122 B=144
    putsColer(bmp, 490, 906); // R=179 G=192 B=204
    putsColer(bmp, 495, 894); // R=107 G=123 B=144
    putsColer(bmp, 495, 919); // R=106 G=122 B=144
    putsColer(bmp, 480, 906); // R=229 G=239 B=245
    putsColer(bmp, 485, 885); // R=230 G=240 B=246
    putsColer(bmp, 485, 927); // R=227 G=238 B=245

    Pixel (470,893): Value=4285234065 R=107 G=123 B=145
Pixel (470,917): Value=4285168272 R=106 G=122 B=144
Pixel (490,906): Value=4289970380 R=179 G=192 B=204
Pixel (495,894): Value=4285234064 R=107 G=123 B=144
Pixel (495,919): Value=4285168272 R=106 G=122 B=144
Pixel (480,906): Value=4293259253 R=229 G=239 B=245
Pixel (485,885): Value=4293325046 R=230 G=240 B=246
Pixel (485,927): Value=4293127925 R=227 G=238 B=245

    delete bmp;
}

int main() {
    // GDI+ 初期化
    Gdiplus::GdiplusStartupInput gdiInput;
    ULONG_PTR gdiToken;
    Gdiplus::GdiplusStartup(&gdiToken, &gdiInput, nullptr);

    putsPixel();

    Gdiplus::GdiplusShutdown(gdiToken);
    return 0;
}