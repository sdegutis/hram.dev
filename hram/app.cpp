#include "app.h"

#include <lua/lua.hpp>

#include "util.h"
#include "window.h"

lua_State* mvm;

#include "image.h"
ID3D11Texture2D* img;

void app::boot()
{
	openConsole();

	mvm = luaL_newstate();
	luaL_openlibs(mvm);

	luaL_dofile(mvm, "foo.lua");

	auto data = new uint8_t[4 * 4 * 4];
	for (int i = 0; i < 4 * 4 * 4; i++) data[i] = rand() % 0xff;
	img = createImage(device, (uint32_t*)data, 4, 4);
	delete[] data;

	devicecontext->CopySubresourceRegion(screen->texture, 0, 6, 10, 0, img, 0, NULL);

}

void app::mouseMoved(int x, int y)
{
	//screen->pset(mousex, mousey, RGB(rand() % 0xff, rand() % 0xff, rand() % 0xff));

	lua_getglobal(mvm, "mousemove");
	lua_pushinteger(mvm, x);
	lua_pushinteger(mvm, y);
	lua_pcall(mvm, 2, 0, 0);

	//devicecontext->CopySubresourceRegion(screen->texture, 0, x, y, 0, img, 0, NULL);

	//s.copyTo(*screen, mousex, mousey);
}

void app::mouseDown(int b)
{
	printf("mouse down %d\n", b);
}

void app::mouseUp(int b)
{
	printf("mouse up %d\n", b);
}

void app::mouseWheel(int d)
{
	printf("wheel %d\n", d);
}

void app::keyDown(int vk)
{
	printf("keydown %d\n", vk);
}

void app::keyUp(int vk)
{
	printf("keyup %d\n", vk);
}

void app::keyChar(int ch)
{
	printf("char %c\n", ch);
}
