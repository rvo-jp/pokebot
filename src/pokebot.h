#ifndef __POKEBOT__
#define __POKEBOT__

void pokebot();

char mainName[256] = "s1";
char subAddress[16][16] = {
    "127.0.0.1:5565",
    "127.0.0.1:5635",
    "127.0.0.1:5575"
};
int subLen = 3;
int devW  = 900;
int devH = 1600;

#endif