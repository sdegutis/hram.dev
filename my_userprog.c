#include <intsafe.h>

#include "my_userprog.h"
#include "my_memory.h"

struct Program userProg;

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

static void init() { usersignal(asmevent_init, 0); }
static void tick(DWORD delta, DWORD now) { usersignal(asmevent_tick, delta); }
static void mouseMoved(int x, int y) { usersignal(asmevent_mousemove, 7); }
static void mouseDown(int b) { usersignal(asmevent_mousedown, b); }
static void mouseUp(int b) { usersignal(asmevent_mouseup, b); }
static void mouseWheel(int d) { usersignal(asmevent_mousewheel, d); }
static void keyDown(int vk) { usersignal(asmevent_keydown, vk); }
static void keyUp(int vk) { usersignal(asmevent_keyup, vk); }
static void syskeyDown(int vk) { usersignal(asmevent_keydown, vk); }
static void syskeyUp(int vk) { usersignal(asmevent_keyup, vk); }
static void keyChar(const char ch) {}
static void sysChar(const char ch) {}

void setupUserProg() {
	userProg.init = init;
	userProg.tick = tick;
	userProg.mouseMoved = mouseMoved;
	userProg.mouseDown = mouseDown;
	userProg.mouseUp = mouseUp;
	userProg.mouseWheel = mouseWheel;
	userProg.keyDown = keyDown;
	userProg.keyUp = keyUp;
	userProg.syskeyDown = syskeyDown;
	userProg.syskeyUp = syskeyUp;
	userProg.keyChar = keyChar;
	userProg.sysChar = sysChar;
}
