#pragma once

#include <intsafe.h>

struct Memory {
	UINT32 time;
	UINT8 cursorcol;
	UINT8 cursorrow;
	UINT8 mousex;
	UINT8 mousey;
	UINT8 reserved1[8];
	UINT8 keys[32];
	UINT64 addrs[26];
	UINT8 screen[128 * 72];
	UINT8 font[16 * 4 * 6 * 6];
	CHAR reserved2[512];
};

extern struct Memory* sys;
extern void (*usersignal)(UINT32 evid, UINT32 evarg);
extern char* usersrc;

void setupMemory();
void blitimmediately();
