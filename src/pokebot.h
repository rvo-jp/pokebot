#ifndef __POKEBOT__
#define __POKEBOT__

DWORD WINAPI pokebot(LPVOID lpParam);

HANDLE hThread = NULL;
BOOL running = TRUE;
char mainName[256] = "bot0";
char subAddress[16][16] = {
    "127.0.0.1:5645"
};
int subLen = 1;

#endif