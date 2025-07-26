#include <Shlwapi.h>
#include <strsafe.h>
#include <shlobj_core.h>

#include "my_memory.h"
#include "my_file.h"

HANDLE file;

void loadUserCodeFromDisk() {
	PWSTR buf;
	PWSTR userfile[MAX_PATH];
	SHGetKnownFolderPath(&FOLDERID_RoamingAppData, 0, NULL, &buf);
	StrCpyW(userfile, buf);
	CoTaskMemFree(buf);
	PathAppendW(userfile, L"hram\\hsig.s");

	file = CreateFileW(userfile, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (GetLastError() != ERROR_ALREADY_EXISTS) {
		static const char src[] =
			"; switch on arg\n"
			"cmp cl, 4\n"
			"je MouseDown\n"
			"cmp cl, 5\n"
			"je MouseUp\n"
			"\n"
			"; if mouse not down then just skip\n"
			"mov al, byte [0x33000]\n"
			"test al, 1\n"
			"jz Skip\n"
			"\n"
			"; draw green at mouse\n"
			"mov rax, 0\n"
			"mov rbx, 0\n"
			"mov al, [0x30007]\n"
			"mov bl, 128\n"
			"mul bl\n"
			"add al, [0x30006]\n"
			"mov byte ptr [rax+0x30100], 0x0f\n"
			"\n"
			"; call blit()\n"
			"sub rsp, 24\n"
			"call [0x30030]\n"
			"add rsp, 24\n"
			"\n"
			"Skip:\n"
			"ret\n"
			"\n"
			"; store mouse-down info\n"
			"\n"
			"MouseDown:\n"
			"mov byte ptr [0x33000], 1\n"
			"ret\n"
			"\n"
			"MouseUp:\n"
			"mov byte ptr [0x33000], 0\n"
			"ret\n"
			;

		DWORD written;
		BOOL ok = WriteFile(file, src, sizeof(src) - 1, &written, NULL);
		//printf("ok: %d\n", ok);
		//printf("written: %d\n", written);
		StringCchCopyA(sys->progsrc, sizeof(src), src);
	}
	else {
		DWORD read;
		BOOL ok = ReadFile(file, sys->progsrc, 0x2000, &read, NULL);
		//printf("ok: %d\n", ok);
		//printf("read: %d\n", read);
	}
}
