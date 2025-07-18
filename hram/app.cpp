#include "app.h"

#include <lua/lua.hpp>

#include "util.h"
#include "window.h"
#include "screen.h"
#include "image.h"
#include "memory.h"

lua_State* mvm;

ID3D11Texture2D* img;



void app::boot()
{
	openConsole();

	mvm = luaL_newstate();
	luaL_openlibs(mvm);

	luaopen_memory(mvm);
	lua_setglobal(mvm, "memory");

	luaopen_image(mvm);
	lua_setglobal(mvm, "image");

	lua_settop(mvm, 0);

	luaL_dofile(mvm, "foo.lua");

	auto data = new uint8_t[4 * 4 * 4];
	for (int i = 0; i < 4 * 4 * 4; i++) data[i] = rand() % 0xff;
	img = createImage(device, (uint32_t*)data, 4, 4);
	delete[] data;

	devicecontext->CopySubresourceRegion(screen->texture, 0, 6, 10, 0, img, 0, NULL);

}

void app::mouseMoved(int x, int y) {
	lua_getglobal(mvm, "mousemove");
	lua_pushinteger(mvm, x);
	lua_pushinteger(mvm, y);
	lua_pcall(mvm, 2, 0, 0);

	devicecontext->CopySubresourceRegion(screen->texture, 0, x, y, 0, img, 0, NULL);

	//screen->pset(mousex, mousey, RGB(rand() % 0xff, rand() % 0xff, rand() % 0xff));
	//devicecontext->CopySubresourceRegion(screen->texture, 0, x, y, 0, img, 0, NULL);
		//s.copyTo(*screen, mousex, mousey);
}

void app::mouseDown(int b) {
	lua_getglobal(mvm, "mousedown");
	lua_pushinteger(mvm, b);
	lua_pcall(mvm, 1, 0, 0);

	useScreen(1 - screeni);
}

void app::mouseUp(int b) {
	lua_getglobal(mvm, "mouseup");
	lua_pushinteger(mvm, b);
	lua_pcall(mvm, 1, 0, 0);
}

void app::mouseWheel(int d) {
	lua_getglobal(mvm, "mousewheel");
	lua_pushinteger(mvm, d);
	lua_pcall(mvm, 1, 0, 0);
}

void app::keyDown(int vk) {
	lua_getglobal(mvm, "keydown");
	lua_pushinteger(mvm, vk);
	lua_pcall(mvm, 1, 0, 0);
}

void app::keyUp(int vk) {
	lua_getglobal(mvm, "keyup");
	lua_pushinteger(mvm, vk);
	lua_pcall(mvm, 1, 0, 0);
}

void app::keyChar(const char ch) {
	lua_getglobal(mvm, "keychar");
	lua_pushlstring(mvm, &ch, 1);
	lua_pcall(mvm, 1, 0, 0);
}
