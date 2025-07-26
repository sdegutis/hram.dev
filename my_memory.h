#pragma once

#include <intsafe.h>

struct System {
	UINT16 appversion;
	UINT8 r1;
	UINT8 r2;
	UINT8 inflags;
	UINT8 keymods;
	UINT8 cursorcol;
	UINT8 cursorrow;
	UINT32 time;
	UINT8 mousex;
	UINT8 mousey;
	UINT16 reserved1;
	UINT8 keys[32];
	UINT64 addrs[26];
	UINT8 screen[128 * 72];
	UINT8 font[16 * 4 * 6 * 6];
	CHAR reserved2[512];
};

extern struct System* sys;
extern void (*usersignal)(UINT32 evid, UINT32 evarg);
extern char* usersrc;

void setupMemory();
void blitimmediately();
