#include <Windows.h>
#include "my_screen.h"
#include <stdio.h>
#include "resource.h"
#include <shlobj_core.h>
#include <KnownFolders.h>
#include "my_fontsheet.h"

int setupWindow(HINSTANCE hInstance, int nCmdShow);
static void setup();
void runLoop();

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, PWSTR pCmdLine, int nCmdShow) {
	SYSTEMTIME time;
	GetSystemTime(&time);
	if (time.wYear > 2025 || time.wMonth > 7 || time.wDay > 27) {
		MessageBox(NULL, L"This HRAM beta version has expired, please get a new one, thanks!", L"HRAM beta version expired", 0);
		return 0;
	}

	void* sysmem = VirtualAlloc(0x30000, 0x8000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (!sysmem) {
		MessageBox(NULL, L"Could not allocate sufficient memory.", L"Fatal error", 0);
		return 1;
	}

	if (setupWindow(hInstance, nCmdShow)) {
		MessageBox(NULL, L"Could not setup window.", L"Fatal error", 0);
		return 1;
	}

	setup();
	runLoop();
	return 0;
}



extern ID3D11Device* device;
extern ID3D11DeviceContext* devicecontext;
extern struct Screen screen;


void draw();
void toggleFullscreen();
void blitimmediately();

struct AppState {
	UINT16 appversion;
	UINT8 eventid;
	UINT8 eventarg;
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

struct AppState* sys = 0x30000;
void (*usersignal)() = 0x34000;
static char* userasm = 0x34000;
static char* usersrc = 0x35000;

int aplusbtimes2(int a, int b) {
	return (a + b) * 2;
}

static void initfont();
const char* assembly_error(int err);
int assemble_string(void* dst, size_t* dst_size, char* src);

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

void callsig(enum asmevent ev, UINT32 arg);

static void setup() {

	AllocConsole();
	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
	freopen_s((FILE**)stdin, "CONIN$", "r", stdin);

	CHAR szFileName[MAX_PATH];
	GetModuleFileNameA(NULL, szFileName, MAX_PATH);
	char* exename = strrchr(szFileName, '\\') + 1;
	BOOL skipwelcome = strchr(exename, '0') != NULL;

	sys->appversion = 221;

	initfont();

	int funcs = 0;
	sys->addrs[funcs++] = aplusbtimes2;
	sys->addrs[funcs++] = toggleFullscreen;
	sys->addrs[funcs++] = blitimmediately;

	HMODULE handle = GetModuleHandle(NULL);
	HRSRC rc = FindResource(handle, MAKEINTRESOURCE(IDR_MYTEXTFILE), MAKEINTRESOURCE(TEXTFILE));
	HGLOBAL rcData = LoadResource(handle, rc);
	DWORD size = SizeofResource(handle, rc);
	const char* data = LockResource(rcData);

	PWSTR wpath;
	UINT8 userdir[MAX_PATH];
	SHGetKnownFolderPath(&FOLDERID_RoamingAppData, 0, NULL, &wpath);
	WideCharToMultiByte(CP_UTF8, 0, wpath, -1, userdir, MAX_PATH, NULL, NULL);
	CoTaskMemFree(wpath);

	unsigned char* base = userasm;
	memset(base, 100, 100);

	for (int i = 0; i < 10; i++) {
		printf("%x ", base[i]);
	}
	printf("\n");

	char* src =
		"mov rax, 0xff\n"
		"mov [0x30100], rax\n"

		"mov rax, [0x3000c]\n"
		"mov [0x30109], rax\n"

		"sub rsp, 24\n"
		"call [0x30040]\n"
		"add rsp, 24\n"

		"ret\n";

	size_t codesize = 0x2000;
	int err = assemble_string(usersignal, &codesize, src);

	if (err) {
		printf("err = %s\n", assembly_error(err));
		printf("err = %d\n", err);
	}
	else {

		printf("size = %d\n", codesize);

		for (int i = 0; i < codesize; i++) {
			printf("%x ", base[i]);
		}
		printf("\n");

		callsig(asmevent_init, 0);
	}
}

void callsig(enum asmevent ev, UINT32 arg) {
	sys->eventid = ev;
	sys->eventarg = arg;
	usersignal();
}

void blitimmediately() {
	devicecontext->lpVtbl->UpdateSubresource(devicecontext, screen.texture, 0, NULL, &sys->screen, 128, 0);
	draw();
}

void tick(DWORD delta, DWORD now) {
	sys->time = now;
	callsig(asmevent_tick, delta);

	if (sys->inflags & 1) {
		blitimmediately();
	}

	sys->inflags = 0;
}

void mouseMoved(int x, int y) {
	callsig(asmevent_mousemove, 7);
	sys->mousex = x;
	sys->mousey = y;
}

void togglekeystate(int vk, int down) {
	UINT8 bit = 0;
	if /**/ (vk == VK_CONTROL) bit = 1;
	else if (vk == VK_MENU)    bit = 2;
	else if (vk == VK_SHIFT)   bit = 3;

	if (bit) {
		bit--;
		sys->keymods = sys->keymods & ~(1 << bit) | down << bit;
	}

	UINT8 byteindex = vk / 8;
	bit = vk % 8;
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



#define FW (4)
#define FH (6)
#define SW (16)
#define SH (6)

static void initfont() {
	PUINT8 ptr = sys->font;
	int z = 0;
	for (int sy = 0; sy < SH; sy++) {
		for (int sx = 0; sx < SW; sx++) {
			for (int fy = 0; fy < FH; fy++) {
				for (int fx = 0; fx < FW; fx++) {
					int i = (fy * SW * FW) + (sy * SH * SW * FW) + (sx * FW) + fx;
					*ptr++ = fontdata[i] ? 0xf0 : 0;
				}
			}
		}
	}
}
