#ifndef __POKEBOT__
#define __POKEBOT__

#include <windows.h>
#include <stdio.h>
#include <process.h>

DWORD WINAPI Pokebot(LPVOID lpParam);

extern char mainName[256];
extern char subAddress[16][16];
extern int subLen;
extern int devW;
extern int devH;

#endif