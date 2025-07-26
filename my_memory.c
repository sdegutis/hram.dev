#include "my_memory.h"

#include <Windows.h>

#include "my_screen.h"
#include "my_window.h"

struct Memory* sys = 0x30000;
void (*usersignal)(UINT32 evid, UINT32 evarg) = 0x34000;
char* usersrc = 0x36000;

void setupMemory() {
	void* sysmem = VirtualAlloc(0x30000, 0x8000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (!sysmem) {
		MessageBox(NULL, L"Could not allocate sufficient memory.", L"Fatal error", 0);
		ExitProcess(1);
	}

	sys->appversion = 221;
	initfont(sys->font);

	int funcs = 0;
	sys->addrs[funcs++] = toggleFullscreen;
	sys->addrs[funcs++] = blitimmediately;
}

void blitimmediately() {
	devicecontext->lpVtbl->UpdateSubresource(devicecontext, screen.texture, 0, NULL, &sys->screen, 128, 0);
	draw();
}
