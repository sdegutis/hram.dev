#pragma once

#include <intsafe.h>

struct Program {
	void (*tick)(DWORD delta, DWORD now);
	void (*mouseMoved)(int x, int y);
	void (*mouseDown)(int b);
	void (*mouseUp)(int b);
	void (*mouseWheel)(int d);
	void (*keyDown)(int vk);
	void (*keyUp)(int vk);
	void (*syskeyDown)(int vk);
	void (*syskeyUp)(int vk);
	void (*keyChar)(const char ch);
	void (*sysChar)(const char ch);
};

extern struct Program mainProg;
