#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>

#include "app.hpp"

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
	SDL_Init(SDL_INIT_VIDEO);

	auto app = new App();
	*appstate = app;

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
	auto app = (App*)appstate;
	app->iterate();
	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* e)
{
	auto app = (App*)appstate;

	switch (e->type) {

	case SDL_EVENT_WINDOW_RESIZED:
		app->resized();
		break;

	case SDL_EVENT_MOUSE_MOTION:
		app->mouseMoved(e->motion.x, e->motion.y);
		break;

	case SDL_EVENT_MOUSE_BUTTON_DOWN:
	case SDL_EVENT_MOUSE_BUTTON_UP:
		app->mouseButton(e->button.button, e->button.down);
		break;

	case SDL_EVENT_QUIT:
		return SDL_APP_SUCCESS;

	}
	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
	auto app = (App*)appstate;
	delete app;
}
