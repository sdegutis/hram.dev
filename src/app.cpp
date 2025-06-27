#include "app.hpp"

#include <lua.hpp>

App::App()
{
	fileManager.copyBootFiles();


	lua_State* lua = luaL_newstate();
	printf("%p\n", lua);
	luaL_openlibs(lua);
	luaL_dofile(lua, fileManager.bootFilePath.string().c_str());

}

void App::iterate()
{
	canvas.iterate();
}

void App::resized()
{
	canvas.resized();
}

void App::mouseMoved(int x, int y)
{
	canvas.mouseMoved(x, y);
}

void App::mouseButton(int button, bool down)
{
	printf("%d, %d\n", button, down);
}
