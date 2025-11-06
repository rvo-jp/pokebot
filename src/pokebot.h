#ifndef __POKEBOT__
#define __POKEBOT__

#include <windows.h>
#include <stdio.h>
#include <process.h>

unsigned __stdcall Pokebot(void* arg);

extern char mainName[256];
extern char subAddress[16][16];
extern int subLen;
extern int devW;
extern int devH;

#endif