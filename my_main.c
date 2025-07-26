#include <Windows.h>
#include "my_screen.h"
#include <stdio.h>
#include "resource.h"
#include <shlobj_core.h>
#include <KnownFolders.h>

#include "my_font.h"
#include "my_window.h"


// forward decl

static void checkLicense();
static void createMemory();
static void setup();


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, PWSTR pCmdLine, int nCmdShow) {
	checkLicense();
	createMemory();
	setupWindow(hInstance, nCmdShow);
	setup();
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

static void createMemory() {
	void* sysmem = VirtualAlloc(0x30000, 0x8000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (!sysmem) {
		MessageBox(NULL, L"Could not allocate sufficient memory.", L"Fatal error", 0);
		ExitProcess(1);
	}
}

void blitimmediately();

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

struct System* sys = 0x30000;
void (*usersignal)(UINT32 evid, UINT32 evarg) = 0x34000;
static char* usersrc = 0x36000;


const char* assemble_string(void* dst, size_t* dst_size, char* src);

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

	initfont(sys->font);

	int funcs = 0;
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

	unsigned char* base = usersignal;
	memset(base, 0, 100);

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
		"call [0x30038]\n"
		"add rsp, 24\n"

		"ret\n";

	size_t codesize = 0x2000;
	const char* err = assemble_string(usersignal, &codesize, src);

	if (err) {
		printf("err = %s\n", err);
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

static int running = 1;

void callsig(enum asmevent ev, UINT32 arg) {
	if (running) {
		usersignal(ev, arg);
	}
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

