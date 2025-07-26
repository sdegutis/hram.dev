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
			"mov rax, rcx        ; copy event arg to rax\n"
			"mov [0x30100], rax  ; copy rax to screen pixel 1\n"
			"\n"
			"mov rax, [0x30006]\n"
			"mov [0x30109], rax\n"
			"\n"
			"sub rsp, 24\n"
			"call [0x30030]\n"
			"add rsp, 24\n"
			"\n"
			"ret\n";

		DWORD written;
		BOOL ok = WriteFile(file, src, sizeof(src) - 1, &written, NULL);
		printf("ok: %d\n", ok);
		printf("written: %d\n", written);
		StringCchCopyA(usersrc, sizeof(src), src);
	}
	else {
		DWORD read;
		BOOL ok = ReadFile(file, usersrc, 0x2000, &read, NULL);
		printf("ok: %d\n", ok);
		printf("read: %d\n", read);
	}

	printf("SOURCE\n");
	printf("======\n");
	printf("%s", usersrc);
	printf("======\n");
}
