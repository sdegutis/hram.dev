#include <Windows.h>
#include <stdio.h>
#include <KnownFolders.h>

#include "resource.h"

#include "my_screen.h"
#include "my_main.h"
#include "my_window.h"
#include "my_asm.h"
#include "my_memory.h"
#include "my_file.h"


#define APP_VERSION (100)

static void openConsole();
static void checkLicense();
static void callsig(enum asmevent ev, UINT32 arg);

static int running = 1;

enum asmevent {
	asmevent_init,
	asmevent_tick,
	asmevent_mousemove,
	asmevent_mousewheel,
	asmevent_mousedown,
	asmevent_mouseup,
	asmevent_keydown,
	asmevent_keyup,
};

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, PWSTR pCmdLine, int nCmdShow) {
	checkLicense();
	setupMemory();
	openConsole();
	loadUserCodeFromDisk();
	setupWindow(hInstance, nCmdShow);

	char* src =
		"mov rax, rcx\n"
		"mov [0x30100], rax\n"

		"mov rax, [0x30006]\n"
		"mov [0x30109], rax\n"

		//"mov rax, [0x30107]\n"
		//"mul rax, 128\n"
		//"add rax, [0x30106]\n"
		//"mov [0x30100+rax], rax\n"

		"sub rsp, 24\n"
		"call [0x30030]\n"
		"add rsp, 24\n"

		"ret\n";

	size_t codesize = 0x2000;
	const char* err = assemble_string(usersignal, &codesize, src);

	if (err) {
		printf("err = %s\n", err);
	}
	else {
		printf("size = %d\n", codesize);
		callsig(asmevent_init, APP_VERSION);
	}


	runLoop();
	return 0;
}

static void checkLicense() {
	SYSTEMTIME time;
	GetSystemTime(&time);
	if (time.wYear > 2025 || time.wMonth > 7 || time.wDay > 27) {
		MessageBox(NULL, L"This HRAM beta version has expired, please get a new one, thanks!", L"HRAM beta version expired", 0);
		ExitProcess(0);
	}
}



static void openConsole() {
	AllocConsole();
	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
	freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
}

static void callsig(enum asmevent ev, UINT32 arg) {
	if (running) {
		usersignal(ev, arg);
	}
}

void tick(DWORD delta, DWORD now) {
	sys->time = now;
	callsig(asmevent_tick, delta);
}

void mouseMoved(int x, int y) {
	sys->mousex = x;
	sys->mousey = y;
	callsig(asmevent_mousemove, 7);
}

void togglekeystate(int vk, int down) {
	UINT8 byteindex = vk / 8;
	UINT8 bit = vk % 8;
	sys->keys[byteindex] = (sys->keys[byteindex] & ~(1 << bit)) | (down << bit);
}

void mouseDown(int b) {
	callsig(asmevent_mousedown, b);
}

void mouseUp(int b) {
	callsig(asmevent_mouseup, b);
}

void mouseWheel(int d) {
	callsig(asmevent_mousewheel, d);
}

void keyDown(int vk) {
	togglekeystate(vk, 1);
	callsig(asmevent_keydown, vk);
}

void keyUp(int vk) {
	togglekeystate(vk, 0);
	callsig(asmevent_keyup, vk);
}

void syskeyDown(int vk) {
	togglekeystate(vk, 1);
	callsig(asmevent_keydown, vk);
}

void syskeyUp(int vk) {
	togglekeystate(vk, 0);
	callsig(asmevent_keyup, vk);
}

void keyChar(const char ch) {
	//callsig(asmevent_keychar, ch);
}

void sysChar(const char ch) {
	//callsig(asmevent_keychar, ch);
}

