#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>

//#include <vars.h>
#include <lua.hpp>
#include "canvas.hpp"

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
	lua_State* lib = luaL_newstate();
	printf("%p ?\n", lib);

	SDL_Init(SDL_INIT_VIDEO);

	auto canvas = new Canvas();
	*appstate = canvas;
	canvas->draw();

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
	auto canvas = (Canvas*)appstate;
	canvas->iterate();
	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* e)
{
	auto canvas = (Canvas*)appstate;

	switch (e->type) {

	case SDL_EVENT_WINDOW_RESIZED:
		canvas->resized();
		break;

	case SDL_EVENT_MOUSE_MOTION:
		canvas->mouseMoved(e->motion.x, e->motion.y);
		break;

	case SDL_EVENT_MOUSE_BUTTON_DOWN:
	case SDL_EVENT_MOUSE_BUTTON_UP:
		//printf("%d, %d", e->button.button, e->button.down);
		break;

	case SDL_EVENT_QUIT:
		return SDL_APP_SUCCESS;

	}
	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
	auto canvas = (Canvas*)appstate;
	delete canvas;
}
